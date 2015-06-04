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

public:
    GaugePlot();

    void setVoltageRange(double min, double max);
    void setTimeRange(double time);

    inline double getTimeRange() {
        return timeRange;
    }

    void setFrameMode(bool state);

    QColor getChannelColor(int channel);

    void clearAllChannel();

public slots:
    void setChannelVisible(int channel, bool on);
    void appendMeasurement(Measurement &data);

    void showFrame(int duration, QList<QVector<double>> &data);

private:
    void setupGraphChannel(int channel, QColor color);
    void updateGraphChannel(int channel, double time, double voltage);
};