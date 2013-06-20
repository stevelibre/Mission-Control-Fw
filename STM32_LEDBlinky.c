
#include "stm32f4xx_gpio.h"

#include "FreeRTOS.h"
#include "task.h"

#include "STM32_LEDBlinky.h"
#include "data_trace.h"


void LED_orange(void *pvParameters)
{
	trace("Task: LED_orange started\n");
    GPIO_ResetBits(GPIOD, GPIO_Pin_13);

	TASK_LOOP {
		GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
		vTaskDelay(500 / portTICK_RATE_MS);
	}
}
//	Green LED (Port D, pin 12)
//
void LED_green(void *pvParameters)
{
	trace("Task: LED_green started\n");
	GPIO_ResetBits(GPIOD, GPIO_Pin_12);

	TASK_LOOP {
		GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
		vTaskDelay(2000 / portTICK_RATE_MS);
	}
}

//
//	Red LED (Port D, pin 13)
//
void LED_red(void *pvParameters)
{
	trace("Task: LED_red started\n");
	GPIO_ResetBits(GPIOD, GPIO_Pin_14);

	TASK_LOOP {
		GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
		vTaskDelay(750 / portTICK_RATE_MS);
	}
}

//
//	Blue LED (Port D, pin 13)
//
void LED_blue(void *pvParameters)
{
	trace("Task: LED_blue started\n");
	GPIO_ResetBits(GPIOD, GPIO_Pin_15);

	TASK_LOOP {
		GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
		vTaskDelay(100 / portTICK_RATE_MS);
	}
}

void IO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}