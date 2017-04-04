#include "mbed.h"

DigitalOut led1(LED1);
Serial pc(USBTX, USBRX);

int main() {
    bool abort = false;

    while(!abort) {
        led1 = !led1;
        pc.printf("This is a print\r\n");
        Thread::wait(1000);
    }    
}

