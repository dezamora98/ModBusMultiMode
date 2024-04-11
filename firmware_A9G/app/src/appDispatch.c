#include "app.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <api_os.h>
#include <api_event.h>
#include <api_hal_gpio.h>

#define GPIO_LED1 GPIO_PIN27

static bool SYS_READY = false;

void waitSystemReady(void)
{
    while (!SYS_READY)
    {
        OS_Sleep(10);
    }
}

static void GPIO_SYS_blink(GPIO_PIN pin, uint32_t ms)
{
    GPIO_config_t S_config;
    GPIO_config_t config = {
        .defaultLevel = GPIO_LEVEL_LOW,
        .mode = GPIO_MODE_OUTPUT,
        .pin = pin,
    };

    GPIO_GetConfig(pin, &S_config);

    GPIO_Init(config);

    GPIO_Set(pin, GPIO_LEVEL_HIGH);

    OS_Sleep(ms);

    GPIO_Set(pin, GPIO_LEVEL_LOW);

    GPIO_Init(S_config); // Restore initial configuration pin
}

void EventDispatch(API_Event_t *pEvent)
{
    switch (pEvent->id)
    {
        // system
    case API_EVENT_ID_POWER_ON: // param1: shows power on cause:
    {
        printf("EVENT_POWER_ON");
        GPIO_SYS_blink(GPIO_LED1, 50);
        break;
    }
    case API_EVENT_ID_SYSTEM_READY:
    {
        printf("EVENT_ID_SYSTEM_READY");
        GPIO_SYS_blink(GPIO_LED1, 50);
        SYS_READY = true;
        break;
    }

        // ERROR
    case API_EVENT_ID_MALLOC_FAILED:
    {
        while (true)
        {
            printf("EVENT_ID_MALLOC_FAILED");
            GPIO_SYS_blink(GPIO_LED1, 50);
        }
        break;
    }

        // power
    case API_EVENT_ID_POWER_INFO:
    {
        break; // param1: (PM_Charger_State_t<<16|charge_level(%)) , param2: (PM_Battery_State_t<<16|battery_voltage(mV))
    }

    // keypad
#ifdef __API_KEY_H
    case API_EVENT_ID_KEY_DOWN: // param1:key id(Key_ID_t)
        break;
    case API_EVENT_ID_KEY_UP: // param1:key id(Key_ID_t)
        break;
#endif

        // ussd
#ifdef __API_SS_H_
    case API_EVENT_ID_USSD_IND:
        break; // pParam1: USSD_Type_t
    case API_EVENT_ID_USSD_SEND_SUCCESS:
        break; // pParam1: USSD_Type_t
    case API_EVENT_ID_USSD_SEND_FAIL:
        break; // param1:error code(USSD_Error_t) param2:error code2(USSD_Error_t)
#endif

        // SMS
#ifdef __API_SMD_H_
    case API_EVENT_ID_SMS_SENT:
        break;
    case API_EVENT_ID_SMS_RECEIVED:
        break; // param1:SMS_Encode_Type_t, param2:message content length, pParam1:message header info, pParam2:message content
    case API_EVENT_ID_SMS_ERROR:
        break; // param1:SMS_Error_t cause
    case API_EVENT_ID_SMS_LIST_MESSAGE:
        break; // pParam1:SMS_Message_Info_t*(!!!pParam1->data need to free!!!)
#endif
        // uart
#ifdef __API_HAL_UART_H__
    case API_EVENT_ID_UART_RECEIVED:
        break; // param1: uart number, param2: length, pParam1: data
#endif

        // GPS
#ifdef __API_GPS_H_
    case API_EVENT_ID_GPS_UART_RECEIVED:
        break; // param1: length, pParam1: data
#endif

        // CALL
#ifdef __API_CALL_H
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
#endif

        // network
#ifdef __API_NETWORK_H_
    case API_EVENT_ID_NETWORK_REGISTERED_HOME:
        printf("EVENT_ID_NETWORK_REGISTERED_HOME");
        break;
    case API_EVENT_ID_NETWORK_REGISTERED_ROAMING:
        printf("EVENT_ID_NETWORK_REGISTERED_ROAMING");
        break;
    case API_EVENT_ID_NETWORK_REGISTER_SEARCHING:
        printf("EVENT_ID_NETWORK_REGISTER_SEARCHING");
        break;
    case API_EVENT_ID_NETWORK_REGISTER_DENIED:
        printf("EVENT_ID_NETWORK_REGISTER_DENIED");
        break;
    case API_EVENT_ID_NETWORK_REGISTER_NO:
        printf("EVENT_ID_NETWORK_REGISTER_NO");
        break;
    case API_EVENT_ID_NETWORK_DEREGISTER:
        printf("EVENT_ID_NETWORK_DEREGISTER");
        break;
    case API_EVENT_ID_NETWORK_DETACHED:
        printf("EVENT_ID_NETWORK_DETACHED");
        break;
    case API_EVENT_ID_NETWORK_ATTACH_FAILED:
        printf("EVENT_ID_NETWORK_ATTACH_FAILED");
        break;
    case API_EVENT_ID_NETWORK_ATTACHED:
        printf("EVENT_ID_NETWORK_ATTACHED");
        break;
    case API_EVENT_ID_NETWORK_DEACTIVED:
        printf("EVENT_ID_NETWORK_DEACTIVED");
        break;
    case API_EVENT_ID_NETWORK_ACTIVATE_FAILED:
        printf("EVENT_ID_NETWORK_ACTIVATE_FAILED");
        break;
    case API_EVENT_ID_NETWORK_ACTIVATED:
        printf("EVENT_ID_NETWORK_ACTIVATED");
        break;
    case API_EVENT_ID_NETWORK_GOT_TIME: // pParam1: RTC_Time_t*
        printf("EVENT_ID_NETWORK_GOT_TIME");
        break;
    case API_EVENT_ID_NETWORK_CELL_INFO: // param1:cell number(1 serving cell and param1-1 neighbor cell) , pParam1: Network_Location_t*
        printf("EVENT_ID_NETWORK_CELL_INFO");
        break;
#endif

        // socket
#ifdef __API_SOCKET__H_
    case API_EVENT_ID_SOCKET_CONNECTED:
        printf("EVENT_ID_SOCKET_CONNECTED");
        break; // param1: socketFd
    case API_EVENT_ID_SOCKET_CLOSED:
        printf("EVENT_ID_SOCKET_CLOSED");
        break; // param1: socketFd
    case API_EVENT_ID_SOCKET_SENT:
        printf("EVENT_ID_SOCKET_SENT");
        break; // param1: socketFd
    case API_EVENT_ID_SOCKET_RECEIVED:
        printf("EVENT_ID_SOCKET_RECEIVED");
        break; // param1: socketFd, param2: length of data received
    case API_EVENT_ID_SOCKET_ERROR:
        printf("EVENT_ID_SOCKET_ERROR");
        break; // param1: socketFd, param2: error cause(API_Socket_Error_t)

    // DNS
    case API_EVENT_ID_DNS_SUCCESS:
        printf("EVENT_ID_DNS_SUCCESS");
        break; // param1:IP address(uint32_t), pPram1:domain(char*), pParam2:ip(char*)
    case API_EVENT_ID_DNS_ERROR:
        printf("EVENT_ID_DNS_ERROR");
        break;
#endif

    default:
        break;
    }
}
