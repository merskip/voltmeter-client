#pragma once

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include "Measurement.hpp"

class ClientSocket : public QTcpSocket {
    Q_OBJECT

public:
    ClientSocket() : QTcpSocket() { }

    inline bool isConnected() {
        return state() == ConnectedState;
    }

public slots:
    void connectToServer(QString &host, quint16 &port);
    void disconnect();

    Measurement downloadMeasurement();

    QList<QVector<double>> downloadFrame(int duration);

signals:
    void measurementDownloaded(Measurement &data);
    void frameDownloaded(int duration, QList<QVector<double>> &data);

private:
    QByteArray readLine();
    double toVoltage(int value);
};
