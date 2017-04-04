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
#ifndef SPICAR_GNSS_H_
#define SPICAR_GNSS_H_
#include "mbed.h"
#include "C027_Support/GPS.h"

class SpiCar_GNSS
{
public:
    SpiCar_GNSS(Serial *pc);
    ~SpiCar_GNSS();

    void stop_thread();
    void loop();

private:
#if 1   // use GPSI2C class
    GPSI2C gnss;
#else   // or GPSSerial class
    GPSSerial gnss;
#endif

    bool abort;
    Serial *pc;
    Timer wait_timer;
    int refresh_time;
    bool do_set_time;
};

#endif // SPICAR_GNSS_H_
