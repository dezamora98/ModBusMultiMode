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
 * File: $Id: portserial.c,v 1.60 2013/08/13 15:07:05 Armink $
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


/* ----------------------- Static variables ---------------------------------*/
// #define MODBUS_CONTROL_PIN_INDEX GPIO_PIN10
// #define MODBUS_USE_CONTROL_PIN
/* ----------------------- Defines ------------------------------------------*/
/* serial transmit event */
#define EVENT_SERIAL_TRANS_START    (1<<0)

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTRxISR(UART_Callback_Param_t param);

static UART_Config_t CONFIG_UART;
static UART_Port_t MB_UART;
static char *i_RxBuffer;

/* ----------------------- Start implementation -----------------------------*/
bool xMBPortSerialInit(uint8_t ucPort, uint32_t ulBaudRate, uint8_t ucDataBits, eMBParity eParity)
{
     printf("init serial");
#ifdef MODBUS_USE_CONTROL_PIN
    GPIO_config_t CONFIG_PIN_RTS;
    GPIO_GetConfig(MODBUS_CONTROL_PIN_INDEX, &CONFIG_PIN_RTS);
    CONFIG_PIN_RTS.defaultLevel = GPIO_LEVEL_HIGH;
    CONFIG_PIN_RTS.mode = GPIO_MODE_OUTPUT;
    GPIO_Init(CONFIG_PIN_RTS);
    // rt_pin_mode(MODBUS_CONTROL_PIN_INDEX, PIN_MODE_OUTPUT);
#endif

    MB_UART = (UART_Port_t)ucPort;
    CONFIG_UART.baudRate = (UART_Baud_Rate_t)ulBaudRate;
    CONFIG_UART.dataBits = (UART_Data_Bits_t)ucDataBits;
    CONFIG_UART.parity = (UART_Parity_t)eParity;
    CONFIG_UART.stopBits = (UART_Stop_Bits_t)UART_STOP_BITS_1;
    CONFIG_UART.errorCallback = NULL;
    CONFIG_UART.rxCallback = (UART_Callback_t)prvvUARTRxISR;
    CONFIG_UART.useEvent = false;

    printf("UART_CONFIG = {B:%d, DB:%d, P:%d, SB:%d}", CONFIG_UART.baudRate,
           CONFIG_UART.dataBits, CONFIG_UART.parity, CONFIG_UART.stopBits);

    UART_Init(MB_UART, CONFIG_UART);

    return true;
}


void vMBPortSerialEnable(bool xRxEnable, bool xTxEnable)
{
    // static eMBEventType *event = NULL;

    if (xRxEnable)
    {
        /* enable RX interrupt */
        /* switch 485 to receive mode */
#if defined(MODBUS_USE_CONTROL_PIN)
        GPIO_Set(MODBUS_CONTROL_PIN_INDEX, GPIO_LEVEL_LOW);
#endif
        UART_Init(MB_UART, CONFIG_UART);
    }
    else
    {
        /* disable RX interrupt */
        /* switch 485 to transmit mode */
#if defined(MODBUS_USE_CONTROL_PIN)
        GPIO_Set(MODBUS_CONTROL_PIN_INDEX, GPIO_LEVEL_HIGH);
#endif
        UART_Init(MB_UART, CONFIG_UART);
    }
    if (xTxEnable)
    {
        /* start serial transmit */
        while (pxMBFrameCBTransmitterEmpty());
        
    }
}

void vMBPortClose(void)
{
    UART_Close(MB_UART);
}

void xMBMasterPortSerialClose(void)
{
    vMBMasterPortClose();
}

bool xMBPortSerialPutByte(char ucByte)
{
    UART_Write(MB_UART, &ucByte, 1);
    return true;
}

bool xMBPortSerialGetByte(char * pucByte)
{
    *pucByte = *i_RxBuffer;
    return true;
}

/*
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call
 * xMBPortSerialPutByte( ) to send the character.
 */
void prvvUARTRxISR(UART_Callback_Param_t param)
{
    uint32_t i = 0;
    if (param.port == MB_UART)
    {
        for (i = 0; i != param.length; ++i)
        {
            i_RxBuffer = param.buf + i;
            pxMBFrameCBByteReceived();
        }
    }
}


