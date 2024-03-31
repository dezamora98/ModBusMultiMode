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

static void send_thread_entry(void *parameter);
static void mb_master_poll(void *parameter);
static bool mb_master_sample(void);

void app(void *pData)
{
    printf("INIT APP");
    while (1)
    {

        if (!mb_master_sample())
            printf("MODBUS_MASTER->ERROR");
        else
            printf("MODBUS_FUNCIONANDO");

        OS_Sleep(1000);
    }
}

#if true

static void send_thread_entry(void *parameter)
{
    eMBMasterReqErrCode error_code = MB_MRE_NO_ERR;
    uint16_t error_count = 0;
    uint16_t data[MB_SEND_REG_NUM] = {0};

    OS_Sleep(1000);
    printf("start->tarea send_thread_entry");
    OS_Sleep(1000);


    while (1)
    {
        /* Test Modbus Master */
        data[0] = (uint16_t)(50);
        data[1] = (uint16_t)(30);

        error_code = eMBMasterReqWriteMultipleHoldingRegister(SLAVE_ADDR,                /* salve address */
                                                              MB_SEND_REG_START,         /* register start address */
                                                              MB_SEND_REG_NUM,           /* register total number */
                                                              data,                      /* data to be written */
                                                              OS_TIME_OUT_WAIT_FOREVER); /* timeout */

        /* Record the number of errors */
        if (error_code != MB_MRE_NO_ERR)
        {
            error_count++;
        }
        OS_Sleep(100);
    }
}

static void mb_master_poll(void *parameter)
{
    OS_Sleep(1000);
    printf("start->tarea mb_master_poll");
    OS_Sleep(1000);

    eMBMasterInit(MB_RTU, PORT_NUM, PORT_BAUDRATE, PORT_PARITY);

    OS_Sleep(1000);
    printf("start->tarea mb_master_poll --> eMBMasterInit(MB_RTU, PORT_NUM, PORT_BAUDRATE, PORT_PARITY);");
    OS_Sleep(1000);

    eMBMasterEnable();

    OS_Sleep(1000);
    printf("start->tarea mb_master_poll --> eMBMasterEnable();");
    OS_Sleep(1000);

    while (1)
    {
        eMBMasterPoll();
        OS_Sleep(MB_POLL_CYCLE_MS);
    }
}

static bool mb_master_sample(void)
{
    static uint8_t is_init = 0;
    HANDLE tid1 = NULL, tid2 = NULL;

    OS_Sleep(1000);
    printf("start->función mb_master_sample");
    OS_Sleep(1000);

    if (is_init > 0)
    {
        printf("sample is running\n");
        return true;
    }

    tid1 = OS_CreateTask(mb_master_poll, NULL, NULL, 2048, MAX_TASK_PR, 0, 0, "mb_master_pull");
    if (tid1 == NULL)
    {
        printf("ERROR->tareas no creadas");
        return false;
    }

    tid2 = OS_CreateTask(send_thread_entry, NULL, NULL, 2048, MAX_TASK_PR, 0, 0, "mb_master_pull");
    if (tid2 == NULL)
    {
        printf("ERROR->tareas no creadas");
        return false;
    }

    return true;
}

#endif // !_MB_H