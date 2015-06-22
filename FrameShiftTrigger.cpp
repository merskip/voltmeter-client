#include "FrameShiftTrigger.hpp"

FrameShiftTrigger::FrameShiftTrigger(TriggerOptions &options, Connection::Frame &frame)
        : options(options),
          frame(frame) {
    this->frameSize = frame.size();
    this->sizeHalf = frameSize / 2;
}

int FrameShiftTrigger::calculateShift() {
    for (int i = 0; i < this->sizeHalf; i++) {
        indexes = getDataIndexes(i);
        if (canCheckOnLeft()) {
            if (checkValuesOnLeft())
                return -i;
        }
        if (canCheckOnRight()) {
            if (checkValuesOnRight())
                return i;
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


bool FrameShiftTrigger::canCheckOnLeft() {
    return indexes.nextLeft >= 0;
}

bool FrameShiftTrigger::canCheckOnRight() {
    return indexes.nextRight < frameSize;
}


bool FrameShiftTrigger::checkValuesOnLeft() {
    // Nazwy indeksy są względne względem środka,
    // więc następny indeks po lewej
    // jest pierwszym licząc od początku
    double firstValue = frame.at(indexes.nextLeft).at(options.channel);
    double secondValue = frame.at(indexes.left).at(options.channel);
    return checkValues(firstValue, secondValue);
}

bool FrameShiftTrigger::checkValuesOnRight() {
    double firstValue = frame.at(indexes.right).at(options.channel);
    double secondValue = frame.at(indexes.nextRight).at(options.channel);
    return checkValues(firstValue, secondValue);
}

bool FrameShiftTrigger::checkValues(double firstValue, double secondValue) {
    bool belongs = belongsTo(options.voltage, firstValue, secondValue);
    bool correctEdge = isCorrectEdge(firstValue, secondValue);
    return belongs && correctEdge;
}

bool FrameShiftTrigger::isCorrectEdge(double firstValue, double secondValue) {
    if (options.edge == TriggerOptions::FallingEdge)
        return firstValue > secondValue;
    else if (options.edge == TriggerOptions::RisingEdge)
        return firstValue < secondValue;
    else
        // Dla dowolnej krawędzi (AnyEdge),
        // sprawdzenie krawędzi zawsze zawraca true
        return true;
}

bool FrameShiftTrigger::belongsTo(double value, double a, double b) {
    double min = std::min(a, b);
    double max = std::max(a, b);
    return value >= min && value <= max;
}
