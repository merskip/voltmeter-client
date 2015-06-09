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
    bool isBusy = false;
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
    MainWindow();

    Connection *getConnection();

public slots:
    void setConnection(Connection *connection);

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
