#pragma once

#include <QWidget>
#include <QTimeEdit>
#include <QCheckBox>
#include "ChannelPanel.hpp"

class SidePanel : public QWidget {
    Q_OBJECT

private:
    ChannelPanel *channelPanel[5];

    QCheckBox *frameModeCheck;
    QTimeEdit *timeRangeEdit;
    QTimeEdit *timeIntervalEdit;

public:
    SidePanel();

    ChannelPanel *getChannelPanel(int channel);
    QTimeEdit *getTimeRangeEdit();
    QTimeEdit *getTimeIntervalEdit();

    int getTimeRangeMillis();
    int getTimeIntervalMillis();

private slots:
    void handleFrameModeStateChanged(int state);

signals:
    void frameModeChanged(bool isFrameMode);
};