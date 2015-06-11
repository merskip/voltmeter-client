#pragma once

#include <QObject>
#include <QVector>
#include <QByteArray>

struct Measurement {
    double time;
    struct {
        int value;
        double voltage;
    } channel[5];
};

class Connection : public QObject {
    Q_OBJECT

public:
    typedef QList<QVector<double>> Frame;

    enum State {
        Connected,
        Disconnected,
        Connecting
    };

protected:
    int valueMax = 255;
    double voltageMax = 5.1;

private:
    State state = Disconnected;

public:
    virtual bool isConnected() {
        return getConnectionState() == Connected;
    }

    inline State getConnectionState() {
        return state;
    }

    inline void setConnectionState(State state) {
        this->state = state;
        emit stateChanged(this->state);
        if (state == Connected)
            emit connected();
    }

    virtual QString toStringAddress() = 0;

public slots:
    virtual void createConnection() = 0;
    virtual void closeConnection() = 0;

    virtual Measurement downloadOne() = 0;
    virtual Connection::Frame downloadFrame(int duration) = 0;

protected:
    double toVoltage(int value);

signals:
    void stateChanged(Connection::State status);
    void errorOccurred(QString message);

    void connected();

    void downloadedOne(Measurement &data);
    void downloadedFrame(Connection::Frame &frame);
};
