/**
  ******************************************************************************
  * @file    main.c
  * @author  anirudhr
  * @brief   Main application file.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define max(a, b)	((a) > (b) ? a : b)


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	uint32_t QE_Count = 0, QE_Count_prev = 1;
	uint8_t QE_direc = 0;
	__IO uint16_t (*pFrame)[LCD_WIDTH];
	int32_t trigPt = -1, waveIdxStart, dispIdxStart, i, j, temp;

	/* Enable the CPU Cache */
	CPU_CACHE_Enable();
	/* Configure the system clock to 200 MHz */
	SystemClock_Config();

	/* Initialize peripherals */
	SDRAM_init();
	LTDC_init();
	DMA2D_init();
	Timers_init();
	LEDs_Buttons_init();
	I2C_TS_init();

	/* clear the screen */
	clearScreen();

	/* Init µGUI */
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

	initFields();

	/* Initialize measurement module */
	measure_init();

	/* Initialize and start data capture */
	ADC_init();

	/* Main loop */
	while(1)
	{
		/* check for trigger and display waveform */
		if(CH1_acq_comp && CH2_acq_comp){
			CH1_acq_comp = CH2_acq_comp = 0;

			trigPt = processTriggers();

			if(trigPt == -1){
				UG_TextboxSetBackColor(&window_1, TXB_ID_6, RUNSTOP_ICON_COLOR_TRGWT);
			}

			/* Draw waveforms */
			if((trigmodeVals[trigmode] == TRIGMODE_AUTO && runstopVals[runstop] != RUNSTOP_STOP) ||
					(trigmodeVals[trigmode] == TRIGMODE_SNGL && trigPt != -1 && runstopVals[runstop] != RUNSTOP_STOP) ||
					(trigmodeVals[trigmode] == TRIGMODE_NORM && trigPt != -1 && runstopVals[runstop] != RUNSTOP_STOP)){
				/* Clear the wave draw buffer */
				fillScreenWave(C_BLACK);
				while(DMA2D->CR & 0x01);

				if(trigPt < ADC_PRETRIGBUF_SIZE){
					waveIdxStart = (trigmodeVals[trigmode] == TRIGMODE_AUTO && trigPt == -1) ? 0 : max(0, trigPt - toff);	/* display waveform sample index start */
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

				pFrame = (__IO uint16_t (*)[LCD_WIDTH])LCD_DRAW_BUFFER_WAVE;
				for(j = 0; j < LCD_WIDTH; j++){
					/* CH1 */
					temp = (float32_t)(voff1 + CH1_ADC_vals[waveIdxStart+j])/vscaleVals[vscale1];
					if(temp > 119)	temp = 119;
					else if(temp < 0)  temp = 0;
					i = 134 - temp;
					if(dispIdxStart+j < LCD_WIDTH)
						pFrame[i][dispIdxStart+j] = CH1_COLOR;

					/* CH2 */
					temp = (float32_t)(voff2 + CH2_ADC_vals[waveIdxStart+j])/vscaleVals[vscale2];
					if(temp > 119)	temp = 119;
					else if(temp < 0)  temp = 0;
					i = 254 - temp;
					if(dispIdxStart+j < LCD_WIDTH)
						pFrame[i][dispIdxStart+j] = CH2_COLOR;
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
		if(Meas_pending){
			DisplayMeasurements();
			Meas_pending = 0;
		}

		/* Interrupt from quadrature encoder push button switch */
		if(QE_PB_interrupt){
			switchNextField();				/* Switch to the next field in the menubars */
			QE_PB_interrupt = 0;
		}

		/* Check quadrature encoder state */
		QE_Count = (TIM_QE->CNT & 0xFFFFU) >> 1U;
		if(QE_Count != QE_Count_prev){
			QE_direc = TIM_QE->CR1 & 0x10;		/* check rotation direction */

			changeFieldValue(QE_direc);

			QE_Count_prev = QE_Count;
		}

		UG_Update();

		/* Update screen */
		updateToScreen();
		while(DMA2D->CR & 0x01);
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

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
