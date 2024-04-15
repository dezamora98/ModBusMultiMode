#include "app.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <api_inc_os.h>
#include <api_os.h>
#include "mb.h"
#include "mb_m.h"

#define SLAVE_ADDR 0x02
#define PORT_NUM UART1
#define PORT_BAUDRATE UART_BAUD_RATE_9600
#define PORT_PARITY UART_PARITY_NONE

#define MB_SEND_REG_START 2
#define MB_SEND_REG_NUM 2

void app(void *pData)
{
    waitSystemReady();
    printf("INIT APP");

    eMBErrorCode eMBState = eMBMasterInit(MB_RTU, PORT_NUM, PORT_BAUDRATE, PORT_PARITY);
    printf("MODBUS-->MASTER_INIT--STATE(%d)", (int)eMBState);
    if (eMBState != MB_ENOERR)
        return;

    eMBState = eMBMasterEnable();
    printf("MODBUS-->MASTER_ENABLE--STATE(%d)", (int)eMBState);
    if (eMBState != MB_ENOERR)
        return;

    eMBMasterReqErrCode error_code = MB_MRE_NO_ERR;
    uint16_t data[MB_SEND_REG_NUM] = {0};

    OS_Sleep(5000);

    while (true)
    {
        /* Test Modbus Master */
        data[0] = (uint16_t)(50);
        data[1] = (uint16_t)(30);

        printf("MODBUS --> Enviando datos a registros");
        error_code = eMBMasterReqWriteMultipleHoldingRegister(SLAVE_ADDR,        /* salve address */
                                                              MB_SEND_REG_START, /* register start address */
                                                              MB_SEND_REG_NUM,   /* register total number */
                                                              data,              /* data to be written */
                                                              OS_WAIT_FOREVER);  /* timeout */
        printf("MODBUS-ERROR --> %d", (int)error_code);
        OS_Sleep(1000);
    }
}
