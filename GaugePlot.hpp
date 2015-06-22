#pragma once

#include <iostream>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include "lib/qcustomplot.h"
#include "Connection.hpp"
#include "ShowMode.hpp"
#include "TriggerOptions.hpp"

class GaugePlot : public QCustomPlot {
    Q_OBJECT

private:
    QCPGraph *graph[5];
    QCPGraph *dot[5];

    double timeRange;
    TriggerOptions triggerOptions;

public:
    GaugePlot();

    inline void setTimeRangeMillis(int time) {
        setTimeRange((double) time / 1000);
    }

    void setVoltageRange(double min, double max);
    void setTimeRange(double time);

    inline double getTimeRange() {
        return timeRange;
    }

    QColor getChannelColor(int channel);

    void clearAllChannel();

public slots:
    void setShowMode(ShowMode mode);
    void setTriggerOptions(TriggerOptions options);
    void setChannelVisible(int channel, bool on);
    void appendMeasurement(Measurement &data);

    void showFrame(Connection::Frame &frame);

private:
    void setupRealTimeMode();
    void setupFrameMode();

    void setupGraphChannel(int channel, QColor color);
    void updateGraphChannel(int channel, double time, double voltage);

    void moveGraphForTrigger(Connection::Frame &data);
    int getShiftForTrigger(Connection::Frame &data);
    void moveGraph(int shift, int margin = 0);


signals:
    void isDone();
};