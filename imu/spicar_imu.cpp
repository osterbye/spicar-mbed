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
#include "spicar_imu.h"

SpiCar_IMU::SpiCar_IMU(PinName sda, PinName scl, uint8_t xgAddr, uint8_t mAddr, Serial *pc) :
            abort(false), pc(pc), refresh_time(500) {
    imu = new LSM9DS1(sda, scl, xgAddr, mAddr);
}

SpiCar_IMU::~SpiCar_IMU() {
}

bool SpiCar_IMU::initialize() {
    if (!imu->begin()) {
        pc->printf("Failed to communicate with LSM9DS1.\r\n");
        return false;
    }
    return true;
}

void SpiCar_IMU::stop_thread() {
    abort = true;
}

void SpiCar_IMU::loop() {
    wait_timer.start();
    while (!abort) {
        // TODO: align with ODR, so we don't have busy loops
        while (!imu->tempAvailable());
        imu->readTemp();
        while (!imu->magAvailable());
        imu->readMag();
        while (!imu->accelAvailable());
        imu->readAccel();
        while (!imu->gyroAvailable());
        imu->readGyro();

#ifdef TEST_IMU
        test_imu(imu->calcAccel(imu->ax), imu->calcAccel(imu->ay), imu->calcAccel(imu->az), imu->calcMag(imu->mx),
                 imu->calcMag(imu->my));
#endif
        Thread::wait((refresh_time - wait_timer.read_ms()));
        wait_timer.reset();
    }
}

void SpiCar_IMU::calibrate() {
    imu->calibrate(1);
    imu->calibrateMag(0);
}

#ifdef TEST_IMU
// Test code taken from the original LSM9DS1 library example application.
#define PI 3.14159
// Earth's magnetic field varies by location. Add or subtract a declination to get a more accurate heading.
// Calculate your's here: http://www.ngdc.noaa.gov/geomag-web/#declination
#define DECLINATION 3.59714 // Declination (degrees) in Copenhagen, Denmark, 170406.
// Calculate pitch, roll, and heading.
// Pitch/roll calculations taken from this app note:
// http://cache.freescale.com/files/sensors/doc/app_note/AN3461.pdf?fpsp=1
// Heading calculations taken from this app note:
// http://www51.honeywell.com/aero/common/documents/myaerospacecatalog-documents/Defense_Brochures-documents/Magnetic__Literature_Application_notes-documents/AN203_Compass_Heading_Using_Magnetometers.pdf
void SpiCar_IMU::test_imu(float ax, float ay, float az, float mx, float my) {
    float roll = atan2(ay, az);
    float pitch = atan2(-ax, sqrt(ay * ay + az * az));
    // touchy trig stuff to use arctan to get compass heading (scale is 0..360)
    mx = -mx;
    float heading;
    if (my == 0.0) {
        heading = (mx < 0.0) ? 180.0 : 0.0;
    } else {
        heading = atan2(mx, my)*360.0/(2.0*PI);
    }
    //pc->printf("heading atan=%f \n\r",heading);
    heading -= DECLINATION; //correct for geo location
    if (heading > 180.0) {
        heading = heading - 360.0;
    } else if (heading < -180.0) {
        heading = 360.0 + heading;
    } else if (heading < 0.0) {
        heading = 360.0 + heading;
    }

    // Convert everything from radians to degrees: heading *= 180.0 / PI;
    pitch *= 180.0 / PI;
    roll  *= 180.0 / PI;

    pc->printf("Pitch: %f,    Roll: %f degress\n\r",pitch,roll);
    pc->printf("Magnetic Heading: %f degress\n\r",heading);
}
#endif
