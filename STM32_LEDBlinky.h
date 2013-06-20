#ifndef _STM32_LED_BLINKY_H
#define _STM32_LED_BLINKY_H

#define	TASK_LOOP	for (;;)

void LED_orange(void *pvParameters);
void LED_green(void *pvParameters);
void LED_red(void *pvParameters);
void LED_blue(void *pvParameters);
void IO_Init(void);

#endif