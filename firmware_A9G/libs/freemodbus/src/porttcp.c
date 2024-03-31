/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2019 flybreak <guozhanxin@rt-thread.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.60 2019/07/11 17:04:32 flybreak $
 */

#include "port.h"

#ifdef PKG_MODBUS_SLAVE_TCP
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "tcpserver.h"

/* ----------------------- Defines  -----------------------------------------*/
#define MB_TCP_DEFAULT_PORT     502
#define MB_TCP_BUF_SIZE     ( 256 + 7 )

/* ----------------------- Static variables ---------------------------------*/
static tcpclient_t mb_client;
static uint8_t    prvvTCPBuf[MB_TCP_BUF_SIZE];
static uint16_t   prvvTCPLength;

static void tcpserver_event_notify(tcpclient_t client, rt_uint8_t event)
{
    static rt_tick_t recv_tick = 0;
    switch (event)
    {
    case TCPSERVER_EVENT_CONNECT:
        if (mb_client == RT_NULL)
        {
            mb_client = client;
        }
        else
        {
            if(rt_tick_get() - recv_tick > 30 * RT_TICK_PER_SECOND) /* set timeout as 30s */
            {
                tcpserver_close(mb_client);
                mb_client = client;
                recv_tick = rt_tick_get();
            }
            else
            {
                tcpserver_close(client);
                rt_kprintf("Multi-host is not supported, please disconnect the current host first!\n");
            }
        }
        break;
    case TCPSERVER_EVENT_RECV:
        if( mb_client == client)
        {
            recv_tick = rt_tick_get();
            prvvTCPLength = tcpserver_recv(mb_client, &prvvTCPBuf, MB_TCP_BUF_SIZE, 100);
            if (prvvTCPLength)
            {
                xMBPortEventPost(EV_FRAME_RECEIVED);
            }
        }
        break;
    case TCPSERVER_EVENT_DISCONNECT:
        if (mb_client == client)
            mb_client = RT_NULL;
        break;
    default:
        break;
    }
}

bool
xMBTCPPortInit(uint16_t usTCPPort)
{
    struct tcpserver *serv;

    if (usTCPPort == 0)
        usTCPPort = MB_TCP_DEFAULT_PORT;

    serv = tcpserver_create(0, usTCPPort);

    tcpserver_set_notify_callback(serv, tcpserver_event_notify);

    return true;
}

void
vMBTCPPortClose(void)
{
    tcpserver_destroy(mb_client->server);
}

void
vMBTCPPortDisable(void)
{
    tcpserver_close(mb_client);
}

bool
xMBTCPPortGetRequest(uint8_t **ppucMBTCPFrame, uint16_t *usTCPLength)
{
    *ppucMBTCPFrame = &prvvTCPBuf[0];
    *usTCPLength = prvvTCPLength;

    return true;
}

bool
xMBTCPPortSendResponse(const uint8_t *pucMBTCPFrame, uint16_t usTCPLength)
{
    rt_int16_t ret;
    bool bFrameSent = false;

    if (mb_client)
    {
        ret = tcpserver_send(mb_client, (void *)pucMBTCPFrame, usTCPLength, 0);
        if (ret == usTCPLength)
            bFrameSent = true;
    }
    return bFrameSent;
}
#endif
