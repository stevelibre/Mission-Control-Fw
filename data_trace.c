#include "FreeRTOS.h"
#include "task.h"

#include "data_trace.h"


void trace(const char* str)
{
	vTaskSuspendAll();
	debug_printf(str);
	xTaskResumeAll();
}
void trace_data(const char* str, const short data)
{
	vTaskSuspendAll();
	debug_printf(str,data);
	xTaskResumeAll();
}
