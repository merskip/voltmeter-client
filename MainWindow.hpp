#pragma once

#include <QMainWindow>
#include <QTimer>
#include "GaugePlot.hpp"
#include "ClientSocket.hpp"
#include "ConnectionPanel.hpp"
#include "ChannelPanel.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    ClientSocket *clientSocket;
    GaugePlot *plot;
    QTimer *timer;

    ConnectionPanel *connectionPanel;
    ChannelPanel *channelPanel[5];
    QTimeEdit *timeRangeEdit;

public:
    MainWindow(QString serverHost, quint16 serverPort);

private:
    void setNullVoltage();

private slots:
    void doConnect(QString &host, quint16 &port);
    void doDisconnect();
    void socketStateChanged(QAbstractSocket::SocketState state);

    void timerTick();
    void timeRangeChanged(QTime time);

    void voltageChanged(Measurement &data);
};