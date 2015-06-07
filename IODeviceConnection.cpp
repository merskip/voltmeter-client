#include "IODeviceConnection.hpp"



void IODeviceConnection::disconnect() {
    device->close();
}

QByteArray IODeviceConnection::readOneLine() {
    QByteArray line, fragment;

    while (!line.contains('\n')) {
        if (!device->bytesAvailable()) {
            if (!device->waitForReadyRead(1000))
                break;
        }

        fragment = device->readLine();
        line.append(fragment);
    }

    return line.trimmed();
}
