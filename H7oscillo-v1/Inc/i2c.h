/**
  ******************************************************************************
  * @file    i2c.h
  * @author  anirudhr
  * @brief   Header file for i2c.c.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C_H
#define __I2C_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f7xx_ll_i2c.h"


#define I2C_TIMEOUT		10		/* 10ms */

/* External function declarations */
uint8_t I2C_WriteSingle(I2C_TypeDef* bus, uint8_t dev_addr, uint8_t reg, uint8_t val);
uint8_t I2C_ReadSingle(I2C_TypeDef* bus, uint8_t dev_addr, uint8_t reg);

#endif /* __I2C_H */
