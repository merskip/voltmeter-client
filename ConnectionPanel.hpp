#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include "Connection.hpp"

#define DEFAULT_NETWORK_HOST "192.168.1.8"
#define DEFAULT_NETWORK_PORT 22444

class ConnectionPanel : public QWidget {
    Q_OBJECT

public:
    enum Type {
        Network,
        SerialPort,
        Emulator
    };

private:
    QComboBox *typeSelect;

    struct {
        QLabel *hostLabel;
        QLineEdit *hostEdit;
        QLabel *portLabel;
        QSpinBox *portEdit;
    } network;

    QHBoxLayout *mainLayout;
    QHBoxLayout *contentLayout;

    Connection::State connectionState;
    QPushButton *connectBtn;

    QCheckBox *frameModeCheck;

public:
    ConnectionPanel();

    bool isFrameMode();

private:
    void setupParamsLayout(Type type);
    void setupNetworkLayout(QHBoxLayout *layout);

    void clearContentLayout();

    void sendDoConnect();

private slots:
    void handleTypeChanged(int index);

    void handleConnectBtn();
    void handleFrameModeStateChanged(int state);

public slots:
    void setConnectionState(Connection::State state);

signals:
    void connectionChanged(Connection *connection);

    void doConnect(Connection::Params params);
    void doDisconnect();

    void frameModeChanged(bool isFrameMode);
};

Q_DECLARE_METATYPE(ConnectionPanel::Type);