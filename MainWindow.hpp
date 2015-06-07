#pragma once

#include <QMainWindow>
#include <QTimer>
#include "Connection.hpp"
#include "GaugePlot.hpp"
#include "ConnectionPanel.hpp"
#include "SidePanel.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    bool isFrameMode = false;
    bool isSocketError = false;

    Connection *connection;
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

signals:
    void doDownloadOne();
    void doDownloadFrame(int duration);

private slots:
    void timerTick();

    void plotIsDone();

    void doStart();

    void connectionStateChanged(Connection::State state);
    void connectionErrorOccurred(QString error);

    void timeRangeChanged(QTime time);
    void timeIntervalChanged(QTime time);

    void frameModeChanged(bool isFrameMode);

    void voltageChanged(Measurement &data);

private:
    void setNullVoltage();
};
