#pragma once

#include "TriggerOptions.hpp"
#include "Connection.hpp"

class FrameShiftTrigger {
    typedef TriggerOptions::EdgeType EdgeType;

    struct DataIndexes {
        int left, nextLeft;
        int right, nextRight;
    };

public:
    TriggerOptions options;
    Connection::Frame &frame;
    int frameSize;
    int sizeHalf;

    double fraction;
    bool isOnLeft;

    DataIndexes indexes;
    double firstValue;
    double secondValue;
    EdgeType edge;

public:
    FrameShiftTrigger(TriggerOptions &options, Connection::Frame &frame);

    double calculateShift();

private:
    DataIndexes getDataIndexes(int radius);

    double getDoubleShift(int i);
    double getFractionShift();

    bool isFrameSizeOdd();

    bool canCheckOnLeft();
    bool canCheckOnRight();

    void calculateValuesOnLeft();
    void calculateValuesOnRight();

    bool checkValuesOnLeft();
    bool checkValuesOnRight();

    bool checkValues(double firstValue, double secondValue);

    bool isCorrectEdge();
    EdgeType getEdgeType();

    static bool belongsTo(double value, double a, double b);
};


