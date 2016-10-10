#pragma once

#include <QIODevice>
#include "Connection.hpp"

class IODeviceConnection : public Connection {
    Q_OBJECT

protected:
    QIODevice *device;

    IODeviceConnection(QIODevice *device = nullptr)
            : device(device) { }

    QIODevice *getDevice() const {
        return device;
    }

    QByteArray readOneLine();
    QByteArray readByteArray(int size);

};