#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sdk_init.h>
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
            printf("Error de formato en JSON");
        }
        else
        {
            printf("%s",json_getPropertyValue(json, "test"));
            printf("num = %ld",json_getInteger(json_getProperty(json,"num")));
        }
        OS_Sleep(1000);
    }
}
