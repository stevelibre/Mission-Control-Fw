#include <stdio.h>
#include <stdint.h>
#include <string.h>


#include <cross_studio_io.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_exti.h"
#include "stm32f4_discovery.h"
//
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"


#include "STM32_LEDBlinky.h"
#include "data_trace.h"
#include "number_format.h"
#include "mc_globals.h"

#include "I2C.h"
#include "ADXL345.h"
#include "HMC5883L.h"
#include "ITG3200.h"
#include "EM_406A_GPS.h"
#include "usart_lib.h"


/* Private variables ---------------------------------------------------------*/

__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

 __IO uint32_t TimingDelay;


#define BLINK_TASK_PRIO tskIDLE_PRIORITY + 1
#define WRITE_QUEUE_LENGTH 3


I2C_InitTypeDef I2C_InitStructure;

xQueueHandle USARTQueueHandle;
struct dataset_3d 
{
   int x;
   int y;
   int z;
  
};
typedef struct dataset_3d dataset_3d;

static void SensorStickReadTask(void *pvParameters);
static void QueueReadTestTask(void *pvParameters);
int correct_number_format(int messed_up);



xQueueHandle AccelWriteQueueHandle;
xQueueHandle CompassWriteQueueHandle;
xQueueHandle GyroWriteQueueHandle;
xQueueHandle USARTQueueHandle;


//sensor_data_3d sensor_data_channels[3];


static void QueueReadTestTask(void * pvParameters){


int accel_read_buffer[WRITE_QUEUE_LENGTH];
int compass_read_buffer[WRITE_QUEUE_LENGTH];
static int gyro_read_buffer[WRITE_QUEUE_LENGTH];
uint8_t gyro_trans_buffer[3];
uint8_t compass_trans_buffer[TRANS_BUFFER_LENGTH];
uint8_t accel_trans_buffer[TRANS_BUFFER_LENGTH];

//uint8_t init_buf[16];
int pvarx=0, pvary =0, pvarz =0;
int count=0;
portBASE_TYPE gyroQueueStatus;
portBASE_TYPE accelQueueStatus;
portBASE_TYPE compassQueueStatus;



 for(;;){
  
  STM_EVAL_LEDOn(LED6);
  accelQueueStatus = xQueueReceive(AccelWriteQueueHandle,accel_read_buffer,45);
 compassQueueStatus = xQueueReceive(CompassWriteQueueHandle,compass_read_buffer,45);
  gyroQueueStatus = xQueueReceive(GyroWriteQueueHandle,gyro_read_buffer,45); 
  STM_EVAL_LEDOff(LED6);

 
    if (accelQueueStatus == pdPASS) {

      memset(accel_trans_buffer,0,TRANS_BUFFER_LENGTH);
      pvarx = correct_number_format(accel_read_buffer[0]);
      pvary = correct_number_format(accel_read_buffer[1]);
      pvarz = correct_number_format(accel_read_buffer[2]);
     trace_data("accel x: %d\n",pvarx);
     trace_data("accel y: %d\n",pvary);
     trace_data("accel z: %d\n",pvarz);

      if(accel_trans_buffer != NULL) {
        snprintf(accel_trans_buffer,TRANS_BUFFER_LENGTH,"a %d, %d, %d \n",pvarx, pvary, pvarz);
      }

    }else{
        trace("ERROR receiving from the accel queue\n");
    }
#ifdef USB_OTG
    VCP_send_str( accel_trans_buffer ); 
#else
    USART_putstring(USART2,accel_trans_buffer);
#endif  

    if (compassQueueStatus == pdPASS) {

    memset( compass_trans_buffer,0,TRANS_BUFFER_LENGTH);
    pvarx = correct_number_format(compass_read_buffer[0]);
    pvary = correct_number_format(compass_read_buffer[1]);
    pvarz = correct_number_format(compass_read_buffer[2]);

    trace_data("compass x: %d\n",pvarx);
    trace_data("compass y: %d\n",pvary);
    trace_data("compass z: %d\n",pvarz);
    if( compass_trans_buffer != NULL) {
      snprintf(compass_trans_buffer,TRANS_BUFFER_LENGTH,"c %d, %d, %d \n", pvarx, pvary, pvarz);
    }
  }else{
      debug_printf("ERROR receiving from the compass queue\n");
  }

#ifdef USB_OTG
  VCP_send_str( compass_trans_buffer);
#else
  USART_putstring(USART2, compass_trans_buffer);
#endif 

//
  if(gyroQueueStatus == pdPASS) {

  trace_data("gyro x: %d\n",gyro_read_buffer[0]);
  trace_data("gyro y: %d\n",gyro_read_buffer[1]);
  trace_data("gyro z: %d\n",gyro_read_buffer[2]);

  memset( gyro_trans_buffer,0,TRANS_BUFFER_LENGTH);
  pvarx = correct_number_format(gyro_read_buffer[0]);
  pvary = correct_number_format(gyro_read_buffer[1]);
  pvarz = correct_number_format(gyro_read_buffer[2]);
 
  if(gyro_trans_buffer != NULL) {
    snprintf(gyro_trans_buffer,TRANS_BUFFER_LENGTH,"g %d, %d, %d \n",pvarx, pvary, pvarz);
  }

  }else{
    trace("ERROR receiving from the gyro queue\n");
  }
  #ifdef USB_OTG
  VCP_send_str( gyro_trans_buffer );
  #else
  USART_putstring(USART2, gyro_trans_buffer);
  #endif
  debug_printf ("DEBUG : loop counter : %d \n", count);
  count ++;
 }

}

static void SensorStickReadTask(void * pvparameter)
{
     uint8_t accel_dev_id = 0;
     uint8_t accel_reg_content = 0 ;
     uint8_t accel_data3d[12];
     uint8_t gyro_data3d[6];
     uint8_t compass_data3d[6];
     uint8_t compass_read_content =0;
     uint8_t gyro_readback = 0 ;
     
     portBASE_TYPE gyroQueueStatus;
     portBASE_TYPE accelQueueStatus;
     portBASE_TYPE compassQueueStatus;
       
     int accel_data_2comp[6];
     int compass_data_2comp[3];
     int gyro_data_2comp[3];
     float gyro_degrees[3] = {0.0,0.0,0.0};
     float gyro_temp =0.0;
       
     trace("Task: Sensor Stick readout started\n");      
//        
//     ADXL345_init(I2C1);
//     HMC5883_init(I2C1);
//     ITG3200_init(I2C1);
     
     gyro_readback = ITG3200_read_register(I2C1, ITG3200_ADDRESS, ITG3200_WHO_AM_I);
     debug_printf("gyro id  reg : %d \n", gyro_readback);
         
//     accel_dev_id = ADXL345_read_ack(I2C1, ADXL345_DEVICE_ID_REG, ADXL345_SLAVE_WRITE_ADDR );
//     ADXL345_read_nack(I2C1, ADXL345_DEVICE_ID_REG, ADXL345_SLAVE_WRITE_ADDR );
//     trace_data("ADXL345 device ID :  %d \n",accel_dev_id);

      //trace("starting ADXL345 self test\n");
        //ADXL345_start_selftest();


       for(;;){ 
      

          memset(accel_data_2comp,0,3);
          memset(accel_data3d,0,6);
          memset (compass_data_2comp,0,3);
          memset (compass_data3d,0,6);
          memset (gyro_data_2comp,0,3);
          memset (gyro_data3d,0,6);

       ADXL345_read_burst(I2C1, ADXL345_SLAVE_WRITE_ADDR ,accel_data3d, 6 /*2 datasets*/ );
       HMC5883_read_burst(I2C1, HMC5883_DEVICE_ID_ADDR_WRITE,compass_data3d,6);
       ITG3200_read_burst(I2C1, ITG3200_ADDRESS, gyro_data3d, 6);

       for (int i = 0;i < 3; ++i) {
         accel_data_2comp[i] = (int)accel_data3d[2*i] + (((int)accel_data3d[2*i + 1]) << 8);
         compass_data_2comp[i] = (int)compass_data3d[2*i] + (((int)compass_data3d[2*i + 1]) << 8);
         gyro_data_2comp[i] =  (int)gyro_data3d[2*i] + (((int)gyro_data3d[2*i + 1]) << 8);

          }
          //gyro_temp = ITG3200_read_temp(I2C1, ITG3200_ADDRESS);
          //debug_printf("gyro temp : %f \n",gyro_temp);
         //ITG3200_calc_degrees(gyro_data_2comp, gyro_temp, gyro_degrees);

          
           //  debug_printf("gyro deg x: %f\n",gyro_degrees[0]);
           //  debug_printf("gyro deg y: %f\n",gyro_degrees[1]);
          //  debug_printf("gyro deg z: %f\n",gyro_degrees[2]);

 accelQueueStatus = xQueueSendToBack(AccelWriteQueueHandle,accel_data_2comp,3);
         
        if(accelQueueStatus != pdPASS){
          debug_printf("Error sending on the accel queue\n");

         }

         compassQueueStatus = xQueueSendToBack(CompassWriteQueueHandle,compass_data_2comp,3);
         if (compassQueueStatus  != pdPASS){
          debug_printf("Error sending on the compass queue\n");
         }
       
         gyroQueueStatus = xQueueSendToBack(GyroWriteQueueHandle, gyro_data_2comp, 3);
         
        if(gyroQueueStatus != pdPASS) {
         trace("Error sending on the gyro queue\n");
          }
    taskYIELD();   
    }

}

int main(void)
 {
	/* System Initialization. */
	SystemInit();
	SystemCoreClockUpdate();

	debug_printf("System Core Clock is running at: %dMHz\n",SystemCoreClock/1000000);

	IO_Init();
        I2C1_Init();
        
        Init_USART2();
        debug_printf("USART2 init ok. \n");
        Init_USART3();
        debug_printf("USART3 init ok. \n");

#ifdef USB_OTG

USBD_Init(&USB_OTG_dev,
            USB_OTG_FS_CORE_ID,
            &USR_desc,
            &USBD_CDC_cb,
                &USR_cb);
 
       debug_printf("USB Serial Interface setup completed \n");

#endif
       debug_printf("Completed Initialization \n");

//       debug_printf("INFO: Creating USART  Queue\n");
//       
//       if ( (USARTQueueHandle = xQueueCreate( (unsigned portBASE_TYPE) QUEUE_DATA_CHANNELS, datasize ))  == NULL ){
//          debug_printf("ERROR : Failed to allocate memory for the serial terminal USART queue :-( \n");
//       }

         trace("INFO: Creating Accel Queue\n");
       if ( (AccelWriteQueueHandle = xQueueCreate( (unsigned portBASE_TYPE) WRITE_QUEUE_LENGTH, datasize ))  == NULL ){
        trace("ERROR : Failed to allocate memory for accelerometer write queue :-( \n");
       }
       
         trace("INFO: Creating Compass Queue\n");
       if ( (CompassWriteQueueHandle =  xQueueCreate((unsigned portBASE_TYPE) WRITE_QUEUE_LENGTH,datasize)) == NULL) {
        trace("ERROR : Failed to allocate memory for compass write queue :-( \n");
       }
       trace("INFO: Creating Gyro Queue\n");
       
       if( ( GyroWriteQueueHandle = xQueueCreate((unsigned portBASE_TYPE) WRITE_QUEUE_LENGTH, datasize)) == NULL  ) {
       
        trace("ERROR : Failed to allocate memory for gyro write queue :-( \n");
       }
        
        debug_printf("INFO: Creating tasks \n");
        /// xTaskCreate( ADXL345_readoutTask, (signed char*)"AccelReadOut", 128,NULL, BLINK_TASK_PRIO,NULL);
        //xTaskCreate( ADXL345_readoutTask, (signed char*)"AccelReadOut", 128,(void*) &USARTQueueHandle, BLINK_TASK_PRIO+1,NULL);
        //xTaskCreate( HMC5883_readoutTask, (signed char*)"CompassReadOut", 128,(void*) &USARTQueueHandle, BLINK_TASK_PRIO+1,NULL);
        //xTaskCreate( ITG32000_readoutTask, (signed char*)"CompassReadOut", 128,(void*) &USARTQueueHandle, BLINK_TASK_PRIO+1,NULL);
      //xTaskCreate(SensorStickReadTask,(signed char*)"SensorRead" , 256, NULL, BLINK_TASK_PRIO +1, NULL);
      //xTaskCreate(QueueReadTestTask,(signed char*) "QueueReadTest",128, NULL,BLINK_TASK_PRIO ,NULL);
        //xTaskCreate(USART_WriteQueueTask,(signed char*) "USARTWriteQueue",128, (void*) &USARTQueueHandle,BLINK_TASK_PRIO+1 ,NULL);
       xTaskCreate(GPSReadoutTask, (signed char*)"GPSReadout", 256, NULL, BLINK_TASK_PRIO,NULL);

        debug_printf("FreeRTOS Scheduler started\n");
	vTaskStartScheduler();
        //paranoid ..
        while(1);
}
