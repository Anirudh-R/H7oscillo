/**
  ******************************************************************************
  * @file    main.c
  * @author  anirudhr
  * @brief   Main application file.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

char buf1[11], buf2[11], buf3[11], buf4[11];

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	uint32_t QE_Count = 0, QE_Count_prev = 1;
	__IO uint16_t (*pFrame)[LCD_WIDTH];
	uint8_t currWind = WINDOW1;
	uint32_t i, j;

	/* Enable the CPU Cache */
	CPU_CACHE_Enable();
	/* Configure the system clock to 200 MHz */
	SystemClock_Config();

	/* Initialize peripherals */
	SDRAM_init();
	LTDC_init();
	DMA2D_init();
	ADC_init();
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

	/* Initialize measurement module */
	measure_init();

	/* Main loop */
	while(1)
	{
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
			if(currWind == WINDOW1){
				UG_WindowShow(&window_2);	/* bottom menubar */
				currWind = WINDOW2;
			}
			else if(currWind == WINDOW2){
				if(showWindow3){
					UG_WindowShow(&window_3);	/* menu page 1 */
					currWind = WINDOW3;
				}
				else if(showWindow4){			/* menu page 2 */
					UG_WindowShow(&window_4);
					currWind = WINDOW4;
				}
				else{
					UG_WindowShow(&window_1);
					currWind = WINDOW1;
				}
			}
			else if((currWind == WINDOW3) && showWindow5){
				UG_WindowShow(&window_5);
				currWind = WINDOW5;
			}
			else{
				UG_WindowShow(&window_1);	/* top menubar */
				currWind = WINDOW1;
			}

			/* Display measurements */
			if(measure1.param != MEAS_NONE){
				if(measure1.param == MEAS_FREQ){
					strcpy(buf1, "F:");
					hertzToStr(calcMeasure(measure1.src, MEAS_FREQ) * 48000, buf1 + 2);
				}
				else if(measure1.param == MEAS_DUTY){
					strcpy(buf1, measParamTexts[measure1.param]);
					strcat(buf1, ":");
					itoa(calcMeasure(measure1.src, measure1.param), buf1 + 5, 10);
					strcat(buf1, "%");
				}
				else{
					strcpy(buf1, measParamTexts[measure1.param]);
					strcat(buf1, ":");
					voltsToStr(calcMeasure(measure1.src, measure1.param), buf1 + 5);
				}
				UG_TextboxSetBackColor(&window_2, TXB_ID_3, (measure1.src == CHANNEL1) ? CH1_COLOR : CH2_COLOR);
				UG_TextboxSetText(&window_2, TXB_ID_3, buf1);
			}
			else{
				UG_TextboxSetBackColor(&window_2, TXB_ID_3, INACTIVE_ICON_COLOR);
				UG_TextboxSetText(&window_2, TXB_ID_3, "--");
			}

			if(measure2.param != MEAS_NONE){
				strcpy(buf2, measParamTexts[measure2.param]);
				strcat(buf2, ":");
				voltsToStr(calcMeasure(measure2.src, measure2.param), buf2 + 5);
				UG_TextboxSetBackColor(&window_2, TXB_ID_4, (measure2.src == CHANNEL1) ? CH1_COLOR : CH2_COLOR);
				UG_TextboxSetText(&window_2, TXB_ID_4, buf2);
			}
			else{
				UG_TextboxSetBackColor(&window_2, TXB_ID_4, INACTIVE_ICON_COLOR);
				UG_TextboxSetText(&window_2, TXB_ID_4, "--");
			}

			if(measure3.param != MEAS_NONE){
				strcpy(buf3, measParamTexts[measure3.param]);
				strcat(buf3, ":");
				voltsToStr(calcMeasure(measure3.src, measure3.param), buf3 + 5);
				UG_TextboxSetBackColor(&window_2, TXB_ID_5, (measure3.src == CHANNEL1) ? CH1_COLOR : CH2_COLOR);
				UG_TextboxSetText(&window_2, TXB_ID_5, buf3);
			}
			else{
				UG_TextboxSetBackColor(&window_2, TXB_ID_5, INACTIVE_ICON_COLOR);
				UG_TextboxSetText(&window_2, TXB_ID_5, "--");
			}

			if(measure4.param != MEAS_NONE){
				strcpy(buf4, measParamTexts[measure4.param]);
				strcat(buf4, ":");
				voltsToStr(calcMeasure(measure4.src, measure4.param), buf4 + 5);
				UG_TextboxSetBackColor(&window_2, TXB_ID_6, (measure4.src == CHANNEL1) ? CH1_COLOR : CH2_COLOR);
				UG_TextboxSetText(&window_2, TXB_ID_6, buf4);
			}
			else{
				UG_TextboxSetBackColor(&window_2, TXB_ID_6, INACTIVE_ICON_COLOR);
				UG_TextboxSetText(&window_2, TXB_ID_6, "--");
			}

			TS_read_pending = 0;
		}

		/* Check quadrature encoder state */
		QE_Count = (TIM_QE->CNT & 0xFFFFU) >> 1U;
		if(QE_Count != QE_Count_prev){
			tscale += (TIM_QE->CR1 & 0x10)?(tscale==0?0:-1):(tscale==6?0:1);	/* check rotation direction */
			TimADC_init(tscale);
			QE_Count_prev = QE_Count;
		}

		/* clear the wave draw buffer */
		fillScreenWave(C_BLACK);
		while(DMA2D->CR & 0x01);

		/* Draw waveforms */
		pFrame = (__IO uint16_t (*)[LCD_WIDTH])LCD_DRAW_BUFFER_WAVE;
		for(j = 0; j < LCD_WIDTH; j++){
			/* ADC CH1 */
			i = (63 + 64/vscale) - (CH1_ADC_vals[j]/(2*vscale));
			pFrame[i][j] = CH1_COLOR;
			/* ADC CH2 */
			i = (191 + 64/vscale) - (CH2_ADC_vals[j]/(2*vscale));
			pFrame[i][j] = CH2_COLOR;
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
