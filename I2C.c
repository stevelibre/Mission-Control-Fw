
#include "I2C.h"


void I2C1_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;

    I2C_StructInit(&I2C_InitStructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);

     RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
     RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);

    I2C_DeInit(I2C1);

    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; //I2C_DutyCycle_16_9
    I2C_InitStructure.I2C_OwnAddress1 =  0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C_CLOCK_SPEED;


    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_Cmd(I2C1, ENABLE);

    //I2C_AcknowledgeConfig(I2C1, ENABLE);
}



void I2C2_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;

    I2C_StructInit(&I2C_InitStructure);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);

     RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
     RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);

    I2C_DeInit(I2C1);

    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; //I2C_DutyCycle_16_9
    I2C_InitStructure.I2C_OwnAddress1 =  0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C_CLOCK_SPEED;


    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_Cmd(I2C1, ENABLE);

    //I2C_AcknowledgeConfig(I2C1, ENABLE);
}


void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction){
	// wait until I2C1 is not busy anymore
          while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));

	// Send I2C1 START condition
	I2C_GenerateSTART(I2Cx, ENABLE);

	// wait for I2C1 EV5 --> Slave has acknowledged start condition
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

	// Send slave Address for write
	I2C_Send7bitAddress(I2Cx, address, direction);

	/* wait for I2C1 EV6, check if
	 * either Slave has acknowledged Master transmitter or
	 * Master receiver mode, depending on the transmission
	 * direction
	 */
	if(direction == I2C_Direction_Transmitter){
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	}
	else if(direction == I2C_Direction_Receiver){
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	}
}

void I2C_stop(I2C_TypeDef* I2Cx){
	// Send I2C1 STOP Condition
	I2C_GenerateSTOP(I2Cx, ENABLE);
}


//uint8_t I2C_read_register(I2C_TypeDef * I2Cx, uint8_t slave_addr, uint8_t regaddr) {
//
// I2C_start(I2Cx, slave_addr, I2C_Direction_Transmitter);
//    I2C_SendData(I2Cx,regaddr);
//    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET);
//    I2C_stop(I2Cx);
//    I2C_start(I2Cx,0xD1, I2C_Direction_Receiver);
//    I2C_AcknowledgeConfig(I2Cx, ENABLE);
//	// wait until one byte has been received
//	while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
//	// read data from I2C data register and return data byte
//	uint8_t data = I2C_ReceiveData(I2Cx);
//        I2C_AcknowledgeConfig(I2Cx,DISABLE);
//        I2C_stop(I2Cx);
//       return data;
//
//
//}

//void I2C_read_burst(I2C_TypeDef * I2Cx, uint8_t slave_addr , uint8_t read_reg ,uint8_t * data_out, uint8_t nBytes){
//
//    I2C_start(I2Cx, slave_addr, I2C_Direction_Transmitter);
//    I2C_SendData(I2Cx,read_reg);
//    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF) == RESET);
//    I2C_stop(I2Cx);
//    I2C_start(I2Cx, slave_addr, I2C_Direction_Receiver);
//    I2C_AcknowledgeConfig(I2Cx, ENABLE);
//
//    for(int i= 0; i < nBytes;  i++) {
//
//      while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
//        data_out[i] = I2C_ReceiveData(I2Cx);
//      }
//      I2C_AcknowledgeConfig(I2Cx, DISABLE);
//      I2C_stop(I2Cx);
//}