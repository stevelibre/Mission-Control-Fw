
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "stdio.h"
#include "string.h"

#include "mc_globals.h"
#include "number_format.h"
#include "I2C.h"
#include "ADXL345.h"


void ADXL345_exti0_init(){

  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  /* Enable GPIOB clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* Configure PB7 pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOB, &GPIO_InitStructure);


  /* Connect EXTI Line0 to  */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource7);

  /* Configure EXTI Line0 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI Line0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}



void ADXL345_init(I2C_TypeDef * I2Cx){
  uint8_t readback_data;
  ADXL345_write_register(I2Cx,ADXL345_REGISTER_PWRCTL,ADXL345_SLAVE_WRITE_ADDR,ADXL345_PWRCTL_MEASURE);
  readback_data = ADXL345_read_ack(I2Cx,ADXL345_REGISTER_PWRCTL,ADXL345_SLAVE_WRITE_ADDR);
  ADXL345_read_nack(I2Cx,ADXL345_REGISTER_PWRCTL,ADXL345_SLAVE_WRITE_ADDR);
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
       
          while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET);
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
      debug_printf("2 comp val: %d\n", accel_data_2comp[j]);
    }

  }

  ADXL345_write_register(I2C1,
                       ADXL345_DATA_FORMAT_REG,
                       ADXL345_SLAVE_WRITE_ADDR,
                       ADXL345_DISABLE_SELFTEST);
}



//void ADXL345_detect_freefall(I2C_TypeDef* I2Cx){
//
//
////ADXL345_exti0_init(); //initialize INT PIN7
//
//// set the treshold value for the freefall detection on 437,5 uG
//
//ADXL345_write_register(I2Cx,
//                       ADXL345_TRESH_FREE_FALL_REG,
//                       ADXL345_SLAVE_WRITE_ADDR,
//                       ADXL345_TEST_FREE_FALL_IMPACT_VAL );
//
////set the time that the device needs to be exposed to a negative g force to  detect a freefall event
//
//ADXL345_write_register(I2Cx,
//                       ADXL345_TIME_FREE_FALL_REG,
//                       ADXL345_SLAVE_WRITE_ADDR,
//                       ADXL345_TEST_TIME_FREE_FALL);
//
//ADXL345_write_register(I2Cx,
//                       ADXL354_INT_ENABLE_REG,
//                       ADXL345_SLAVE_WRITE_ADDR,
//                       ADXL345_DTAP_FREEFALL_DETECT_MASK);
//ADXL345_exti0_init();
//}


void ADXL345_readoutTask(void *pvParameters){

uint8_t accel_data3d[6];
//uint8_t accel_trans_buffer[TRANS_BUFFER_LENGTH];
int accel_read_buffer[3];

int pvarx =0;
int pvary =0;
int pvarz =0;

//xQueueHandle *USARTQueueHandle;
portBASE_TYPE  USARTQueueStatus;

// if(pvParameters != NULL){
//  USARTQueueHandle = (xQueueHandle *) pvParameters ;
// }else{
//      debug_printf("ERROR : ADXL345_readoutTask routine needs xQueueHandle as a parameter");
// }


  for(;;){

      ADXL345_read_burst(I2C1, ADXL345_SLAVE_WRITE_ADDR ,accel_data3d, 6 /*2 datasets*/ );
      for (int i = 0;i < 3; ++i) {
         accel_read_buffer[i] = (int)accel_data3d[2*i] + (((int)accel_data3d[2*i + 1]) << 8);
      }

      memset(sensor_trans_buffer,0,TRANS_BUFFER_LENGTH);
      pvarx = correct_number_format(accel_read_buffer[0]);
      pvary = correct_number_format(accel_read_buffer[1]);
      pvarz = correct_number_format(accel_read_buffer[2]);
      debug_printf("accel x: %d\n",pvarx);
      debug_printf("accel y: %d\n",pvary);
      debug_printf("accel z: %d\n",pvarz);

      if(sensor_trans_buffer != NULL) {
        snprintf(sensor_trans_buffer,TRANS_BUFFER_LENGTH,"a %d, %d, %d \n",pvarx, pvary, pvarz);
        debug_printf("%s",sensor_trans_buffer);
      }

      USARTQueueStatus= xQueueSendToBack(USARTQueueHandle, sensor_trans_buffer,3);
         
      if(USARTQueueStatus != pdPASS){
       debug_printf("ADXL345 Error sending to the USART queue\n");
      }
         
      if(USARTQueueStatus == errQUEUE_FULL){
       debug_printf("ADXL345 Error USART queue already full\n");
      }

     taskYIELD(); 
  }
}

