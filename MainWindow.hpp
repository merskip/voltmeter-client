#pragma once

#include <QMainWindow>
#include <QTimer>
#include "GaugePlot.hpp"
#include "ClientSocket.hpp"
#include "ConnectionPanel.hpp"
#include "SidePanel.hpp"


class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    bool isSocketError = false;
    ClientSocket *clientSocket;
    GaugePlot *plot;
    QTimer *timer;

    ConnectionPanel *connectionPanel;
    ChannelPanel *channelPanel[5];
    SidePanel *sidePanel;

public:
    MainWindow(QString serverHost, quint16 serverPort);

    QString getServerConnectedAddress();

private:
    void setNullVoltage();

    static QString socketErrorToString(QAbstractSocket::SocketError error);

private slots:
    void doConnect(QString &host, quint16 &port);
    void doDisconnect();

    void socketStateChanged(QAbstractSocket::SocketState state);
    void socketError(QAbstractSocket::SocketError error);

    void timerTick();

    void timeRangeChanged(QTime time);
    void timeIntervalChanged(QTime time);

    void voltageChanged(Measurement &data);
};