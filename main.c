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


/* Private variables ---------------------------------------------------------*/

__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

 __IO uint32_t TimingDelay;


#define BLINK_TASK_PRIO tskIDLE_PRIORITY + 1
#define WRITE_QUEUE_LENGTH 10
#define TRANS_BUFFER_LENGTH 12
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


static void QueueReadTestTask(void * pvParameters){


int accel_read_buffer[WRITE_QUEUE_LENGTH];
int compass_read_buffer[WRITE_QUEUE_LENGTH];
int gyro_read_buffer[WRITE_QUEUE_LENGTH];
uint8_t gyro_trans_buffer[TRANS_BUFFER_LENGTH];
uint8_t compass_trans_buffer[TRANS_BUFFER_LENGTH];
uint8_t accel_trans_buffer[TRANS_BUFFER_LENGTH];

uint8_t init_buf[16];
int pvar=0;
int count=0;

USBD_Init(&USB_OTG_dev,
            USB_OTG_FS_CORE_ID,
            &USR_desc,
            &USBD_CDC_cb,
                &USR_cb);
 
 short len = VCP_get_string(&init_buf[0]);
 if(len)
    VCP_send_str(&init_buf[0]);


 trace("USB Serial Interface setup completed \n");

 for(;;){

  //vTaskDelay(1);

  xQueueReceive(AccelWriteQueueHandle,accel_read_buffer,WRITE_QUEUE_LENGTH);
  xQueueReceive(CompassWriteQueueHandle,compass_read_buffer,WRITE_QUEUE_LENGTH);
  xQueueReceive(GyroWriteQueueHandle,gyro_read_buffer,WRITE_QUEUE_LENGTH); 
 
  trace_data("accel x: %d\n",accel_read_buffer[0]);
  trace_data("accel y: %d\n",accel_read_buffer[1]);
  trace_data("accel z: %d\n",accel_read_buffer[2]);
  memset( accel_trans_buffer,0,TRANS_BUFFER_LENGTH);
  pvar = correct_number_format(accel_read_buffer[0]);
  sprintf(accel_trans_buffer,"ax %d\n",pvar);
  VCP_send_str( accel_trans_buffer );
  memset( accel_trans_buffer,0,TRANS_BUFFER_LENGTH);
  pvar = correct_number_format(accel_read_buffer[1]);
  sprintf(accel_trans_buffer,"ay %d\n",pvar);
  VCP_send_str( accel_trans_buffer );
  memset( accel_trans_buffer,0,TRANS_BUFFER_LENGTH);
  pvar = correct_number_format(accel_read_buffer[2]);
  sprintf(accel_trans_buffer,"az %d\n",pvar);
  VCP_send_str( accel_trans_buffer );

  trace_data("compass x: %d\n",compass_read_buffer[0]);
  trace_data("compass y: %d\n",compass_read_buffer[1]);
  trace_data("compass z: %d\n",compass_read_buffer[2]);

  memset( compass_trans_buffer,0,TRANS_BUFFER_LENGTH);
  pvar = correct_number_format(compass_read_buffer[0]);
  snprintf(compass_trans_buffer,sizeof(compass_trans_buffer),"cx %d\n",pvar);
  VCP_send_str( compass_trans_buffer );
  memset( compass_trans_buffer,0,TRANS_BUFFER_LENGTH);
  pvar = correct_number_format(compass_read_buffer[1]);
  snprintf(compass_trans_buffer,sizeof(compass_trans_buffer),"cy %d\n",pvar);
  VCP_send_str( compass_trans_buffer );
  memset( compass_trans_buffer,0,TRANS_BUFFER_LENGTH);
  pvar = correct_number_format(compass_read_buffer[2]);
  snprintf(compass_trans_buffer,sizeof(compass_trans_buffer),"cz %d\n",pvar);
  VCP_send_str( compass_trans_buffer );
 

  trace_data("gyro x: %d\n",gyro_read_buffer[0]);
  trace_data("gyro y: %d\n",gyro_read_buffer[1]);
  trace_data("gyro z: %d\n",gyro_read_buffer[2]);

  memset( gyro_trans_buffer,0,TRANS_BUFFER_LENGTH);
  pvar = correct_number_format(gyro_read_buffer[0]);
  snprintf(gyro_trans_buffer,sizeof(gyro_trans_buffer),"gx %d\n",pvar);
  VCP_send_str( gyro_trans_buffer );
  memset( gyro_trans_buffer,0, TRANS_BUFFER_LENGTH);
  pvar = correct_number_format(gyro_read_buffer[1]);
  snprintf(gyro_trans_buffer,sizeof(gyro_trans_buffer),"gy %d\n",pvar);
  VCP_send_str( gyro_trans_buffer );
  memset( gyro_trans_buffer,0,TRANS_BUFFER_LENGTH);
  pvar = correct_number_format(gyro_read_buffer[2]);
  snprintf(gyro_trans_buffer,sizeof(gyro_trans_buffer),"gz %d\n",pvar);
  VCP_send_str( gyro_trans_buffer );
  trace_data ("DEBUG : loop counter : %d \n", count);
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
       
     int accel_data_2comp[6];
     int compass_data_2comp[3];
     int gyro_data_2comp[3];
       
     trace("Task: Sensor Stick readout started\n");      
        
     ADXL345_init(I2C1);
     HMC5883_init(I2C1);
     ITG3200_init(I2C1);
       
     gyro_readback = ITG3200_read_register(I2C1,ITG3200_ADDRESS,ITG3200_WHO_AM_I);
     trace_data("gyro id  reg : %d \n", gyro_readback);
         
     accel_dev_id = ADXL345_read_ack(I2C1, ADXL345_DEVICE_ID_REG, ADXL345_SLAVE_WRITE_ADDR );
     ADXL345_read_nack(I2C1, ADXL345_DEVICE_ID_REG, ADXL345_SLAVE_WRITE_ADDR );
     trace_data("ADXL345 device ID :  %d \n",accel_dev_id);




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
 
         xQueueSendToBack(AccelWriteQueueHandle,accel_data_2comp,WRITE_QUEUE_LENGTH);
         xQueueSendToBack(CompassWriteQueueHandle,compass_data_2comp,WRITE_QUEUE_LENGTH);
         xQueueSendToBack(GyroWriteQueueHandle,gyro_data_2comp, WRITE_QUEUE_LENGTH);
       }
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

        debug_printf("Completed Initialization \n");

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

       xTaskCreate(SensorStickReadTask,(signed char*)"SensorRead" , 256, NULL, BLINK_TASK_PRIO +1, NULL);
       xTaskCreate(QueueReadTestTask,(signed char*) "QueueReadTest",128, NULL,BLINK_TASK_PRIO,NULL);
	xTaskCreate(LED_orange, (signed char*)"Orange", 128, NULL,BLINK_TASK_PRIO , NULL);
	//xTaskCreate(LED_green,  (signed char*)"Green",  128, NULL, BLINK_TASK_PRIO, NULL);
	//xTaskCreate(LED_red,    (signed char*)"Red",    128, NULL, BLINK_TASK_PRIO , NULL);
	//xTaskCreate(LED_blue,   (signed char*)"Blue",   128, NULL, BLINK_TASK_PRIO, NULL);
	debug_printf("FreeRTOS Scheduler started\n");
	vTaskStartScheduler();
}


int correct_number_format(int messed_up) {
      
      if(messed_up & TWOS_COMPLEMENT_SIGN_MASK) 
       messed_up -= 65536;
       
       return messed_up;

 }     