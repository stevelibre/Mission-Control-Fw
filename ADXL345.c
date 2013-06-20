#include "I2C.h"
#include "ADXL345.h"

#include "data_trace.h"
#include "string.h"



void ADXL345_init(I2C_TypeDef * I2Cx){
  uint8_t readback_data;
  ADXL345_write_register(I2Cx,ADXL345_REGISTER_PWRCTL,ADXL345_SLAVE_WRITE_ADDR,ADXL345_PWRCTL_MEASURE);
  readback_data = ADXL345_read_ack(I2Cx,ADXL345_REGISTER_PWRCTL,ADXL345_SLAVE_WRITE_ADDR);
  ADXL345_read_nack(I2Cx,ADXL345_REGISTER_PWRCTL,ADXL345_SLAVE_WRITE_ADDR);
  trace_data("PowerCTRL register: %d\n", readback_data);
}

uint8_t ADXL345_read_ack(I2C_TypeDef * I2Cx, uint8_t regaddr,uint8_t devread ){
       
       I2C_start(I2Cx, devread, I2C_Direction_Transmitter);
       I2C_SendData(I2Cx, regaddr); //send register address to be read from
       while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET)
       I2C_stop(I2Cx);
       I2C_start(I2Cx,(uint8_t)ADXL345_SLAVE_READ_ADDR , I2C_Direction_Receiver);
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2Cx);
        I2C_stop(I2Cx);
	return data;
}

void ADXL345_read_burst(I2C_TypeDef * I2Cx, uint8_t devread, uint8_t * data_out, uint8_t nBytes ){

        //write mode byte to the fifo control register
          ADXL345_write_register(I2C1, ADXL345_FIFO_CONTROL_REG, devread, ADXL345_FIFO_MODE(ADXL345_FIFO_STREAM));

          I2C_start(I2Cx, devread, I2C_Direction_Transmitter);
          I2C_SendData(I2Cx, ADXL345_X_AXIS_DATA_REG0); 
          //send register address to be read from first (x0) the ADXL345 increments pointer 
          //to access follow up  registers automatically
       
          while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET)
            I2C_stop(I2Cx);
            I2C_start(I2Cx,(uint8_t)ADXL345_SLAVE_READ_ADDR , I2C_Direction_Receiver);
            I2C_AcknowledgeConfig(I2Cx, ENABLE);

            for(int i = 0; i < nBytes-1; i++) {
            // wait until one byte has been received
             while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
          // read data from I2C data register and return data byte
            data_out[i] = I2C_ReceiveData(I2Cx);
            }
         data_out[nBytes -1]  = ADXL345_read_nack(I2Cx, ADXL345_Z_AXIS_DATA_REG1, (uint8_t)ADXL345_SLAVE_READ_ADDR);
}

uint8_t ADXL345_read_nack(I2C_TypeDef* I2Cx, uint8_t regaddr, uint8_t devread){
        
	// disable acknowledge of received data
	I2C_AcknowledgeConfig(I2Cx, DISABLE);

        (void)I2Cx->SR2;
	
        // wait until one byte has been received
	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2Cx);
        I2C_stop(I2Cx);
	return data;
}



void ADXL345_write_register(I2C_TypeDef* I2Cx, uint8_t regaddr, uint8_t devread, uint8_t data_in){
      
       I2C_start(I2Cx, devread, I2C_Direction_Transmitter);
       I2C_SendData(I2Cx, regaddr);
       I2C_AcknowledgeConfig(I2Cx, ENABLE);
       I2C_SendData(I2Cx, data_in);
       while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED) );
        I2C_stop(I2Cx);
        I2C_AcknowledgeConfig(I2Cx,DISABLE);
        I2C_stop(I2Cx);

}



void ADXL345_start_selftest(){

uint8_t bytes[12];
int accel_data_2comp[6];

  ADXL345_write_register(I2C1,
                        ADXL345_DATA_FORMAT_REG,
                        ADXL345_SLAVE_WRITE_ADDR, 
                        ADXL345_ENABLE_SELFTEST);

  for(int i = 0; i < 20; i++){

    memset(accel_data_2comp,0,6);

    ADXL345_read_burst(I2C1, ADXL345_SLAVE_WRITE_ADDR, bytes, 6);

    for(int j = 0; j < 3; j++) {
      accel_data_2comp[j] = (int)bytes[2*j] + (((int)bytes[2*j + 1]) << 8);
      trace_data("2 comp val: %d\n", accel_data_2comp[j]);
    }

  }

  ADXL345_write_register(I2C1,
                       ADXL345_DATA_FORMAT_REG,
                       ADXL345_SLAVE_WRITE_ADDR,
                       ADXL345_DISABLE_SELFTEST);
}