#include <iostream>
#include "ShiftFrameTrigger.hpp"

ShiftFrameTrigger::ShiftFrameTrigger(TriggerOptions &options, Connection::Frame &frame)
        : FrameTrigger(options, frame) {
    this->sizeHalf = frameSize / 2;

    if (isFoundAnyTriggerCall())
        this->centerTrigger = calculateCenterTriggerCall();
}

double ShiftFrameTrigger::getShift() {
    if (!isFoundAnyTriggerCall())
        return 0.0;

    if (isFrameSizeOdd())
        return centerTrigger.key - sizeHalf - 0.5;
    else
        return centerTrigger.key - sizeHalf;
}

FrameTrigger::TriggerCall ShiftFrameTrigger::getCenterTriggerCall() {
    return this->centerTrigger;
}

FrameTrigger::TriggerCall ShiftFrameTrigger::calculateCenterTriggerCall() {
    QList<TriggerCall> calls = getTriggerCallsList();
    TriggerCall closest = calls[0];
    for (TriggerCall trigger : calls) {
        if (isCloser(trigger, closest))
            closest = trigger;
    }
    return closest;
}

bool ShiftFrameTrigger::isCloser(TriggerCall first, TriggerCall second) {
    return distanceFromCenter(first) < distanceFromCenter(second);
}

double ShiftFrameTrigger::distanceFromCenter(TriggerCall trigger) {
    return fabs(sizeHalf - trigger.key);
}
