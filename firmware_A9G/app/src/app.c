#include "app.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <api_inc_os.h>
#include <api_os.h>
#include "mb.h"
#include "mb_m.h"

#define SLAVE_ADDR 0x01
#define PORT_NUM UART1
#define PORT_BAUDRATE UART_BAUD_RATE_9600
#define PORT_PARITY UART_PARITY_NONE

#define MB_SEND_REG_START 2
#define MB_SEND_REG_NUM 2

#define MB_POLL_CYCLE_MS 500
HANDLE tid1 = NULL, tid2 = NULL;

static void send_thread_entry(void *parameter);
static void mb_master_poll(void *parameter);

void app(void *pData)
{
    eMBErrorCode eMBState;

    waitSystemReady();
    printf("INIT APP");
    eMBState = eMBMasterInit(MB_RTU, PORT_NUM, PORT_BAUDRATE, PORT_PARITY);
    printf("MODBUS-->MASTER_INIT--STATE(%d)", (int)eMBState);
    eMBState = eMBMasterEnable();
    printf("MODBUS-->MASTER_ENABLE--STATE(%d)", (int)eMBState);

    tid1 = OS_CreateTask(mb_master_poll, NULL, NULL, 2048*2, MAX_TASK_PR+1, 0, 0, "mb_master_pull");
    if (tid1 == NULL)
    {
        printf("ERROR->tareas no creadas");
        return;
    }

    tid2 = OS_CreateTask(send_thread_entry, NULL, NULL, 2048, MAX_TASK_PR+1, 0, 0, "mb_master_pull");
    if (tid2 == NULL)
    {
        printf("ERROR->tareas no creadas");
        return;
    }

    while (true)
    {
        OS_Sleep(OS_WAIT_FOREVER);
    }
}

static void send_thread_entry(void *parameter)
{
    eMBMasterReqErrCode error_code = MB_MRE_NO_ERR;
    uint16_t data[MB_SEND_REG_NUM] = {0};

    OS_Sleep(5000);

    printf("MODBUS--START-->  send_thread_entry");
    while (true)
    {
        /* Test Modbus Master */
        data[0] = (uint16_t)(50);
        data[1] = (uint16_t)(30);

        printf("MODBUS --> Enviando datos a registros");
        error_code = eMBMasterReqWriteMultipleHoldingRegister(SLAVE_ADDR,                /* salve address */
                                                              MB_SEND_REG_START,         /* register start address */
                                                              MB_SEND_REG_NUM,           /* register total number */
                                                              data,                      /* data to be written */
                                                              OS_TIME_OUT_WAIT_FOREVER); /* timeout */

        /* Record the number of errors */
        printf("MODBUS-ERROR --> %d", (int)error_code);
        OS_Sleep(100);
    }
}

static void mb_master_poll(void *parameter)
{
    int eCode = 0;
    printf("MODBUS--START-->MASTER_POLL");
    while (true)
    {
        eCode = (int)eMBMasterPoll();
        //printf("MODBUS-POLL_eCODE --> %d", eCode);
        OS_Sleep(5);
    }
}