#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include "Connection.hpp"

class ConnectionPanel : public QWidget {
    Q_OBJECT

private:
    QLineEdit *hostEdit;
    QSpinBox *portEdit;

    Connection::State connectionState;
    QPushButton *connectBtn;

    QCheckBox *frameModeCheck;

public:
    ConnectionPanel(QString host, quint16 port);

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