#ifndef EM_406A_GPS_H
#define EM_406A_GPS_H


struct gps_data{
  float longitude;
  float latitude;

} em406a_data;

void readFromEM406A_GPS();
void writeToEM406A_GPS(char * comm);

int EM406A_GPS_sample(USART_TypeDef*, struct gps_data *, char *);
void EM406A_GPS_getline(USART_TypeDef*, char *);



#endif