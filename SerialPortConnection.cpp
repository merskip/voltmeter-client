#include <iostream>
#include <QThread>
#include <QDebug>
#include "SerialPortConnection.hpp"

void SerialPortConnection::createConnection() {
    setConnectionState(Connecting);

    serial = new QSerialPort(this);
    device = serial;

    serial->setPortName(portName);
    serial->setBaudRate(baudRate);

    QObject::connect(serial, SIGNAL(error(QSerialPort::SerialPortError)),
                     this, SLOT(serialError(QSerialPort::SerialPortError)));

    if (serial->open(QIODevice::ReadWrite))
        setConnectionState(Connected);
    else
        setConnectionState(Disconnected);
}

void SerialPortConnection::closeConnection() {
    if (serial->isOpen())
        serial->close();

    setConnectionState(Disconnected);
}

QString SerialPortConnection::toStringAddress() {
    QString addr;
    addr += portName;
    addr += " (";
    addr += QString::number(baudRate);
    addr += ")";
    return addr;
}

Measurement SerialPortConnection::downloadOne() {
    serial->write("get_one\n");
    serial->waitForBytesWritten(1000);
    QByteArray message = readOneLine();

    QList<QByteArray> dataList = message.split(' ');
    if (dataList.size() < 5) {
        std::cerr << "error: unexception message: " << message.toStdString() << std::endl;
        return {};
    }

    double time = dataList.at(0).toDouble() / 1000;
    int value1 = dataList.at(1).toInt();
    int value2 = dataList.at(2).toInt();
    int value3 = dataList.at(3).toInt();
    int value4 = dataList.at(4).toInt();

    Measurement data = {
            time,
            {{0, 0},
             {value1, toVoltage(value1)},
             {value2, toVoltage(value2)},
             {value3, toVoltage(value3)},
             {value4, toVoltage(value4)}
            }
    };
    emit downloadedOne(data);
    return data;
}

Connection::Frame SerialPortConnection::downloadFrame(int duration) {
    QByteArray request;
    request.append("frame");
    request.append(" ");
    request.append(QString::number(duration));
    request.append("\n");
    serial->write(request);
    serial->waitForBytesWritten(1000);

    Connection::Frame frame;
    while (true) {
        QByteArray line = readByteArray(5);

        char state = line.at(0);
        char value1 = line.at(1);
        char value2 = line.at(2);
        char value3 = line.at(3);
        char value4 = line.at(4);

        // Jeśli pierwszy bajt to 1 znaczy to, że dostępne są dane
        if (state != 1) {
            break;
        }

        QVector<double> data(5);
        data[0] = 0;
        data[1] = toVoltage(static_cast<unsigned char>(value1));
        data[2] = toVoltage(static_cast<unsigned char>(value2));
        data[3] = toVoltage(static_cast<unsigned char>(value3));
        data[4] = toVoltage(static_cast<unsigned char>(value4));
        frame.append(data);
    }

    emit downloadedFrame(frame);
    return frame;
}


void SerialPortConnection::serialError(QSerialPort::SerialPortError serialError) {
    if (serialError != QSerialPort::NoError) {
        closeConnection();
        QString error =  serialErrorToString(serialError);
        emit errorOccurred(error);
    }
}

QString SerialPortConnection::serialErrorToString(QSerialPort::SerialPortError error) {
    switch (error) {
        case QSerialPort::NoError:
            return "Brak błędu";
        case QSerialPort::DeviceNotFoundError:
            return "Urządzenie nie istnieje";
        case QSerialPort::PermissionError:
            return "Urządznie otwarte przez inny proces lub brak uprawnień do otwarcia";
        case QSerialPort::OpenError:
            return "Inne urządznie zostało już otwarte";
        case QSerialPort::ParityError:
            return "Wykryto błąd parzystości";
        case QSerialPort::FramingError:
            return "Wykryto błąd protokołu";
        case QSerialPort::BreakConditionError:
            return "Wykryto stan przerwania";
        case QSerialPort::WriteError:
            return "Błąd we/wy podczas zapisu";
        case QSerialPort::ReadError:
            return "Błąd we/wy podczas czytania";
        case QSerialPort::ResourceError:
            return "Urządznie nie jest dostępne";
        case QSerialPort::UnsupportedOperationError:
            return "Próbowano wykonać niedozwoloną operacją";
        case QSerialPort::TimeoutError:
            return "Minął limit czasu operacji";
        case QSerialPort::NotOpenError:
            return "Urządznie nie jest otwarte";
        case QSerialPort::UnknownError:
        default:
            return "Wystąpił niezdefiniowany błąd";
    }
}
