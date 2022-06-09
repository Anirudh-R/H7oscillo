/**
  ******************************************************************************
  * @file    bot.c
  * @author  anirudhr
  * @brief   Implements Bulk-only Transport functions.
  ******************************************************************************
  */

/* Includes */
#include "bot.h"


uint8_t INQUIRY_DATA[36] = {0x00, 0x80, 0x02, 0x02, 36-4, 0x00, 0x00, 0x00,
							'M',  'i',  'n',  'i',  ' ',  'D',  'S',  'O',
							'D',  'i',  's',  'k',  ' ',  ' ',  ' ',  ' ',
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00};

static uint8_t rdyForNxtCBW = 1;

/**
  * @brief  Process a BOT cmd/data packet.
  * @param  buf: cmd/data OUT packet
  * @param  len: length of buf
  * @retval Error code (or) Length of total IN data + CSW to send back to the host
  */
int32_t BOT_process(const uint8_t* buf, uint8_t len)
{
	uint32_t dCBWTag, dCBWDataTransferLength;
	uint8_t bmCBWFlags, bCBWCBLength;
	uint8_t CBWCB[16];
	uint8_t *respDataPtr = (uint8_t *)USB_DATA_BUFFER;
	uint32_t respLen, i;

	/* what has come must be a CBW */
	if(rdyForNxtCBW){
		uint32_t signature = buf[3] << 24 | buf[2] << 16 | buf[1] << 8 | buf[0];			/* data arrives in LSB first order */

		/* verify if its a valid CBW */
		if(signature == CBW_SIGNATURE && len == CBW_SIZE){
			dCBWTag = buf[7] << 24 | buf[6] << 16 | buf[5] << 8 | buf[4];
			dCBWDataTransferLength = buf[11] << 24 | buf[10] << 16 | buf[9] << 8 | buf[8];
			bmCBWFlags = buf[12];
			bCBWCBLength = buf[14];

			/* the actual SCSI cmd */
			for(i = 0; i < 16; i++){
				CBWCB[i] = buf[i + 15];
			}

			/* check the SCSI operation code */
			switch(CBWCB[0])
			{
				/* INQUIRY */
				case 0x12:
					for(i = 0; i < 36; i++)
						respDataPtr[i] = INQUIRY_DATA[i];

					respLen = 36;
					break;
			}

			/* append CSW to the response */
			respDataPtr[respLen++] = CSW_SIGNATURE;
			respDataPtr[respLen++] = CSW_SIGNATURE >> 8;
			respDataPtr[respLen++] = CSW_SIGNATURE >> 16;
			respDataPtr[respLen++] = CSW_SIGNATURE >> 24;
			respDataPtr[respLen++] = dCBWTag;
			respDataPtr[respLen++] = dCBWTag >> 8;
			respDataPtr[respLen++] = dCBWTag >> 16;
			respDataPtr[respLen++] = dCBWTag >> 24;
			respDataPtr[respLen++] = 0;
			respDataPtr[respLen++] = 0;
			respDataPtr[respLen++] = 0;
			respDataPtr[respLen++] = 0;
			respDataPtr[respLen++] = 0;

			return respLen;
		}
		else{
			return BOT_CBW_INVALID;
		}
	}

	/* it must be data */
	else{

	}

	return 0;
}
