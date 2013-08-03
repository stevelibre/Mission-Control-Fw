#include "I2C.h"
#include "ITG3200.h"
#include "stdio.h"


void ITG3200_init(I2C_TypeDef * I2Cx){
   uint8_t regvalue;
   //Set DLPF to 42 Hz 
  ITG3200_write_register(I2Cx, ITG3200_DLPF_FS, ITG3200_ADDRESS, ITG3200_FULLSCALE | ITG3200_42HZ);

  //Sanity check! Make sure the register value is correct.
  regvalue = ITG3200_read_register(I2Cx, 0xD0, ITG3200_DLPF_FS);
  trace_data("ITG3200 init reg val : %d\n " , regvalue);

}
 
void ITG3200_start_selftest(){
}

uint8_t ITG3200_read_register(I2C_TypeDef * I2Cx, uint8_t devwrite, uint8_t regaddr){

    I2C_start(I2Cx, devwrite, I2C_Direction_Transmitter);
    I2C_SendData(I2Cx,regaddr);
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET);
    I2C_stop(I2Cx);
    I2C_start(I2Cx,0xD1, I2C_Direction_Receiver);
    I2C_AcknowledgeConfig(I2Cx, ENABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2Cx);
        I2C_AcknowledgeConfig(I2Cx,DISABLE);
        I2C_stop(I2Cx);
       return data;
}

void ITG3200_read_burst(I2C_TypeDef * I2Cx, uint8_t devwrite ,uint8_t * data_out, uint8_t nBytes){

     // I2C_read_burst(I2Cx, slave_addr,ITG3200_XOUT_H, data_out, nBytes);



    I2C_start(I2Cx, devwrite, I2C_Direction_Transmitter);
    I2C_SendData(I2Cx,ITG3200_XOUT_H);
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET);
    I2C_stop(I2Cx);
    I2C_start(I2Cx,devwrite, I2C_Direction_Receiver);
    I2C_AcknowledgeConfig(I2Cx, ENABLE);

    for(int i= 0; i < nBytes;  i++) {

      while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
        data_out[i] = I2C_ReceiveData(I2Cx);
      }
      //set the stack pointer to X MSB
      //I2C_SendData(I2Cx,HMC8883_DATA_OUT_X_MSB_REG_ADDR);
      I2C_AcknowledgeConfig(I2Cx, DISABLE);
      I2C_stop(I2Cx);
}


float ITG3200_read_temp(I2C_TypeDef* I2Cx, uint8_t devwrite){
uint8_t temp_h;
uint8_t temp_l;
int temp_out = 0;
float retval ; //=  0.0 ;

temp_h =  ITG3200_read_register(I2Cx, devwrite, ITG3200_TEMP_OUT_H );
temp_l =  ITG3200_read_register(I2Cx, devwrite, ITG3200_TEMP_OUT_L);

temp_out = (int) temp_h <<8 |  (int) temp_l;

retval = 35.0 + ((temp_out + 13200.0)/280.0);

return retval;

}


void ITG3200_write_register(I2C_TypeDef* I2Cx, uint8_t regaddr, uint8_t devwrite, uint8_t data_in){

       I2C_start(I2Cx, devwrite, I2C_Direction_Transmitter);
       I2C_SendData(I2Cx, regaddr);
       I2C_AcknowledgeConfig(I2Cx, ENABLE);
       I2C_SendData(I2Cx, data_in);
       while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED) );
       I2C_stop(I2Cx);
}

//void ITG3200_calibrate_offsets(int *raw_in, int *offsets){
//}


void ITG3200_calc_degrees(int * gyro_2complement, float gyro_temp, float * gyro_degrees, int * gyro_offsets) {
float scalar = 14.375;
int elapsedSeconds =1;

  gyro_2complement[0] -= gyro_offsets[0];
  gyro_2complement[1] -= gyro_offsets[1];
  gyro_2complement[2] -= gyro_offsets[2];
  gyro_degrees[0] += (gyro_2complement[0] * elapsedSeconds) / scalar;
  gyro_degrees[1] += (gyro_2complement[1] * elapsedSeconds) / scalar;
  gyro_degrees[2] += (gyro_2complement[2] * elapsedSeconds) / scalar;
  }
