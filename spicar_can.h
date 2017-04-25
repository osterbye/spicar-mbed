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
#ifndef SPICAR_CAN_H_
#define SPICAR_CAN_H_

#include "mbed.h"
#include "canfilter.h"

#define BUFFER_SIZE        100
#define MAX_FILTER_COUNT   50

class SpiCar_CAN {
public:
    SpiCar_CAN();
    ~SpiCar_CAN();
    void loop();
    int send(CANMessage &msg);
    void attachCallback(unsigned int id, Callback<void()> func);

private:
    const uint32_t baud;
    CAN bus; /* TODO: support multiple buses */
    Mail<CANMessage, BUFFER_SIZE> rxMailbox;
    Mail<CANMessage, BUFFER_SIZE> txMailbox;
    CANMessage msgTmp; // allocated once for temporary storage
    CANFilter filters[MAX_FILTER_COUNT];
    Timer waitTimer;
    int refresh_time;

    void rxIRQ();
};

#endif // SPICAR_CAN_H_
