#pragma once

#include <iostream>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include "lib/qcustomplot.h"
#include "Measurement.hpp"

class GaugePlot : public QCustomPlot {
    Q_OBJECT

private:
    QCPGraph *graph[5];
    QCPGraph *dot[5];

    bool isFrameMode;
    double timeRange;
    double triggerVoltage;

public:
    GaugePlot();

    void setVoltageRange(double min, double max);
    void setTimeRange(double time);

    inline double getTimeRange() {
        return timeRange;
    }

    void setFrameMode(bool state);
    void setTriggerVoltage(double voltage);

    QColor getChannelColor(int channel);

    void clearAllChannel();

public slots:
    void setChannelVisible(int channel, bool on);
    void appendMeasurement(Measurement &data);

    void showFrame(int duration, QList<QVector<double>> &data);

private:
    void setupGraphChannel(int channel, QColor color);
    void updateGraphChannel(int channel, double time, double voltage);

    static int getShiftForTrigger(int channel, double voltage, QList<QVector<double>> &data);
    static bool belongsTo(double value, double a, double b);
};