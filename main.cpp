#include "mbed.h"

DigitalOut led1(LED1);
Serial pc(USBTX, USBRX);
Timer waitTimer;

int main() {
    const int loopTime = 1000;
    bool abort = false;

    waitTimer.start();

    while(!abort) {
        led1 = !led1;
        pc.printf("This is a print\r\n");
        
        Thread::wait((loopTime - waitTimer.read_ms()));
        waitTimer.reset();
    }    
}

