#pragma once

#include <QWidget>
#include <QTimeEdit>
#include "ChannelPanel.hpp"

class SidePanel : public QWidget {
    Q_OBJECT

private:
    ChannelPanel *channelPanel[5];
    QTimeEdit *timeRangeEdit;
    QTimeEdit *timeIntervalEdit;

public:
    SidePanel();

    ChannelPanel *getChannelPanel(int channel);
    QTimeEdit *getTimeRangeEdit();
    QTimeEdit *getTimeIntervalEdit();

    int getTimeRangeMillis();
    int getTimeIntervalMillis();
};