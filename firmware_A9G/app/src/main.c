#include "stdint.h"
#include "stdbool.h"
#include "api_os.h"
#include "api_event.h"
#include "api_debug.h"
#include "tiny-json.h"

void MainTask(VOID *pData)
{
    const char str[] = "{\"test\":\"Hola Mundo\"}";
    json_t mem[4];
    const json_t *json = json_create(str, mem, 4);

    while (1)
    {
        if (json == NULL)
        {
            Trace(1, "Error de formato en JSON");
        }
        else
        {
            Trace(1, json_getPropertyValue(json, "test"));
        }
        OS_Sleep(1000);
    }
}

void EventDispatch(API_Event_t *pEvent)
{
    switch (pEvent->id)
    {
    case API_EVENT_ID_POWER_ON:
        break;
    case API_EVENT_ID_NO_SIMCARD:
        break;
    case API_EVENT_ID_NETWORK_REGISTERED_HOME:
    case API_EVENT_ID_NETWORK_REGISTERED_ROAMING:
        break;
    default:
        break;
    }
}