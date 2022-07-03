/**
  ******************************************************************************
  * @file    bot.c
  * @author  anirudhr
  * @brief   Implements Bulk-only Transport functions and SCSI commands.
  ******************************************************************************
  */

/* Includes */
#include "bot.h"


static const uint8_t INQUIRY_DATA[INQUIRY_DATA_LEN] = {0x00, 0x80, 0x04, 0x02, 0x20, 0x00, 0x00, 0x00,
							'H',  '7',  '-',  'O',  's',  'c',  'i',  'l',
							'l',  'o',  ' ',  'D',  'i',  's',  'k',  ' ',
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00};

static const uint8_t READFMTCPTY_DATA[READFMTCPTY_DATA_LEN] = {0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x10, 0x00,
							0x02, 0x00, 0x01, 0x00};

static const uint8_t READCPTY10_DATA[READCPTY10_DATA_LEN] = {0x00, 0x00, 0x0F, 0xFF, 0x00, 0x00, 0x10, 0x00};

static const uint8_t MODESNS6_DATA[MODESNS6_DATA_LEN] = {0x03, 0x00, 0x00, 0x00};

static const uint8_t REQSENSE_DATA[REQSENSE_DATA_LEN] = {0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C,
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
  * @retval Error code (or) Length of total IN data + CSW to send back to the host. Return data is stored at USB_DATA_BUFFER.
  */
int32_t BOT_process(const uint8_t* buf, uint8_t len)
{
	uint32_t dCBWTag, dCBWDataTransferLength;
	uint8_t bmCBWFlags, bCSWStatus = 0x00;
	const uint8_t* CBWCB = buf + 15;														/* the actual SCSI cmd */
	uint8_t *respDataPtr = (uint8_t *)USB_DATA_BUFFER;
	uint32_t dCSWDataResidue;
	uint32_t alloclen, respLen, i;

	/* what has come must be a CBW */
	if(rdyForNxtCBW){
		uint32_t signature = buf[3] << 24 | buf[2] << 16 | buf[1] << 8 | buf[0];			/* data arrives in LSB first order */

		rdyForNxtCBW = 0;																	/* set status as cmd in progress */

		/* verify if its a valid CBW */
		if(signature == CBW_SIGNATURE && len == CBW_SIZE){
			dCBWTag = buf[7] << 24 | buf[6] << 16 | buf[5] << 8 | buf[4];
			dCBWDataTransferLength = buf[11] << 24 | buf[10] << 16 | buf[9] << 8 | buf[8];
			bmCBWFlags = buf[12];

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

					/* address out of range */
					if(lba + tfrlen > NUM_BLOCKS){
						bCSWStatus = 0x01;
						senseKey = SENSEKEY_ILLEGAL_REQUEST;
						addSnsCode = ASC_ADDR_OUTOFRANGE;
						addSnsCodeQual = ASCQ_ADDR_OUTOFRANGE;
						respLen = dCBWDataTransferLength;											/* send some dCBWDataTransferLength junk values */
					}
					/* transfer lengths don't match */
					else if(BLOCK_SIZE * tfrlen != dCBWDataTransferLength){
						bCSWStatus = 0x01;
						senseKey = SENSEKEY_ILLEGAL_REQUEST;
						addSnsCode = ASC_INV_CDB_FIELD;
						addSnsCodeQual = ASCQ_INV_CDB_FIELD;
						respLen = dCBWDataTransferLength;											/* send some dCBWDataTransferLength junk values */
					}
					else{
						uint32_t tfrLen1 = 0, tfrLen2 = 0;
						uint8_t fail = 0;

						respLen = BLOCK_SIZE * tfrlen;

						/* if transfer size is large, split into 2 transfers because Flash DMA has limit */
						if(tfrlen > 8){
							tfrLen1 = 8;
							tfrLen2 = tfrlen - tfrLen1;
						}
						else{
							tfrLen1 = tfrlen;
						}

						if(tfrLen1 != 0){
							fail = QSPI_flash_read(respDataPtr, lba*BLOCK_SIZE, tfrLen1*BLOCK_SIZE);
						}
						if(!fail && tfrLen2 != 0){
							fail = QSPI_flash_read(respDataPtr + tfrLen1*BLOCK_SIZE, (lba + tfrLen1)*BLOCK_SIZE, tfrLen2*BLOCK_SIZE);
						}

						/* read fail, set status as device not ready (host may retry later) */
						if(fail){
							bCSWStatus = 0x01;
							senseKey = SENSEKEY_NOT_READY;
							addSnsCode = ASC_NOTRDY_BECOMINGRDY;
							addSnsCodeQual = ASCQ_NOTRDY_BECOMINGRDY;
						}
					}
					break;

				/* unsupported cmd */
				default:
					#ifdef USBDEBUG
					printf("INVCMD %02X\n", CBWCB[0]);
					#endif

					if(!(bmCBWFlags & 0x80))
						respLen = 0;										/* data OUT cmd, nothing to send back */
					else
						respLen = dCBWDataTransferLength;					/* data IN cmd, send some dCBWDataTransferLength junk values */

					bCSWStatus = 0x01;										/* cmd failed */
					senseKey = SENSEKEY_ILLEGAL_REQUEST;					/* store request sense field vals for use when it is issued */
					addSnsCode = ASC_INVALID_CMD;
					addSnsCodeQual = ASCQ_INVALID_CMD;
					break;
			}

			#ifdef USBDEBUG
			if(bmCBWFlags & 0x80){
				printf("reqd %lu\n", dCBWDataTransferLength);
			}
			if(bCSWStatus){
				printf("cmd failed\n");
			}
			#endif

			respLen = min(respLen, dCBWDataTransferLength);					/* transfer utmost dCBWDataTransferLength bytes */

			/* append CSW to the response */
			dCSWDataResidue = (bCSWStatus == 0x00) ? dCBWDataTransferLength - respLen : dCBWDataTransferLength;
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
			if(bCSWStatus == 0x00){
				return respLen;
			}
			/* there is going to be data OUT from the host */
			else if(!(bmCBWFlags & 0x80) && dCBWDataTransferLength > 0){
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
		#ifdef USBDEBUG
		printf("BOT Data\n");
		#endif

		dataOutCnt -= len;													/* no. of bytes pending */

		if(dataOutCnt == 0)
			return CSW_SIZE;												/* data OUT stage done, send CSW */
		else
			return BOT_DATAOUTSTAGE;
	}

	return 0;
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
