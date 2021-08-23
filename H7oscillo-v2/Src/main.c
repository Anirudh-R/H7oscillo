/**
  ******************************************************************************
  * @file    main.c
  * @author  anirudhr
  * @brief   Main application file.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	FRESULT fsres;
	uint8_t fileStoreDone = 0, fileReadDone = 0;
	uint8_t skip = 0;

	uint32_t QE_Count = 0, QE_Count_prev = 1;
	uint8_t QE_direc = 0;

	__IO uint16_t (*pFrame)[LCD_WIDTH] = (__IO uint16_t (*)[LCD_WIDTH])LCD_DRAW_BUFFER_WAVE;

	int32_t trigPt = -1, waveIdxStart = 0, dispIdxStart = 0, lenResampledSig = -1;
	uint8_t origtscale = 0, oldtscale = 0;
	int32_t	i, j, temp;


	/* Enable the CPU Cache */
	CPU_CACHE_Enable();
	/* Configure the system clock to 200 MHz */
	SystemClock_Config();
	/* Configure MPU */
	MPU_Config();

	/* Initialize peripherals */
	SDRAM_init();
	LTDC_init();
	DMA2D_init();
	Timers_init();
	LEDs_Buttons_init();
	I2C_TS_init();

	/* clear the screen */
	clearScreen();

	/* Initialize uGUI */
	UG_Init(&gui, (void(*)(UG_S16,UG_S16,UG_COLOR))pset, LCD_WIDTH, LCD_HEIGHT);

	/* Hardware-accelerated graphics functions */
	UG_DriverRegister(DRIVER_FILL_FRAME, (void*)fillFrameUGUI);
	UG_DriverEnable(DRIVER_FILL_FRAME);
	UG_DriverRegister(DRIVER_DRAW_LINE, (void*)drawLineUGUI);
	UG_DriverEnable(DRIVER_DRAW_LINE);

	/* clear uGUI layer */
	UG_FillScreen(C_BLACK);

	/* Create windows and other GUI elements */
	initUI();

	/* Display initial field values */
	initFields();

	/* Initialize measurement module */
	measure_init();

	/* Initialize QSPI peripheral and the flash memory */
	QSPI_init();
	QSPI_flash_init();

	/* if user button is pushed during reset, format the drive with FAT */
	if(readUserBtnState()){
		uint8_t* workBuf = (uint8_t *)SCRATCH_BUFFER;		/* use SDRAM as working buffer for format process */

		LL_GPIO_SetOutputPin(LED1_GPIO_PORT, LED1_PIN);

		fsres = f_mkfs("", &mkfsParam, workBuf, SECTOR_SIZE);
		if(fsres == FR_OK){	/* success */
			printf("Successfully formatted flash.\n");
			LL_GPIO_ResetOutputPin(LED1_GPIO_PORT, LED1_PIN);
		}
		else{				/* hang */
			printf("Flash formatting failed, error: %d.\n", fsres);
			hangFirmware();
		}
	}

	/* force mount the FAT volume */
	fsres = f_mount(&fatFs, "", 1);
	if(fsres != FR_OK){
		printf("FAT FS mounting failed, error: %d\n", fsres);
		hangFirmware();
	}

	/* Initialize and start data capture */
	ADC_init();

	/* Main loop */
	while(1)
	{
		/* check for trigger and display waveforms */
		if(CH1_acq_comp && CH2_acq_comp){
			CH1_acq_comp = CH2_acq_comp = 0;

			trigPt = processTriggers();

			/* trigger condition not met */
			if(trigPt == -1 && runstopVals[runstop] != RUNSTOP_STOP){
				UG_TextboxSetBackColor(&window_1, TXB_ID_6, RUNSTOP_ICON_COLOR_TRGWT);
			}

			/* Draw waveforms/spectrum */
			if((trigmodeVals[trigmode] == TRIGMODE_AUTO && runstopVals[runstop] != RUNSTOP_STOP) ||
					(trigmodeVals[trigmode] == TRIGMODE_SNGL && trigPt != -1 && runstopVals[runstop] != RUNSTOP_STOP) ||
					(trigmodeVals[trigmode] == TRIGMODE_NORM && trigPt != -1 && runstopVals[runstop] != RUNSTOP_STOP))
			{
				if(chDispMode != CHDISPMODE_FFT){
					fillScreenWave(C_BLACK);	/* clear the wave draw buffer */

					origtscale = tscale;		/* store the original time scale of waveform */

					if(trigPt < ADC_PRETRIGBUF_SIZE){
						waveIdxStart = (trigmodeVals[trigmode] == TRIGMODE_AUTO && trigPt == -1) ? 0 : max(0, trigPt - toff);	/* display-waveform sample index start */
						dispIdxStart = (trigmodeVals[trigmode] == TRIGMODE_AUTO && trigPt == -1) ? 0 : max(0, toff - trigPt);	/* display offset to start drawing the waveform */
					}
					else if(trigPt >= ADC_PRETRIGBUF_SIZE && trigPt < ADC_PRETRIGBUF_SIZE + ADC_TRIGBUF_SIZE){
						waveIdxStart = (trigmodeVals[trigmode] == TRIGMODE_AUTO && trigPt == -1) ? ADC_PRETRIGBUF_SIZE : trigPt - toff;
						dispIdxStart = 0;
					}
					else{
						waveIdxStart = (trigmodeVals[trigmode] == TRIGMODE_AUTO && trigPt == -1) ? ADC_PRETRIGBUF_SIZE + ADC_TRIGBUF_SIZE - TOFF_LIMIT : trigPt - toff;
						dispIdxStart = 0;
					}

					for(j = 0; j < LCD_WIDTH - waveIdxStart; j++){
						/* CH1 */
						temp = (float32_t)(voff1 + CH1_ADC_vals[waveIdxStart+j])/vscaleVals[vscale1];
						if(chDispMode == CHDISPMODE_SPLIT){
							if(temp > CHDISPMODE_SPLIT_SIGMAX)	temp = CHDISPMODE_SPLIT_SIGMAX;
						}
						else{
							if(temp > CHDISPMODE_MERGE_SIGMAX)	temp = CHDISPMODE_MERGE_SIGMAX;
						}
						if(temp < 0)  temp = 0;
						if(chDispMode == CHDISPMODE_SPLIT)
							i = CHDISPMODE_SPLIT_CH1BOT - temp;
						else
							i = CHDISPMODE_MERGE_CHBOT - temp;
						if(dispIdxStart+j < LCD_WIDTH)
							pFrame[i][dispIdxStart+j] = CH1_COLOR;

						/* CH2 */
						if(chDispMode != CHDISPMODE_SNGL){
							temp = (float32_t)(voff2 + CH2_ADC_vals[waveIdxStart+j])/vscaleVals[vscale2];
							if(chDispMode == CHDISPMODE_SPLIT){
								if(temp > CHDISPMODE_SPLIT_SIGMAX)	temp = CHDISPMODE_SPLIT_SIGMAX;
							}
							else{
								if(temp > CHDISPMODE_MERGE_SIGMAX)	temp = CHDISPMODE_MERGE_SIGMAX;
							}
							if(temp < 0)  temp = 0;
							if(chDispMode == CHDISPMODE_SPLIT)
								i = CHDISPMODE_SPLIT_CH2BOT - temp;
							else
								i = CHDISPMODE_MERGE_CHBOT - temp;
							if(dispIdxStart+j < LCD_WIDTH)
								pFrame[i][dispIdxStart+j] = CH2_COLOR;
						}

						/* Math waveform */
						if(mathOp != MATH_OP_NONE){
							if(mathOp == MATH_OP_1P2)
								temp = (mathVoff + 0.5f*((float32_t)CH1_ADC_vals[waveIdxStart+j] + CH2_ADC_vals[waveIdxStart+j]))/vscaleVals[mathVscale];
							else if(mathOp == MATH_OP_1M2)
								temp = (mathVoff + (float32_t)CH1_ADC_vals[waveIdxStart+j] - CH2_ADC_vals[waveIdxStart+j])/vscaleVals[mathVscale];
							else if(mathOp == MATH_OP_2M1)
								temp = (mathVoff + (float32_t)CH2_ADC_vals[waveIdxStart+j] - CH1_ADC_vals[waveIdxStart+j])/vscaleVals[mathVscale];
							else
								temp = (mathVoff + ((float32_t)CH1_ADC_vals[waveIdxStart+j]*CH2_ADC_vals[waveIdxStart+j])/256.0f)/vscaleVals[mathVscale];

							if(temp > CHDISPMODE_MERGE_SIGMAX)	temp = CHDISPMODE_MERGE_SIGMAX;
							if(temp < 0)  temp = 0;
							i = CHDISPMODE_MERGE_CHBOT - temp;
							if(dispIdxStart+j < LCD_WIDTH)
								pFrame[i][dispIdxStart+j] = MATH_COLOR;
						}
					}
				}
				/* Draw spectrum */
				else{
					if(measPending){
						fillScreenWave(C_BLACK);			/* clear the wave draw buffer */

						calcSpectrum(fftSrcChannel);

						for(j = 0; j < LCD_WIDTH; j++){
							temp = chSpectrum[j];
							if(temp > CHDISPMODE_MERGE_SIGMAX)	temp = CHDISPMODE_MERGE_SIGMAX;

							for(int32_t k = 0; k <= temp; k++){
								i = CHDISPMODE_MERGE_CHBOT - k;
								pFrame[i][j] = FFT_COLOR;
							}
						}
					}
				}

				/* go to STOP mode after a single mode trigger event */
				if(trigmodeVals[trigmode] == TRIGMODE_SNGL){
					trigPt = -1;
					goToField(FLD_RUNSTOP);
					changeFieldValue(0);	/* STOP */
				}
				else if(trigPt != -1){
					UG_TextboxSetBackColor(&window_1, TXB_ID_6, RUNSTOP_ICON_COLOR_RUN);
				}
			}

			/* capture next waveform frame */
			if((trigmodeVals[trigmode] != TRIGMODE_SNGL || trigPt == -1) && runstopVals[runstop] != RUNSTOP_STOP){
				ADC_Ch1_reinit();
				ADC_Ch2_reinit();
			}
		}

		/* zoom-in/out the captured waveform (termed static mode) */
		if(runstopVals[runstop] == RUNSTOP_STOP){
			int32_t redrawWf = 0;

			/* not in static mode */
			if(!staticMode){
				/* a touch to the center of the screen starts static mode */
				if(TS_DetectNumTouches() > 0){
					TS_GetXY(&TS_Y, &TS_X);
					if(chDispMode != CHDISPMODE_FFT && mathOp == MATH_OP_NONE && 215 < TS_X && TS_X < 265 && 111 < TS_Y && TS_Y < 161){
						staticMode = 1;
						oldtscale = origtscale;
						toffStm = toff;
						drawRedBorder();					/* to indicate static mode */
						lenResampledSig = resampleChannels(waveIdxStart, LCD_WIDTH - abs(dispIdxStart - waveIdxStart), origtscale, origtscale);
						while(TS_DetectNumTouches() > 0);	/* wait for touch to be removed */
					}
				}
			}
			else{
				/* a second touch to the center of the screen exits static mode */
				if(TS_DetectNumTouches() > 0){
					TS_GetXY(&TS_Y, &TS_X);
					if(215 < TS_X && TS_X < 265 && 111 < TS_Y && TS_Y < 161){
						staticMode = 0;
						clearRedBorder();
						drawGrid();
						dispToff();		/* re-display toff in case it was changed out of limits in static mode */
						goToField(FLD_RUNSTOP);
						changeFieldValue(1);	/* RUN */
						while(TS_DetectNumTouches() > 0);
					}
				}
			}

			if(staticMode){
				/* sample frequency changed, resample the captured waveform */
				if(tscale != oldtscale){
					lenResampledSig = resampleChannels(waveIdxStart, LCD_WIDTH - abs(dispIdxStart - waveIdxStart), origtscale, tscale);

					/* given samplerate conversion not possible, revert tscale */
					if(lenResampledSig == -1){
						goToField(FLD_TSCALE);

						if(tscale > oldtscale)
							changeFieldValue(1);
						else
							changeFieldValue(0);
					}
					else{
						oldtscale = tscale;
						toffStm = toff;
						redrawWf = 1;		/* redraw the waveforms */
					}
				}
				else if(vscale1Changed || vscale2Changed || voff1Changed
						|| voff2Changed || toffChanged){
					drawRedBorder();	/* since it would have got erased due to moving cursors */
					redrawWf = 1;
				}

				if(redrawWf){
					int32_t trigPtStm, waveIdxStartStm, dispIdxStartStm;

					trigPtStm = chkTrigResampSig(lenResampledSig);

					waveIdxStartStm = max(0, min(trigPtStm - toffStm, lenResampledSig - 1));
					dispIdxStartStm = max(0, min(toffStm - trigPtStm, LCD_WIDTH - 1));

					/* limit toffStm value so that waveform doesn't go out of screen */
					if(trigPtStm - toffStm > lenResampledSig - 1)
						toffStm = trigPtStm - lenResampledSig + 1;
					else if(toffStm - trigPtStm > LCD_WIDTH - 1)
						toffStm = trigPtStm + LCD_WIDTH - 1;

					fillScreenWave(C_BLACK);				/* clear the wave draw buffer */

					/* Draw the resampled signal */
					for(j = 0; j < LCD_WIDTH && waveIdxStartStm+j < lenResampledSig; j++){
						/* CH1 */
						temp = (float32_t)(voff1 + CH1_ResampledVals[waveIdxStartStm+j])/vscaleVals[vscale1];
						if(chDispMode == CHDISPMODE_SPLIT){
							if(temp > CHDISPMODE_SPLIT_SIGMAX)	temp = CHDISPMODE_SPLIT_SIGMAX;
						}
						else{
							if(temp > CHDISPMODE_MERGE_SIGMAX)	temp = CHDISPMODE_MERGE_SIGMAX;
						}
						if(temp < 0)  temp = 0;
						if(chDispMode == CHDISPMODE_SPLIT)
							i = CHDISPMODE_SPLIT_CH1BOT - temp;
						else
							i = CHDISPMODE_MERGE_CHBOT - temp;
						if(dispIdxStartStm+j < LCD_WIDTH)
							pFrame[i][dispIdxStartStm+j] = CH1_COLOR;

						/* CH2 */
						if(chDispMode != CHDISPMODE_SNGL){
							temp = (float32_t)(voff2 + CH2_ResampledVals[waveIdxStartStm+j])/vscaleVals[vscale2];
							if(chDispMode == CHDISPMODE_SPLIT){
								if(temp > CHDISPMODE_SPLIT_SIGMAX)	temp = CHDISPMODE_SPLIT_SIGMAX;
							}
							else{
								if(temp > CHDISPMODE_MERGE_SIGMAX)	temp = CHDISPMODE_MERGE_SIGMAX;
							}
							if(temp < 0)  temp = 0;
							if(chDispMode == CHDISPMODE_SPLIT)
								i = CHDISPMODE_SPLIT_CH2BOT - temp;
							else
								i = CHDISPMODE_MERGE_CHBOT - temp;
							if(dispIdxStartStm+j < LCD_WIDTH)
								pFrame[i][dispIdxStartStm+j] = CH2_COLOR;
						}
					}

					vscale1Changed = vscale2Changed = voff1Changed = voff2Changed = toffChanged = 0;
					redrawWf = 0;
				}
			}
		}

		/* Read touch and display windows */
		if(TS_read_pending){
			/* Get touch inputs */
			if(TS_DetectNumTouches() > 0){
				/* currently, only single touch supported */
				TS_GetXY(&TS_Y, &TS_X);		/* X & Y coords are swapped and passed, since TS physical orientation is reverse */
				UG_TouchUpdate(TS_X, TS_Y, TOUCH_STATE_PRESSED);
			}
			else{
				UG_TouchUpdate(-1, -1, TOUCH_STATE_RELEASED);
			}

			/* uGUI can show only one window at a time. Switch between them to display multiple at the same time. */
			switchNextWindow();

			TS_read_pending = 0;
		}

		/* Calculate and display the selected measurements */
		if(measPending){
			DisplayMeasurements();
			measPending = 0;
		}

		/* Interrupt from quadrature encoder push button switch */
		if(QE_PB_interrupt){
			switchNextField();				/* Switch to the next field in the menubars */
			QE_PB_interrupt = 0;
		}

		/* Check quadrature encoder state */
		QE_Count = (TIM_QE->CNT & 0xFFFFU) >> 1U;
		if(QE_Count != QE_Count_prev){
			QE_direc = (TIM_QE->CR1 & 0x10) >> 4;	/* check rotation direction */

			if(getCurrMathField() == MATHFLD_NONE && getCurrCursorField() == CURSORFLD_NONE)
				changeFieldValue(QE_direc);
			else if(getCurrCursorField() == CURSORFLD_NONE)
				changeFieldValueMath(QE_direc);
			else
				changeFieldValueCursor(QE_direc);

			QE_Count_prev = QE_Count;
		}

		UG_Update();

		/* Update screen */
		updateToScreen();

		/* file operations */
		if(!fileStoreDone && skip++ == 5){
			UINT bw;

			f_open(&fp, "screen.img", FA_CREATE_ALWAYS | FA_WRITE);
			fsres = f_write(&fp, (const void *)LCD_FRAME_BUFFER, 130560, &bw);
			f_close(&fp);
			if(bw != 130560){
				printf("File write error: %d.\n", fsres);
				hangFirmware();
			}

			printf("Stored image\n");
			LL_mDelay(3000);

			clearScreen();			/* clear screen */

			printf("Cleared screen\n");
			LL_mDelay(3000);

			fileStoreDone = 1;
		}
		else if(fileStoreDone && !fileReadDone){
			UINT br;

			f_open(&fp, "screen.img", FA_READ);
			fsres = f_read(&fp, (void *)LCD_FRAME_BUFFER, 130560, &br);
			f_close(&fp);

			if(br != 130560){
				printf("File read error: %d.\n", fsres);
				hangFirmware();
			}

			printf("Read image\n");

			fileReadDone = 1;
			while(1);
		}
	}
}


/**
  * @brief  Provides a tick value in milliseconds.
  * @param  None
  * @retval tick value
  */
__INLINE uint32_t Get_tick(void)
{
	return (uint32_t)Tick_1ms;
}

/**
  * @brief  Hang the firmware forever.
  * @param  None
  * @retval None
  */
void hangFirmware(void)
{
	/* Infinite loop */
	while (1){
		LL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
		LL_mDelay(100);
	}
}
