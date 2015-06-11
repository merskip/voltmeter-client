#include <iostream>
#include "NetworkConnection.hpp"

void NetworkConnection::createConnection() {
    socket = new QTcpSocket();
    device = socket;

    QObject::connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                     this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));

    QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
                     this, SLOT(socketError(QAbstractSocket::SocketError)));

    socket->connectToHost(serverHost, serverPort);
}

void NetworkConnection::closeConnection() {
    if (socket->isOpen())
        socket->close();
}

QString NetworkConnection::toStringAddress() {
    QString addr;
    addr += serverHost;
    addr += ":";
    addr += QString::number(serverPort);
    return addr;
}

Measurement NetworkConnection::downloadOne() {
    socket->write("get_one");
    socket->waitForReadyRead(1000);
    QByteArray message = this->readOneLine();

    QList<QByteArray> dataList = message.split(' ');
    if (dataList.size() < 5) {
        QString error = QString("unexception message: %1").arg(QString(message));
        emit errorOccurred(error);
        return {};
    }

    double time = dataList[0].toDouble() / 1000;
    int value1 = dataList[1].toInt();
    int value2 = dataList[2].toInt();
    int value3 = dataList[3].toInt();
    int value4 = dataList[4].toInt();

    Measurement data = {
            time,
            {{0, 0},
             {value1, toVoltage(value1)},
             {value2, toVoltage(value2)},
             {value3, toVoltage(value3)},
             {value4, toVoltage(value4)}
            }
    };
    emit downloadedOne(data);
    return data;
}

Connection::Frame NetworkConnection::downloadFrame(int duration) {
    QByteArray request;
    request.append("frame");
    request.append(" ");
    request.append(QString::number(duration));
    request.append("\n");
    socket->write(request);

    socket->waitForReadyRead(1000);
    QByteArray message = this->readOneLine();
    int dataSize = message.toUInt();

    Connection::Frame frame;
    for (int i = 0; i < dataSize; i++) {
        QByteArray line = this->readOneLine();
        QList<QByteArray> lineSplit = line.split(' ');

        if (lineSplit.size() != 4) {
            QString error = QString("unexception message: %1").arg(QString(line));
            emit errorOccurred(error);
            return {};
        }

        int value1 = lineSplit[0].toInt();
        int value2 = lineSplit[1].toInt();
        int value3 = lineSplit[2].toInt();
        int value4 = lineSplit[3].toInt();

        QVector<double> data(5);
        data[0] = 0;
        data[1] = toVoltage(value1);
        data[2] = toVoltage(value2);
        data[3] = toVoltage(value3);
        data[4] = toVoltage(value4);
        frame.append(data);
    }

    emit downloadedFrame(frame);
    return frame;
}

void NetworkConnection::socketStateChanged(QAbstractSocket::SocketState socketState) {
    switch (socketState) {
        case QAbstractSocket::ConnectedState:
            setConnectionState(Connected);
            break;
        case QAbstractSocket::UnconnectedState:
            setConnectionState(Disconnected);
            break;
        case QAbstractSocket::ConnectingState:
            setConnectionState(Connecting);
            break;
        default: break; // Ignorowanie reszty stanów
    }
}

void NetworkConnection::socketError(QAbstractSocket::SocketError socketError) {
    closeConnection();
    QString error = socketErrorToString(socketError);
    emit errorOccurred(error);
}

QString NetworkConnection::socketErrorToString(QAbstractSocket::SocketError error) {
    switch (error) {
        case QAbstractSocket::ConnectionRefusedError:
            return "Połączenie zostało odrzucone";
        case QAbstractSocket::RemoteHostClosedError:
            return "Zdalny host zakończył połączenie";
        case QAbstractSocket::HostNotFoundError:
            return "Adres nie został znaleziony";
        case QAbstractSocket::SocketAccessError:
            return "Aplikacja nie posiada odpowiednich uprawnień";
        case QAbstractSocket::SocketResourceError:
            return "Zbyt mało zasobów do utworzenia gniazda";
        case QAbstractSocket::SocketTimeoutError:
            return "Minął limit czasu operacji";
        case QAbstractSocket::DatagramTooLargeError:
            return "Datagram był większy niż limit systemu operacyjnego";
        case QAbstractSocket::NetworkError:
            return "Wystąpił problem z siecią";
        case QAbstractSocket::AddressInUseError:
            return "Adres jest już w użyciu";
        case QAbstractSocket::SocketAddressNotAvailableError:
            return "Adres nie jest dostępny";
        case QAbstractSocket::UnsupportedSocketOperationError:
            return "Żądana operacja nie jest wspierana";
        case QAbstractSocket::UnfinishedSocketOperationError:
            return "Ostatnia operacja jeszcze się nie zakończyła";
        case QAbstractSocket::ProxyAuthenticationRequiredError:
            return "Wymagana autoryzacja przez serwer proxy";
        case QAbstractSocket::SslHandshakeFailedError:
            return "Nie udał się SSL/TLS handshake";
        case QAbstractSocket::ProxyConnectionRefusedError:
            return "Nie można połączyć się z serwerem proxy";
        case QAbstractSocket::ProxyConnectionClosedError:
            return "Połączenie z serwerem proxy zostało nieoczekiwanie zamknięte";
        case QAbstractSocket::ProxyConnectionTimeoutError:
            return "Połączenie z serwerem proxy przekroczyło limit czasu";
        case QAbstractSocket::ProxyNotFoundError:
            return "Adres proxy nie został odnaleziony";
        case QAbstractSocket::ProxyProtocolError:
            return "Błąd protokołu proxy";
        case QAbstractSocket::OperationError:
            return "Próbowano wykonać niedozwoloną operacją";
        case QAbstractSocket::SslInternalError:
            return "Wewnętrzny błąd biblioteki SSL";
        case QAbstractSocket::SslInvalidUserDataError:
            return "Nieprawidłowe dane dostarczone do biblioteki SSL";
        case QAbstractSocket::TemporaryError:
            return "Chwilowy błąd";
        case QAbstractSocket::UnknownSocketError:
        default:
            return "Wystąpił niezdefiniowany błąd";
    }
}
