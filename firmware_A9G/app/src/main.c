#include <stdlib.h>
#include "stdint.h"
#include "stdbool.h"
#include "api_os.h"
#include "api_event.h"
#include "api_debug.h"
#include "tiny-json.h"

void MainTask(VOID *pData)
{
    const char str[] = "{\"test\":\"Hola Mundo\", \"num\":1205}";
    json_t mem[4];
    const json_t *json = json_create((char*)str, mem, 4);
    

    while (1)
    {
        if (json == NULL)
        {
            Trace(1, "Error de formato en JSON");
        }
        else
        {
            Trace(1, json_getPropertyValue(json, "test"));
            Trace(1, "num = %ld",json_getInteger(json_getProperty(json,"num")));
        }
        OS_Sleep(1000);
    }
}
