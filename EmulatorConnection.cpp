#include <iostream>
#include <QThread>
#include <QDateTime>
#include <QElapsedTimer>
#include <QtMath>
#include "EmulatorConnection.hpp"

void EmulatorConnection::createConnection() {
    setConnectionState(Connecting);
    setConnectionState(Connected);
}

void EmulatorConnection::closeConnection() {
    setConnectionState(Disconnected);
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

        QThread::usleep(200);
    }

    emit downloadedFrame(frame);
    return frame;
}

Measurement EmulatorConnection::calculate() {
    qint64 nsecs = timer.nsecsElapsed();
    quint64 millis = (quint64) (nsecs * 1.0e-6);

    int value1 = (int) ((qSin(nsecs * 1.0e-9 * 300) + 1.0) / 2.0 * 255.0);
    int value2 = millis % 80 >= 40 ? 255 : 0;
    int value3 = (int) (millis * 3 % 5000 / 5000.0 * 255.0 / 2.0);
    int value4 = qrand() % 255;

    if (millis * 3 % 10000 > 5000)
        value3 = (int) (255.0 / 2.0 - value3);

    Measurement data = {
            nsecs * 1.0e-9,
            {{0, 0},
             {value1, toVoltage(value1)},
             {value2, toVoltage(value2)},
             {value3, toVoltage(value3)},
             {value4, toVoltage(value4)}
            }
    };
    return data;
}
