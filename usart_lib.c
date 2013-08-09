
#include <stdint.h>
#include <stdio.h>

#include "stm32f4xx_usart.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "mc_globals.h"
#include "usart_lib.h"


static int RxOverflow = 0;

// TxPrimed is used to signal that Tx send buffer needs to be primed
// to commence sending -- it is cleared by the IRQ, set by uart_write

static int TxPrimed = 0;

struct Queue {
  uint16_t pRD, pWR;
  uint8_t  q[QUEUE_SIZE]; 
};

static struct Queue UART2_TXq, UART2_RXq;


void Init_USART3(){
  
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  USART_ClockInitTypeDef USART_ClockInitStructure;
 
//enable bus clocks
 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3);
 
//Set USART1 Tx (PD.05) as AF push-pull
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
 
  //Set USART2 Rx (PD.06) as input floating
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOD, &GPIO_InitStructure);



  USART_ClockStructInit(&USART_ClockInitStructure);
  USART_ClockInit(USART3, &USART_ClockInitStructure);
  USART_InitStructure.USART_BaudRate = 4800;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
  USART_InitStructure.USART_StopBits = USART_StopBits_1; 
  USART_InitStructure.USART_Parity = USART_Parity_No ; 
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

  //Write USART2 parameters
  USART_Init(USART3, &USART_InitStructure);

  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // enable the USART2 receive interrupt 

//  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		 
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
//  NVIC_Init(&NVIC_InitStructure);
 
  //Enable USART2
  USART_Cmd(USART3, ENABLE);
}


void Init_USART2(){
  
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  USART_ClockInitTypeDef USART_ClockInitStructure;
 
//enable bus clocks
 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
 
//Set USART1 Tx (PD.05) as AF push-pull
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
 
  //Set USART2 Rx (PD.06) as input floating
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);



  USART_ClockStructInit(&USART_ClockInitStructure);
  USART_ClockInit(USART2, &USART_ClockInitStructure);
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
  USART_InitStructure.USART_StopBits = USART_StopBits_1; 
  USART_InitStructure.USART_Parity = USART_Parity_No ; 
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

  //Write USART2 parameters
  USART_Init(USART2, &USART_InitStructure);

  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // enable the USART2 receive interrupt 

//  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		 
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
//  NVIC_Init(&NVIC_InitStructure);
 
  //Enable USART2
  USART_Cmd(USART2, ENABLE);
}

static int QueueFull(struct Queue *q)
{
  return (((q->pWR + 1) % QUEUE_SIZE) == q->pRD);
}

static int QueueEmpty(struct Queue *q)
{
  return (q->pWR == q->pRD);
}

int QueueAvail(struct Queue *q)
{
  return (QUEUE_SIZE + q->pWR - q->pRD) % QUEUE_SIZE;
}

int Enqueue(struct Queue *q, const uint8_t *data, uint16_t len)
{
  int i;
  for (i = 0; !QueueFull(q) && (i < len); i++)
    {
      q->q[q->pWR] = data[i];
      q->pWR = ((q->pWR + 1) ==  QUEUE_SIZE) ? 0 : q->pWR + 1;
    }
  return i;
}

 int Dequeue(struct Queue *q, uint8_t *data, uint16_t len)
{
  int i;
  for (i = 0; !QueueEmpty(q) && (i < len); i++)
    {
      data[i] = q->q[q->pRD];
      q->pRD = ((q->pRD + 1) ==  QUEUE_SIZE) ? 0 : q->pRD + 1;
    }
return i;
}


//void USART2_IRQHandler(void)
//{
//  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
//    {
//      uint8_t  data;
//
//      // clear the interrupt
//
//      USART_ClearITPendingBit(USART2, USART_IT_RXNE);
//
//      // buffer the data (or toss it if there's no room 
//      // Flow control is supposed to prevent this
//
//      data = USART_ReceiveData(USART2) & 0xff;
//      if (!Enqueue(&UART2_RXq, &data, 1))
//        RxOverflow = 1;
//
//      // If queue is above high water mark, disable nRTS
//
//      if (QueueAvail(&UART2_RXq) > HIGH_WATER)
//        GPIO_WriteBit(GPIOD, GPIO_Pin_6, 1);   
//    }
//  
//  if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
//    {   
//      /* Write one byte to the transmit data register */
//
//      uint8_t data;
//
//      if (Dequeue(&UART2_TXq, &data, 1))
//        {
//          USART_SendData(USART2, data);
//        }
//      else
//        {
//          // if we have nothing to send, disable the interrupt
//          // and wait for a kick
//
//          USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
//          TxPrimed = 0;
//        }
//    }
//}

int uart_getchar(void){
uint8_t data;

while (!Dequeue(&UART2_RXq, &data, sizeof(&data)));
return data;
}


//void  uart_putchar(int c){
//
//  while (!Enqueue(&UART2_TXq , &c, sizeof(c)))
//  if (!TxPrimed) {
//    TxPrimed = 1;
//    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
//  }
//
//}


int USART_putchar (USART_TypeDef* USARTx, int ch)  {

  while (!(USARTx->SR & USART_FLAG_TXE));

  USARTx->DR = (ch & 0x1FF);

  return (ch);
}
/*------------------------------------------------------------------------
  GetKey

  Read character to Serial Port.

 *----------------------------------------------------------------------------*/
int USART_getchar (USART_TypeDef* USARTx)  {

  while (!(USARTx->SR & USART_FLAG_RXNE));

  return ((int)(USARTx->DR & 0x1FF));
}


void USART_putstring(USART_TypeDef* USARTx, char * string) {
//char * start;
 for(char * start = string; *start != '\0'; start++){
    USART_putchar(USARTx, *start);
  } 
 }


/*ssize_t uart_write(uint8_t uart, const uint8_t *buf, size_t nbyte)
{
  uint8_t data;
  int i = 0;

  if (uart == 2 && nbyte)
    {
      i = Enqueue(&UART2_TXq, buf, nbyte);
  
      // if we added something and the Transmitter isn't working
      // give it a kick by turning on the buffer empty interrupt

      if (!TxPrimed)
	{
	  TxPrimed = 1;

	  // This implementation guarantees that USART_IT_Config
	  // is not called simultaneously in the interrupt handler and here.

	  USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
	}
    }
  return i;
}

ssize_t uart_read (uint8_t uart, uint8_t *buf, size_t nbyte)
{
  int i = 0;

  if (uart == 2)
    {

      i = Dequeue(&UART2_RXq, buf, nbyte);

      // If the queue has fallen below high water mark, enable nRTS

      if (QueueAvail(&UART2_RXq) <= HIGH_WATER)
	GPIO_WriteBit(GPIOA, GPIO_Pin_12, 0);    
    }
  return i;
}


*/

void USART_WriteQueueTask(void * pvParameters) {

xQueueHandle * pUSARTQueueHandle;
portBASE_TYPE  USARTQueueStatus;
char usart_receive_buffer[TRANS_BUFFER_LENGTH];

if(pvParameters != NULL){
      pUSARTQueueHandle = (xQueueHandle *) pvParameters ;
  }else{
      debug_printf("ERROR : USART Write Queue Task routine needs xQueueHandle as a parameter");
  }

   for(;;){

    if(uxQueueMessagesWaiting(pUSARTQueueHandle) != 0){
      debug_printf("WARNING: USART Write Queue should have been full \n");
    }

    USARTQueueStatus = xQueueReceive(pUSARTQueueHandle,usart_receive_buffer,100);
    
    if(USARTQueueStatus == pdPASS){
      
#ifdef USB_OTG
      VCP_send_str( usart_receive_buffer);
#else
      USART_putstring(USART2, usart_receive_buffer);
#endif 
    }else{
    if(USARTQueueStatus == errQUEUE_EMPTY){
      debug_printf("ERROR: USART Queue empty while trying to read\n");
    }

      debug_printf("ERROR : could not receieve from USART Queue \n");
    }
  } //for(ever)
}