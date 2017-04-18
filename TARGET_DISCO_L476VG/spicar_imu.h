#ifndef SPICAR_IMU_H_
#define SPICAR_IMU_H_

#define LSM9DS1_PRIMARY_XG_ADDR 0
#define LSM9DS1_PRIMARY_M_ADDR 0

class SpiCar_IMU
{
public:
    SpiCar_IMU(PinName sda, PinName scl, uint8_t xgAddr, uint8_t mAddr, Serial *pc) {}
    ~SpiCar_IMU() {}

    bool initialize() {return false;}
    void stop_thread() {}
    void loop() {}
    void calibrate() {}
#ifdef TEST_IMU
    void test_imu(float ax, float ay, float az, float mx, float my) {}
#endif
};

#endif // SPICAR_IMU_H_