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
    bool isSocketError = false;

    Connection *connection;
    GaugePlot *plot;
    QTimer *timer;
    QThread *thread;

    ConnectionPanel *connectionPanel;
    ChannelPanel *channelPanel[5];
    SidePanel *sidePanel;

    ShowMode showMode = RealTimeMode;
    int timeFrame;

public:
    MainWindow();

    bool isRealTimeMode();
    bool isFrameMode();


    bool isConnected();
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

    void showModeChanged(ShowMode mode);

    void timeRangeChanged(int timeRange);
    void timeIntervalChanged(int timeInterval);
    void timeFrameChanged(int timeFrame);

    void voltageChanged(Measurement &data);

private:
    void startRealTimeMode();
    void startFrameMode();

    void setNullVoltage();
};
