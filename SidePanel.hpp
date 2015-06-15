#pragma once

#include <QWidget>
#include <QTimeEdit>
#include <QCheckBox>
#include <QStackedWidget>
#include "ChannelPanel.hpp"

class SidePanel : public QWidget {
    Q_OBJECT

private:
    ChannelPanel *channelPanel[5];

    QStackedWidget *stackedPanel;
    QVBoxLayout *normalLayout;
    QVBoxLayout *frameLayout;
    QWidget *normalPage;
    QWidget *framePage;

    QCheckBox *frameModeCheck;

    QTimeEdit *timeRangeEdit;
    QTimeEdit *timeIntervalEdit;
    QTimeEdit *timeFrameEdit;

public:
    SidePanel();

    ChannelPanel *getChannelPanel(int channel);

    void setFrameMode(bool enabled);

    int getTimeRange() {
        return timeRangeEdit->time().msecsSinceStartOfDay();
    }

    int getTimeInterval() {
        return timeIntervalEdit->time().msecsSinceStartOfDay();
    }

    int getTimeFrame() {
        return timeFrameEdit->time().msecsSinceStartOfDay();
    }

private slots:
    void handleFrameModeStateChanged(int state);

    void handleTimeRangeEdit(QTime time);
    void handleTimeIntervalEdit(QTime time);
    void handleTimeFrameEdit(QTime time);

signals:
    void frameModeChanged(bool isFrameMode);

    void timeRangeChanged(int timeRange);
    void timeIntervalChanged(int timeInterval);
    void timeFrameChanged(int timeFrame);
};