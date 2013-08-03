#ifndef USART_LIB_H
#define USART_LIB_H


#define QUEUE_SIZE 32
#define HIGH_WATER (QUEUE_SIZE - 6)

struct Queue;


void Init_USART3();
int Enqueue(struct Queue *q, const uint8_t *data, uint16_t len);
int Dequeue(struct Queue *q, uint8_t *data, uint16_t len);

int USART_getchar(void);
int USART_putchar(int);


//size_t uart_write(uint8_t uart, const uint8_t *buf, size_t nbyte);
//size_t uart_read (uint8_t uart, uint8_t *buf, size_t nbyte);


#endif