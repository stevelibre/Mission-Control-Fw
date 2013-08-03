#include "I2C.h"
#include "HMC5883L.h"

/* setup compass for continuous measurement mode */

void HMC5883_init(I2C_TypeDef * I2Cx){ 
uint8_t testval;
  HMC5883_write_register(I2Cx, HMC5883_CONFIGURATION_REG_A, HMC5883_DEVICE_ID_ADDR_WRITE, 0x70);
  HMC5883_write_register(I2Cx, HMC5883_CONFIGURATION_REG_B, HMC5883_DEVICE_ID_ADDR_WRITE, 0xA0);
  HMC5883_write_register(I2Cx, HMC5883_MODE_REG, HMC5883_DEVICE_ID_ADDR_WRITE, 0x00);
   HMC5883_read_register(I2Cx, HMC5883_DEVICE_ID_ADDR_WRITE, HMC5883_CONFIGURATION_REG_B);
   trace_data("Compass reg val: %d\n", testval);
}

void HMC5883_start_selftest(I2C_TypeDef * I2Cx){
}

uint8_t HMC5883_read_register(I2C_TypeDef * I2Cx, uint8_t devwrite, uint8_t regaddr){

    I2C_start(I2Cx, devwrite, I2C_Direction_Transmitter);
    I2C_SendData(I2Cx, regaddr);
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET);
    I2C_stop(I2Cx);
    I2C_start(I2Cx,HMC5883_DEVICE_ID_ADDR_READ, I2C_Direction_Receiver);
     I2C_AcknowledgeConfig(I2Cx, ENABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2Cx);
        I2C_stop(I2Cx);
        I2C_AcknowledgeConfig(I2Cx,DISABLE);
       // I2C_stop(I2Cx);
       return data;

}

void HMC5883_read_burst(I2C_TypeDef * I2Cx, uint8_t devwrite, uint8_t * data_out, uint8_t nBytes){
//
//    HMC5883_write_register(I2Cx,HMC5883_CONFIGURATION_REG_A ,HMC5883_DEVICE_ID_ADDR_WRITE, 0x70);
//    HMC5883_write_register(I2Cx, HMC5883_CONFIGURATION_REG_B, HMC5883_DEVICE_ID_ADDR_WRITE, 0xA0);
//    HMC5883_write_register(I2Cx, HMC5883_MODE_REG, HMC5883_DEVICE_ID_ADDR_WRITE, 0x00);
    
    I2C_start(I2Cx, devwrite, I2C_Direction_Transmitter);
    I2C_SendData(I2Cx,HMC5883_DATA_OUT_Z_LSB_REG_ADDR);
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET);
    I2C_stop(I2Cx);
    I2C_start(I2Cx,HMC5883_DEVICE_ID_ADDR_READ, I2C_Direction_Receiver);
    I2C_AcknowledgeConfig(I2Cx, ENABLE);

    for(int i= 0; i < nBytes;  i++) {

      while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
      data_out[i] = I2C_ReceiveData(I2Cx);
      }
      //set the stack pointer to X MSB
      I2C_SendData(I2Cx,HMC8883_DATA_OUT_X_MSB_REG_ADDR);
      I2C_AcknowledgeConfig(I2Cx, DISABLE);
      I2C_stop(I2Cx);
//    I2C_AcknowledgeConfig(I2Cx,DISABLE);
//     while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED) );
//      I2C_ReceiveData(I2Cx);
//    I2C_stop(I2Cx);

}


void HMC5883_write_register(I2C_TypeDef* I2Cx, uint8_t regaddr, uint8_t devwrite, uint8_t data_in){
  
   I2C_start(I2Cx, devwrite, I2C_Direction_Transmitter);
   I2C_SendData(I2Cx, regaddr);
   I2C_AcknowledgeConfig(I2Cx, ENABLE);
       I2C_SendData(I2Cx, data_in);
       while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED) );
        I2C_stop(I2Cx);
        I2C_AcknowledgeConfig(I2Cx,DISABLE);
        I2C_stop(I2Cx);
}

