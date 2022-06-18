/**
  ******************************************************************************
  * @file    usb_core.c
  * @author  anirudhr
  * @brief   Implements USB FS device-specific functions.
  ******************************************************************************
  */

/* Includes */
#include "usb_core.h"


static uint8_t USB_IN_transfer(volatile USB_OTG_INEndpointTypeDef* ep, volatile uint32_t* txfifo, const uint8_t buf[], uint32_t len);
static void USB_handle_error(void);

static volatile USB_OTG_GlobalTypeDef* const USBFSG = USB_OTG_FS;																										/* OTGFS Global registers */
static volatile USB_OTG_DeviceTypeDef* const USBFSD = (USB_OTG_DeviceTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_DEVICE_BASE);											/* OTGFS Device mode registers */
static volatile USB_OTG_INEndpointTypeDef* const EP0IN = (USB_OTG_INEndpointTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_IN_ENDPOINT_BASE + 0*USB_OTG_EP_REG_SIZE);		/* Control endpoint IN registers */
static volatile USB_OTG_OUTEndpointTypeDef* const EP0OUT = (USB_OTG_OUTEndpointTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_OUT_ENDPOINT_BASE + 0*USB_OTG_EP_REG_SIZE);	/* Control endpoint OUT registers */
static volatile USB_OTG_INEndpointTypeDef* const EP1 = (USB_OTG_INEndpointTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_IN_ENDPOINT_BASE + 1*USB_OTG_EP_REG_SIZE);		/* Bulk-IN endpoint registers */
static volatile USB_OTG_OUTEndpointTypeDef* const EP2 = (USB_OTG_OUTEndpointTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_OUT_ENDPOINT_BASE + 2*USB_OTG_EP_REG_SIZE);		/* Bulk-OUT endpoint registers */
static volatile uint32_t* const EP0FIFO = (uint32_t *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_FIFO_BASE + 0*USB_OTG_FIFO_SIZE);												/* FIFO addresses for EP0, EP1, EP2 */
static volatile uint32_t* const EP1FIFO = (uint32_t *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_FIFO_BASE + 1*USB_OTG_FIFO_SIZE);
static volatile uint32_t* const EP2FIFO = (uint32_t *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_FIFO_BASE + 2*USB_OTG_FIFO_SIZE);

static uint32_t usbRxPacket[16];
uint8_t setAddrFlag = 0;
uint8_t devAddr;

/**
  * @brief  Initialize the USB core for device mode operation.
  * @param  None
  * @retval None
  */
void USB_init(void)
{
	/* configure GPIOs */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);

	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_11, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_12, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode(GPIOD, LL_GPIO_PIN_5, LL_GPIO_MODE_OUTPUT);

	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_10, LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_11, LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_12, LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetPinSpeed(GPIOD, LL_GPIO_PIN_5, LL_GPIO_SPEED_FREQ_LOW);

	LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_10, LL_GPIO_AF_10);
	LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_11, LL_GPIO_AF_10);
	LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_12, LL_GPIO_AF_10);

	LL_GPIO_SetPinOutputType(GPIOD, LL_GPIO_PIN_5, LL_GPIO_OUTPUT_PUSHPULL);								/* disable the power switch */
	LL_GPIO_SetOutputPin(GPIOD, LL_GPIO_PIN_5);

	/* configure 48MHz OTG_FS clock */
	LL_RCC_PLLSAI_Disable();
	MODIFY_REG(RCC->PLLSAICFGR, RCC_PLLSAICFGR_PLLSAIN | RCC_PLLSAICFGR_PLLSAIR | RCC_PLLSAICFGR_PLLSAIP, (192U << RCC_PLLSAICFGR_PLLSAIN_Pos) | LL_RCC_PLLSAIR_DIV_5 | (0x01 << RCC_PLLSAICFGR_PLLSAIP_Pos));
	LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_PLLSAI);
	LL_RCC_PLLSAI_Enable();

	/* enable USB clock and reset */
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_OTGFS);
	LL_AHB2_GRP1_ForceReset(LL_AHB2_GRP1_PERIPH_OTGFS);
	LL_AHB2_GRP1_ReleaseReset(LL_AHB2_GRP1_PERIPH_OTGFS);

	NVIC_SetPriority(OTG_FS_IRQn, 8);
	NVIC_EnableIRQ(OTG_FS_IRQn);

	USBFSG->GAHBCFG |= USB_OTG_GAHBCFG_GINT | USB_OTG_GAHBCFG_TXFELVL;										/* unmask global USB interrupts */
	MODIFY_REG(USBFSG->GUSBCFG, USB_OTG_GUSBCFG_FDMOD | USB_OTG_GUSBCFG_TRDT | USB_OTG_GUSBCFG_TOCAL,
			0x01 << USB_OTG_GUSBCFG_FDMOD_Pos | 0x06 << USB_OTG_GUSBCFG_TRDT_Pos | 0x00 << USB_OTG_GUSBCFG_TOCAL_Pos);	/* force device mode */
	LL_mDelay(25);
	USBFSG->GINTMSK |= USB_OTG_GINTMSK_OTGINT | USB_OTG_GINTMSK_MMISM;										/* unmask required global interrupts */

	/* check if it is in device mode */
	if(USBFSG->GINTSTS & USB_OTG_GINTSTS_CMOD){
		printf("Error: Not in device mode!\n");
		USB_handle_error();
	}

	USBFSG->GCCFG |= USB_OTG_GCCFG_VBDEN | USB_OTG_GCCFG_PWRDWN;											/* power-on transceiver */
	MODIFY_REG(USBFSD->DCFG, USB_OTG_DCFG_DSPD | USB_OTG_DCFG_NZLSOHSK, 0x03 << USB_OTG_DCFG_DSPD_Pos | 0x00 << USB_OTG_DCFG_NZLSOHSK_Pos);	/* set speed to full-speed */
	USBFSG->GINTSTS = 0xFFFFFFFF;
	USBFSG->GINTMSK |= USB_OTG_GINTMSK_USBRST | USB_OTG_GINTMSK_ENUMDNEM | USB_OTG_GINTMSK_ESUSPM |
			USB_OTG_GINTMSK_USBSUSPM | USB_OTG_GINTMSK_SOFM | USB_OTG_GINTMSK_RXFLVLM;						/* unmask required interrupts */
	USBFSD->DCTL |= USB_OTG_DCTL_SDIS;																		/* soft-disconnect device till configuration is done */
	EP0OUT->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
	USBFSD->DAINTMSK |= (0x01 << 16) | (0x01 << 0);															/* unmask EP0 IN & OUT interrupts */
	USBFSD->DOEPMSK |= USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM;
	USBFSD->DIEPMSK |= USB_OTG_DIEPMSK_XFRCM | USB_OTG_DIEPMSK_TOM;

	/* setup FIFOs */
	USBFSG->GRXFSIZ = 64;																					/* RX FIFO size = 64 words */
	USBFSG->DIEPTXF0_HNPTXFSIZ = (16 << 16) | 64;															/* EP0-IN TX FIFO size = 16 words, starting from the end of RX FIFO */
	USBFSG->DIEPTXF[0] = (16 << 16) | 80;																	/* EP1 TX FIFO size = 16 words, starting from the end of EP0-IN TX FIFO */

	MODIFY_REG(EP0IN->DIEPCTL, USB_OTG_DIEPCTL_MPSIZ | USB_OTG_DIEPCTL_TXFNUM | USB_OTG_DIEPCTL_EPENA,		/* 64-byte max packet size, enable EP0-IN */
			0x00 << USB_OTG_DIEPCTL_MPSIZ_Pos | 0x00 << USB_OTG_DIEPCTL_TXFNUM_Pos | 0x01 << USB_OTG_DIEPCTL_EPENA_Pos);
	MODIFY_REG(EP0OUT->DOEPTSIZ, USB_OTG_DOEPTSIZ_STUPCNT, 0x01 << USB_OTG_DOEPTSIZ_STUPCNT_Pos);			/* can receive single SETUP packet at a time */
	MODIFY_REG(EP0OUT->DOEPCTL, USB_OTG_DOEPCTL_MPSIZ | USB_OTG_DOEPCTL_EPENA,								/* 64-byte max packet size, enable EP0-OUT */
			0x00 << USB_OTG_DOEPCTL_MPSIZ_Pos | 0x01 << USB_OTG_DOEPCTL_EPENA_Pos);

	USBFSG->GOTGCTL |= USB_OTG_GOTGCTL_BVALOEN | USB_OTG_GOTGCTL_BVALOVAL;									/* set B-session to be always valid */
	USBFSD->DCTL &= ~USB_OTG_DCTL_SDIS;																		/* connect device */

	return;
}

/**
  * @brief  Configure EP1 and EP2 bulk endpoints for operation.
  * @param  None
  * @retval None
  */
void USB_config_bulkEPs(void)
{
	EP1->DIEPCTL |= USB_OTG_DIEPCTL_SNAK;
	EP2->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
	USBFSD->DAINTMSK |= (0x01 << 1) | (0x01 << 18);															/* unmask EP1 (IN) and EP2 (OUT) interrupts */
	MODIFY_REG(EP1->DIEPCTL, USB_OTG_DIEPCTL_MPSIZ | USB_OTG_DIEPCTL_TXFNUM | USB_OTG_DIEPCTL_EPENA			/* 64-byte max packet size, bulk type, enable EP1 */
			| USB_OTG_DIEPCTL_EPTYP | USB_OTG_DIEPCTL_SD0PID_SEVNFRM | USB_OTG_DIEPCTL_USBAEP,
			0x40 << USB_OTG_DIEPCTL_MPSIZ_Pos | 0x01 << USB_OTG_DIEPCTL_TXFNUM_Pos | 0x01 << USB_OTG_DIEPCTL_EPENA_Pos |
			0x02 << USB_OTG_DIEPCTL_EPTYP_Pos | 0x01 << USB_OTG_DIEPCTL_SD0PID_SEVNFRM_Pos | 0x01 << USB_OTG_DIEPCTL_USBAEP_Pos);
	MODIFY_REG(EP2->DOEPCTL, USB_OTG_DOEPCTL_MPSIZ | USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_EPTYP | 		/* 64-byte max packet size, bulk type, enable EP2 */
			USB_OTG_DOEPCTL_SD0PID_SEVNFRM | USB_OTG_DOEPCTL_USBAEP, 0x40 << USB_OTG_DOEPCTL_MPSIZ_Pos |
			0x01 << USB_OTG_DOEPCTL_EPENA_Pos | 0x02 << USB_OTG_DOEPCTL_EPTYP_Pos | 0x01 << USB_OTG_DOEPCTL_SD0PID_SEVNFRM_Pos
			| 0x01 << USB_OTG_DOEPCTL_USBAEP_Pos);
	EP2->DOEPCTL |= USB_OTG_DOEPCTL_CNAK;
}

/**
  * @brief  Handle USB interrupt events.
  * @param  None
  * @retval None
  */
void USB_handle_event(void)
{
	/* USB Reset */
	if(USBFSG->GINTSTS & USB_OTG_GINTSTS_USBRST){
		USBFSG->GINTSTS |= USB_OTG_GINTSTS_USBRST;
		MODIFY_REG(USBFSG->GRSTCTL, USB_OTG_GRSTCTL_TXFNUM, 0x10 << USB_OTG_GRSTCTL_TXFNUM_Pos);
		USBFSG->GRSTCTL |= USB_OTG_GRSTCTL_TXFFLSH;									/* flush all TX FIFOs */
		while(USBFSG->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH);
		MODIFY_REG(USBFSD->DCFG, USB_OTG_DCFG_DAD, 0 << USB_OTG_DCFG_DAD_Pos);		/* set addr to 0 */
		setAddrFlag = 0;
		EP0OUT->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;									/* set NAK of OUT EPs */
		EP2->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
		#ifdef USBDEBUG
		printf("USB Reset\n");
		#endif
	}

	/* Suspend */
	else if(USBFSG->GINTSTS & USB_OTG_GINTSTS_USBSUSP){
		USBFSG->GINTSTS |= USB_OTG_GINTSTS_USBSUSP;
		#ifdef USBDEBUG
		printf("USB Suspend\n");
		#endif
	}

	/* USB Speed enumeration complete */
	else if(USBFSG->GINTSTS & USB_OTG_GINTSTS_ENUMDNE){
		USBFSG->GINTSTS |= USB_OTG_GINTSTS_ENUMDNE;
		if(((USBFSD->DSTS & USB_OTG_DSTS_ENUMSPD) >> USB_OTG_DSTS_ENUMSPD_Pos) != 0x03){
			printf("Error: Not in Full speed mode!\n");
			USB_handle_error();
		}
		else{
			EP0OUT->DOEPCTL |= USB_OTG_DOEPCTL_CNAK;								/* clear NAK bit to ACK SETUP packets */
		}
	}

	/* Received new packet */
	else if(USBFSG->GINTSTS & USB_OTG_GINTSTS_RXFLVL){
		uint32_t rxpacketsts = USBFSG->GRXSTSP;
		uint8_t packettype = (rxpacketsts & USB_OTG_GRXSTSP_PKTSTS) >> USB_OTG_GRXSTSP_PKTSTS_Pos;
		uint8_t bytecnt = (rxpacketsts & USB_OTG_GRXSTSP_BCNT) >> USB_OTG_GRXSTSP_BCNT_Pos;
		uint8_t dstEP = (rxpacketsts & USB_OTG_GRXSTSP_EPNUM) >> USB_OTG_GRXSTSP_EPNUM_Pos;
		uint8_t* packetPtr = (uint8_t *)usbRxPacket;
		//printf("Rx %d bytes of type %d to ep %d\n", bytecnt, packettype, dstEP);

		if(dstEP == 0){
			if(bytecnt){
				for(uint32_t i = 0; i < (bytecnt + 3)/4; i++)
					usbRxPacket[i] = *EP0FIFO;

				#ifdef USBDEBUG
				for(uint32_t i = 0; i < bytecnt; i++)
					printf("%02X ", packetPtr[i]);
				printf("\n");
				#endif
			}
			else if(packettype == 0x02){
				#ifdef USBDEBUG
				printf("Rx ZLP\n");
				#endif
				EP0OUT->DOEPCTL |= USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA;		/* clear NAK bit and re-enable EP */
				return;
			}
			else{
				EP0OUT->DOEPCTL |= USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA;		/* clear NAK bit and re-enable EP */
				return;
			}
		}

		/* SETUP packet */
		if(packettype == 0x06){
			/* Get Descriptor, Device Descriptor */
			if(packetPtr[1] == 0x06 && packetPtr[3] == 0x01){
				USB_IN_transfer(EP0IN, EP0FIFO, deviceDescriptor, SIZ_DEVICE_DESC);
				#ifdef USBDEBUG
				printf("Tx %d bytes\n", SIZ_DEVICE_DESC);
				#endif
			}
			/* Set Address */
			else if(packetPtr[1] == 0x05){
				devAddr = packetPtr[2];
				setAddrFlag = 1;
				USB_IN_transfer(EP0IN, EP0FIFO, NULL, 0);
				#ifdef USBDEBUG
				printf("Tx ZLP\n");
				#endif
			}
			/* Get Descriptor, Configuration Descriptor */
			else if(packetPtr[1] == 0x06 && packetPtr[3] == 0x02){
				uint8_t nBytesToSend = min(packetPtr[6], SIZ_CONFIG_DESC);
				USB_IN_transfer(EP0IN, EP0FIFO, configDescriptor, nBytesToSend);
				#ifdef USBDEBUG
				printf("Tx %d bytes\n", nBytesToSend);
				#endif
			}
			/* Get Descriptor, String Descriptor */
			else if(packetPtr[1] == 0x06 && packetPtr[3] == 0x03){
				/* Get supported languages */
				if(packetPtr[4] == 0x00 && packetPtr[5] == 0x00){
					uint8_t nBytesToSend = min(packetPtr[6], SIZ_STRING_LANGID);
					USB_IN_transfer(EP0IN, EP0FIFO, stringLangID, nBytesToSend);
					#ifdef USBDEBUG
					printf("Tx %d bytes\n", nBytesToSend);
					#endif
				}
				/* String manufacturer */
				else if(packetPtr[2] == 0x01){
					uint8_t nBytesToSend = min(packetPtr[6], SIZ_STRING_MANUFAC);
					USB_IN_transfer(EP0IN, EP0FIFO, stringManufac, nBytesToSend);
					#ifdef USBDEBUG
					printf("Tx %d bytes\n", nBytesToSend);
					#endif
				}
				/* String product */
				else if(packetPtr[2] == 0x02){
					uint8_t nBytesToSend = min(packetPtr[6], SIZ_STRING_PRODUCT);
					USB_IN_transfer(EP0IN, EP0FIFO, stringProduct, nBytesToSend);
					#ifdef USBDEBUG
					printf("Tx %d bytes\n", nBytesToSend);
					#endif
				}
				/* String serial */
				else if(packetPtr[2] == 0x03){
					uint8_t nBytesToSend = min(packetPtr[6], SIZ_STRING_SERIAL);
					USB_IN_transfer(EP0IN, EP0FIFO, stringSerial, nBytesToSend);
					#ifdef USBDEBUG
					printf("Tx %d bytes\n", nBytesToSend);
					#endif
				}
				/* String interface */
				else if(packetPtr[2] == 0x04){
					uint8_t nBytesToSend = min(packetPtr[6], SIZ_STRING_INTERFACE);
					USB_IN_transfer(EP0IN, EP0FIFO, stringInterface, nBytesToSend);
					#ifdef USBDEBUG
					printf("Tx %d bytes\n", nBytesToSend);
					#endif
				}
				else{
					printf("Req for undef str desc\n");
				}
			}
			/* Get Descriptor, Device Qualifier */
			else if(packetPtr[1] == 0x06 && packetPtr[3] == 0x06){
				uint8_t nBytesToSend = min(packetPtr[6], SIZ_DEV_QUALIFIER);
				USB_IN_transfer(EP0IN, EP0FIFO, devQualifier, nBytesToSend);
				#ifdef USBDEBUG
				printf("Tx %d bytes\n", nBytesToSend);
				#endif
			}
			/* Clear Feature */
			else if(packetPtr[1] == 0x01 && packetPtr[3] == 0x00 && packetPtr[5] == 0x01){
				EP1->DIEPCTL &= ~USB_OTG_DIEPCTL_STALL;									/* clear the STALL of bulk IN EP */
				EP1->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;							/* reset data toggle */
				#ifdef USBDEBUG
				printf("STALL clred\n");
				#endif
			}
			/* Set Configuration */
			else if(packetPtr[1] == 0x09){
				if(packetPtr[2] == 0x01){
					USB_config_bulkEPs();
					USB_IN_transfer(EP0IN, EP0FIFO, NULL, 0);
					#ifdef USBDEBUG
					printf("Tx ZLP\n");
					#endif
				}
				else{
					printf("Invalid config set\n");
				}
			}
			/* Set Interface */
			else if(packetPtr[1] == 0x0B){
				if(packetPtr[4] == 0x00){
					USB_config_bulkEPs();
					USB_IN_transfer(EP0IN, EP0FIFO, NULL, 0);
					#ifdef USBDEBUG
					printf("Tx ZLP\n");
					#endif
				}
				else{
					printf("Invalid intrf set\n");
				}
			}
			/* Class request, Bulk-Only Mass Storage Reset */
			else if(packetPtr[0] == 0x21 && packetPtr[1] == 0xFF){
				/* Do nothing */
			}
			/* Class request, Get Max LUN */
			else if(packetPtr[0] == 0xA1 && packetPtr[1] == 0xFE){
				uint8_t nBytesToSend = min(packetPtr[6], 1);
				uint8_t tempBuf[1] = {0x00};
				USB_IN_transfer(EP0IN, EP0FIFO, tempBuf, nBytesToSend);
				#ifdef USBDEBUG
				printf("Tx %d bytes\n", nBytesToSend);
				#endif
			}
			else{
				printf("Unknown SETUP pkt\n");
			}

			EP0OUT->DOEPCTL |= USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA;			/* clear NAK bit and re-enable EP, as it is changed by OTG core after SETUP rx */
		}

		/* Bulk EP2 (OUT) data */
		else if(dstEP == 2){
			if(bytecnt){
				uint8_t *respDataPtr = (uint8_t *)USB_DATA_BUFFER;
				int32_t respLen;
				uint8_t stallReqd;

				for(uint32_t i = 0; i < (bytecnt + 3)/4; i++)
					usbRxPacket[i] = *EP2FIFO;

				#ifdef USBDEBUG
				for(uint32_t i = 0; i < bytecnt; i++)
					printf("%02X ", packetPtr[i]);
				printf("\n");
				#endif

				respLen = BOT_process(packetPtr, bytecnt, &stallReqd);

				if(respLen > CSW_SIZE){
					USB_IN_transfer(EP1, EP1FIFO, respDataPtr, respLen - CSW_SIZE);					/* send response data to host */
					#ifdef USBDEBUG
					printf("Tx %lu bytes\n", respLen - CSW_SIZE);
					#endif
				}

				if(respLen > 0){
					USB_IN_transfer(EP1, EP1FIFO, respDataPtr + respLen - CSW_SIZE, CSW_SIZE);		/* send CSW to host */
					#ifdef USBDEBUG
					printf("Tx CSW\n");
					#endif
					BOT_setCmdDone();
				}
			}
			else if(packettype == 0x02){
				#ifdef USBDEBUG
				printf("Rx EP2 ZLP\n");
				#endif
			}

			EP2->DOEPCTL |= USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA;
		}

		/* Could be transfer complete or NAK interrupts */
		else{
			EP0OUT->DOEPCTL |= USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA;
			EP2->DOEPCTL |= USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA;
		}
	}

	/* Other */
	else{
		USBFSG->GINTSTS = 0xFFFFFFFF;
	}
}

/**
  * @brief  IN control/bulk transfer in polling mode.
  * @param  ep: EP handle
  * @param  txfifo: pointer to tx fifo for the EP
  * @param  buf: data bytes to be sent
  * @param  len: length of buf in bytes
  * @retval success = 0, fail = 1
  */
static uint8_t USB_IN_transfer(volatile USB_OTG_INEndpointTypeDef* ep, volatile uint32_t* txfifo, const uint8_t buf[], uint32_t len)
{
	uint32_t txfrsz = len;
	uint32_t pktcnt = (len + MAX_PACKET_SZ - 1)/MAX_PACKET_SZ;
	uint8_t splen = len % MAX_PACKET_SZ;
	uint32_t i, j;

	if(ep == EP0IN){
		ep->DIEPCTL &= ~USB_OTG_DIEPCTL_SD0PID_SEVNFRM;						/* force DATA1 PID for control transfers */
	}

	if(len == 0){															/* ZLP */
		MODIFY_REG(ep->DIEPTSIZ, USB_OTG_DIEPTSIZ_XFRSIZ | USB_OTG_DIEPTSIZ_PKTCNT, 0 << USB_OTG_DIEPTSIZ_XFRSIZ_Pos |
				1 << USB_OTG_DIEPTSIZ_PKTCNT_Pos);
		ep->DIEPCTL |= USB_OTG_DIEPCTL_EPENA | USB_OTG_DIEPCTL_CNAK;

		if(setAddrFlag){
			MODIFY_REG(USBFSD->DCFG, USB_OTG_DCFG_DAD, devAddr << USB_OTG_DCFG_DAD_Pos);
			setAddrFlag = 0;
			//printf("Addr = 0x%02X\n", (USBFSD->DCFG & USB_OTG_DCFG_DAD) >> USB_OTG_DCFG_DAD_Pos);
		}

		return 0;
	}
	else{
		MODIFY_REG(ep->DIEPTSIZ, USB_OTG_DIEPTSIZ_XFRSIZ | USB_OTG_DIEPTSIZ_PKTCNT, txfrsz << USB_OTG_DIEPTSIZ_XFRSIZ_Pos |
				pktcnt << USB_OTG_DIEPTSIZ_PKTCNT_Pos);
		ep->DIEPCTL |= USB_OTG_DIEPCTL_EPENA | USB_OTG_DIEPCTL_CNAK;
	}

	for(i = 0; i < pktcnt; i++){
		/* wait for TX FIFO to become empty */
		while(!(ep->DIEPINT & USB_OTG_DIEPINT_TXFE));

		/* the last packet is a short packet */
		if(i == pktcnt - 1 && splen){
			for(j = 0; j < (splen + 3)/4; j++){
				if(j == (splen + 3)/4 - 1 && splen % 4){
					uint8_t templen = splen % 4;
					uint32_t temp = 0;

					if(templen == 1) temp |= *((uint8_t *)buf + i*MAX_PACKET_SZ + 4*j);
					else if(templen == 2) temp |= *((uint16_t *)buf + i*MAX_PACKET_SZ/2 + 2*j);
					else{
						temp |= *((uint16_t *)buf + i*MAX_PACKET_SZ/2 + 2*j);
						temp |= *((uint8_t *)buf + i*MAX_PACKET_SZ + 4*j + 2) << 16;
					}

					*txfifo = temp;
					//printf("%08X\n", temp);
				}
				else{
					*txfifo = *((uint32_t *)buf + i*(MAX_PACKET_SZ/4) + j);
					//printf("%08X\n", *((uint32_t *)buf + i*(MAX_PACKET_SZ/4) + j));
				}
			}
		}
		/* max-size packet */
		else{
			for(j = 0; j < MAX_PACKET_SZ/4; j++){
				*txfifo = *((uint32_t *)buf + i*(MAX_PACKET_SZ/4) + j);
				//printf("%08X\n", *((uint32_t *)buf + i*(MAX_PACKET_SZ/4) + j));
			}
		}
	}

	ep->DIEPINT |= USB_OTG_DIEPINT_XFRC;
	while(!(ep->DIEPINT & USB_OTG_DIEPINT_XFRC));								/* wait for transfer complete and clear flag */
	ep->DIEPINT |= USB_OTG_DIEPINT_XFRC;

	MODIFY_REG(USBFSG->GRSTCTL, USB_OTG_GRSTCTL_TXFNUM, 0x10 << USB_OTG_GRSTCTL_TXFNUM_Pos);
	USBFSG->GRSTCTL |= USB_OTG_GRSTCTL_TXFFLSH;									/* flush all TX FIFOs */
	while(USBFSG->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH);

	return 0;
}

/**
  * @brief  Handle USB errors.
  * @param  None
  * @retval None
  */
static void USB_handle_error(void)
{
	while(1);	/* hang firmware */
}
