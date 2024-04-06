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

HANDLE osTaskHandle = NULL;
HANDLE mainTaskHandle = NULL;

HANDLE getOsTaskHandle(void)
{
    return osTaskHandle;
}

HANDLE getMainTaskHandle(void)
{
    return mainTaskHandle;
}

static void OSTask(void *pv)
{
    API_Event_t *event = NULL;

    mainTaskHandle = OS_CreateTask(app, NULL, NULL, 2048, MAX_TASK_PR, 0, 0, "app");

    while (true)
    {
        if (OS_WaitEvent(osTaskHandle, (void **)&event, OS_TIME_OUT_WAIT_FOREVER))
        {
            EventDispatch(event);
            free(event->pParam1);
            free(event->pParam2);
            free(event);
        }
    }
}

void app_Main(void)
{
    osTaskHandle = OS_CreateTask(OSTask, NULL, NULL, 2048, MAX_TASK_PR - 1, 0, 0, "OSTask");
    OS_SetUserMainHandle(&osTaskHandle);
}