#pragma once

#include <iostream>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include "lib/qcustomplot.h"
#include "Connection.hpp"
#include "ShowMode.hpp"
#include "TriggerOptions.hpp"
#include "ShiftFrameTrigger.hpp"

class GaugePlot : public QCustomPlot {
    Q_OBJECT

private:
    QCPGraph *graph[5];

    ShowMode showMode;
    double timeRange;
    double timeFrame;

    TriggerOptions triggerOptions;
    QCPGraph *triggersPoints;
    QCPItemLine *triggerVLine;
    QCPItemLine *triggerHLine;

public:
    GaugePlot();

    inline void setTimeRangeMillis(int time) {
        setTimeRange((double) time / 1000);
    }

    inline void setTimeFrameMillis(int time) {
        setTimeFrame((double) time / 1000);
    }

    void setVoltageRange(double min, double max);
    void setTimeRange(double time);
    void setTimeFrame(double time);

    QColor getChannelColor(int channel);

    void clearAllChannel();
    void setTriggerLinesVisible(bool visible);

public slots:
    void setShowMode(ShowMode mode);
    void setTriggerOptions(TriggerOptions options);
    void setChannelVisible(int channel, bool on);
    bool isChannelVisible(int channel);

    void appendMeasurement(Measurement &data);
    void showFrame(Connection::Frame &frame);

private:
    QCPGraph *createNewChannel(QColor color);
    void setupAxis();
    void createTriggersPointsAndLines();
    QCPItemLine *createNewTriggerLine();
    QCPGraph *createTriggersPoints();
    void setupTriggersPoints();

    void setupRealTimeMode();
    void setupFrameMode();

    void updateGraphChannel(int channel, double time, double voltage);

    void applyTrigger(Connection::Frame &data);
    void moveGraphForTrigger(ShiftFrameTrigger &frameTrigger);
    void moveGraph(double shift);

    void updateTriggersPoints(FrameTrigger &frameTrigger);
    void updateTriggerLinesPosition();
    void updateTriggerVerticalLinePosition();
    void updateTriggerHorizontalLinePosition();

signals:
    void isDone();
};