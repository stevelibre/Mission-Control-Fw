
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
    float time = 0.0;
    char ns, ew;
    int lock = 0;
    float longitude = 0.0;
    float latitude = 0.0;
    
 
   // while(1) {  
       memset(msg,0,256);
       EM406A_GPS_getline(USARTx,msg);
        // Check if it is a GPGGA msg (matches both locked and non-locked msg)
        if(sscanf(msg, "GPGGA,%f,%f,%c,%f,%c,%d", &time, latitude, &ns, &longitude, &ew, &lock) >= 1) { 
            if(!lock) {
                gpsd->longitude = 0.0;
                gpsd->latitude = 0.0;        
                return 0;
            } else {
                if(ns == 'S') {    latitude  *= -1.0; }
                if(ew == 'W') {    longitude *= -1.0; }
                float degrees = latitude / 100.0f;
                float minutes = latitude - (degrees * 100.0f);
                gpsd->latitude = degrees + minutes / 60.0f;    
                degrees = longitude / 100.0f * 0.01f;
                minutes = longitude - (degrees * 100.0f);
                gpsd->longitude = degrees + minutes / 60.0f;
                return 1;
            }
        }
    //}
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







