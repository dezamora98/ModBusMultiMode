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
 * File: $Id: portserial_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <api_inc_uart.h>
#include <api_hal_uart.h>
#include <api_inc_os.h>
#include <api_os.h>

/* ----------------------- Modbus includes ----------------------------------*/
#include "port.h"
#include "mb.h"
#include "mbport.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Static variables ---------------------------------*/
/* software simulation serial transmit IRQ handler thread */
static HANDLE thread_serial_soft_trans_irq;

// #define MODBUS_MASTER_RT_CONTROL_PIN_INDEX GPIO_PIN10
// #define RT_MODBUS_MASTER_USE_CONTROL_PIN

/* ----------------------- Defines ------------------------------------------*/
/* serial transmit event */
#define EVENT_SERIAL_TRANS_START (1 << 0)

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR(void);
static void prvvUARTRxISR(void);
static void serial_rx_ind(UART_Callback_Param_t param);
static void serial_soft_trans_irq(void *parameter);

static UART_Config_t CONFIG_UART;
static UART_Port_t MB_UART;

/* ----------------------- Start implementation -----------------------------*/
bool xMBMasterPortSerialInit(uint8_t ucPort, uint32_t ulBaudRate, uint8_t ucDataBits, eMBParity eParity)
{
    // printf("init serial");
#if defined(RT_MODBUS_MASTER_USE_CONTROL_PIN)
    GPIO_config_t CONFIG_PIN_RTS;
    GPIO_GetConfig(MODBUS_MASTER_RT_CONTROL_PIN_INDEX, &CONFIG_PIN_RTS);
    CONFIG_PIN_RTS.defaultLevel = GPIO_LEVEL_HIGH;
    CONFIG_PIN_RTS.mode = GPIO_MODE_OUTPUT;
    GPIO_Init(CONFIG_PIN_RTS);
    // rt_pin_mode(MODBUS_MASTER_RT_CONTROL_PIN_INDEX, PIN_MODE_OUTPUT);
#endif

    MB_UART = (UART_Port_t)ucPort;
    CONFIG_UART.baudRate = (UART_Baud_Rate_t)ulBaudRate;
    CONFIG_UART.dataBits = (UART_Data_Bits_t)ucDataBits;
    CONFIG_UART.parity = (UART_Parity_t)eParity;
    CONFIG_UART.stopBits = (UART_Stop_Bits_t)UART_STOP_BITS_1;
    CONFIG_UART.errorCallback = NULL;                        // ver cual es.
    CONFIG_UART.rxCallback = (UART_Callback_t)serial_rx_ind; // ver cual es.
    CONFIG_UART.useEvent = false;                            // ver eso.

    printf("UART_CONFIG = {B:%d, DB:%d, P:%d, SB:%d}", CONFIG_UART.baudRate,
           CONFIG_UART.dataBits, CONFIG_UART.parity, CONFIG_UART.stopBits);

    //UART_Init(MB_UART, CONFIG_UART);

    // printf("creando interrupción de transmición por software");
    thread_serial_soft_trans_irq = OS_CreateTask(serial_soft_trans_irq, NULL, NULL, 2048, MAX_TASK_PR, 0, 0, "MMB-TX");
    // printf("interrupción de transmición por software creada");
    return true;
}

void vMBMasterPortSerialEnable(bool xRxEnable, bool xTxEnable)
{
    static eMBMasterEventType *event = NULL;

    if (xRxEnable)
    {
        /* enable RX interrupt */
        CONFIG_UART.useEvent = true;
        // serial->ops->control(serial, RT_DEVICE_CTRL_SET_INT, (void *)RT_DEVICE_FLAG_INT_RX);
        /* switch 485 to receive mode */
#if defined(MODBUS_MASTER_USE_CONTROL_PIN)
        GPIO_Set(MODBUS_MASTER_RT_CONTROL_PIN_INDEX, GPIO_LEVEL_LOW);
#endif
    }
    else
    {
        /* switch 485 to transmit mode */
#if defined(MODBUS_MASTER_USE_CONTROL_PIN)
        GPIO_Set(MODBUS_MASTER_RT_CONTROL_PIN_INDEX, GPIO_LEVEL_HIGH);
#endif
        /* disable RX interrupt */
        CONFIG_UART.useEvent = false;
    }
    if (xTxEnable)
    {
        /* start serial transmit */
        event = (eMBMasterEventType *)malloc(sizeof(eMBMasterEventType));
        *event = EVENT_SERIAL_TRANS_START;
        OS_SendEvent(thread_serial_soft_trans_irq, event, OS_TIME_OUT_WAIT_FOREVER, OS_EVENT_PRI_NORMAL);
    }
    else if (OS_IsEventAvailable(thread_serial_soft_trans_irq))
    {
        /* stop serial transmit */
        //OS_WaitEvent(thread_serial_soft_trans_irq, (void **)&event, OS_WAIT_FOREVER);
        free(event);
    }

    UART_Init(MB_UART, CONFIG_UART);
}

void vMBMasterPortClose(void)
{
    UART_Close(MB_UART);
}

void xMBMasterPortSerialClose(void)
{
    vMBMasterPortClose();
}

bool xMBMasterPortSerialPutByte(char ucByte)
{
    UART_Write(MB_UART, &ucByte, 1);
    return true;
}

bool xMBMasterPortSerialGetByte(char *pucByte)
{
    UART_Read(MB_UART, pucByte, 1, 1);
    return true;
}

/*
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call
 * xMBPortSerialPutByte( ) to send the character.
 */
void prvvUARTTxReadyISR(void)
{
    pxMBMasterFrameCBTransmitterEmpty();
}

/*
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvUARTRxISR(void)
{
    pxMBMasterFrameCBByteReceived();
}

// ----------------------------esto está bueno, es la forma en la que quitan el problema de no tener interrupción de transmición, pero AL MENOS TIENEN EL EVENTO, EN EL CSDK EL EVENTO DE TX NO ESTÁ.--------------------
/**
 * Software simulation serial transmit IRQ handler.
 *
 * @param parameter parameter
 */
static void serial_soft_trans_irq(void *parameter)
{
    eMBMasterEventType *recved_event = NULL;
    printf("MODBUS-->serial_soft_trans_irq");

    while (true)
    {
        /* waiting for serial transmit start */
        if (OS_WaitEvent(thread_serial_soft_trans_irq, (void **)&recved_event, OS_WAIT_FOREVER))
        {
            /* execute modbus callback */
            if (*recved_event == EVENT_SERIAL_TRANS_START) // --Este es el único evento que tiene thread_serial_soft_trans_irq por lo que no le veo sentido
                prvvUARTTxReadyISR();
            OS_Free(recved_event);
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/**
 * This function is serial receive callback function
 *
 * @param param  serial device, buffer pointer and size.
 *
 * @return return RT_EOK
 */
static void serial_rx_ind(UART_Callback_Param_t param)
{
    prvvUARTRxISR();
}

#endif
