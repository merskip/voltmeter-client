#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QAbstractSocket>

class ConnectionPanel : public QWidget {
    Q_OBJECT

public:
    enum ConnectionState {
        Connected,
        Disconnected,
        Connecting
    };

private:
    QLineEdit *hostEdit;
    QSpinBox *portEdit;

    ConnectionState connectionState;
    QPushButton *connectBtn;

public:
    ConnectionPanel(QString host, quint16 port);

private slots:
    void handleConnectBtn();

public slots:
    void setConnectState(QAbstractSocket::SocketState state);
    void setConnectState(ConnectionState state);

signals:
    void doConnect(QString &host, quint16 &port);
    void doDisconnect();
};