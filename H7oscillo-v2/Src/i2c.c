/**
  ******************************************************************************
  * @file    i2c.c
  * @author  anirudhr
  * @brief   Implements I2C functions.
  ******************************************************************************
  */

/* Includes */
#include "i2c.h"


/**
  * @brief  Write a single register of a 7-bit addressed I2C slave.
  * @param  bus: I2C bus
  * @param  dev_addr: slave address
  * @param  reg: register to be written to
  * @param  val: value to be written
  * @retval success=0, fail=1
  */
uint8_t I2C_WriteSingle(I2C_TypeDef* bus, uint8_t dev_addr, uint8_t reg, uint8_t val)
{
	uint32_t tickstart;

	/* send slave address */
	LL_I2C_HandleTransfer(bus, dev_addr, LL_I2C_ADDRSLAVE_7BIT, 2, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

	/* wait until TXIS flag is set */
	tickstart = Get_tick();
	while(!LL_I2C_IsActiveFlag_TXIS(bus)){
		/* check if slave acknowledged */
		if(LL_I2C_IsActiveFlag_NACK(bus)){
			return 1;
		}
		if(Get_tick() - tickstart > I2C_TIMEOUT){
			return 1;
		}
	}

	/* send memory address */
	LL_I2C_TransmitData8(bus, reg);

	/* wait until TXIS flag is set */
	tickstart = Get_tick();
	while(!LL_I2C_IsActiveFlag_TXIS(bus)){
		/* check if slave acknowledged */
		if(LL_I2C_IsActiveFlag_NACK(bus)){
			return 1;
		}
		if(Get_tick() - tickstart > I2C_TIMEOUT){
			return 1;
		}
	}

	/* send data */
	LL_I2C_TransmitData8(bus, val);

	/* wait for the STOPF flag to be set */
	tickstart = Get_tick();
	while(!LL_I2C_IsActiveFlag_STOP(bus)){
		if(Get_tick() - tickstart > I2C_TIMEOUT){
			return 1;
		}
	}

	/* clear STOPF flag */
	LL_I2C_ClearFlag_STOP(bus);

	return 0;
}

/**
  * @brief  Read a single register of a 7-bit addressed I2C slave.
  * @param  bus: I2C bus
  * @param  dev_addr: slave address
  * @param  reg: register to be read from
  * @retval Read data
  */
uint8_t I2C_ReadSingle(I2C_TypeDef* bus, uint8_t dev_addr, uint8_t reg)
{
	uint8_t val;
	uint32_t tickstart;

	/* send slave address */
	LL_I2C_HandleTransfer(bus, dev_addr, LL_I2C_ADDRSLAVE_7BIT, 1, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

	/* wait until TXIS flag is set */
	tickstart = Get_tick();
	while(!LL_I2C_IsActiveFlag_TXIS(bus)){
		/* check if slave acknowledged */
		if(LL_I2C_IsActiveFlag_NACK(bus)){
			return 0xFF;
		}
		if(Get_tick() - tickstart > I2C_TIMEOUT){
			return 0xFF;
		}
	}

	/* send memory address */
	LL_I2C_TransmitData8(bus, reg);

	/* wait until TXIS flag is set */
	tickstart = Get_tick();
	while(!LL_I2C_IsActiveFlag_TC(bus)){
		/* check if slave acknowledged */
		if(LL_I2C_IsActiveFlag_NACK(bus)){
			return 0xFF;
		}
		if(Get_tick() - tickstart > I2C_TIMEOUT){
			return 0xFF;
		}
	}

	/* send slave address */
	LL_I2C_HandleTransfer(bus, dev_addr, LL_I2C_ADDRSLAVE_7BIT, 1, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);

	/* wait until RXNE flag is set */
	tickstart = Get_tick();
	while(!LL_I2C_IsActiveFlag_RXNE(bus)){
		/* check if slave acknowledged */
		if(LL_I2C_IsActiveFlag_NACK(bus)){
			return 0xFF;
		}
		if(Get_tick() - tickstart > I2C_TIMEOUT){
			return 0xFF;
		}
	}

	val = LL_I2C_ReceiveData8(bus);

	/* wait for the STOPF flag to be set */
	tickstart = Get_tick();
	while(!LL_I2C_IsActiveFlag_STOP(bus)){
		if(Get_tick() - tickstart > I2C_TIMEOUT){
			return 0xFF;
		}
	}

	/* clear STOPF flag */
	LL_I2C_ClearFlag_STOP(bus);

	return val;
}

