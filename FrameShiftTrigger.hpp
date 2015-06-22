#pragma once

#include "TriggerOptions.hpp"
#include "Connection.hpp"

class FrameShiftTrigger {
    struct DataIndexes {
        int left, nextLeft;
        int right, nextRight;
    };

private:
    TriggerOptions options;
    Connection::Frame &frame;
    int frameSize;
    int sizeHalf;

    DataIndexes indexes;

public:
    FrameShiftTrigger(TriggerOptions &options, Connection::Frame &frame);

    int calculateShift();

private:
    DataIndexes getDataIndexes(int radius);

    bool canCheckOnLeft();
    bool canCheckOnRight();

    bool checkValuesOnLeft();
    bool checkValuesOnRight();

    bool checkValues(double firstValue, double secondValue);

    bool isCorrectEdge(double firstValue, double secondValue);

    static bool belongsTo(double value, double a, double b);
};


