#include <iostream>
#include "FrameShiftTrigger.hpp"

FrameShiftTrigger::FrameShiftTrigger(TriggerOptions &options, Connection::Frame &frame)
        : options(options),
          frame(frame) {
    this->frameSize = frame.size();
    this->sizeHalf = frameSize / 2;
}

double FrameShiftTrigger::calculateShift() {
    for (int i = 0; i < this->sizeHalf; i++) {
        indexes = getDataIndexes(i);
        if (canCheckOnLeft()) {
            isOnLeft = true;
            calculateValuesOnLeft();
            if (checkValuesOnLeft()) {
                return getDoubleShift(-i - 1);
            }
        }
        if (canCheckOnRight()) {
            isOnLeft = false;
            calculateValuesOnRight();
            if (checkValuesOnRight())
                return getDoubleShift(i);
        }
    }
    return 0;
}

FrameShiftTrigger::DataIndexes FrameShiftTrigger::getDataIndexes(int radius) {
    return {
            .left = sizeHalf - radius,
            .nextLeft = sizeHalf - radius - 1,

            .right = sizeHalf + radius,
            .nextRight = sizeHalf + radius + 1
    };
}

double FrameShiftTrigger::getDoubleShift(int i) {
    fraction = getFractionShift();

    if (isFrameSizeOdd())
        fraction += 0.5;

    if (edge == EdgeType::FallingEdge)
        fraction -= 1.0;

    return i - fraction;
}

double FrameShiftTrigger::getFractionShift() {
    double trigger = options.voltage;
    return trigger / (firstValue - secondValue);
}


bool FrameShiftTrigger::isFrameSizeOdd() {
    return frameSize % 2 != 0;
}

bool FrameShiftTrigger::canCheckOnLeft() {
    return indexes.nextLeft >= 0;
}

bool FrameShiftTrigger::canCheckOnRight() {
    return indexes.nextRight < frameSize;
}

void FrameShiftTrigger::calculateValuesOnLeft() {
// Nazwy indeksy są względne względem środka,
    // więc następny indeks po lewej
    // jest pierwszym licząc od początku
    firstValue = frame.at(indexes.nextLeft).at(options.channel);
    secondValue = frame.at(indexes.left).at(options.channel);
    edge = getEdgeType();
}

void FrameShiftTrigger::calculateValuesOnRight() {
    firstValue = frame.at(indexes.right).at(options.channel);
    secondValue = frame.at(indexes.nextRight).at(options.channel);
    edge = getEdgeType();
}

bool FrameShiftTrigger::checkValuesOnLeft() {
    return checkValues(firstValue, secondValue);
}

bool FrameShiftTrigger::checkValuesOnRight() {
    return checkValues(firstValue, secondValue);
}

bool FrameShiftTrigger::checkValues(double firstValue, double secondValue) {
    bool belongs = belongsTo(options.voltage, firstValue, secondValue);
    bool correctEdge = isCorrectEdge();
    return belongs && correctEdge;
}

bool FrameShiftTrigger::isCorrectEdge() {
    if (options.edge == EdgeType::AnyEdge)
        return true;
    return options.edge == edge;
}

FrameShiftTrigger::EdgeType FrameShiftTrigger::getEdgeType() {
    if (firstValue > secondValue)
        return EdgeType::FallingEdge;
    else if (firstValue < secondValue)
        return EdgeType::RisingEdge;
    else
        return EdgeType::AnyEdge;
}

bool FrameShiftTrigger::belongsTo(double value, double a, double b) {
    double min = std::min(a, b);
    double max = std::max(a, b);
    return value >= min && value <= max;
}
