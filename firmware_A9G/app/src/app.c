#include "app.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <api_inc_os.h>
#include <api_os.h>
#include "mb.h"
//#include "mb_m.h"
#include <assert.h>

#define SLAVE_ADDR 0x01
#define PORT_NUM UART1
#define PORT_BAUDRATE UART_BAUD_RATE_9600
#define PORT_PARITY UART_PARITY_NONE

#define MB_POLL_CYCLE_MS 500
HANDLE tid1 = NULL

static void mb_poll(void *parameter);

void app(void *pData)
{
    waitSystemReady();
    printf("INIT APP");
    eMBErrorCode eMBState = MB_ENOERR;
    eMBState = eMBInit(MB_RTU, SLAVE_ADDR, PORT_NUM, PORT_BAUDRATE, PORT_PARITY);
    assert(eMBState == MB_ENOERR);
    eMBState = eMBEnable();
    assert(eMBState == MB_ENOERR);

    tid1 = OS_CreateTask(mb_poll, NULL, NULL, 2048 * 2, MAX_TASK_PR + 2, 0, 0, "mb_poll");
    if (tid1 == NULL)
    {
        printf("ERROR->tareas no creadas");
        return;
    }else{
        printf("Tarea creada");
    }

    while (true)
    {
        OS_Sleep(OS_WAIT_FOREVER);
    }
}

static void mb_poll(void *parameter)
{
    printf("MODBUS--START-->POLL");
    while (true)
    {
        eMBPoll();
        printf("MODBUS-POLL");
        OS_Sleep(MB_POLL_CYCLE_MS);
    }
}