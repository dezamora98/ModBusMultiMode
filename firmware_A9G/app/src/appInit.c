/**
 * @file app_main.c
 * @author Daniel Enquique Zamora Sifredo (dezamora98@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <app.h>
#include <stdlib.h>
#include <stdio.h>
#include <api_os.h>
#include <api_event.h>

HANDLE os_TaskHandle = NULL;
HANDLE mainTaskHandle = NULL;

void os_task(void *pv)
{
    API_Event_t *event = NULL;

    mainTaskHandle = OS_CreateTask(app, NULL, NULL, 2048, MAX_TASK_PR, 0, 0, "MainTask");

    while (1)
    {
        if (OS_WaitEvent(os_TaskHandle, (void **)&event, OS_TIME_OUT_WAIT_FOREVER))
        {
            EventDispatch(event);
            OS_Free(event->pParam1);
            OS_Free(event->pParam2);
            OS_Free(event);
        }
    }
}

void app_Main(void)
{
    os_TaskHandle = OS_CreateTask(os_task, NULL, NULL, 2048, 0, 0, 0, "os_task");
    OS_SetUserMainHandle(&os_TaskHandle);
}