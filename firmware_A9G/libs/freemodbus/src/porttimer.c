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
 * File: $Id: porttimer.c,v 1.60 2013/08/13 15:07:05 Armink $
 */

/* ----------------------- Platform includes --------------------------------*/

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <api_inc_os.h>
#include <api_os.h>
#include <api_event.h>
/* ----------------------- Modbus includes ----------------------------------*/
#include "port.h"
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"

/* ----------------------- Variables ----------------------------------------*/
static HANDLE vMBTimerTaskHandle = NULL;
static uint32_t usT35TimeOut50us;
static uint32_t TMR_T35;

/* ----------------------- static functions ---------------------------------*/
static void vMBTimerTask(void *parameter)
{
    API_Event_t *event = NULL;

    while (true)
    {
        OS_WaitEvent(vMBTimerTaskHandle, (void **)&event, OS_WAIT_FOREVER);
        free(event->pParam1);
        free(event->pParam2);
        free(event);
    }
}

static void prvvTIMERExpiredISR(void *vp)
{
    (void)pxMBPortCBTimerExpired();
}

/* ----------------------- Start implementation -----------------------------*/
bool xMBPortTimersInit(uint16_t usTimeOut50us)
{
    /* backup T35 ticks */
    vMBTimerTaskHandle = OS_CreateTask(vMBTimerTask, NULL, NULL, 2048, MAX_TASK_PR + 1, 0, 0, "vMBTimerTask");
    if (vMBTimerTaskHandle == NULL)
    {
        return false;
    }
    usT35TimeOut50us = (uint32_t)(usTimeOut50us);
    return true;
}

void vMBPortTimersEnable(void)
{
    TMR_T35 = (uint32_t)((50 * usT35TimeOut50us) / 1000) + 1;
    OS_StartCallbackTimer(vMBTimerTaskHandle, TMR_T35, prvvTIMERExpiredISR, NULL);
}

void vMBPortTimersDisable(void)
{
    OS_StopCallbackTimer(vMBTimerTaskHandle, prvvTIMERExpiredISR, NULL);
}


