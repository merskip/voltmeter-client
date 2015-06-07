#pragma once

#include <QIODevice>
#include "Connection.hpp"

class IODeviceConnection : public Connection {
    Q_OBJECT

protected:
    QIODevice *device;

    IODeviceConnection(QIODevice *device)
            : device(device) { }

    QByteArray readOneLine();

public:
    virtual void disconnect();

    QIODevice *getDevice() const {
        return device;
    }

};