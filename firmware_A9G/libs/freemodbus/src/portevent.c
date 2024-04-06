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
#include <stdint.h>
#include "mbconfig.h"
#include "port.h"
#include <api_inc_os.h> 
#include <api_os.h>
#include <api_event.h>

#ifdef MB_SLAVE
#endif //MB_SLAVE
#if MB_SLAVE_RTU_ENABLED == 0 || MB_SLAVE_ASCII_ENABLED > 0
/* ----------------------- Variables ----------------------------------------*/
static HANDLE xMBEventManager_S = NULL;
/* ----------------------- local implementation -----------------------------*/

bool xMBPortEventPost(eMBEventType eEvent);
static void vMBPortEventManager(void *parameter)
{
    while (true)
    {
        OS_Sleep(OS_WAIT_FOREVER);
    }
}
/* ----------------------- start implementation -----------------------------*/
bool xMBPortEventInit( void )
{
   xMBEventManager_S = OS_CreateTask(vMBPortEventManager, NULL, NULL, 2048, MAX_TASK_PR, 0, 0, "MB_EventManager");
    
    if (xMBEventManager_S != NULL)
    {
        printf("MODBUS-EVENT-->EVENT_MANAGER_INIT");
        return true;
    }
    return false; 
}

bool xMBMasterPortEventPost(eMBMasterEventType eEvent)
{
    bool status = false;
    eMBEventType *event = (eMBEventType *)malloc(sizeof(eMBEventType));
    if (!event)
    {
        printf("MMB no memory");
        return false;
    }
    *event = eEvent;
    status = OS_SendEvent(xMBEventManager_S, event, OS_WAIT_FOREVER, OS_EVENT_PRI_NORMAL);
    printf("MODBUS-EVENT-POST--> Status(%s)  Event(%d) ", status ? "true" : "false", (int)eEvent);
    return status;
}

bool xMBPortEventGet(eMBEventType *eEvent)
{
    eMBEventType *recvedEvent = NULL;

    OS_WaitEvent(xMBEventManager_S, (void **)&recvedEvent, OS_WAIT_FOREVER);
    if ((*recvedEvent & (EV_READY | EV_FRAME_RECEIVED | EV_EXECUTE |
                         EV_FRAME_SENT)) == 0)
    {
        printf("MODBUS-->NO EVENT");
        return false;
    }
    /* the enum type couldn't convert to int type */
    switch (*recvedEvent)
    {
    case EV_READY:
        *eEvent = EV_READY;
        break;
    case EV_EXECUTE:
        *eEvent = EV_EXECUTE;
        break;
    case EV_FRAME_SENT:
        *eEvent = EV_FRAME_SENT;
        break;
    case EV_FRAME_RECEIVED:
        *eEvent = EV_FRAME_RECEIVED;
    default:
        break;
    }

    free(recvedEvent);
    return true;
}
#endif 
