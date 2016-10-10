#include "IODeviceConnection.hpp"

QByteArray IODeviceConnection::readOneLine() {
    QByteArray line, fragment;

    while (!line.contains('\n')) {
        if (!device->bytesAvailable()) {
            if (!device->isReadable() || !device->waitForReadyRead(1000))
                break;
        }

        fragment = device->readLine();
        line.append(fragment);
    }

    return line;
}

QByteArray IODeviceConnection::readByteArray(int size) {
    QByteArray line, fragment;

    while (line.size() != size) {
        if (!device->bytesAvailable()) {
            if (!device->isReadable() || !device->waitForReadyRead(1000))
                break;
        }

        fragment = device->read(size - line.size());
        line.append(fragment);
    }

    return line;
}