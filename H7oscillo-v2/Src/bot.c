/**
  ******************************************************************************
  * @file    bot.c
  * @author  anirudhr
  * @brief   Implements Bulk-only Transport functions.
  ******************************************************************************
  */

/* Includes */
#include "bot.h"


const uint8_t INQUIRY_DATA[INQUIRY_DATA_LEN] = {0x00, 0x80, 0x04, 0x02, 0x20, 0x00, 0x00, 0x00,
							'H',  '7',  '-',  'O',  's',  'c',  'i',  'l',
							'l',  'o',  ' ',  'D',  'i',  's',  'k',  ' ',
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00};

const uint8_t READFMTCPTY_DATA[READFMTCPTY_DATA_LEN] = {0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x10, 0x00,
							0x02, 0x00, 0x01, 0x00};

const uint8_t READCPTY10_DATA[READCPTY10_DATA_LEN] = {0x00, 0x00, 0x0F, 0xFF, 0x00, 0x00, 0x10, 0x00};

const uint8_t MODESNS6_DATA[MODESNS6_DATA_LEN] = {0x03, 0x00, 0x00, 0x00};

const uint8_t REQSENSE_DATA[REQSENSE_DATA_LEN] = {0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00};

static uint8_t rdyForNxtCBW = 1;
static uint8_t senseKey = 0;					/* Request Sense command field values */
static uint8_t addSnsCode = 0;
static uint8_t addSnsCodeQual = 0;
static uint32_t dataOutCnt = 0;

/**
  * @brief  Process a BOT cmd/data packet.
  * @param  buf: cmd/data OUT packet
  * @param  len: length of buf
  * @param  stallReqd: return value 0:No stall, 1:IN EP stall, 2:OUT EP stall, 3:BOTH EP stall
  * @retval Error code (or) Length of total IN data + CSW to send back to the host
  */
int32_t BOT_process(const uint8_t* buf, uint8_t len, uint8_t* stallReqd)
{
	uint32_t dCBWTag, dCBWDataTransferLength;
	uint8_t bmCBWFlags, bCBWCBLength, bCSWStatus;
	const uint8_t* CBWCB = buf + 15;														/* the actual SCSI cmd */
	uint8_t *respDataPtr = (uint8_t *)USB_DATA_BUFFER;
	uint32_t dCSWDataResidue;
	uint32_t alloclen, respLen, i;

	*stallReqd = BOT_NO_STALL;

	/* what has come must be a CBW */
	if(rdyForNxtCBW){
		uint32_t signature = buf[3] << 24 | buf[2] << 16 | buf[1] << 8 | buf[0];			/* data arrives in LSB first order */

		rdyForNxtCBW = 0;																	/* set status as cmd in progress */

		/* verify if its a valid CBW */
		if(signature == CBW_SIGNATURE && len == CBW_SIZE){
			dCBWTag = buf[7] << 24 | buf[6] << 16 | buf[5] << 8 | buf[4];
			dCBWDataTransferLength = buf[11] << 24 | buf[10] << 16 | buf[9] << 8 | buf[8];
			bmCBWFlags = buf[12];
			bCBWCBLength = buf[14];
			bCSWStatus = 0;

			/* check the SCSI operation code */
			switch(CBWCB[0])
			{
				/* TEST UNIT READY */
				case SCSI_TESTUNITRDY:
					#ifdef USBDEBUG
					printf("TESTUNITRDY\n");
					#endif
					respLen = 0;
					break;

				/* INQUIRY */
				case SCSI_INQUIRY:
					#ifdef USBDEBUG
					printf("INQUIRY\n");
					#endif
					alloclen = CBWCB[3] << 8 | CBWCB[4];
					respLen = min(alloclen, INQUIRY_DATA_LEN);

					for(i = 0; i < respLen; i++){
						respDataPtr[i] = INQUIRY_DATA[i];
					}
					break;

				/* READ FORMAT CAPACITIES */
				case SCSI_READFMTCPTY:
					#ifdef USBDEBUG
					printf("READFMTCPTY\n");
					#endif
					alloclen = CBWCB[7] << 8 | CBWCB[8];
					respLen = min(alloclen, READFMTCPTY_DATA_LEN);

					for(i = 0; i < respLen; i++){
						respDataPtr[i] = READFMTCPTY_DATA[i];
					}
					break;

				/* READ CAPACITY 10 */
				case SCSI_READCPTY10:
					#ifdef USBDEBUG
					printf("READCPTY\n");
					#endif
					respLen = READCPTY10_DATA_LEN;

					for(i = 0; i < respLen; i++){
						respDataPtr[i] = READCPTY10_DATA[i];
					}
					break;

				/* MODE SENSE 6 */
				case SCSI_MODESNS6:
					#ifdef USBDEBUG
					printf("MODESNS\n");
					#endif
					respLen = MODESNS6_DATA_LEN;

					for(i = 0; i < respLen; i++){
						respDataPtr[i] = MODESNS6_DATA[i];
					}
					break;

				/* START STOP UNIT */
				case SCSI_STRTSTPUNIT:
					#ifdef USBDEBUG
					printf("STRTSTPUNIT\n");
					#endif
					respLen = 0;
					break;

				/* REQUEST SENSE */
				case SCSI_REQSENSE:
					#ifdef USBDEBUG
					printf("REQSENSE\n");
					#endif
					respLen = REQSENSE_DATA_LEN;

					for(i = 0; i < respLen; i++){
						respDataPtr[i] = REQSENSE_DATA[i];
					}

					respDataPtr[2] = senseKey;
					respDataPtr[12] = addSnsCode;
					respDataPtr[12] = addSnsCodeQual;
					break;

				/* READ 10 */
				case SCSI_READ10:;
					#ifdef USBDEBUG
					printf("READ10\n");
					#endif

					uint32_t lba = CBWCB[2] << 24 | CBWCB[3] << 16 | CBWCB[4] << 8 | CBWCB[5];		/* starting block address */
					uint16_t tfrlen = CBWCB[7] << 8 | CBWCB[8];										/* no. of blocks to read */

					respLen = BLOCK_SIZE * tfrlen;

					if(tfrlen != 0){
						QSPI_flash_read(respDataPtr, lba*BLOCK_SIZE, respLen);
					}
					break;

				/* unsupported cmd */
				default:
					#ifdef USBDEBUG
					printf("INVCMD %02X\n", CBWCB[0]);
					#endif
					respLen = 0;

					bCSWStatus = 1;											/* cmd failed */
					senseKey = 0x05;										/* store request sense field vals for use when it is issued */
					addSnsCode = 0x20;
					addSnsCodeQual = 0x00;
					break;
			}

			#ifdef USBDEBUG
			printf("reqd %lu\n", dCBWDataTransferLength);
			#endif

			respLen = min(respLen, dCBWDataTransferLength);					/* transfer utmost dCBWDataTransferLength bytes */

			/* append CSW to the response */
			dCSWDataResidue = dCBWDataTransferLength - respLen;
			respDataPtr[respLen++] = (uint8_t)(CSW_SIGNATURE);
			respDataPtr[respLen++] = (uint8_t)(CSW_SIGNATURE >> 8);
			respDataPtr[respLen++] = (uint8_t)(CSW_SIGNATURE >> 16);
			respDataPtr[respLen++] = (uint8_t)(CSW_SIGNATURE >> 24);
			respDataPtr[respLen++] = dCBWTag;
			respDataPtr[respLen++] = dCBWTag >> 8;
			respDataPtr[respLen++] = dCBWTag >> 16;
			respDataPtr[respLen++] = dCBWTag >> 24;
			respDataPtr[respLen++] = dCSWDataResidue;
			respDataPtr[respLen++] = dCSWDataResidue >> 8;
			respDataPtr[respLen++] = dCSWDataResidue >> 16;
			respDataPtr[respLen++] = dCSWDataResidue >> 24;
			respDataPtr[respLen++] = bCSWStatus;

			/* successful cmd */
			if(bCSWStatus == 0){
				return respLen;
			}
			/* there is going to be data OUT from the host */
			else if(CBWCB[0] == SCSI_WRITE10){
				dataOutCnt = dCBWDataTransferLength;						/* no. of bytes that will be tranferred in the data OUT stage */
				return BOT_DATAOUTSTAGE;
			}
			/* unsuccessful cmd */
			else{
				return respLen;
			}
		}

		/* invalid CBW */
		else{
			return BOT_CBW_INVALID;
		}
	}

	/* it must be data (we don't accept writes to disk, so ignore the data) */
	else{
		printf("RX BOT DATA\n");//
		dataOutCnt -= len;													/* no. of bytes pending */

		if(dataOutCnt == 0)
			return CSW_SIZE;												/* data OUT stage done, send CSW */
		else
			return BOT_DATAOUTSTAGE;
	}

	return 0;
}

/**
  * @brief  Check if a BOT cmd is in progress.
  * @param  None
  * @retval 1=ready for next cmd, 0=current cmd still in progress
  */
uint8_t BOT_isRdyForNxtCmd()
{
	return rdyForNxtCBW;
}

/**
  * @brief  Mark the current cmd as done.
  * @param  None
  * @retval None
  */
void BOT_setCmdDone()
{
	rdyForNxtCBW = 1;
}
