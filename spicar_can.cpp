#include "spicar_can.h"

#ifdef TARGET_DISCO_L476VG
    #define CAN1_RD_PIN    PD_0 // on P1 extension connector (4th from bottom)
    #define CAN1_TD_PIN    PB_9 // on U5 LCD socket pin 13 (bottom row, 2nd from right)
#endif
#ifdef TARGET_UBLOX_C027
    #define CAN1_RD_PIN    CANRD
    #define CAN1_TD_PIN    CANTD
    #define CAN1_SB_PIN    CANS // standby (low=normal, high=standby/rxonly)
#endif

SpiCar_CAN::SpiCar_CAN(): baud(1000000), bus(CAN1_RD_PIN, CAN1_TD_PIN), refresh_time(20) {
    bus.frequency(baud);
    bus.attach(this, &SpiCar_CAN::rxIRQ, CAN::RxIrq);
    /* TODO: error handling by attaching handlers to following IRQs:
        CAN::BeIrq for bus error
        CAN::EwIrq for error warning
        CAN::DoIrq for data overrun
        CAN::WuIrq for wake-up
        CAN::EpIrq for error passive
    */
}

SpiCar_CAN::~SpiCar_CAN() {

}

void SpiCar_CAN::loop() {
    waitTimer.start();
    while(1) {
        int success;
        /* send pending messages */
        osEvent evt = txMailbox.get(0);
        if (evt.status == osEventMail) {
            CANMessage *msg = (CANMessage*)evt.value.p;
            success = bus.write(*msg);
            if (success) {
                txMailbox.free(msg);
            } /* else, msg is not freed and sending is retried on next loop */
            // TODO: report error? console is bad choice as disconnected CAN bus
            // would overload serial communication
        }
        /* call attached callbacks for received messages */
        evt = rxMailbox.get(0);
        if (evt.status == osEventMail) {
            CANMessage *msg = (CANMessage*)evt.value.p;
            // TODO: if configured, write msg to SD card file log
            for (int i = 0; i < MAX_FILTER_COUNT; i++) {
                filters[i].handleMatch(*msg);
            }
        }
        Thread::wait((refresh_time - waitTimer.read_ms()));
        waitTimer.reset();
    }
}

void SpiCar_CAN::rxIRQ() {
    if(bus.read(msgTmp)) {
        CANMessage * rxMsg = rxMailbox.alloc();
        if (rxMsg) {
            *rxMsg = msgTmp;
            rxMailbox.put(rxMsg);
        } // TODO: handle else case (rx buffer overrun)
    }
}

int SpiCar_CAN::send(CANMessage &msg) {
    CANMessage * txMsg = txMailbox.alloc();
    if (txMsg) {
        *txMsg = msgTmp;
        txMailbox.put(txMsg);
        return 1;    
    } else {
        return 0;
    }
}

void SpiCar_CAN::attachCallback(unsigned int id, Callback<void()> func) {
}
