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
 * 0x40912732 takes in account the big rising and aims a clock of 100khz
 */
#define I2Cx_TIMING                 ((uint32_t)0x40912732)

#define TS_I2C_ADDRESS              ((uint16_t)0x70)

/* Maximum touches detected simultaneously */
#define TS_MAX_NB_TOUCH             ((uint32_t) FT5336_MAX_DETECTABLE_TOUCH)

/* Possible gesture IDs */
#define GEST_ID_NO_GESTURE   		0x00  	/* Gesture not defined / recognized */
#define GEST_ID_SWIPE_RIGHT   		0x01  	/* Gesture swipe Right */
#define GEST_ID_SWIPE_DOWN    		0x02  	/* Gesture swipe Down */
#define GEST_ID_SWIPE_LEFT    		0x03  	/* Gesture swipe Left */
#define GEST_ID_SWIPE_UP    		0x04  	/* Gesture swipe Up */

#define SWIPE_MAX_HALFWIDTH			45		/* a horiz/vert swipe shouldn't deviate more than SWIPE_MAX_HALFWIDTH from the initial y/x coordinate */
#define SWIPE_MIN_DISTANCE			100		/* minimum distance for it to be considered a swipe */
#define SWIPE_XINIT_MIN				180		/* the initial x & y coordinates of a swipe must be in this range */
#define SWIPE_XINIT_MAX				300
#define SWIPE_YINIT_MIN				75
#define SWIPE_YINIT_MAX				195

extern uint16_t TS_X, TS_Y;
extern __IO uint8_t TS_read_pending;

/* External function declarations */
extern void TS_init(void);
extern uint8_t TS_DetectNumTouches(void);
extern void TS_GetXY(uint16_t* X, uint16_t* Y);
extern uint8_t TS_GetGesture(void);
extern void TS_ProcessTouch(uint16_t X, uint16_t Y);
extern void TS_ReleaseTouch(void);
extern uint8_t TS_WriteSingle(uint8_t reg, uint8_t val);
extern uint8_t TS_ReadSingle(uint8_t reg);

#endif /* __TOUCH_H */
