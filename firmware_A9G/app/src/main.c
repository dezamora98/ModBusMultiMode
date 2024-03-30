#include <stdlib.h>
#include <cs_types.h>
#include <stdbool.h>
#include <stdio.h>
#include <api_inc_os.h>
#include <api_os.h>
#include "tiny-json.h"
#include "mb.h"
#include "mb_m.h"


#define SLAVE_ADDR      0x01
#define PORT_NUM        UART1
#define PORT_BAUDRATE   UART_BAUD_RATE_9600
#define PORT_PARITY     UART_PARITY_NONE

#define MB_POLL_THREAD_PRIORITY  10
#define MB_SEND_THREAD_PRIORITY  RT_THREAD_PRIORITY_MAX - 1

#define MB_SEND_REG_START  2
#define MB_SEND_REG_NUM    2

#define MB_POLL_CYCLE_MS   500

static void send_thread_entry(void *parameter);
static void mb_master_poll(void *parameter);
static bool mb_master_sample(void);

void MainTask(void *pData)
{
    while (1)
    {
        if(!mb_master_sample())
        {
            printf("MODBUS_MASTER->ERROR");
        }
    }
}


static void send_thread_entry(void *parameter)
{
    eMBMasterReqErrCode error_code = MB_MRE_NO_ERR;
    uint16_t error_count = 0;
    uint16_t data[MB_SEND_REG_NUM] = {0};

    while (1)
    {
        /* Test Modbus Master */
        data[0] = (uint16_t)(50);
        data[1] = (uint16_t)(30);

        error_code = eMBMasterReqWriteMultipleHoldingRegister(SLAVE_ADDR,          /* salve address */
                                                              MB_SEND_REG_START,   /* register start address */
                                                              MB_SEND_REG_NUM,     /* register total number */
                                                              data,                /* data to be written */
                                                              OS_TIME_OUT_WAIT_FOREVER); /* timeout */

        /* Record the number of errors */
        if (error_code != MB_MRE_NO_ERR)
        {
            error_count++;
        }
    }
}

static void mb_master_poll(void *parameter)
{
    eMBMasterInit(MB_RTU, PORT_NUM, PORT_BAUDRATE, PORT_PARITY);
    eMBMasterEnable();

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

    if (is_init > 0)
    {
        printf("sample is running\n");
        return true;
    }

    tid1 = OS_CreateTask(mb_master_poll,NULL,NULL,512,OS_EVENT_PRI_URGENT,0,0,"mb_master_pull");
    if (tid1 == NULL)
    {
        goto __exit;
    }

    tid2 = OS_CreateTask(send_thread_entry,NULL,NULL,512,OS_EVENT_PRI_NORMAL,0,0,"mb_master_pull");
    if (tid2 == NULL)
    {
        goto __exit;
    }
    
    return true;

    __exit:
        printf("ERROR->tareas no creadas");
        return false;
}