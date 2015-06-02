#include <iostream>
#include "ClientSocket.hpp"


Measurement ClientSocket::downloadMeasurement() {
    this->write("get_one");
    this->waitForReadyRead(1000);
    QByteArray raw_message = this->readAll();
    QString message = QString(raw_message).trimmed();

    QStringList dataList = message.split(' ');
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

double ClientSocket::toVoltage(int value) {
    return (double) value / 255 * 5.1;
}
