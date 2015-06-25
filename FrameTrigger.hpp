#pragma once

#include <QList>
#include "TriggerOptions.hpp"
#include "Connection.hpp"

class FrameTrigger {
public:
    typedef TriggerOptions::EdgeType EdgeType;

    struct TriggerCall {
        int index;
        double key;
        double voltage;
        EdgeType edge;
    };

protected:
    TriggerOptions options;
    Connection::Frame &frame;
    int frameSize;

private:
    QList<TriggerCall> triggerCallsList;

    int currentIndex;
    double firstValue;
    double secondValue;
    EdgeType edge;

public:
    FrameTrigger(TriggerOptions &options, Connection::Frame &frame);

    QList<TriggerCall> getTriggerCallsList();

    inline bool isFoundAnyTriggerCall() {
        return triggerCallsList.size() > 0;
    }

private:
    void calculateTriggerCallsList();

    void addTriggerCall();
    double getFractionKey();

    bool includeTriggerCall();

    bool isCorrectEdge();
    EdgeType getEdgeType();

    static bool belongsTo(double value, double a, double b);
};