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

    return line.trimmed();
}
