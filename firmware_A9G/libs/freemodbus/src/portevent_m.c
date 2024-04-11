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
static HANDLE xMBEventManager_H = NULL;
static HANDLE xMasterRunRes = NULL;
/* ---------------------------local implementation---------------------------*/
bool xMBMasterPortEventPost(eMBMasterEventType eEvent);
static void vMBMastertPortEventManager(void *parameter)
{
    while (true)
    {
        OS_Sleep(OS_WAIT_FOREVER);
    }
}

/* ----------------------- Start implementation -----------------------------*/
bool xMBMasterPortEventInit(void)
{
    xMBEventManager_H = OS_CreateTask(vMBMastertPortEventManager, NULL, NULL, 2048, MAX_TASK_PR + 2, 0, 0, "MB_EventManager");
    if (xMBEventManager_H != NULL)
    {
        printf("MODBUS-EVENT-->EVENT_MANAGER_INIT");
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
    eMBMasterEventType *event = (eMBMasterEventType *)malloc(sizeof(eMBMasterEventType));
    if (!event)
    {
        return false;
    }
    *event = eEvent;
    status = OS_SendEvent(xMBEventManager_H, event, OS_WAIT_FOREVER, OS_EVENT_PRI_NORMAL);
    return status;
}

bool xMBMasterPortEventGet(eMBMasterEventType *eEvent)
{
    eMBMasterEventType *recvedEvent = NULL;
    OS_WaitEvent(xMBEventManager_H, (void **)&recvedEvent, OS_WAIT_FOREVER);
    /* the enum type couldn't convert to int type */
    *eEvent = *recvedEvent;
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

    xMBMasterPortEventPost(EV_MASTER_ERROR_RESPOND_TIMEOUT);

    /* You can add your code under here. */
    printf("MB_MASTER -> EV_MASTER_ERROR_RESPOND_TIMEOUT:\n");
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
    xMBMasterPortEventPost(EV_MASTER_ERROR_RECEIVE_DATA);
    /* You can add your code under here. */
    printf("MB_MASTER -> EV_MASTER_ERROR_RECEIVE_DATA:\n");
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
    xMBMasterPortEventPost(EV_MASTER_ERROR_EXECUTE_FUNCTION);
    /* You can add your code under here. */
    printf("MB_MASTER -> EV_MASTER_ERROR_EXECUTE_FUNCTION:\n");
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
    xMBMasterPortEventPost(EV_MASTER_PROCESS_SUCESS);
    /* You can add your code under here. */
    printf("MB_MASTER -> EV_MASTER_PROCESS_SUCESS:\n");
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
    eMBMasterReqErrCode eErrStatus = MB_MRE_NO_ERR;
    eMBMasterEventType *recvedEvent = NULL;

    OS_WaitEvent(xMBEventManager_H, (void **)&recvedEvent, OS_WAIT_FOREVER);

    switch ((eMBMasterEventType)*recvedEvent)
    {
    case EV_MASTER_ERROR_RESPOND_TIMEOUT:
        eErrStatus = MB_MRE_TIMEDOUT;
        break;
    case EV_MASTER_ERROR_RECEIVE_DATA:
        eErrStatus = MB_MRE_REV_DATA;
        break;
    case EV_MASTER_ERROR_EXECUTE_FUNCTION:
        eErrStatus = MB_MRE_EXE_FUN;
        break;
    case EV_MASTER_PROCESS_SUCESS:
    default:
        break;
    }

    free(recvedEvent);
    return eErrStatus;
}

#endif
