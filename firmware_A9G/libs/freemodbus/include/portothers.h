#ifndef _PORTOTHERS_H_
#define _PORTOTHERS_H_

#include <stdint.h>
#include <stdbool.h>

#include "mb_m.h"
#include "mb.h"
#include "mbport.h"

bool xMBMasterStart(eMBMode eMode, uint8_t ucPort, uint32_t ulBaudRate, eMBParity eParity);

#endif // !_PORTOTHERS_H_