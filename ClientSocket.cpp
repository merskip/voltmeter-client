#include <iostream>
#include <QtCore/qthread.h>
#include "ClientSocket.hpp"

void ClientSocket::connectToServer(QString &host, quint16 &port) {
    this->connectToHost(host, port);
}

void ClientSocket::disconnect() {
    this->close();
}

Measurement ClientSocket::downloadMeasurement() {
    this->write("get_one");
    this->waitForReadyRead(1000);
    QByteArray message = this->readLine();

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
    emit measurementDownloaded(data);
    return data;
}


QList<QVector<double>> ClientSocket::downloadFrame(int duration) {
    QByteArray request;
    request.append("frame");
    request.append(" ");
    request.append(QString::number(duration));
    request.append("\n");
    this->write(request);

    this->waitForReadyRead(1000);
    QByteArray message = this->readLine();

    int dataSize = message.toUInt();
//    std::cout << "Liczba pomiarów: " << dataSize << std::endl;

    QList<QVector<double>> data;
    for (int i = 0; i < dataSize; i++) {
        QByteArray line = this->readLine();
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

    emit frameDownloaded(duration, data);
    return data;
}


QByteArray ClientSocket::readLine() {
    QByteArray line, fragment;

    while (!line.contains('\n')) {
        if (!bytesAvailable()) {
            if (!waitForReadyRead())
                break;
        }

        fragment = QIODevice::readLine();
        line.append(fragment);
    }

    return line.trimmed();
}

double ClientSocket::toVoltage(int value) {
    return (double) value / 255 * 5.1;
}
