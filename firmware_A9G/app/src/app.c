#include "app.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <api_inc_os.h>
#include <api_os.h>
#include "mb.h"
#include "mb_m.h"
#include "portothers.h"

#define SLAVE_ADDR 0x01
#define PORT_NUM UART1
#define PORT_BAUDRATE UART_BAUD_RATE_9600
#define PORT_PARITY UART_PARITY_NONE

#define MB_SEND_REG_START 0
#define MB_SEND_REG_NUM 4

void app(void *pData)
{
    waitSystemReady();
    printf("INIT APP");

    xMBMasterStart(MB_RTU, PORT_NUM, PORT_BAUDRATE, PORT_PARITY);

    eMBMasterReqErrCode error_code = MB_MRE_NO_ERR;
    uint16_t data[MB_SEND_REG_NUM] = {0};
    eMBMasterRegHoldingCB((uint8_t *)(data), MB_SEND_REG_START, 2, MB_REG_WRITE);

    OS_Sleep(5000);

    printf("MODBUS--START MODBUS_TASK");

    while (true)
    {
        /* Test Modbus Master */

        data[0]++;
        data[1]--;
        printf("evniando registros");
        error_code = eMBMasterReqWriteMultipleHoldingRegister(SLAVE_ADDR,        /* salve address */
                                                              MB_SEND_REG_START, /* register start address */
                                                              2,                 /* register total number */
                                                              data,              /* data to be written */
                                                              20);               /* timeout */

        if (error_code != MB_MRE_NO_ERR)
            printf("MODBUS-ERROR_WRITE --> %d", (int)error_code);

        // error_code = eMBMasterReqReadHoldingRegister(SLAVE_ADDR, MB_SEND_REG_START + 2, 2, OS_WAIT_FOREVER);

        /*
        OS_Sleep(100);

        if (error_code != MB_MRE_NO_ERR)
            printf("MODBUS-ERROR_READ --> %d", (int)error_code);

        //printf("REG2 = %d , REG3 = %d", data[2], data[3]);
        */

        OS_Sleep(1000);
    }
}
