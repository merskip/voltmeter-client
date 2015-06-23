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

    double timeRange;
    TriggerOptions triggerOptions;
    QCPItemLine *triggerVLine;
    QCPItemLine *triggerHLine;

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
    void setTriggerLinesVisible(bool visible);

public slots:
    void setShowMode(ShowMode mode);
    void setTriggerOptions(TriggerOptions options);
    void setChannelVisible(int channel, bool on);

    void appendMeasurement(Measurement &data);
    void showFrame(Connection::Frame &frame);

private:
    QCPGraph *createNewChannel(QColor color);
    void setupAxis();
    void createTriggerLines();
    QCPItemLine *createNewTriggerLine();

    void setupRealTimeMode();
    void setupFrameMode();

    void updateGraphChannel(int channel, double time, double voltage);

    void moveGraphForTrigger(Connection::Frame &data);
    int getShiftForTrigger(Connection::Frame &data);
    void moveGraph(int shift, int margin = 0);

    void setupTriggerLinesPosition();
    void setupTriggerVerticalLinePosition();
    void setupTriggerHorizontalLinePosition();

signals:
    void isDone();
};