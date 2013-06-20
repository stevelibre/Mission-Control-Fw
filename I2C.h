#ifndef _I2C_H
#define _I2C_H

#include "stm32f4xx_i2c.h"

#define I2C_CLOCK_SPEED    100000
#define I2C_CLOCK_SPEED_FM 400000


void I2C1_Init(void);
void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);
void I2C_stop(I2C_TypeDef* I2Cx);


#endif