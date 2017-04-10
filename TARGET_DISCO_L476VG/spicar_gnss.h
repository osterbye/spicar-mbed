#ifndef SPICAR_GNSS_H_
#define SPICAR_GNSS_H_

#include "mbed.h"

class SpiCar_GNSS
{
public:
    SpiCar_GNSS(Serial *pc) {}
    ~SpiCar_GNSS() {}

    void stop_thread() {}
    void loop() {}
};


#endif // SPICAR_GNSS_H_