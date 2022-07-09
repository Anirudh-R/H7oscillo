/**
  ******************************************************************************
  * @file    touch.c
  * @author  anirudhr
  * @brief   Implements touch functions.
  ******************************************************************************
  */

/* Includes */
#include "touch.h"


uint16_t TS_X, TS_Y;				/* XY coordinates of touch point */
__IO uint8_t TS_read_pending = 0;	/* Pending touch screen read */
static uint8_t touch_pressed = 0;
static uint8_t gesture_valid = 0;
static uint16_t X_ini, Y_ini;		/* initial X & Y */
static uint16_t X_curr, Y_curr;		/* current X & Y */

/**
  * @brief  Touch screen IC FT5336 initialization.
  * @param  None
  * @retval None
  */
void TS_init(void)
{
	LL_mDelay(200);		/* Wait for 200ms after power-up before accessing registers */

	/* Nothing else to be done for FT5336 */
}

/**
  * @brief  Get number of current touches detected.
  * @param  None
  * @retval Number of touches
  */
uint8_t TS_DetectNumTouches(void)
{
	uint8_t nTouches;

	nTouches = TS_ReadSingle(FT5336_TD_STAT_REG);

	/* Invalid */
	if(nTouches > TS_MAX_NB_TOUCH){
		nTouches = 0;
	}

	return nTouches;
}

/**
  * @brief  Read the X and Y coordinates of the touch.
  * @param  X: pointer to X data
  * @param  Y: pointer to Y data
  * @retval None
  */
void TS_GetXY(uint16_t* X, uint16_t* Y)
{
	/* Read X coordinate */
	*X = ((TS_ReadSingle(FT5336_P1_XH_REG) & 0x0F) << 8) | TS_ReadSingle(FT5336_P1_XL_REG);

	/* Read Y coordinate */
	*Y = ((TS_ReadSingle(FT5336_P1_YH_REG) & 0x0F) << 8) | TS_ReadSingle(FT5336_P1_YL_REG);
}

/**
  * @brief  Read the last touch gesture ID.
  * @param  None
  * @retval Gesture ID
  */
uint8_t TS_GetGesture(void)
{
	uint8_t gestureID;
	int16_t xdiff, ydiff;

	/* gesture is only valid when the touch is removed and swipe is within bounds */
	if(!gesture_valid || X_ini < SWIPE_XINIT_MIN || X_ini > SWIPE_XINIT_MAX || Y_ini < SWIPE_YINIT_MIN || Y_ini > SWIPE_YINIT_MAX){
		return GEST_ID_NO_GESTURE;
	}

	xdiff = (int16_t)X_curr - (int16_t)X_ini;
	ydiff = (int16_t)Y_curr - (int16_t)Y_ini;

	gestureID = GEST_ID_NO_GESTURE;
	if(xdiff > SWIPE_MIN_DISTANCE && abs(ydiff) < SWIPE_MAX_HALFWIDTH)
		gestureID = GEST_ID_SWIPE_RIGHT;
	else if(xdiff < -SWIPE_MIN_DISTANCE && abs(ydiff) < SWIPE_MAX_HALFWIDTH)
		gestureID = GEST_ID_SWIPE_LEFT;
	else if(ydiff > SWIPE_MIN_DISTANCE && abs(xdiff) < SWIPE_MAX_HALFWIDTH)
		gestureID = GEST_ID_SWIPE_DOWN;
	else if(ydiff < -SWIPE_MIN_DISTANCE && abs(xdiff) < SWIPE_MAX_HALFWIDTH)
		gestureID = GEST_ID_SWIPE_UP;

	gesture_valid = 0;			/* invalidate gesture after it is read once */

	return gestureID;
}

/**
  * @brief  Process a touch event.
  * @param  X: x data
  * @param  Y: y data
  * @retval None
  */
void TS_ProcessTouch(uint16_t X, uint16_t Y)
{
	if(!touch_pressed){
		X_ini = X;			/* record initial x & y */
		Y_ini = Y;
		touch_pressed = 1;
	}
	else{
		X_curr = X;			/* current x & y */
		Y_curr = Y;
	}
}

/**
  * @brief  Process a touch release event.
  * @param  None
  * @retval None
  */
void TS_ReleaseTouch(void)
{
	if(touch_pressed){
		gesture_valid = 1;		/* gesture is valid only when the touch is removed */
	}

	touch_pressed = 0;
}

/**
  * @brief  Write a single touch screen register.
  * @param  reg: register to be written to
  * @param  val: value to be written
  * @retval success=0, fail=1
  */
uint8_t TS_WriteSingle(uint8_t reg, uint8_t val)
{
	return I2C_WriteSingle(TS_I2Cx, TS_I2C_ADDRESS, reg, val);
}

/**
  * @brief  Read a single touch screen register.
  * @param  reg: register to be read from
  * @retval Read data
  */
uint8_t TS_ReadSingle(uint8_t reg)
{
	return I2C_ReadSingle(TS_I2Cx, TS_I2C_ADDRESS, reg);
}

