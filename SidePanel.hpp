#pragma once

#include <QWidget>
#include <QTimeEdit>
#include <QCheckBox>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QDialog>
#include "ChannelPanel.hpp"
#include "ShowMode.hpp"
#include "TriggerOptionsDialog.hpp"

class SidePanel : public QWidget {
    Q_OBJECT

private:
    ChannelPanel *channelPanel[5];

    QStackedWidget *stackedPanel;
    QVBoxLayout *realTimeLayout;
    QVBoxLayout *frameLayout;
    QWidget *realTimePage;
    QWidget *framePage;

    QCheckBox *frameModeCheck;

    QTimeEdit *timeRangeEdit;
    QTimeEdit *timeIntervalEdit;
    QTimeEdit *timeFrameEdit;

    TriggerOptionsDialog *triggerOptionsDialog;
    QPushButton *triggerOptionsBtn;
    QPushButton *continueOneShotBtn;

public:
    SidePanel();

    ChannelPanel *getChannelPanel(int channel);

    void setEnableContinueOneShot(bool state);

    ShowMode getShowMode();
    void setShowMode(ShowMode mode);

    int getTimeRange() {
        return timeRangeEdit->time().msecsSinceStartOfDay();
    }

    int getTimeInterval() {
        return timeIntervalEdit->time().msecsSinceStartOfDay();
    }

    int getTimeFrame() {
        return timeFrameEdit->time().msecsSinceStartOfDay();
    }

private:
    void setupRealTimeMode();
    void setupFrameMode();

private slots:
    void handleFrameModeStateChanged(int state);

    void handleTriggerSettings();

    void handleTimeRangeEdit(QTime time);
    void handleTimeIntervalEdit(QTime time);
    void handleTimeFrameEdit(QTime time);

    void handleTriggerOptionsChanged(TriggerOptions options);
    void handleContinueOneShowClick();

signals:
    void showModeChanged(ShowMode mode);

    void timeRangeChanged(int timeRange);
    void timeIntervalChanged(int timeInterval);
    void timeFrameChanged(int timeFrame);

    void triggerOptionsChanged(TriggerOptions options);
    void continueOneShot();
};