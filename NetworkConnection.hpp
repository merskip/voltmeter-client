#pragma once

#include "IODeviceConnection.hpp"
#include <QTcpSocket>
#include <QHostAddress>

class NetworkConnection : public IODeviceConnection {
    Q_OBJECT

private:
    QString serverHost;
    quint16 serverPort;

protected:
    QTcpSocket *socket;

public:
    NetworkConnection() { }

    virtual void connect(Params params);

    virtual QString toStringAddress();

private slots:
    void socketStateChanged(QAbstractSocket::SocketState socketState);
    void socketError(QAbstractSocket::SocketError socketError);

private:
    static QString socketErrorToString(QAbstractSocket::SocketError error);

public:
    virtual Measurement downloadOne();

    virtual Frame downloadFrame(int duration);
};