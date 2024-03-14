/*
 * @File  app_main.c
 * @Brief An example of SDK's mini system
 *
 * @Author: Neucrack
 * @Date: 2017-11-11 16:45:17
 * @Last Modified by: Neucrack
 * @Last Modified time: 2017-11-11 18:24:56
 */
#include "stdint.h"
#include "stdbool.h"
#include "api_os.h"
#include "api_event.h"
#include "api_debug.h"

#define AppMain_TASK_STACK_SIZE (1024 * 2)
#define AppMain_TASK_PRIORITY 1
HANDLE mainTaskHandle = NULL;

#include "main.c"


void EventDispatch(API_Event_t *pEvent)
{
    switch (pEvent->id)
    {
    // system
    case API_EVENT_ID_POWER_ON: // param1: shows power on cause:
        break;
    case API_EVENT_ID_SYSTEM_READY:
        break;
    case API_EVENT_ID_KEY_DOWN: // param1:key id(Key_ID_t)
        break;
    case API_EVENT_ID_KEY_UP: // param1:key id(Key_ID_t)
        break;

    // SIM card
    case API_EVENT_ID_NO_SIMCARD: // param1: card id
        break;
    case API_EVENT_ID_SIMCARD_DROP: // param1: card id
        break;

    // signal
    case API_EVENT_ID_SIGNAL_QUALITY: // param1: CSQ(0~31,99(unknown)), param2:RXQUAL(0~7,99(unknown))  (RSSI = CSQ*2-113)
        break;

    // network
    case API_EVENT_ID_NETWORK_REGISTERED_HOME:
        break;
    case API_EVENT_ID_NETWORK_REGISTERED_ROAMING:
        break;
    case API_EVENT_ID_NETWORK_REGISTER_SEARCHING:
        break;
    case API_EVENT_ID_NETWORK_REGISTER_DENIED:
        break;
    case API_EVENT_ID_NETWORK_REGISTER_NO:
        break;
    case API_EVENT_ID_NETWORK_DEREGISTER:
        break;
    case API_EVENT_ID_NETWORK_DETACHED:
        break;
    case API_EVENT_ID_NETWORK_ATTACH_FAILED:
        break;
    case API_EVENT_ID_NETWORK_ATTACHED:
        break;
    case API_EVENT_ID_NETWORK_DEACTIVED:
        break;
    case API_EVENT_ID_NETWORK_ACTIVATE_FAILED:
        break;
    case API_EVENT_ID_NETWORK_ACTIVATED:
        break;
    case API_EVENT_ID_NETWORK_GOT_TIME: // pParam1: RTC_Time_t*
        break;
    case API_EVENT_ID_NETWORK_CELL_INFO: // param1:cell number(1 serving cell and param1-1 neighbor cell) , pParam1: Network_Location_t*
        break;

    // DNS
    case API_EVENT_ID_DNS_SUCCESS:
        break; // param1:IP address(uint32_t), pPram1:domain(char*), pParam2:ip(char*)
    case API_EVENT_ID_DNS_ERROR:
        break;

    // socket
    case API_EVENT_ID_SOCKET_CONNECTED:
        break; // param1: socketFd
    case API_EVENT_ID_SOCKET_CLOSED:
        break; // param1: socketFd
    case API_EVENT_ID_SOCKET_SENT:
        break; // param1: socketFd
    case API_EVENT_ID_SOCKET_RECEIVED:
        break; // param1: socketFd, param2: length of data received
    case API_EVENT_ID_SOCKET_ERROR:
        break; // param1: socketFd, param2: error cause(API_Socket_Error_t)

    // SMS
    case API_EVENT_ID_SMS_SENT:
        break;
    case API_EVENT_ID_SMS_RECEIVED:
        break; // param1:SMS_Encode_Type_t, param2:message content length, pParam1:message header info, pParam2:message content
    case API_EVENT_ID_SMS_ERROR:
        break; // param1:SMS_Error_t cause
    case API_EVENT_ID_SMS_LIST_MESSAGE:
        break; // pParam1:SMS_Message_Info_t*(!!!pParam1->data need to free!!!)

    // uart
    case API_EVENT_ID_UART_RECEIVED:
        break; // param1: uart number, param2: length, pParam1: data

    // GPS
    case API_EVENT_ID_GPS_UART_RECEIVED:
        break; // param1: length, pParam1: data

    // CALL
    case API_EVENT_ID_CALL_DIAL:
        break; // param1: isSuccess, param2:error code(CALL_Error_t)
    case API_EVENT_ID_CALL_HANGUP:
        break; // param1: is remote release call, param2:error code(CALL_Error_t)
    case API_EVENT_ID_CALL_INCOMING:
        break; // param1: number type, pParam1:number
    case API_EVENT_ID_CALL_ANSWER:
        break;
    case API_EVENT_ID_CALL_DTMF:
        break; // param1: key

    // error
    case API_EVENT_ID_MALLOC_FAILED:
        break;

    // ussd
    case API_EVENT_ID_USSD_IND:
        break; // pParam1: USSD_Type_t
    case API_EVENT_ID_USSD_SEND_SUCCESS:
        break; // pParam1: USSD_Type_t
    case API_EVENT_ID_USSD_SEND_FAIL:
        break; // param1:error code(USSD_Error_t) param2:error code2(USSD_Error_t)

    // power
    case API_EVENT_ID_POWER_INFO:
        break; // param1: (PM_Charger_State_t<<16|charge_level(%)) , param2: (PM_Battery_State_t<<16|battery_voltage(mV))

    case API_EVENT_ID_MAX:
        break;
    }
}

void app_Main(void)
{

    API_Event_t *event = NULL;

    mainTaskHandle = OS_CreateTask(MainTask,
                                   NULL, NULL, AppMain_TASK_STACK_SIZE, AppMain_TASK_PRIORITY, 0, 0, "MainTask");
    OS_SetUserMainHandle(&mainTaskHandle);

    while (1)
    {
        if (OS_WaitEvent(mainTaskHandle, (PVOID)(&event), OS_TIME_OUT_WAIT_FOREVER))
        {
            EventDispatch(event);
            OS_Free(event->pParam1);
            OS_Free(event->pParam2);
            OS_Free(event);
        }
    }
}