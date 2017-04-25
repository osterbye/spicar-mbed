#include "canfilter.h"

CANFilter::CANFilter() {
    setEnabled(false);
}

CANFilter::CANFilter(uint32_t id, uint32_t mask, Callback<void (const CANMessage &)> handler) {
    this->id = id & 0x1FFFFFFF;
    this->mask = mask;
    this->handler = handler;
    setEnabled(true);
}

CANFilter::CANFilter(uint32_t id, Callback<void (const CANMessage &)> handler) {
    this->id = id & 0x1FFFFFFF;
    this->mask = 0x1FFFFFFF;
    this->handler = handler;
    setEnabled(true);
}

CANFilter::CANFilter(Callback<void (const CANMessage &)> handler) {
    this->id = 0;
    this->mask = 0;
    this->handler = handler;
    setEnabled(true);
}

CANFilter::~CANFilter() {
}

bool CANFilter::matches(const CANMessage & msg) {
    return enabled && ((id & mask) == (msg.id & mask));
}

bool CANFilter::handleMatch(const CANMessage & msg) {
    if (enabled && ((id & mask) == (msg.id & mask))) {
        handler(msg);
        return 1;
    } else {
        return 0;
    }
}

bool CANFilter::isEnabled() const
{
    return enabled;
}

void CANFilter::setEnabled(bool value)
{
    /* TODO: implement low-level hardware acceptance filter handling to prevent
    burning CPU cycles for each CAN frame */
    enabled = value;
}
