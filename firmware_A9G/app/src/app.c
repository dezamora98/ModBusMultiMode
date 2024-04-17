#include "app.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <api_inc_os.h>
#include <api_os.h>
#include "mb.h"
#include "mb_m.h"
#include "mbutils.h"
#include <assert.h>

#define SLAVE_ADDR 0x02
#define PORT_NUM UART1
#define PORT_BAUDRATE UART_BAUD_RATE_9600
#define PORT_PARITY UART_PARITY_NONE

#define MB_SEND_REG_START 2
#define MB_SEND_REG_NUM 2

typedef struct
{
    uint8_t ID;
    uint8_t *CoilBuf;
    uint8_t *DiscInBuf;
    uint16_t *RegInBuf;
    uint16_t *RegHoldBuf;
} MBSlave_t;

void app(void *pData)
{
    waitSystemReady();
    printf("INIT APP");
    eMBErrorCode eMBState = MB_ENOERR;

    MBSlave_t ST = {
        .ID = SLAVE_ADDR,
        .CoilBuf = xMBMasterGetCoilBuf(SLAVE_ADDR),
        .DiscInBuf = xMBMasterGetDiscInBuf(SLAVE_ADDR),
        .RegHoldBuf = xMBMasterGetRegHoldBuf(SLAVE_ADDR),
        .RegInBuf = xMBMasterGetRegInBuf(SLAVE_ADDR),
    };

    assert(ST.CoilBuf && ST.DiscInBuf && ST.RegHoldBuf &&ST.RegInBuf);

    eMBState = eMBMasterInit(MB_RTU, PORT_NUM, PORT_BAUDRATE, PORT_PARITY);
    assert(eMBState == MB_ENOERR);

    eMBState = eMBMasterEnable();
    assert(eMBState == MB_ENOERR);

    printf("INIT TEST");
    while (true)
    {
        /* Test Modbus Master */
        eMBMasterReqReadHoldingRegister(ST.ID, 0, 8, OS_WAIT_FOREVER);
        eMBMasterReqReadInputRegister(ST.ID, 0, 8, OS_WAIT_FOREVER);
        eMBMasterReqReadCoils(ST.ID, 0, 8, OS_WAIT_FOREVER);
        eMBMasterReqReadDiscreteInputs(ST.ID, 0, 8, OS_WAIT_FOREVER);

        printf("--------------------MASTER-TEST-------------------");
        printf("| ID  | TYPE | ADDR 1 | ADDR 2 | ADDR 3 | ADDR 4 |");
        printf("| S%d | HREG | %d | %d | %d | %d |", ST.ID, ST.RegHoldBuf[0], ST.RegHoldBuf[1], ST.RegHoldBuf[2], ST.RegHoldBuf[3]);
        printf("| S%d | IREG | %d | %d | %d | %d |", ST.ID, ST.DiscInBuf[0], ST.DiscInBuf[1], ST.DiscInBuf[2], ST.DiscInBuf[3]);
        printf("| S%d | COIL | %d | %d | %d | %d |", ST.ID, xMBUtilGetBits(ST.CoilBuf, 0, 1), xMBUtilGetBits(ST.CoilBuf, 2, 1), xMBUtilGetBits(ST.CoilBuf, 3, 1), xMBUtilGetBits(ST.CoilBuf, 4, 1));
        printf("| S%d | DSIN | %d | %d | %d | %d |", ST.ID, xMBUtilGetBits(ST.DiscInBuf, 0, 1), xMBUtilGetBits(ST.DiscInBuf, 2, 1), xMBUtilGetBits(ST.DiscInBuf, 3, 1), xMBUtilGetBits(ST.DiscInBuf, 4, 1));

        OS_Sleep(1000);
    }
}
