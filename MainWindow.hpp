#pragma once

#include <QMainWindow>
#include <QTimer>
#include "GaugePlot.hpp"
#include "ClientSocket.hpp"
#include "ChannelPanel.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    ClientSocket *client;
    GaugePlot *plot;
    QTimer *timer;

    ChannelPanel *channelPanel[5];
    QTimeEdit *timeRangeEdit;

public:
    MainWindow();

private slots:
    void timerTick();
    void timeRangeChanged(QTime time);

    void voltageChanged(Measurement &data);
};