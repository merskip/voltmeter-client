#pragma once

#include "TriggerOptions.hpp"
#include "Connection.hpp"
#include "FrameTrigger.hpp"

class ShiftFrameTrigger : public FrameTrigger {
private:
    double sizeHalf;
    TriggerCall centerTrigger;

public:
    ShiftFrameTrigger(TriggerOptions &options, Connection::Frame &frame);

    double getShift();
    TriggerCall getCenterTriggerCall();

private:
    TriggerCall calculateCenterTriggerCall();
    bool isCloser(TriggerCall first, TriggerCall second);
    double distanceFromCenter(TriggerCall trigger);

    inline bool isFrameSizeOdd() {
        return frameSize % 2 != 0;
    }
};


