#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <api_inc_os.h>
#include <api_os.h>
#include <api_event.h>

#include "mb_m.h"
#include "mb.h"
#include "mbport.h"
#include "portothers.h"
#include "user_mb_app.h"

static void vMBMasterPortPollTask(void *parameter)
{
    eMBErrorCode eCode = eMBMasterPoll();
    if (eCode != MB_ENOERR)
        printf("MODBUS-->eMBErrorCode(%d)", (int)eCode);
    OS_StartCallbackTimer(OS_GetUserMainHandle(), MB_POLL_CYCLE_MS, vMBMasterPortPollTask, NULL);
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

    if (!OS_StartCallbackTimer(OS_GetUserMainHandle(), MB_POLL_CYCLE_MS, vMBMasterPortPollTask, NULL))
    {
        printf("MODBUS-->POLL ERROR");
        eMBMasterDisable();
        eMBMasterClose();
        return false;
    }

    printf("MODBUS-->POLL START");
    return true;
}
