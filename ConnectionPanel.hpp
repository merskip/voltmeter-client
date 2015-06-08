#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include "Connection.hpp"

#define DEFAULT_NETWORK_HOST "192.168.1.8"
#define DEFAULT_NETWORK_PORT 22444

class ConnectionPanel : public QWidget {
    Q_OBJECT

private:
    QLineEdit *hostEdit;
    QSpinBox *portEdit;

    Connection::State connectionState;
    QPushButton *connectBtn;

    QCheckBox *frameModeCheck;

public:
    ConnectionPanel();

    bool isFrameMode();

private slots:
    void handleConnectBtn();
    void handleFrameModeStateChanged(int state);

public slots:
    void setConnectionState(Connection::State state);

signals:
    void doConnect(Connection::Params params);
    void doDisconnect();

    void frameModeChanged(bool isFrameMode);
};