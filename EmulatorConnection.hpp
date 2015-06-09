#pragma once

#include "Connection.hpp"

class EmulatorConnection : public Connection {
    Q_OBJECT


public:
    EmulatorConnection() { }

    virtual QString toStringAddress();

public slots:
    virtual void createConnection();
    virtual void closeConnection();

    

    virtual Measurement downloadOne();
    virtual Frame downloadFrame(int duration);

private:
    Measurement calculate();

};