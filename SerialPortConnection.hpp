#pragma once

#include "IODeviceConnection.hpp"
#include <QtSerialPort/QSerialPort>

class SerialPortConnection : public IODeviceConnection {
    Q_OBJECT

private:
    QString portName;
    qint32 baudRate;

protected:
    QSerialPort *serial;

public:
    SerialPortConnection() { }

    void setPortName(const QString &portName) {
        this->portName = portName;
    }

    void setBaudRate(qint32 baudRate) {
        this->baudRate = baudRate;
    }

    virtual QString toStringAddress();

public slots:
    virtual void createConnection();
    virtual void closeConnection();

    virtual Measurement downloadOne();
    virtual Frame downloadFrame(int duration);

private slots:
    void serialError(QSerialPort::SerialPortError serialError);

private:
    static QString serialErrorToString(QSerialPort::SerialPortError error);

};