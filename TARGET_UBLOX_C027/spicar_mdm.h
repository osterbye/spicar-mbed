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
#ifndef SPICAR_MDM_H_
#define SPICAR_MDM_H_
#include "mbed.h"
#include "C027_Support/MDM.h"

//#define DEBUG_MDM

class SpiCar_MDM
{
public:
    SpiCar_MDM(Serial *pc);
    ~SpiCar_MDM();

    bool initialize();
    void stop_thread();
    void loop();
#ifdef DEBUG_MDM
    void test_modem();
#endif

private:
    MDMSerial mdm;
    bool abort;
    Serial *pc;
    Timer wait_timer;
    int refresh_time;
    bool modem_is_on;
};

#endif // SPICAR_MDM_H_
