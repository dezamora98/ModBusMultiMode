/**
 * @file portserial_m.c
 * @author Daniel Enquique Zamora Sifredo (dezamora98@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-04-07
 *
 * @copyright Copyright (c) 2024
 *
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

// #define MODBUS_MASTER_CONTROL_PIN_INDEX GPIO_PIN10
// #define MODBUS_MASTER_USE_CONTROL_PIN

/* ----------------------- Defines ------------------------------------------*/
/* serial transmit event */
#define EVENT_SERIAL_TRANS_START (1 << 0)

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTRxISR(UART_Callback_Param_t param);

static UART_Config_t CONFIG_UART;
static UART_Port_t MB_UART;
static char *i_RxBuffer;

/* ----------------------- Start implementation -----------------------------*/
bool xMBMasterPortSerialInit(uint8_t ucPort, uint32_t ulBaudRate, uint8_t ucDataBits, eMBParity eParity)
{
    // printf("init serial");
#ifdef MODBUS_MASTER_USE_CONTROL_PIN
    GPIO_config_t CONFIG_PIN_RTS;
    GPIO_GetConfig(MODBUS_MASTER_CONTROL_PIN_INDEX, &CONFIG_PIN_RTS);
    CONFIG_PIN_RTS.defaultLevel = GPIO_LEVEL_HIGH;
    CONFIG_PIN_RTS.mode = GPIO_MODE_OUTPUT;
    GPIO_Init(CONFIG_PIN_RTS);
    // rt_pin_mode(MODBUS_MASTER_CONTROL_PIN_INDEX, PIN_MODE_OUTPUT);
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

void vMBMasterPortSerialEnable(bool xRxEnable, bool xTxEnable)
{
    // static eMBMasterEventType *event = NULL;

    if (xRxEnable)
    {
        /* enable RX interrupt */
        /* switch 485 to receive mode */
#if defined(MODBUS_MASTER_USE_CONTROL_PIN)
        GPIO_Set(MODBUS_MASTER_CONTROL_PIN_INDEX, GPIO_LEVEL_LOW);
#endif
        UART_Init(MB_UART, CONFIG_UART);
    }
    else
    {
        /* disable RX interrupt */
        /* switch 485 to transmit mode */
#if defined(MODBUS_MASTER_USE_CONTROL_PIN)
        GPIO_Set(MODBUS_MASTER_CONTROL_PIN_INDEX, GPIO_LEVEL_HIGH);
#endif
        UART_Init(MB_UART, CONFIG_UART);
    }
    if (xTxEnable)
    {
        /* start serial transmit */
        while (pxMBMasterFrameCBTransmitterEmpty())
            ;
    }
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
    *pucByte = *i_RxBuffer;
    return true;
}

/*
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvUARTRxISR(UART_Callback_Param_t param)
{
    uint32_t i = 0;
    printf("prvvUARTRxISR");
    if (param.port == MB_UART)
    {
        for (i = 0; i != param.length; ++i)
        {
            i_RxBuffer = param.buf + i;
            pxMBMasterFrameCBByteReceived();
        }
    }
}

#endif
