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
  * @param  pointer to X data
  * @param  pointer to Y data
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
	uint8_t gestureID = GEST_ID_NO_GESTURE;

	gestureID = TS_ReadSingle(FT5336_GEST_ID_REG);

	return gestureID;
}

/**
  * @brief  Write a single touch screen register.
  * @param  register to be written to
  * @param  value to be written
  * @retval success=0, fail=1
  */
uint8_t TS_WriteSingle(uint8_t reg, uint8_t val)
{
	return I2C_WriteSingle(TS_I2Cx, TS_I2C_ADDRESS, reg, val);
}

/**
  * @brief  Read a single touch screen register.
  * @param  register to be read from
  * @retval Read data
  */
uint8_t TS_ReadSingle(uint8_t reg)
{
	return I2C_ReadSingle(TS_I2Cx, TS_I2C_ADDRESS, reg);
}

