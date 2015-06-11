#pragma once

#include <iostream>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include "lib/qcustomplot.h"
#include "Connection.hpp"

class GaugePlot : public QCustomPlot {
    Q_OBJECT

private:
    QCPGraph *graph[5];
    QCPGraph *dot[5];

    double timeRange;
    double triggerVoltage;

public:
    GaugePlot();

    void setVoltageRange(double min, double max);
    void setTimeRange(double time);

    inline double getTimeRange() {
        return timeRange;
    }

    void setTriggerVoltage(double voltage);

    QColor getChannelColor(int channel);

    void clearAllChannel();

public slots:
    void setFrameMode(bool state);
    void setChannelVisible(int channel, bool on);
    void appendMeasurement(Measurement &data);

    void showFrame(Connection::Frame &frame);

private:
    void setupGraphChannel(int channel, QColor color);
    void updateGraphChannel(int channel, double time, double voltage);

    static int getShiftForTrigger(int channel, double voltage, QList<QVector<double>> &data);
    static bool belongsTo(double value, double a, double b);

signals:
    void isDone();
};