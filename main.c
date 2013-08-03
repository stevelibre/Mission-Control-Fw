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
#define TRANS_BUFFER_LENGTH 27 //2
#define TWOS_COMPLEMENT_SIGN_MASK 0x8000

//#define  uint8  short

I2C_InitTypeDef I2C_InitStructure;

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


void GPSReadoutTask(void * pvParameters){
struct gps_data gpsd;
char msg[256];

Init_USART3();
char in =0;
debug_printf("USART3 init ok. \n");

  while(1){

   //while (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);
  //   in = USART_ReceiveData(USART3);
   //   in = USART_getchar();
    //  debug_putchar(in);

        if(EM406A_GPS_sample(&gpsd, msg)) {
            debug_printf("I'm at %f, %f\n", gpsd.longitude, gpsd.latitude);
          } else {
              debug_printf("Oh Dear! No lock :(\n");
          }
    }
}

static void QueueReadTestTask(void * pvParameters){


int accel_read_buffer[WRITE_QUEUE_LENGTH];
int compass_read_buffer[WRITE_QUEUE_LENGTH];
static int gyro_read_buffer[WRITE_QUEUE_LENGTH];
uint8_t gyro_trans_buffer[3];
uint8_t compass_trans_buffer[TRANS_BUFFER_LENGTH];
uint8_t accel_trans_buffer[TRANS_BUFFER_LENGTH];

uint8_t init_buf[16];
int pvarx=0, pvary =0, pvarz =0;
int count=0;
portBASE_TYPE gyroQueueStatus;
portBASE_TYPE accelQueueStatus;
portBASE_TYPE compassQueueStatus;

USBD_Init(&USB_OTG_dev,
            USB_OTG_FS_CORE_ID,
            &USR_desc,
            &USBD_CDC_cb,
                &USR_cb);
 
 trace("USB Serial Interface setup completed \n");

 for(;;){
  
  STM_EVAL_LEDOn(LED6);
 accelQueueStatus = xQueueReceive(AccelWriteQueueHandle,accel_read_buffer,45);
 compassQueueStatus = xQueueReceive(CompassWriteQueueHandle,compass_read_buffer,45);
 // gyroQueueStatus = xQueueReceive(GyroWriteQueueHandle,gyro_read_buffer,45); 
  STM_EVAL_LEDOff(LED6);

 
    if (accelQueueStatus == pdPASS) {

      memset(accel_trans_buffer,0,TRANS_BUFFER_LENGTH);
      pvarx = correct_number_format(accel_read_buffer[0]);
      pvary = correct_number_format(accel_read_buffer[1]);
      pvarz = correct_number_format(accel_read_buffer[2]);
      debug_printf("accel x: %d\n",pvarx);
      debug_printf("accel y: %d\n",pvary);
      debug_printf("accel z: %d\n",pvarz);

      if(accel_trans_buffer != NULL) {
        snprintf(accel_trans_buffer,TRANS_BUFFER_LENGTH,"a %d, %d, %d \n",pvarx, pvary, pvarz);
      }

    }else{
        trace("ERROR receiving from the accel queue\n");
    }
    VCP_send_str( accel_trans_buffer ); 
   

    if (compassQueueStatus == pdPASS) {

    memset( compass_trans_buffer,0,TRANS_BUFFER_LENGTH);
    pvarx = correct_number_format(compass_read_buffer[0]);
    pvary = correct_number_format(compass_read_buffer[1]);
    pvarz = correct_number_format(compass_read_buffer[2]);

    debug_printf("compass x: %d\n",pvarx);
    debug_printf("compass y: %d\n",pvary);
    debug_printf("compass z: %d\n",pvarz);
    if( compass_trans_buffer != NULL) {
      snprintf(compass_trans_buffer,TRANS_BUFFER_LENGTH,"c %d, %d, %d \n", pvarx, pvary, pvarz);
    }
  }else{
      debug_printf("ERROR receiving from the compass queue\n");
  }
  VCP_send_str( compass_trans_buffer);

//
//  if(gyroQueueStatus == pdPASS) {
//
//  trace_data("gyro x: %d\n",gyro_read_buffer[0]);
//  trace_data("gyro y: %d\n",gyro_read_buffer[1]);
//  trace_data("gyro z: %d\n",gyro_read_buffer[2]);
//
//  memset( gyro_trans_buffer,0,TRANS_BUFFER_LENGTH);
//  pvarx = correct_number_format(gyro_read_buffer[0]);
//  pvary = correct_number_format(gyro_read_buffer[1]);
//  pvarz = correct_number_format(gyro_read_buffer[2]);
// 
//  if(gyro_trans_buffer != NULL) {
//    snprintf(gyro_trans_buffer,TRANS_BUFFER_LENGTH,"g %d, %d, %d \n",pvarx, pvary, pvarz);
//  }
//
//  }else{
//    trace("ERROR receiving from the gyro queue\n");
//  }
//  VCP_send_str( gyro_trans_buffer );
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
        
     ADXL345_init(I2C1);
     HMC5883_init(I2C1);
     ITG3200_init(I2C1);
     
     gyro_readback = ITG3200_read_register(I2C1, ITG3200_ADDRESS, ITG3200_WHO_AM_I);
     trace_data("gyro id  reg : %d \n", gyro_readback);
         
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
      // ITG3200_read_burst(I2C1, ITG3200_ADDRESS, gyro_data3d, 6);

       for (int i = 0;i < 3; ++i) {
         accel_data_2comp[i] = (int)accel_data3d[2*i] + (((int)accel_data3d[2*i + 1]) << 8);
         compass_data_2comp[i] = (int)compass_data3d[2*i] + (((int)compass_data3d[2*i + 1]) << 8);
        //   gyro_data_2comp[i] =  (int)gyro_data3d[2*i] + (((int)gyro_data3d[2*i + 1]) << 8);

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
       
      //   gyroQueueStatus = xQueueSendToBack(GyroWriteQueueHandle, gyro_data_2comp, 3);
         
        // if(gyroQueueStatus != pdPASS) {
         // trace("Error sending on the gyro queue\n");
         //}
    taskYIELD();   
    }

}

int main(void)
 {
        
        portBASE_TYPE datasize = 3 * sizeof(int); 
  
	/* System Initialization. */
	SystemInit();
	SystemCoreClockUpdate();

	debug_printf("System Core Clock is running at: %dMHz\n",SystemCoreClock/1000000);

	IO_Init();
        I2C1_Init();

      // ADXL345_detect_freefall(I2C1);

       // debug_printf("Completed Initialization \n");

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
        
        trace("INFO: Creating tasks \n");

      // xTaskCreate(SensorStickReadTask,(signed char*)"SensorRead" , 256, NULL, BLINK_TASK_PRIO +1, NULL);
       // xTaskCreate(QueueReadTestTask,(signed char*) "QueueReadTest",128, NULL,BLINK_TASK_PRIO ,NULL);
       xTaskCreate(LED_orange, (signed char*)"Orange", 128, NULL,BLINK_TASK_PRIO , NULL);
	//xTaskCreate(LED_green,  (signed char*)"Green",  128, NULL, BLINK_TASK_PRIO, NULL);
	//xTaskCreate(LED_red,    (signed char*)"Red",    128, NULL, BLINK_TASK_PRIO , NULL);
	xTaskCreate(GPSReadoutTask, (signed char*)"GPSReadout", 256, NULL, BLINK_TASK_PRIO,NULL);
        debug_printf("FreeRTOS Scheduler started\n");
	vTaskStartScheduler();
        while(1);
}


int correct_number_format(int messed_up) {
      
      if(messed_up & TWOS_COMPLEMENT_SIGN_MASK) 
       messed_up -= 65536;
       
       return messed_up;

 }     