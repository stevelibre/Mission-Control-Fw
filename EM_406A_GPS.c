
#include <cross_studio_io.h>
#include<math.h>
#include <stdio.h>
#include <string.h>

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "usart_lib.h"
#include "EM_406A_GPS.h"

//void readFromEM406A_GPS(){
//
//}
//void writeToEM406A_GPS(char * comm);


//char msg[256];

int EM406A_GPS_sample( USART_TypeDef* USARTx, struct gps_data * gpsd, char * msg) {
    double time = 0.0;
    char ns, ew;
    int lock = 0;
    float longitude = 0.000;
    float latitude = 0.000;

    float foo = 123.666 ;
    float bar = 456.777;
    float baz = 0.000;
    float fuz = 0.000;
    char flag = 'W';
    char flago = 0;
   // char mType[64];
    //char gpgga[5] ="GPGGA";
   short err = 0;

  // latitude = 182907.136;
 //  memset(mType,0,64);
   //sprintf(mType,"GPGGA:%.3f,%.3f,%.3f,%c,0,0",latitude,foo,bar,flag);
  char mType[] = "GPGGA,211235.134,,,,,0,00,,,M,0.0,M,,0000*56";
   debug_printf("%s\n",mType);
   sscanf(mType,"GPGGA,%f,%f,%c,%f",&longitude,&baz,&flago,&fuz);
   debug_printf("fp result: %.3f,%.3f,%c,%.3f\n",longitude,baz,flago,fuz);
 debug_printf("\n");

       memset(mType,0,6);
       memset(msg,0,256);
       EM406A_GPS_getline(USARTx,msg);
       if(msg[0] == '$') msg++;
        // Check if it is a GPGGA msg (matches both locked and non-locked msg)
        if(sscanf(msg, "%s,%f,%f,%c,%f,%c,%d",mType, &time, &latitude, &ns, &longitude, &ew, &lock) >= 1) { 

            if((mType[0] =='G') && (mType[1] =='P') && (mType[2] =='G') && (mType[3] =='G') && (mType[4] =='A')){
              
              if(!lock) {
                  gpsd->longitude = 0.0;
                  gpsd->latitude = 0.0;        
                 return 0;
              } else {
                 if(ns == 'S') {    latitude  *= -1.0; }
                 if(ew == 'W') {    longitude *= -1.0; }
                 float degrees = GPS_trunc(latitude / 100.0f);
                 float minutes = latitude - (degrees * 100.0f);
                  gpsd->latitude = degrees + minutes / 60.0f;    
                  degrees = GPS_trunc(longitude / 100.0f * 0.01f);
                  minutes = longitude - (degrees * 100.0f);
                  gpsd->longitude = degrees + minutes / 60.0f;
                  gpsd->timestamp = time;
                  return 1;
              }
            }
        }
}

void EM406A_GPS_getline(USART_TypeDef* USARTx, char *msg) {
    while(USART_getchar(USARTx) != '$');    // wait for the start of a line
    for(int i=0; i < 256; i++) {
        msg[i] = USART_getchar(USARTx);
        if(msg[i] == '\r') {
            msg[i] = 0;
            debug_printf("%s\n",msg);
            return;
        }
    }
    debug_printf("Overflowed message limit");
}


void GPSReadoutTask(void * pvParameters){
struct gps_data gpsd;
char msg[256];
char in = 0;
char strg[128];
int err;

  while(1){
   
        if(EM406A_GPS_sample(USART3,&gpsd, msg) >=1) {
            memset(strg,0,128);
            sprintf(strg," %f I'm at %f, %f\n",gpsd.timestamp, gpsd.longitude, gpsd.latitude);
            USART_putstring(USART2,strg);
          } else {
            USART_putstring(USART2," No lock on GPS\n");
          }
    }
}


float GPS_trunc(float v) {
    if(v < 0.0) {
        v*= -1.0;
        v = floor(v);
        v*=-1.0;
    } else {
        v = floor(v);
    }
    return v;
}






