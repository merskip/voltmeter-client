#include <iostream>
#include "SerialPortConnection.hpp"

void SerialPortConnection::createConnection() {
    serial = new QSerialPort();
    device = serial;

    serial->setPortName(portName);
    serial->setBaudRate(baudRate);

    QObject::connect(serial, SIGNAL(error(QSerialPort::SerialPortError)),
                     this, SLOT(serialError(QSerialPort::SerialPortError)));

    if (serial->open(QIODevice::ReadWrite)) {
        emit stateChanged(Connected);
        emit connected();
    } else {
        emit stateChanged(Disconnected);
    }
}


void SerialPortConnection::closeConnection() {
    IODeviceConnection::closeConnection();
    emit stateChanged(Disconnected);
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
    serial->write("get_one");
    serial->waitForReadyRead(1000);
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

    Connection::Frame data;
    while (true) {
        QByteArray line = readOneLine();

        if (line.startsWith(':')) { // Koniec pomiarów
            // Liczba miarów jest ignorowana, powinna się zgadzać
            break;
        }

        QList<QByteArray> lineSplit = line.split(' ');
        if (lineSplit.size() != 4) {
            std::cerr << "Unknown message: " << line.toStdString() << std::endl;
            break;
        }

        int value1 = lineSplit.at(0).toInt();
        int value2 = lineSplit.at(1).toInt();
        int value3 = lineSplit.at(2).toInt();
        int value4 = lineSplit.at(3).toInt();

        QVector<double> lineData(5);
        lineData[0] = 0;
        lineData[1] = toVoltage(value1);
        lineData[2] = toVoltage(value2);
        lineData[3] = toVoltage(value3);
        lineData[4] = toVoltage(value4);
        data.append(lineData);
    }

    emit downloadedFrame(data);
    return data;
}


void SerialPortConnection::serialError(QSerialPort::SerialPortError serialError) {
    if (serialError != QSerialPort::NoError) {
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
