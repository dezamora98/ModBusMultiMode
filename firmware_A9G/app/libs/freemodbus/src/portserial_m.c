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

#include "port.h"
#include "stdint.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "api_hal_uart.h"
#include "api_inc_uart.h"
#include "api_inc_gpio.h"
#include "api_hal_gpio.h"
#include "api_event.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Static variables ---------------------------------*/
static uint8_t serial_soft_trans_irq_stack[512];
/* software simulation serial transmit IRQ handler thread */
static HANDLE thread_serial_soft_trans_irq;
/* serial event */
static API_Event_t event_serial;
/* modbus master serial device */
static UART_Port_t serial;

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
BOOL xMBMasterPortSerialInit(UART_Port_t ucPORT, UART_Baud_Rate_t ulBaudRate, UART_Data_Bits_t ucDataBits, UART_Parity_t eParity)
{
#if defined(RT_MODBUS_MASTER_USE_CONTROL_PIN)
    GPIO_config_t CONFIG_PIN_RTS;
    GPIO_GetConfig(MODBUS_MASTER_RT_CONTROL_PIN_INDEX, &CONFIG_PIN_RTS);
    CONFIG_PIN_RTS.defaultLevel = GPIO_LEVEL_HIGH;
    CONFIG_PIN_RTS.mode = GPIO_MODE_OUTPUT;
    GPIO_Init(CONFIG_PIN_RTS);
    // rt_pin_mode(MODBUS_MASTER_RT_CONTROL_PIN_INDEX, PIN_MODE_OUTPUT);
#endif

    MB_UART = ucPORT;
    CONFIG_UART.baudRate = ulBaudRate;
    CONFIG_UART.dataBits = ucDataBits;
    CONFIG_UART.parity = eParity;
    CONFIG_UART.stopBits = UART_STOP_BITS_1;
    CONFIG_UART.errorCallback = NULL; // ver cual es.
    CONFIG_UART.rxCallback = serial_rx_ind;    // ver cual es.
    CONFIG_UART.useEvent = false;     // ver eso.

    UART_Close(MB_UART);

    //    /* set serial name */
    //    rt_snprintf(uart_name,sizeof(uart_name), "uart%d", ucPORT);
    //
    //    dev = rt_device_find(uart_name);
    //    if(dev == RT_NULL)
    //    {
    //        /* can not find uart */
    //        return FALSE;
    //    }
    //    else
    //    {
    //        serial = (struct rt_serial_device*)dev;
    //    }

    //    /* set serial configure parameter */
    //    serial->config.baud_rate = ulBaudRate;
    //    serial->config.stop_bits = STOP_BITS_1;
    //    switch(eParity){
    //    case MB_PAR_NONE: {
    //        serial->config.data_bits = DATA_BITS_8;
    //        serial->config.parity = PARITY_NONE;
    //        break;
    //    }
    //    case MB_PAR_ODD: {
    //        serial->config.data_bits = DATA_BITS_9;
    //        serial->config.parity = PARITY_ODD;
    //        break;
    //    }
    //    case MB_PAR_EVEN: {
    //        serial->config.data_bits = DATA_BITS_9;
    //        serial->config.parity = PARITY_EVEN;
    //        break;
    //    }
    //    }
    //    /* set serial configure */
    //    serial->ops->configure(serial, &(serial->config));
    //
    //    /* open serial device */
    //    if (!rt_device_open(&serial->parent, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX)) {
    //        rt_device_set_rx_indicate(&serial->parent, serial_rx_ind);
    //    } else {
    //        return FALSE;
    //    }
    //

    //    /* software initialize */
    //    rt_event_init(&event_serial, "master event", RT_IPC_FLAG_PRIO);
    //    rt_thread_init(&thread_serial_soft_trans_irq,
    //                   "master trans",
    //                   serial_soft_trans_irq,
    //                   RT_NULL,
    //                   serial_soft_trans_irq_stack,
    //                   sizeof(serial_soft_trans_irq_stack),
    //                   10, 5);
    //    rt_thread_startup(&thread_serial_soft_trans_irq);

    thread_serial_soft_trans_irq = OS_CreateTask(&serial_soft_trans_irq, NULL, &serial_soft_trans_irq_stack, 512, OS_EVENT_PRI_URGENT, 0, 0, "MB-TX");
    OS_StartTask(thread_serial_soft_trans_irq, NULL);
    return TRUE;
}

void vMBMasterPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
    uint32_t recved_event;
    if (xRxEnable)
    {
        /* enable RX interrupt */
        CONFIG_UART.useEvent = true;
        // serial->ops->control(serial, RT_DEVICE_CTRL_SET_INT, (void *)RT_DEVICE_FLAG_INT_RX);
        /* switch 485 to receive mode */
#if defined(RT_MODBUS_MASTER_USE_CONTROL_PIN)
        GPIO_Set(MODBUS_MASTER_RT_CONTROL_PIN_INDEX, GPIO_LEVEL_LOW);
#endif
    }
    else
    {
        /* switch 485 to transmit mode */
#if defined(RT_MODBUS_MASTER_USE_CONTROL_PIN)
        GPIO_Set(MODBUS_MASTER_RT_CONTROL_PIN_INDEX, GPIO_LEVEL_HIGH);
#endif
        /* disable RX interrupt */
        CONFIG_UART.useEvent = false;
        // serial->ops->control(serial, RT_DEVICE_CTRL_CLR_INT, (void *)RT_DEVICE_FLAG_INT_RX);
    }
    if (xTxEnable)
    {
        /* start serial transmit */
        // rt_event_send(&event_serial, EVENT_SERIAL_TRANS_START);
    }
    else
    {
        /* stop serial transmit */
        // rt_event_recv(&event_serial, EVENT_SERIAL_TRANS_START,
        //               RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, 0,
        //               &recved_event);
    }
}

void vMBMasterPortClose(void)
{
    // serial->parent.close(&(serial->parent));
    UART_Close(MB_UART);
}

BOOL xMBMasterPortSerialPutByte(CHAR ucByte)
{
    // serial->parent.write(&(serial->parent), 0, &ucByte, 1);
    UART_Write(MB_UART, &ucByte, 1);
    return TRUE;
}

BOOL xMBMasterPortSerialGetByte(CHAR *pucByte)
{
    // serial->parent.read(&(serial->parent), 0, pucByte, 1);
    UART_Read(MB_UART, pucByte, 1, 1);
    return TRUE;
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
    uint32_t recved_event;
    while (1)
    {
        /* waiting for serial transmit start */
        // rt_event_recv(&event_serial, EVENT_SERIAL_TRANS_START, RT_EVENT_FLAG_OR,
        //               RT_WAITING_FOREVER, &recved_event);
        /* execute modbus callback */
        
        OS_WaitEvent(thread_serial_soft_trans_irq,&recved_event,OS_WAIT_FOREVER);
        if(recved_event == EVENT_SERIAL_TRANS_START)
            prvvUARTTxReadyISR();
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
    return 1;
}

#endif
