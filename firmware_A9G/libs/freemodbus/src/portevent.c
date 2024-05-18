/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portevent.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

/* ----------------------- Modbus includes ----------------------------------*/

#include "mb.h"
#include "mbport.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "mbconfig.h"
#include "port.h"
#include <api_inc_os.h> 
#include <api_os.h>
#include <api_event.h>


#if MB_SLAVE_RTU_ENABLED == 0 || MB_SLAVE_ASCII_ENABLED > 0
/* ----------------------- Variables ----------------------------------------*/
static HANDLE xMBEventManager_S = NULL;
/* ----------------------- local implementation -----------------------------*/

static void vMBPortEventTask(void *parameter)
{
    while (true)
    {
        eMBPoll();
    }
}
/* ----------------------- start implementation -----------------------------*/
bool xMBPortEventInit(void)
{
   xMBEventManager_S = OS_CreateTask(vMBPortEventTask, NULL, NULL, 2048, MAX_TASK_PR, 0, 0, "MB_EventManager");
    
    if (xMBEventManager_S != NULL)
    {
        printf("MODBUS-EVENT-->EVENT_MANAGER_INIT");
        return true;
    }
    return false; 
}

bool xMBPortEventPost(eMBEventType eEvent)
{
    bool status = false;
    API_Event_t *event = (API_Event_t *)malloc(sizeof(API_Event_t));
    if (!event)
    {
        printf("MMB no memory");
        return false;
    }
    event->id = eEvent;
    status = OS_SendEvent(xMBEventManager_S, event, OS_WAIT_FOREVER, OS_EVENT_PRI_NORMAL);
    printf("MODBUS-EVENT-POST--> Status(%s)  Event(%d) ", status ? "true" : "false", (int)eEvent);
    return status;
}

bool xMBPortEventGet(eMBEventType *eEvent)
{
    API_Event_t *recvedEvent = NULL;
    OS_WaitEvent(xMBEventManager_S, (void **)&recvedEvent, OS_WAIT_FOREVER);
    /* the enum type couldn't convert to int type */
    *eEvent = (eMBEventType)recvedEvent->id;
    free(recvedEvent->pParam2);
    free(recvedEvent->pParam1);
    free(recvedEvent);
    return true;
}
#endif 

