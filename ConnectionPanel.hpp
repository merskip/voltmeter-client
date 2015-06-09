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

#define DEFAULT_SERIAL_PORT_NAME "ttyACM0"
#define DEFAULT_SERIAL_BAUD_RATE 9600

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
    Type currentType = Network;

    struct {
        QLabel *hostLabel;
        QLineEdit *hostEdit;
        QLabel *portLabel;
        QSpinBox *portEdit;
    } network;

    struct {
        QLabel *portNameLabel;
        QComboBox *portNameSelect;
        QLabel *baudRateLabel;
        QComboBox *baudRateSelect;
    } serial;

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
    void setupSerialLayout(QHBoxLayout *layout);

    void clearContentLayout();

    void sendDoConnect();
    void sendDoConnectNetwork();
    void sendDoConnectSerialPort();

private slots:
    void handleTypeChanged(int index);

    void handleConnectBtn();
    void handleFrameModeStateChanged(int state);

public slots:
    void setConnectionState(Connection::State state);

signals:
    void connectionChanged(Connection *connection);

    void doConnect();
    void doDisconnect();

    void frameModeChanged(bool isFrameMode);
};

Q_DECLARE_METATYPE(ConnectionPanel::Type);