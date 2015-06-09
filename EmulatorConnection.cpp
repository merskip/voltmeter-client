#include <iostream>
#include <QThread>
#include <QDateTime>
#include <QElapsedTimer>
#include <QtMath>
#include "EmulatorConnection.hpp"

void EmulatorConnection::createConnection() {
    emit stateChanged(Connecting);
    emit stateChanged(Connected);
    emit connected();
    this->state = Connected;
}

void EmulatorConnection::closeConnection() {
    emit stateChanged(Disconnected);
    this->state = Disconnected;
}

QString EmulatorConnection::toStringAddress() {
    return "emulator";
}

Measurement EmulatorConnection::downloadOne() {
    Measurement data = calculate();
    emit downloadedOne(data);
    return data;
}

Connection::Frame EmulatorConnection::downloadFrame(int duration) {
    QElapsedTimer timer = QElapsedTimer();
    timer.start();

    Connection::Frame frame;
    while (!timer.hasExpired(duration)) {
        Measurement values = calculate();

        QVector<double> data(5);
        data[0] = 0;
        data[1] = values.channel[1].voltage;
        data[2] = values.channel[2].voltage;
        data[3] = values.channel[3].voltage;
        data[4] = values.channel[4].voltage;
        frame.append(data);

        QThread::usleep(500);
    }

    emit downloadedFrame(frame);
    return frame;
}

Measurement EmulatorConnection::calculate() {
    qint64 millis = QDateTime::currentMSecsSinceEpoch();
    double time = millis / 1000.0;
    int value1 = millis % 50 > 25 ? 255 : 0;
    int value2 = (int) ((qSin(time) + 1.0) / 2 * 255);
    int value3 = (int) (millis * 3 % 5000 / 5000.0 * 255.0 / 2.0);
    int value4 = qrand() % 255;

    if (millis * 3 % 10000 > 5000)
        value3 = (int) (255.0 / 2.0 - value3);

    Measurement data = {
            time,
            {{0, 0},
             {value1, toVoltage(value1)},
             {value2, toVoltage(value2)},
             {value3, toVoltage(value3)},
             {value4, toVoltage(value4)}
            }
    };
    return data;
}
