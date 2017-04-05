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
#include "ublox/spicar_gnss.h"
#include "ublox/spicar_mdm.h"

#include "benchmarks/benchmark_thread.h"

DigitalOut led1(LED1);
Serial pc(USBTX, USBRX);
Timer waitTimer;

int main() {
    const int loopTime = 1000;
    bool abort = false;
    SpiCar_GNSS gnss(&pc);
    Thread gnss_thread(osPriorityBelowNormal, DEFAULT_STACK_SIZE*0.75);
    SpiCar_MDM mdm(&pc);
    Thread mdm_thread(osPriorityBelowNormal, DEFAULT_STACK_SIZE*1.25);

    gnss_thread.start(&gnss, &SpiCar_GNSS::loop);

    if (mdm.initialize()) {
        mdm_thread.start(&mdm, &SpiCar_MDM::loop);
    }

    waitTimer.start();
    while(!abort) {
        led1 = !led1;
        time_t seconds = time(NULL);
        pc.printf("Time: %s\r\n", ctime(&seconds));

        // Benchmarks
        //print_thread_data(&Thread, &pc);
        print_thread_data(&gnss_thread, &pc);
        print_thread_data(&mdm_thread, &pc);

        Thread::wait((loopTime - waitTimer.read_ms()));
        waitTimer.reset();
    }    
}
