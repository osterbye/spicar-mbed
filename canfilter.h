#ifndef CANFILTER_H
#define CANFILTER_H

#include "mbed.h"
#include "CAN.h"

class CANFilter
{
public:
    CANFilter();
    CANFilter(uint32_t id, uint32_t mask, Callback<void (const CANMessage &)> handler);
    CANFilter(uint32_t id, Callback<void (const CANMessage &)> handler);
    CANFilter(Callback<void(const CANMessage &)> handler);
    ~CANFilter();

    bool matches(const CANMessage &);
    bool handleMatch(const CANMessage &);
    bool isEnabled() const;
    void setEnabled(bool value);

private:
    uint32_t id;
    uint32_t mask;
    bool  enabled;
    Callback<void(const CANMessage &)> handler;
};

#endif // CANFILTER_H
