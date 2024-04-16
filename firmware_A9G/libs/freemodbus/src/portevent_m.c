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
 * File: $Id: portevent_m.c v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <api_inc_os.h>
#include <api_os.h>
#include <api_event.h>

#include "mb.h"
#include "mb_m.h"
#include "mbport.h"
#include "port.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
#define M_MODBUS API_EVENT_ID_MAX + 1

/* ----------------------- Defines ------------------------------------------*/
/* ----------------------- Variables ----------------------------------------*/
static eMBMasterReqErrCode eErrStatus = MB_MRE_NO_ERR;
static HANDLE vMBMasterEventTaskHandle = NULL;
static HANDLE xMasterRunRes = NULL;
static HANDLE xRequestFinish = NULL;
/*-----------------------------static----------------------------------------*/


static void vMBMasterEventTask(void *parameter)
{
    while (true)
    {
        eMBMasterPoll();
    }
}

/* ----------------------- Start implementation -----------------------------*/
bool xMBMasterPortEventInit(void)
{
    vMBMasterEventTaskHandle = OS_CreateTask(vMBMasterEventTask, NULL, NULL, 2048, MAX_TASK_PR + 1, 0, 0, "vMBMasterEventTaskHandle");


    if (vMBMasterEventTaskHandle != NULL)
    {
        printf("MB -> MASTER_EVENT_MANAGER_INIT");
        return true;
    }
    return false;
}

/**
 * @brief Función para enviar eventos (por ahora se coloca como urgente, pero puede que no sea necesario)
 *
 * @param eEvent
 * @return bool
 */
bool xMBMasterPortEventPost(eMBMasterEventType eEvent)
{
    bool status = false;
    API_Event_t *event = (API_Event_t *)malloc(sizeof(API_Event_t));
    if (!event)
    {
        return false;
    }
    event->id = eEvent;
    status = OS_SendEvent(vMBMasterEventTaskHandle, event, OS_WAIT_FOREVER, OS_EVENT_PRI_NORMAL);
    return status;
}

bool xMBMasterPortEventGet(eMBMasterEventType *eEvent)
{
    API_Event_t *recvedEvent = NULL;
    OS_WaitEvent(vMBMasterEventTaskHandle, (void **)&recvedEvent, OS_WAIT_FOREVER);
    /* the enum type couldn't convert to int type */
    *eEvent = (eMBMasterEventType)recvedEvent->id;
    free(recvedEvent->pParam2);
    free(recvedEvent->pParam1);
    free(recvedEvent);
    return true;
}

/**
 * This function is initialize the OS resource for modbus master.
 * Note:The resource is define by OS.If you not use OS this function can be empty.
 *
 */
void vMBMasterOsResInit(void)
{
    xMasterRunRes = OS_CreateSemaphore(1);
    xRequestFinish = OS_CreateSemaphore(0);
}

/**
 * This function is take Mobus Master running resource.
 * Note:The resource is define by Operating System.If you not use OS this function can be just return true.
 *
 * @param lTimeOut the waiting time.
 *
 * @return resource taked result
 */
bool xMBMasterRunResTake(uint32_t lTimeOut)
{
    /*If waiting time is -1 .It will wait forever */
    return OS_WaitForSemaphore(xMasterRunRes, lTimeOut);
}

/**
 * This function is release Mobus Master running resource.
 * Note:The resource is define by Operating System.If you not use OS this function can be empty.
 *
 */
void vMBMasterRunResRelease(void)
{
    /* release resource */
    OS_ReleaseSemaphore(xMasterRunRes);
}
/**
 * This is modbus master respond timeout error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *x
 */
void vMBMasterErrorCBRespondTimeout(uint8_t ucDestAddress, const uint8_t *pucPDUData,
                                    uint16_t ucPDULength)
{
    /**
     * @note This code is use OS's event mechanism for modbus master protocol stack.
     * If you don't use OS, you can change it.
     */
    eErrStatus = MB_MRE_TIMEDOUT;
    OS_ReleaseSemaphore(xRequestFinish);
    /* You can add your code under here. */
    printf("MB -> EV_MASTER_ERROR_RESPOND_TIMEOUT:\n");
}

/**
 * This is modbus master receive data error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBReceiveData(uint8_t ucDestAddress, const uint8_t *pucPDUData,
                                 uint16_t ucPDULength)
{
    /**
     * @note This code is use OS's event mechanism for modbus master protocol stack.
     * If you don't use OS, you can change it.
     */
    eErrStatus = MB_MRE_REV_DATA;
    OS_ReleaseSemaphore(xRequestFinish);
    /* You can add your code under here. */
    printf("MB -> EV_MASTER_ERROR_RECEIVE_DATA:\n");
}

/**
 * This is modbus master execute function error process callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 * @param ucDestAddress destination salve address
 * @param pucPDUData PDU buffer data
 * @param ucPDULength PDU buffer length
 *
 */
void vMBMasterErrorCBExecuteFunction(uint8_t ucDestAddress, const uint8_t *pucPDUData,
                                     uint16_t ucPDULength)
{
    /**
     * @note This code is use OS's event mechanism for modbus master protocol stack.
     * If you don't use OS, you can change it.
     */
    eErrStatus = MB_MRE_EXE_FUN;
    OS_ReleaseSemaphore(xRequestFinish);
    /* You can add your code under here. */
    printf("MB -> EV_MASTER_ERROR_EXECUTE_FUNCTION:\n");
}

/**
 * This is modbus master request process success callback function.
 * @note There functions will block modbus master poll while execute OS waiting.
 * So,for real-time of system.Do not execute too much waiting process.
 *
 */
void vMBMasterCBRequestScuuess(void)
{
    /**
     * @note This code is use OS's event mechanism for modbus master protocol stack.
     * If you don't use OS, you can change it.
     */
    eErrStatus = MB_MRE_NO_ERR;
    OS_ReleaseSemaphore(xRequestFinish);
    /* You can add your code under here. */
}

/**
 * This function is wait for modbus master request finish and return result.
 * Waiting result include request process success, request respond timeout,
 * receive data error and execute function error.You can use the above callback function.
 * @note If you are use OS, you can use OS's event mechanism. Otherwise you have to run
 * much user custom delay for waiting.
 *
 * @return request error code
 */
eMBMasterReqErrCode eMBMasterWaitRequestFinish(void)
{
    eMBMasterReqErrCode t_eErrStatus = eErrStatus;
    OS_WaitForSemaphore(xRequestFinish, OS_WAIT_FOREVER);
    eErrStatus = MB_MRE_NO_ERR;
    return t_eErrStatus;
}

#endif
