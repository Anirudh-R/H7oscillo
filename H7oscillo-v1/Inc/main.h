/**
  ******************************************************************************
  * @file    main.h
  * @author  anirudhr
  * @brief   Contains defines and includes for application.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/* LL drivers */
#include "stm32f7xx_ll_bus.h"
#include "stm32f7xx_ll_rcc.h"
#include "stm32f7xx_ll_system.h"
#include "stm32f7xx_ll_utils.h"
#include "stm32f7xx_ll_pwr.h"
#include "stm32f7xx_ll_exti.h"
#include "stm32f7xx_ll_gpio.h"
#include "stm32f7xx_ll_adc.h"
#include "stm32f7xx_ll_cortex.h"
#include "stm32f7xx_ll_dma.h"
#include "stm32f7xx_ll_tim.h"
#include "stm32f7xx_ll_i2c.h"
#include "arm_math.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

#include "inits.h"
#include "params.h"
#include "globals.h"
#include "ugui_config.h"
#include "ugui.h"
#include "touch.h"
#include "display.h"
#include "ui.h"
#include "measure.h"
#include "triggers.h"


#define max(a, b)	((a) > (b) ? (a) : (b))
#define min(a, b)	((a) < (b) ? (a) : (b))

/* LED1 */
#define LED1_GPIO_PORT                     		GPIOI
#define LED1_PIN                           		LL_GPIO_PIN_1
#define LED1_GPIO_CLK_ENABLE()             		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOI)

/* User push-button */
#define USER_BUTTON_GPIO_PORT                   GPIOI
#define USER_BUTTON_PIN                         LL_GPIO_PIN_11
#define USER_BUTTON_GPIO_CLK_ENABLE()           LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOI)
#define USER_BUTTON_EXTI_LINE                   LL_EXTI_LINE_11
#define USER_BUTTON_EXTI_IRQn                   EXTI15_10_IRQn
#define User_Button_IRQHandler                  EXTI15_10_IRQHandler
#define USER_BUTTON_EXTI_LINE_ENABLE()          LL_EXTI_EnableIT_0_31(USER_BUTTON_EXTI_LINE)
#define USER_BUTTON_EXTI_FALLING_TRIG_ENABLE()  LL_EXTI_EnableFallingTrig_0_31(USER_BUTTON_EXTI_LINE)
#define USER_BUTTON_SYSCFG_SET_EXTI()           do {                                                                   \
                                                  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);                  \
                                                  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTI, LL_SYSCFG_EXTI_LINE11);  \
                                                } while(0)

/* Quadrature encoder */
#define TIM_QE                         			TIM8
#define TIM_QE_CLK_ENABLE()            			LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM8)
#define TIM_QE_GPIO_CLK_ENABLE()	   			LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC)
#define TIM_QE_INP_PORT				   			GPIOC
#define TIM_QE_INP1_PIN				   			LL_GPIO_PIN_6
#define TIM_QE_INP2_PIN				   			LL_GPIO_PIN_7
/* Push button of quadrature encoder module */
#define QE_PB_GPIO_PORT					   		GPIOG
#define QE_PB_PIN					   			LL_GPIO_PIN_6
#define QE_PB_GPIO_CLK_ENABLE()           		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOG)
#define QE_PB_EXTI_LINE 						LL_EXTI_LINE_6
#define QE_IRQn                    	   			EXTI9_5_IRQn
#define QE_IRQHandler                  			EXTI9_5_IRQHandler
#define QE_PB_EXTI_LINE_ENABLE()      			LL_EXTI_EnableIT_0_31(QE_PB_EXTI_LINE)
#define QE_PB_EXTI_RISING_TRIG_ENABLE()  		LL_EXTI_EnableRisingTrig_0_31(QE_PB_EXTI_LINE)
#define QE_PB_SYSCFG_SET_EXTI()       			do {                                                                   \
                                              	  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);                  \
                                              	  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTG, LL_SYSCFG_EXTI_LINE6);  \
                                            	} while(0)
#define QE_PB_DEBOUNCE_TIMEOUT			   		500					/* 500ms */

/* ADC & associated DMA */
/* Channel 1 */
#define CH1_ADC                           		ADC1
#define CH1_ADC_CLK_ENABLE()              		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1)
#define CH1_ADC_DMA_CLK_ENABLE()          		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2)
#define CH1_ADC_INP_GPIO_CLK_ENABLE()    		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA)
#define CH1_ADC_INP_GPIO_PORT            		GPIOA
#define CH1_ADC_INP_PIN                  		LL_GPIO_PIN_0		/* Arduino conn A0 */
#define CH1_ADC_CHANNEL							0U
#define CH1_ADC_DMA		               			DMA2
#define CH1_ADC_DMA_STREAM                		DMA2_Stream0
#define CH1_ADC_DMA_CHANNEL               		LL_DMA_CHANNEL_0
#define CH1_ADC_DMA_IRQn                  		DMA2_Stream0_IRQn
#define CH1_ADC_DMA_IRQHandler            		DMA2_Stream0_IRQHandler
/* Channel 2 */
#define CH2_ADC                           		ADC3
#define CH2_ADC_CLK_ENABLE()              		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC3)
#define CH2_ADC_DMA_CLK_ENABLE()          		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2)
#define CH2_ADC_INP_GPIO_CLK_ENABLE()    		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF)
#define CH2_ADC_INP_GPIO_PORT            		GPIOF
#define CH2_ADC_INP_PIN                  		LL_GPIO_PIN_10		/* Arduino conn A1 */
#define CH2_ADC_CHANNEL							8U
#define CH2_ADC_DMA		               			DMA2
#define CH2_ADC_DMA_STREAM                		DMA2_Stream1
#define CH2_ADC_DMA_CHANNEL               		LL_DMA_CHANNEL_2
#define CH2_ADC_DMA_IRQn                  		DMA2_Stream1_IRQn
#define CH2_ADC_DMA_IRQHandler            		DMA2_Stream1_IRQHandler

/* Timer for ADC conversion triggering */
#define TIM_ADC                         		TIM1
#define TIM_ADC_CLK_ENABLE()            		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1)

/* Display RK043FN48H parameters */
#define RK043FN48H_WIDTH    				    ((uint16_t)480)   /* LCD PIXEL WIDTH  			*/
#define RK043FN48H_HEIGHT   					((uint16_t)272)   /* LCD PIXEL HEIGHT 		    */
#define RK043FN48H_HSYNC            			((uint16_t)41)    /* Horizontal synchronization */
#define RK043FN48H_HBP              			((uint16_t)13)    /* Horizontal back porch      */
#define RK043FN48H_HFP              			((uint16_t)32)    /* Horizontal front porch     */
#define RK043FN48H_VSYNC           				((uint16_t)10)    /* Vertical synchronization   */
#define RK043FN48H_VBP              			((uint16_t)2)     /* Vertical back porch        */
#define RK043FN48H_VFP              			((uint16_t)2)     /* Vertical front porch       */
#define	LCD_WIDTH								RK043FN48H_WIDTH
#define LCD_HEIGHT								RK043FN48H_HEIGHT

/* SDRAM */
#define SDRAM_BANK0_ADDR						((uint32_t)0xC0000000)
#define SDRAM_BANK1_ADDR						((uint32_t)0xC0200000)
#define SDRAM_BANK2_ADDR						((uint32_t)0xC0400000)
#define SDRAM_BANK3_ADDR						((uint32_t)0xC0600000)

/* LCD */
#define LCD_FRAME_BUFFER						SDRAM_BANK0_ADDR	/* Frame buffer for display */
#define LCD_DRAW_BUFFER_UGUI					SDRAM_BANK1_ADDR	/* Draw buffer for uGUI graphics (windows, texts, etc) */
#define LCD_DRAW_BUFFER_WAVE					SDRAM_BANK2_ADDR	/* Draw buffer for the waveforms */
#define UGUI_FRAME_ALPHA						0x8F				/* Blending Alpha factor for the uGUI Layer */
#define WAVE_FRAME_ALPHA						0xFF				/* Blending Alpha factor for the Wave Layer */

/* Touch screen */
#define TS_I2Cx                             	I2C3
#define TS_I2Cx_CLK_ENABLE()                	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C3)
#define TS_I2Cx_FORCE_RESET()               	LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_I2C3)
#define TS_I2Cx_RELEASE_RESET()             	LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I2C3)
#define TS_I2Cx_SCL_SDA_GPIO_CLK_ENABLE()   	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH)
#define TS_I2Cx_SCL_SDA_GPIO_PORT           	GPIOH
#define TS_I2Cx_SCL_PIN                     	LL_GPIO_PIN_7
#define TS_I2Cx_SDA_PIN                     	LL_GPIO_PIN_8
#define TS_I2Cx_SCL_SDA_AF                  	LL_GPIO_AF_4
#define TS_INT_GPIO_CLK_ENABLE()            	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOI)
#define TS_INT_GPIO_CLK_DISABLE()           	LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_GPIOI)
#define TS_INT_GPIO_PORT                    	GPIOI
#define TS_INT_PIN                          	GPIO_PIN_13
#define TS_INT_EXTI_IRQn                    	EXTI15_10_IRQn

/* Timer for Touch screen */
#define TIM_TS	                         		TIM4
#define TIM_TS_CLK_ENABLE()	            		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4)

/* Timer for Measurements */
#define TIM_MEAS	                         	TIM3
#define TIM_MEAS_CLK_ENABLE()	            	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3)

/* Global function prototypes */
extern uint32_t Get_tick(void);

#endif /* __MAIN_H */
