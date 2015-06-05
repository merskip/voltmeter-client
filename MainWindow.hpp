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
    bool isFrameMode = false;
    bool isSocketError = false;

    ClientSocket *clientSocket;
    GaugePlot *plot;
    QTimer *timer;
    QThread *thread;

    ConnectionPanel *connectionPanel;
    ChannelPanel *channelPanel[5];
    SidePanel *sidePanel;

    int timeRange;
    int timeInterval;

public:
    MainWindow(QString serverHost, quint16 serverPort);

    static QString toPrettyPeerAddress(QAbstractSocket *socket);
    static QString toPrettyAddress(QString host, quint16 port);

private:
    void setNullVoltage();

    static QString socketErrorToString(QAbstractSocket::SocketError error);

private slots:
    void timerTick();

    void plotIsDone();

    void socketStateChanged(QAbstractSocket::SocketState state);
    void socketError(QAbstractSocket::SocketError error);

    void timeRangeChanged(QTime time);
    void timeIntervalChanged(QTime time);

    void frameModeChanged(bool isFrameMode);

    void voltageChanged(Measurement &data);

signals:
    void doDownloadMeasurement();
    void doDownloadFrame(int duration);
};
