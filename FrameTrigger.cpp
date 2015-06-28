#include <iostream>
#include "FrameTrigger.hpp"

FrameTrigger::FrameTrigger(TriggerOptions &options, Connection::Frame &frame)
    : options(options),
      frame(frame) {
    this->frameSize = frame.size();
    calculateTriggerCallsList();
}

QList<FrameTrigger::TriggerCall> FrameTrigger::getTriggerCallsList() {
    return this->triggerCallsList;
}

void FrameTrigger::calculateTriggerCallsList() {
    int channel = options.channel;
    for (int i = 0; i < frameSize - 1; i++) {
        currentIndex = i;
        firstValue = frame[i][channel];
        secondValue = frame[i+1][channel];
        edge = getEdgeType();

        if (includeTriggerCall())
            addTriggerCall();
    }
}

void FrameTrigger::addTriggerCall() {
    TriggerCall newTrigger;
    newTrigger.index = currentIndex;
    newTrigger.key = getFractionKey();
    newTrigger.voltage = options.voltage;
    newTrigger.edge = edge;
    triggerCallsList.append(newTrigger);
}

double FrameTrigger::getFractionKey() {
    double fraction =  (options.voltage - secondValue) / (firstValue - secondValue);
    return currentIndex + 1.0 - fraction;
}

bool FrameTrigger::includeTriggerCall() {
    bool belongs = belongsTo(options.voltage, firstValue, secondValue);
    bool correctEdge = isCorrectEdge();
    return belongs && correctEdge;
}

bool FrameTrigger::isCorrectEdge() {
    if (options.edge == EdgeType::AnyEdge)
        return true;
    return options.edge == edge;
}

FrameTrigger::EdgeType FrameTrigger::getEdgeType() {
    if (firstValue > secondValue)
        return EdgeType::FallingEdge;
    else if (firstValue < secondValue)
        return EdgeType::RisingEdge;
    else
        return EdgeType::AnyEdge;
}

bool FrameTrigger::belongsTo(double value, double a, double b) {
    double min = std::min(a, b);
    double max = std::max(a, b);
    return value >= min && value <= max;
}