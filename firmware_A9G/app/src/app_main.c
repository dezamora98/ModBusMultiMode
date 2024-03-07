/*
 * @File  app_main.c
 * @Brief An example of SDK's mini system
 *
 * @Author: Neucrack
 * @Date: 2017-11-11 16:45:17
 * @Last Modified by: Neucrack
 * @Last Modified time: 2017-11-11 18:24:56
 */
#include "stdint.h"
#include "stdbool.h"
#include "api_os.h"
#include "api_event.h"
#include "api_debug.h"

#define AppMain_TASK_STACK_SIZE (1024 * 2)
#define AppMain_TASK_PRIORITY 1
HANDLE mainTaskHandle = NULL;

#include "main.c"

void app_Main(void)
{

    API_Event_t *event = NULL;

    mainTaskHandle = OS_CreateTask(MainTask,
                                   NULL, NULL, AppMain_TASK_STACK_SIZE, AppMain_TASK_PRIORITY, 0, 0, "MainTask");
    OS_SetUserMainHandle(&mainTaskHandle);

    while (1)
    {
        if (OS_WaitEvent(mainTaskHandle, &event, OS_TIME_OUT_WAIT_FOREVER))
        {
            EventDispatch(event);
            OS_Free(event->pParam1);
            OS_Free(event->pParam2);
            OS_Free(event);
        }
    }
}