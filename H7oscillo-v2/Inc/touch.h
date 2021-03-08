/**
  ******************************************************************************
  * @file    touch.h
  * @author  anirudhr
  * @brief   Header file for touch.c.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TOUCH_H
#define __TOUCH_H

/* Includes ------------------------------------------------------------------*/
#include "ft5336.h"
#include "i2c.h"
#include "stm32f7xx_ll_utils.h"


#define TS_I2Cx                     I2C3

/*
 * I2C TIMING Register define when I2C clock source is SYSCLK
 * I2C TIMING is calculated from APB1 source clock = 50 MHz
 * Due to the big MOFSET capacity for adapting the camera level the rising time is very large (>1us)
 * 0x40912732 takes in account the big rising and aims a clock of 100khz
 */
#define I2Cx_TIMING                 ((uint32_t)0x40912732)

#define TS_I2C_ADDRESS              ((uint16_t)0x70)

/* With FT5336, maximum 5 touches detected simultaneously */
#define TS_MAX_NB_TOUCH             ((uint32_t) FT5336_MAX_DETECTABLE_TOUCH)

/* Possible managed gesture identification values returned by touch screen driver IC */
#define GEST_ID_NO_GESTURE   		FT5336_GEST_ID_NO_GESTURE  	/*!< Gesture not defined / recognized */
#define GEST_ID_MOVE_UP      		FT5336_GEST_ID_MOVE_UP  	/*!< Gesture Move Up */
#define GEST_ID_MOVE_RIGHT   		FT5336_GEST_ID_MOVE_RIGHT  	/*!< Gesture Move Right */
#define GEST_ID_MOVE_DOWN    		FT5336_GEST_ID_MOVE_DOWN  	/*!< Gesture Move Down */
#define GEST_ID_MOVE_LEFT    		FT5336_GEST_ID_MOVE_LEFT  	/*!< Gesture Move Left */
#define GEST_ID_ZOOM_IN      		FT5336_GEST_ID_ZOOM_IN  	/*!< Gesture Zoom In */
#define GEST_ID_ZOOM_OUT     		FT5336_GEST_ID_ZOOM_OUT  	/*!< Gesture Zoom Out */

extern uint16_t TS_X, TS_Y;
extern __IO uint8_t TS_read_pending;

/* External function declarations */
extern void TS_init(void);
extern uint8_t TS_DetectNumTouches(void);
extern void TS_GetXY(uint16_t* X, uint16_t* Y);
extern uint8_t TS_GetGesture(void);
extern uint8_t TS_WriteSingle(uint8_t reg, uint8_t val);
extern uint8_t TS_ReadSingle(uint8_t reg);

#endif /* __TOUCH_H */
