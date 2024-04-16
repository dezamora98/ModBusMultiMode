#include "app.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <api_inc_os.h>
#include <api_os.h>
#include "mb.h"
#include "mb_m.h"
#include <assert.h>

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
    eMBMasterReqErrCode error_code = MB_MRE_NO_ERR;
    const uint16_t* data = NULL; 
    eMBErrorCode eMBState = MB_ENOERR;

    eMBState = eMBMasterInit(MB_RTU, PORT_NUM, PORT_BAUDRATE, PORT_PARITY);
    assert(eMBState == MB_ENOERR);

    eMBState = eMBMasterEnable();
    assert(eMBState == MB_ENOERR);

    data = xMBMasterGetRegHoldBuf(SLAVE_ADDR);
    assert(data != NULL);

    while (true)
    {
        /* Test Modbus Master */

        error_code = eMBMasterReqReadHoldingRegister(SLAVE_ADDR, 0, 4, OS_WAIT_FOREVER);

        if (error_code != MB_MRE_NO_ERR)
        {
            printf("MODBUS-ERROR--> %d", (int)error_code);
        }
        else
        {
            printf("{ REG0 : %d, REG1 : %d, REG2 : %d, REG3 : %d }", data[0], data[1], data[2], data[3]);
        }

        OS_Sleep(1000);
    }
}
