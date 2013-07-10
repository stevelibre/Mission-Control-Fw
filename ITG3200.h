#ifndef _ITG3200_H
#define _ITG3200_H

#define ITG3200_WHO_AM_I 0x00
#define ITG3200_SMPLRTDIV 0x15
#define ITG3200_DLPF_FS 0x16
#define ITG3200_INT_CFG 0x17
#define ITG3200_INT_STATUS 0x1A
#define ITG3200_TEMP_OUT_H 0x1B
#define ITG3200_TEMP_OUT_L 0x1C
#define ITG3200_XOUT_H   0x1D
#define ITG3200_XOUT_L 0x1E
#define ITG3200_YOUT_H 0x1F
#define ITG3200_YOUT_L 0x20
#define ITG3200_ZOUT_H 0x21
#define ITG3200_ZOUT_L 0x22
#define ITG3200_PWR_MGM 0x3E

#define ITG3200_ADDRESS 0xD1 
//request burst of 6 bytes from this address
#define ITG3200_FULLSCALE (0x03 << 3)
#define ITG3200_42HZ (0x03)

void ITG3200_init(I2C_TypeDef * I2Cx); 
void ITG3200_start_selftest();
uint8_t ITG3200_read_register(I2C_TypeDef * I2Cx, uint8_t devwrite, uint8_t reg);
void ITG3200_read_burst(I2C_TypeDef * I2Cx, uint8_t devwrite, uint8_t * data_out, uint8_t nBytes);
void ITG3200_write_register(I2C_TypeDef* I2Cx, uint8_t regaddr, uint8_t devwrite, uint8_t data_in);

#endif