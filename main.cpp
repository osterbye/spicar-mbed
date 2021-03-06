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
#include "mbed.h"
#include "spicar_gnss.h"
#include "spicar_mdm.h"
#include "spicar_imu.h"
#include "dispatcher.h"
#include "console.h"

#include "benchmarks/benchmark_thread.h"

DigitalOut led1(LED1);
Serial pc(USBTX, USBRX);
Timer waitTimer;

Thread dispatcherThread;

int main() {
    const int loopTime = 10000;
    bool abort = false;
    SpiCar_GNSS gnss(&pc);
    Thread gnss_thread(osPriorityBelowNormal, 112*8);
    SpiCar_MDM mdm(&pc);
    Thread mdm_thread(osPriorityBelowNormal, 270*8);
    SpiCar_IMU imu(SDA, SCL, LSM9DS1_PRIMARY_XG_ADDR, LSM9DS1_PRIMARY_M_ADDR, &pc);
    Thread imu_thread(osPriorityBelowNormal, 96*8);

    gnss_thread.start(callback(&gnss, &SpiCar_GNSS::loop));

    if (mdm.initialize()) {
        mdm_thread.start(callback(&mdm, &SpiCar_MDM::loop));
    }

    if (imu.initialize()) {
        imu_thread.start(callback(&imu, &SpiCar_IMU::loop));
    }

    console_init(&pc);
    dispatcherThread.start(dispatcher_task);

    waitTimer.start();
    while(!abort) {
        led1 = !led1;
        time_t seconds = time(NULL);
        pc.printf("Time: %s\r\n", ctime(&seconds));

        // Benchmarks
        //print_thread_data(&Thread, &pc);
        print_thread_data(&gnss_thread, &pc);
        print_thread_data(&mdm_thread, &pc);
        print_thread_data(&imu_thread, &pc);

        // execute console task for remaining loopTime
        while ((loopTime - waitTimer.read_ms()) > 0) {
            console_task();
            Thread::wait(5);
        }
        waitTimer.reset();
    }    
}
