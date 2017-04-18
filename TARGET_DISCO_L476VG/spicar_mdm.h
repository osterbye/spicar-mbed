#ifndef SPICAR_MDM_H_
#define SPICAR_MDM_H_

class SpiCar_MDM
{
public:
    SpiCar_MDM(Serial *pc) {}
    ~SpiCar_MDM() {}

    bool initialize() {return false; }
    void stop_thread() {}
    void loop() {}
#ifdef DEBUG_MDM
    void test_modem() {}
#endif
};

#endif // SPICAR_MDM_H_