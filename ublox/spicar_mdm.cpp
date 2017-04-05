/* SpiCar mbed platform
 * Copyright (c) 2017 Spiri Mobility Solutions
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "spicar_mdm.h"

SpiCar_MDM::SpiCar_MDM(Serial *pc) : abort(false), pc(pc), refresh_time(15000), modem_is_on(false) {
}

SpiCar_MDM::~SpiCar_MDM() {
}

bool SpiCar_MDM::initialize() {
    MDMParser::DevStatus devStatus = {};
    MDMParser::NetStatus netStatus = {};
    bool mdmOk = mdm.init(NULL, &devStatus);
    mdm.dumpDevStatus(&devStatus);
    if (mdmOk) {
        // wait until we are connected
        mdmOk = mdm.registerNet(&netStatus);
        mdm.dumpNetStatus(&netStatus);
    }

    if (mdmOk) {
        // join the internet connection
        MDMParser::IP ip = mdm.join(NULL,NULL,NULL);
        if (ip == NOIP) {
            pc->printf("Not able to join network");
            mdmOk = false;
        } else {
            mdm.dumpIp(ip);
            modem_is_on = true;
        }
    }
    return mdmOk;
}

void SpiCar_MDM::stop_thread() {
    abort = true;
}

void SpiCar_MDM::loop() {
    wait_timer.start();
    while (!abort) {
        // check the network status
        MDMParser::NetStatus net_status = {};
        if (mdm.checkNetStatus(&net_status)) {
            mdm.dumpNetStatus(&net_status);
        }

        // checking unread sms
        char buf[512] = "";
        int ix[8];
        int n = mdm.smsList("REC UNREAD", ix, 8);
        if (8 < n) n = 8;
        while (0 < n--) {
            char num[32];
            pc->printf("Unread SMS at index %d\r\n", ix[n]);
            if (mdm.smsRead(ix[n], num, buf, sizeof(buf))) {
                pc->printf("Got SMS from \"%s\" with text \"%s\"\r\n", num, buf);
                pc->printf("Delete SMS at index %d\r\n", ix[n]);
                mdm.smsDelete(ix[n]);
#if 0
                // provide a reply
                const char* reply = "Hello my friend";
                if (strstr(buf, /*w*/"here are you")) {
                    reply = *link ? link : "I don't know"; // reply wil location link
                } else if (strstr(buf, /*s*/"hutdown")) {
                    abort = true, reply = "bye bye";
                }
                printf("Send SMS reply \"%s\" to \"%s\"\r\n", reply, num);
                mdm.smsSend(num, reply);
#endif
            }
        }

        Thread::wait((refresh_time - wait_timer.read_ms()));
        wait_timer.reset();
    }
}

#ifdef DEBUG_MDM
void SpiCar_MDM::test_modem() {
    int ret;
    char buf[512] = "";

    printf("Make a Http Post Request\r\n");
    int socket = mdm.socketSocket(MDMParser::IPPROTO_TCP);
    if (socket >= 0)
    {
        mdm.socketSetBlocking(socket, 10000);
        if (mdm.socketConnect(socket, "mbed.org", 80))
        {
            const char http[] = "GET /media/uploads/mbed_official/hello.txt HTTP/1.0\r\n\r\n";
            mdm.socketSend(socket, http, sizeof(http)-1);

            ret = mdm.socketRecv(socket, buf, sizeof(buf)-1);
            if (ret > 0)
                printf("Socket Recv \"%*s\"\r\n", ret, buf);
            mdm.socketClose(socket);
        }
        mdm.socketFree(socket);
    }

    int port = 7;
    const char* host = "echo.u-blox.com";
    MDMParser::IP ip = mdm.gethostbyname(host);
    char data[] = "\r\nxxx Socket Hello World\r\n"
#ifdef LARGE_DATA
                "00  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
                "01  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
                "02  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
                "03  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
                "04  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"

                "05  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
                "06  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
                "07  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
                "08  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
                "09  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"

                "10  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
                "11  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
                "12  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
                "13  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
                "14  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"

                "15  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
                "16  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
                "17  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
                "18  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
                "19  0123456789 0123456789 0123456789 0123456789 0123456789 \r\n"
#endif
                "End\r\n";

    printf("Testing TCP sockets with ECHO server\r\n");
    socket = mdm.socketSocket(MDMParser::IPPROTO_TCP);
    if (socket >= 0)
    {
        mdm.socketSetBlocking(socket, 10000);
        if (mdm.socketConnect(socket, host, port)) {
            memcpy(data, "\r\nTCP", 5);
            ret = mdm.socketSend(socket, data, sizeof(data)-1);
            if (ret == sizeof(data)-1) {
                printf("Socket Send %d \"%s\"\r\n", ret, data);
            }
            ret = mdm.socketRecv(socket, buf, sizeof(buf)-1);
            if (ret >= 0) {
                printf("Socket Recv %d \"%.*s\"\r\n", ret, ret, buf);
            }
            mdm.socketClose(socket);
        }
        mdm.socketFree(socket);
    }

    printf("Testing UDP sockets with ECHO server\r\n");
    socket = mdm.socketSocket(MDMParser::IPPROTO_UDP, port);
    if (socket >= 0)
    {
        mdm.socketSetBlocking(socket, 10000);
        memcpy(data, "\r\nUDP", 5);
        ret = mdm.socketSendTo(socket, ip, port, data, sizeof(data)-1);
        if (ret == sizeof(data)-1) {
            //printf("Socket SendTo %s:%u " IPSTR " %u \"%s\"\r\n", host, port, IPNUM(ip), ret, data);
            printf("Socket SendTo %s:%u %lu.%lu.%lu.%lu %u \"%s\"\r\n", host, port, IPNUM(ip), ret, data);
        }
        ret = mdm.socketRecvFrom(socket, &ip, &port, buf, sizeof(buf)-1);
        if (ret >= 0) {
            printf("Socket RecvFrom %lu.%lu.%lu.%lu:%u %u \"%.*s\" \r\n", IPNUM(ip),port, ret, ret,buf);
        }
        mdm.socketFree(socket);
    }

    // disconnect
    mdm.disconnect();

    // http://www.geckobeach.com/cellular/secrets/gsmcodes.php
    // http://de.wikipedia.org/wiki/USSD-Codes
    const char* ussd = "*130#"; // You may get answer "UNKNOWN APPLICATION"
    printf("Ussd Send Command %s\r\n", ussd);
    ret = mdm.ussdCommand(ussd, buf);
    if (ret > 0)
        printf("Ussd Got Answer: \"%s\"\r\n", buf);
}
#endif
