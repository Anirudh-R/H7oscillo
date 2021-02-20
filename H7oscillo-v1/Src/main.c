/**
  ******************************************************************************
  * @file    main.c
  * @author  anirudhr
  * @brief   Main application file.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* Function declarations */
static void window_1_callback(UG_MESSAGE* msg);


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  uint32_t QE_Count=0, QE_Count_prev=1;
  __IO uint16_t (*pFrame)[LCD_WIDTH];
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

  UG_DriverRegister(DRIVER_FILL_FRAME, (void*)fillFrameUGUI);
  UG_DriverEnable(DRIVER_FILL_FRAME);
  UG_DriverRegister(DRIVER_DRAW_LINE, (void*)drawLineUGUI);
  UG_DriverEnable(DRIVER_DRAW_LINE);

  /* Create Window 1 */
  UG_WindowCreate(&window_1, obj_buff_wnd_1, UGUI_MAX_OBJECTS, window_1_callback);
  UG_WindowSetStyle(&window_1, WND_STYLE_2D | WND_STYLE_HIDE_TITLE);
  UG_WindowResize(&window_1, LCD_WIDTH - 50, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

  /* Create some Buttons */
  UG_ButtonCreate(&window_1, &button1_0, BTN_ID_0, 5, 25, 45, 65);
  UG_ButtonCreate(&window_1, &button1_1, BTN_ID_1, 5, 70, 45, 110);
  UG_ButtonCreate(&window_1, &button1_2, BTN_ID_2, 5, 115, 45, 155);
  UG_ButtonCreate(&window_1, &button1_3, BTN_ID_3, 5, 160, 45, 200);
  UG_ButtonCreate(&window_1, &button1_4, BTN_ID_4, 5, 0, 45, 14);

  /* Configure Button 1 */
  UG_ButtonSetFont(&window_1, BTN_ID_0, &FONT_6X8);
  UG_ButtonSetBackColor(&window_1, BTN_ID_0, C_LIME);
  UG_ButtonSetText(&window_1, BTN_ID_0, "Hsc+");
  /* Configure Button 2 */
  UG_ButtonSetFont(&window_1, BTN_ID_1, &FONT_6X8);
  UG_ButtonSetBackColor(&window_1, BTN_ID_1, C_RED);
  UG_ButtonSetText(&window_1, BTN_ID_1, "Hsc-");
  /* Configure Button 3 */
  UG_ButtonSetFont(&window_1, BTN_ID_2, &FONT_6X8);
  UG_ButtonSetBackColor(&window_1, BTN_ID_2, C_YELLOW);
  UG_ButtonSetText(&window_1, BTN_ID_2, "Vsc+");
  /* Configure Button 4 */
  UG_ButtonSetFont(&window_1, BTN_ID_3, &FONT_6X8);
  UG_ButtonSetBackColor(&window_1, BTN_ID_3, C_BLUE);
  UG_ButtonSetText(&window_1, BTN_ID_3, "Vsc-");
  /* Configure Button 5 */
  UG_ButtonSetFont(&window_1, BTN_ID_4, &FONT_6X8);
  UG_ButtonSetBackColor(&window_1, BTN_ID_4, C_ORANGE);
  UG_ButtonSetText(&window_1, BTN_ID_4, "MENU");

  /* clear uGUI layer */
  UG_FillScreen(C_BLACK);

  /* Show Window 1 */
  UG_WindowShow(&window_1);

  /* minimize Window 1 */
  UG_WindowResize(&window_1, LCD_WIDTH - 50, 0, LCD_WIDTH - 1, 14);

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
	  pFrame = (uint16_t (*)[LCD_WIDTH])LCD_DRAW_BUFFER_WAVE;
	  for(j = 0; j < LCD_WIDTH; j++){
		  /* ADC CH1 */
		  i = (63 + 64/vscale) - (CH1_ADC_vals[j]/(2*vscale));
		  pFrame[i][j] = C_GREEN;
		  /* ADC CH2 */
		  i = (191 + 64/vscale) - (CH2_ADC_vals[j]/(2*vscale));
		  pFrame[i][j] = C_AQUA;
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


/* Callback function for window 1 */
static void window_1_callback(UG_MESSAGE* msg)
{
	static uint8_t wind1_state = 0;		/* window1 maximized/minimized state */

	if (msg->type == MSG_TYPE_OBJECT)
	{
	  if (msg->id == OBJ_TYPE_BUTTON)
	  {
		  if(msg->event == OBJ_EVENT_PRESSED)
		  {
			 switch(msg->sub_id)
			 {
				case BTN_ID_0:
					tscale += (tscale == 0) ? 0 : -1;	/* increase timescale */
					TimADC_init(tscale);
					break;
				case BTN_ID_1:
					tscale += (tscale == 6) ? 0 : 1;	/* decrease timescale */
					TimADC_init(tscale);
					break;
				case BTN_ID_2:
					vscale += (vscale == 1) ? 0 : -1;	/* increase vertical scale */
					break;
				case BTN_ID_3:
					vscale += (vscale == 4) ? 0 : 1;	/* decrease vertical scale */
					break;
				case BTN_ID_4:
					if(wind1_state == 0){
						UG_WindowResize(&window_1, LCD_WIDTH - 50, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);	/* maximize */
						UG_FillScreen(C_BLACK);		/* clear uGUI layer */
						wind1_state = 1;
					}
					else{
						UG_WindowResize(&window_1, LCD_WIDTH - 50, 0, LCD_WIDTH - 1, 14);	/* minimize */
						UG_FillScreen(C_BLACK);		/* clear uGUI layer */
						wind1_state = 0;
					}
					break;
			 }
		  }
	  }
	}
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
