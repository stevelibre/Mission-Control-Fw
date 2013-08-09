#ifndef _HMC5883L_H
#define _HMC5883L_H

#define HMC5883_CONFIGURATION_REG_A  0x00
#define HMC5883_CONFIGURATION_REG_B  0x01
#define HMC5883_MODE_REG  0x02
#define HMC8883_DATA_OUT_X_MSB_REG_ADDR 0x03 
#define HMC5883_DATA_OUT_X_LSB_REG_ADDR 0x04
#define HMC5883_DATA_OUT_Z_MSB_REG_ADDR 0x05
#define HMC5883_DATA_OUT_Z_LSB_REG_ADDR 0x06
#define HMC5883_DATA_OUT_Y_MSB_REG_ADDR 0x07
#define HMC5883_DATA_OUT_Y_LSB_REG 0x08
#define HMC5883_STATUS_REG 0x09
#define HMC5883_IDENT_REG_A 0x0a 
#define HMC5883_IDENT_REG_B 0x0b
#define HMC5883_IDENT_REG_C 0x0c


#define HMC5883_DEVICE_ID_ADDR_WRITE 0x3C
#define HMC5883_DEVICE_ID_ADDR_READ  0x3D

void HMC5883_init(I2C_TypeDef * I2Cx); //wake the compass
void HMC5883_start_selftest();
uint8_t HMC5883_read_register(I2C_TypeDef * I2Cx, uint8_t devwrite, uint8_t reg);
void HMC5883_read_burst(I2C_TypeDef * I2Cx, uint8_t devwrite, uint8_t * data_out, uint8_t nBytes);
void HMC5883_write_register(I2C_TypeDef* I2Cx, uint8_t regaddr, uint8_t devwrite, uint8_t data_in);
void HMC5883_readoutTask(void *pvParameters);

#endif
