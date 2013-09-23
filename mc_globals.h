#ifndef MC_GLOBALS_H
#define MC_GLOBALS_H

#include <cross_studio_io.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#define TRANS_BUFFER_LENGTH 27
#define QUEUE_DATA_CHANNELS 3

//static char sensor_trans_buffer[QUEUE_DATA_CHANNELS][TRANS_BUFFER_LENGTH];
static char sensor_trans_buffer[TRANS_BUFFER_LENGTH];
static const portBASE_TYPE datasize = TRANS_BUFFER_LENGTH * sizeof(char);
extern xQueueHandle USARTQueueHandle;

struct SensorData3D{
  unsigned char sensor;
  int xval;
  int yval;
  int zval;
  }sensor_data_3d;

//typedef struct sensor_data_3d sensor_data_3d;




#endif