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
 * File: $Id: porttimer_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
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

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Variables ----------------------------------------*/
static HANDLE TimertaskHandle;
static uint32_t usT35TimeOut50us;

static uint32_t TMR_T35;
static uint32_t TMR_DELAY;
static uint32_t TMR_TIMEOUT;
/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR(void *parameter);

/*------------------------local_implementation-------------------------------*/
static void vMBTIMERTask(void *vp)
{
    API_Event_t *event = NULL;

    printf("MODBUS--> vMBTIMERTask--INIT");

    while (1)
    {
        if (OS_WaitEvent(TimertaskHandle, (void **)&event, OS_TIME_OUT_WAIT_FOREVER))
        {
            free(event->pParam1);
            free(event->pParam2);
            free(event);
        }
    }
}

/* ----------------------- Start implementation -----------------------------*/
bool xMBMasterPortTimersInit(uint16_t usTimeOut50us)
{
    /* backup T35 ticks */
    TimertaskHandle = OS_CreateTask(vMBTIMERTask, NULL, NULL, 2048, MAX_TASK_PR, 0, 0, "MB-Timer");
    usT35TimeOut50us = (uint32_t)(usTimeOut50us);
    return true;
}

void vMBMasterPortTimersT35Enable()
{
    TMR_T35 = (uint32_t)((50 * usT35TimeOut50us) / 1000) + 1;
    /* Set current timer mode, don't change it.*/
    vMBMasterSetCurTimerMode(MB_TMODE_T35);
    OS_StartCallbackTimer(TimertaskHandle, TMR_T35, prvvTIMERExpiredISR, &TMR_T35);
    printf("MODBUS --> vMBMasterPortTimersT35Enable");
}

void vMBMasterPortTimersConvertDelayEnable()
{
    TMR_DELAY = MB_MASTER_DELAY_MS_CONVERT;
    /* Set current timer mode, don't change it.*/
    vMBMasterSetCurTimerMode(MB_TMODE_CONVERT_DELAY);
    OS_StartCallbackTimer(TimertaskHandle, MB_MASTER_DELAY_MS_CONVERT, prvvTIMERExpiredISR, &TMR_DELAY);
}

void vMBMasterPortTimersRespondTimeoutEnable()
{
    TMR_TIMEOUT = MB_TMODE_RESPOND_TIMEOUT;
    /* Set current timer mode, don't change it.*/
    vMBMasterSetCurTimerMode(MB_TMODE_RESPOND_TIMEOUT);
    OS_StartCallbackTimer(TimertaskHandle, MB_MASTER_TIMEOUT_MS_RESPOND, prvvTIMERExpiredISR, &TMR_TIMEOUT);
}

void vMBMasterPortTimersDisable(void)
{
    OS_StopCallbackTimer(TimertaskHandle, prvvTIMERExpiredISR, NULL);
}

void prvvTIMERExpiredISR(void *vp)
{
    (void)pxMBMasterPortCBTimerExpired();
    //OS_StartCallbackTimer(TimertaskHandle, *((uint32_t *)vp), prvvTIMERExpiredISR, vp);
    //printf("TMR reload");
}

#endif
