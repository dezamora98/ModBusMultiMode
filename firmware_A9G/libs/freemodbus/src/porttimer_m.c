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
#include "port.h"
#include <stdbool.h>
#include <stdint.h>
#include <api_inc_os.h>
#include <api_os.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Variables ----------------------------------------*/
static HANDLE TimertaskHandle;
static uint32_t usT35TimeOut50us;
static void prvvTIMERExpiredISR(void);
static void timer_timeout_ind(void *parameter);

/*------------------------local_implementation-------------------------------*/
static void vMBTIMERTask(void *vp)
{
    while (1)
    {
        OS_Sleep(OS_WAIT_FOREVER);
    }
}
/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR(void);
/* ----------------------- Start implementation -----------------------------*/
bool xMBMasterPortTimersInit(uint16_t usTimeOut50us)
{
    /* backup T35 ticks */
    TimertaskHandle = OS_CreateTask(vMBTIMERTask, NULL, NULL, 2048, 2, 0, 0, "MB-Timer");
    usT35TimeOut50us = (uint32_t)(usTimeOut50us);
    return true;
}

void vMBMasterPortTimersT35Enable()
{
    uint32_t timer_tick = (uint32_t)((50 * usT35TimeOut50us) / 1000) + 1; // aproximación a 1 ms.

    /* Set current timer mode, don't change it.*/
    vMBMasterSetCurTimerMode(MB_TMODE_T35);
    OS_StartCallbackTimer(TimertaskHandle, timer_tick, timer_timeout_ind, NULL);
}

void vMBMasterPortTimersConvertDelayEnable()
{
    /* Set current timer mode, don't change it.*/
    vMBMasterSetCurTimerMode(MB_TMODE_CONVERT_DELAY);
    OS_StartCallbackTimer(TimertaskHandle, MB_MASTER_DELAY_MS_CONVERT, timer_timeout_ind, NULL);
}

void vMBMasterPortTimersRespondTimeoutEnable()
{
    /* Set current timer mode, don't change it.*/
    vMBMasterSetCurTimerMode(MB_TMODE_RESPOND_TIMEOUT);
    OS_StartCallbackTimer(TimertaskHandle, MB_MASTER_TIMEOUT_MS_RESPOND, timer_timeout_ind, NULL);
}

void vMBMasterPortTimersDisable(void)
{
    OS_StopCallbackTimer(TimertaskHandle, timer_timeout_ind, NULL);
}

void prvvTIMERExpiredISR(void)
{
    (void)pxMBMasterPortCBTimerExpired();
}

static void timer_timeout_ind(void *parameter)
{
    prvvTIMERExpiredISR();
}

#endif
