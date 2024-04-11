#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <api_inc_os.h>
#include <api_os.h>

#include "mb_m.h"
#include "mb.h"
#include "mbport.h"
#include "portothers.h"
#include "user_mb_app.h"

static HANDLE MBMasterPortPollTask_H = NULL;

static void vMBMasterPortPollTask(void *parameter)
{
    eMBErrorCode eCode = 0;
    printf("MODBUS--START-->MASTER_POLL");
    while (true)
    {
        eCode = eMBMasterPoll();
        if (eCode != MB_ENOERR)
            printf("MODBUS-->eMBErrorCode(%d)", (int)eCode);
        OS_Sleep(MB_POLL_CYCLE_MS);
    }
}

bool xMBMasterStart(eMBMode eMode, uint8_t ucPort, uint32_t ulBaudRate, eMBParity eParity)
{
    eMBErrorCode eMBState = MB_ENOERR;
    eMBState = eMBMasterInit(eMode, ucPort, ulBaudRate, eParity);
    printf("MODBUS-->MASTER_INIT--STATE(%d)", (int)eMBState);
    if (eMBState != MB_ENOERR)
        return false;


    eMBState = eMBMasterEnable();
    printf("MODBUS-->MASTER_ENABLE--STATE(%d)", (int)eMBState);
    if (eMBState != MB_ENOERR)
        return false;


    MBMasterPortPollTask_H = OS_CreateTask(vMBMasterPortPollTask, NULL, NULL, 2048, MAX_TASK_PR, 0, 0, "MB_MasterPollTask");
    if (MBMasterPortPollTask_H == NULL)
    {
        printf("MODBUS-->POLL ERROR");
        eMBMasterDisable();
        eMBMasterClose();
        return false;
    }

    return true;
}
