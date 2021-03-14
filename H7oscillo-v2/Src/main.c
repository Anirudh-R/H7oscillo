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
	uint32_t QE_Count = 0, QE_Count_prev = 1;
	uint8_t QE_direc = 0;
	__IO uint16_t (*pFrame)[LCD_WIDTH];
	int32_t i, j, temp;

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
		/* Read touch, display windows and measurements */
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

			/* Display the chosen measurements */
			DisplayMeasurements();

			TS_read_pending = 0;
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

		/* Clear the wave draw buffer */
		fillScreenWave(C_BLACK);
		while(DMA2D->CR & 0x01);

		/* Draw waveforms */
		pFrame = (__IO uint16_t (*)[LCD_WIDTH])LCD_DRAW_BUFFER_WAVE;
		for(j = 0; j < LCD_WIDTH; j++){
			/* ADC CH1 */
			temp = (float32_t)(voff1 + CH1_ADC_vals[j])/vscaleVals[vscale1];
			if(temp > 119)	temp = 119;
			else if(temp < 0)  temp = 0;
			i = 134 - temp;
			pFrame[i][j] = CH1_COLOR;

			/* ADC CH2 */
			temp = (float32_t)(voff2 + CH2_ADC_vals[j])/vscaleVals[vscale2];
			if(temp > 119)	temp = 119;
			else if(temp < 0)  temp = 0;
			i = 254 - temp;
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
