#pragma once

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include "Measurement.hpp"

class ClientSocket : public QTcpSocket {
    Q_OBJECT

public:
    ClientSocket() : QTcpSocket() { }

    Measurement downloadMeasurement();

signals:
    void measurementDownloaded(Measurement &data);

private:
    double toVoltage(int value);
};
