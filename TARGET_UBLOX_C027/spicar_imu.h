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
#ifndef SPICAR_IMU_H_
#define SPICAR_IMU_H_
#include "mbed.h"
#include "LSM9DS1/LSM9DS1.h"

#define TEST_IMU


#define LSM9DS1_PRIMARY_XG_ADDR 0xD6
#define LSM9DS1_PRIMARY_M_ADDR  0x3C

class SpiCar_IMU
{
public:
    SpiCar_IMU(PinName sda, PinName scl, uint8_t xgAddr, uint8_t mAddr, Serial *pc);
    ~SpiCar_IMU();

    bool initialize();
    void stop_thread();
    void loop();
    void calibrate();
#ifdef TEST_IMU
    void test_imu(float ax, float ay, float az, float mx, float my);
#endif

private:
    LSM9DS1 *imu;
    bool abort;
    Serial *pc;
    Timer wait_timer;
    int refresh_time;
};

#endif // SPICAR_IMU_H_
