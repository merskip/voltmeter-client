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

    void setServerHost(const QString &serverHost) {
        this->serverHost = serverHost;
    }

    void setServerPort(quint16 serverPort) {
        this->serverPort = serverPort;
    }

    virtual QString toStringAddress();

public slots:
    virtual void createConnection();

    virtual Measurement downloadOne();
    virtual Frame downloadFrame(int duration);

private slots:
    void socketStateChanged(QAbstractSocket::SocketState socketState);
    void socketError(QAbstractSocket::SocketError socketError);

private:
    static QString socketErrorToString(QAbstractSocket::SocketError error);


};