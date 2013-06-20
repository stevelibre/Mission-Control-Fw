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


#include "STM32_LEDBlinky.h"
#include "data_trace.h"

#include "I2C.h"
#include "ADXL345.h"
#include "HMC5883L.h"
#include "ITG3200.h"


#define BLINK_TASK_PRIO tskIDLE_PRIORITY + 1
#define WRITE_QUEUE_LENGTH 8

#define  uint8  short

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

xQueueHandle AccelWriteQueueHandle;
xQueueHandle CompassWriteQueueHandle;
xQueueHandle GyroWriteQueueHandle;


static void QueueReadTestTask(void * pvParameters){
int accel_read_buffer[3];
int compass_read_buffer[3];
for(;;){

 xQueueReceive(AccelWriteQueueHandle,accel_read_buffer,20);
 xQueueReceive(CompassWriteQueueHandle,compass_read_buffer,20);
 
 trace_data("accel x: %d\n",accel_read_buffer[0]);
 trace_data("accel y: %d\n",accel_read_buffer[1]);
 trace_data("accel z: %d\n",accel_read_buffer[2]);
 trace_data("compass x: %d\n",compass_read_buffer[0]);
 trace_data("compass y: %d\n",compass_read_buffer[1]);
 trace_data("compass z: %d\n",compass_read_buffer[2]);
 }

}

static void SensorStickReadTask(void * pvparameter)
{
       uint8_t accel_dev_id = 0;
       uint8_t accel_reg_content = 0 ;
       uint8_t accel_data3d[12];
       uint8_t compass_read_content =0;
       uint8_t gyro_readback = 0 ;
       
     trace("Task: Sensor Stick readout started\n");      
        
     ADXL345_init(I2C1);
      //HMC5883_init(I2C1);
      //compass_read_content = HMC5883_read_register(I2C1, HMC5883_DEVICE_ID_ADDR_WRITE, HMC5883_CONFIGURATION_REG_A );
      // trace_data("compass config reg: %d\n", compass_read_content);
      ITG3200_init(I2C1);
      
        
       
       gyro_readback = ITG3200_read_register(I2C1,ITG3200_ADDRESS,ITG3200_WHO_AM_I);
       trace_data("gyro reg : %d \n", gyro_readback);
         
     //accel_dev_id = ADXL345_read_ack(I2C1, ADXL345_DEVICE_ID_REG, ADXL345_SLAVE_WRITE_ADDR );
     //ADXL345_read_nack(I2C1, ADXL345_DEVICE_ID_REG, ADXL345_SLAVE_WRITE_ADDR );
     //trace_data("Accelerometer device ID :  %d \n",accel_dev_id);
//
//       trace_data("Trying to write and read back a register (content) : %d\n",ADXL345_FIFO_MODE(ADXL345_FIFO_STREAM));
//      
//       ADXL345_write_register(I2C1, ADXL345_FIFO_CONTROL_REG,ADXL345_SLAVE_WRITE_ADDR,ADXL345_FIFO_MODE(ADXL345_FIFO_STREAM) );
//       accel_reg_content =  ADXL345_read_ack(I2C1, ADXL345_FIFO_CONTROL_REG, ADXL345_SLAVE_WRITE_ADDR );
//       ADXL345_read_nack(I2C1, ADXL345_FIFO_CONTROL_REG, ADXL345_SLAVE_WRITE_ADDR );
//       trace_data("read back : %d\n",accel_reg_content);
     // int accel_data_2comp[6];
     // int compass_data_2comp[3];
    int gyro_data_2comp[3];
      //uint8_t compass_data3d[6];

      //trace("starting ADXL345 self test\n");
        //ADXL345_start_selftest();

//        for(;;){
//
//          memset(accel_data_2comp,0,3);
//          memset(accel_data3d,0,6);
//          memset (compass_data_2comp,0,3);
//          memset (compass_data3d,0,6);
          //memset (gyro_data_2comp,0,3);
          //memset (gyro_data3d,0,6);

    //      ADXL345_read_burst(I2C1, ADXL345_SLAVE_WRITE_ADDR ,accel_data3d, 6 /*2 datasets*/ );
     //     HMC5883_read_burst(I2C1, HMC5883_DEVICE_ID_ADDR_WRITE,compass_data3d,6);
          //ITG32000_read_burst(I2C1, ITG3200_ADDRESS, 

//          for(int i=0; i < 6; i++){    
//              trace_data(" val: %d\n",accel_data3d[i] );
//          }
          
//          trace("displaying current buffer\n");
//
       //for (int i = 0;i < 3; ++i) {
       //   accel_data_2comp[i] = (int)accel_data3d[2*i] + (((int)accel_data3d[2*i + 1]) << 8);
        //  compass_data_2comp[i] = (int)compass_data3d[2*i] + (((int)compass_data3d[2*i + 1]) << 8);
          //gyro_data_2comp[i] =  (int)gyro_data3d[2*i] + (((int)gyro_data3d[2*i + 1]) << 8);
 
       
          //xQueueSendToBack(AccelWriteQueueHandle,accel_data_2comp,10);
          //xQueueSendToBack(CompassWriteQueueHandle,compass_data_2comp,10);
          //xQueueSendToBack(GyroWriteQueueHandle,gyro_data_2comp, 10);


//            trace_data("2 comp accel val: %d\n", accel_data_2comp[i]);
         //     trace_data("2 comp compass val: %d\n", compass_data_2comp[i]);
         //}
       // }
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
//
//         trace("INFO: Creating Accel Queue\n");
//       if ( (AccelWriteQueueHandle = xQueueCreate( (unsigned portBASE_TYPE) WRITE_QUEUE_LENGTH, datasize ))  == NULL ){
//        trace("ERROR : Failed to allocate memory for accelerometer write queue :-( \n");
//       }
//       
//         trace("INFO: Creating Compass Queue\n");
//       if ( (CompassWriteQueueHandle =  xQueueCreate((unsigned portBASE_TYPE) WRITE_QUEUE_LENGTH,datasize)) == NULL) {
//        trace("ERROR : Failed to allocate memory for compass write queue :-( \n");
//       }
      // trace("INFO: Creating Gyro Queue\n");
       //if( ( GyroWriteQueueHandle = xQueueCreate((unsigned portBASE_TYPE) WRITE_QUEUE_LENGTH, datasize)) == NULL  ) {
       // trace("ERROR : Failed to allocate memory for gyro write queue :-( \n");
       //}
        
        trace("INFO: Creating tasks \n");

        xTaskCreate(SensorStickReadTask,(signed char*)"SensorRead" , 512, NULL, BLINK_TASK_PRIO +1, NULL);
        //xTaskCreate(QueueReadTestTask,(signed char*) "QueueReadTest",1024, NULL,BLINK_TASK_PRIO,NULL);
	xTaskCreate(LED_orange, (signed char*)"Orange", 128, NULL,BLINK_TASK_PRIO , NULL);
	//xTaskCreate(LED_green,  (signed char*)"Green",  128, NULL, BLINK_TASK_PRIO, NULL);
	//xTaskCreate(LED_red,    (signed char*)"Red",    128, NULL, BLINK_TASK_PRIO , NULL);
	//xTaskCreate(LED_blue,   (signed char*)"Blue",   128, NULL, BLINK_TASK_PRIO, NULL);
	debug_printf("FreeRTOS Scheduler started\n");
	vTaskStartScheduler();
}