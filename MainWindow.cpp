#include "MainWindow.hpp"

MainWindow::MainWindow(QString serverHost, quint16 serverPort) {
    clientSocket = new ClientSocket();
    plot = new GaugePlot();
    timer = new QTimer();

    thread = new QThread();
    clientSocket->moveToThread(thread);
    thread->start();
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTick()));

    connectionPanel = new ConnectionPanel(serverHost, serverPort);
    sidePanel = new SidePanel();

    plot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    plot->setMinimumSize(300, 150);
    connectionPanel->layout()->setContentsMargins(0, 0, 0, 0);
    sidePanel->layout()->setContentsMargins(0, 0, 0, 0);
    sidePanel->setFixedWidth(120);

    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->addWidget(plot);
    contentLayout->addWidget(sidePanel);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(connectionPanel);
    layout->addLayout(contentLayout);

    QWidget *centerWidget = new QWidget();
    centerWidget->setLayout(layout);
    setCentralWidget(centerWidget);

    channelPanel[0] = nullptr;
    for (int i = 1; i <= 4; i++) {
        channelPanel[i] = sidePanel->getChannelPanel(i);

        QColor color = plot->getChannelColor(i);
        channelPanel[i]->setChannelColor(color);
        channelPanel[i]->layout()->setContentsMargins(0, 0, 0, 0);

        connect(channelPanel[i], SIGNAL(channelVisibleChanged(int, bool)),
                plot, SLOT(setChannelVisible(int, bool)));
    }
    channelPanel[1]->setChannelVisible(true);
    channelPanel[2]->setChannelVisible(false);
    channelPanel[3]->setChannelVisible(false);
    channelPanel[4]->setChannelVisible(false);

    connect(sidePanel->getTimeRangeEdit(), SIGNAL(timeChanged(QTime)),
            this, SLOT(timeRangeChanged(QTime)));
    connect(sidePanel->getTimeIntervalEdit(), SIGNAL(timeChanged(QTime)),
            this, SLOT(timeIntervalChanged(QTime)));

    // Bez tego nie działa,
    // dalsze połączenia z clientSocket nie będą działać
    qRegisterMetaType<QString>("QString&");
    qRegisterMetaType<quint16>("quint16&");
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    qRegisterMetaType<Measurement>("Measurement&");
    qRegisterMetaType<QList<QVector<double>>>("QList<QVector<double> >&");

    connect(connectionPanel, SIGNAL(doConnect(QString&, quint16&)),
            clientSocket, SLOT(connectToServer(QString&, quint16&)));
    connect(connectionPanel, SIGNAL(doDisconnect()),
            clientSocket, SLOT(disconnect()));

    connect(this, SIGNAL(doDownloadMeasurement()),
            clientSocket, SLOT(downloadMeasurement()));
    connect(this, SIGNAL(doDownloadFrame(int)),
            clientSocket, SLOT(downloadFrame(int)));

    connect(clientSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
    connect(clientSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError(QAbstractSocket::SocketError)));

    connect(clientSocket, SIGNAL(measurementDownloaded(Measurement&)),
            plot, SLOT(appendMeasurement(Measurement&)));
    connect(clientSocket, SIGNAL(measurementDownloaded(Measurement&)),
            this, SLOT(voltageChanged(Measurement&)));

    connect(clientSocket, SIGNAL(frameDownloaded(int, QList<QVector<double>>&)),
            plot, SLOT(showFrame(int, QList<QVector<double>>&)));

    connect(plot, SIGNAL(isDone()), this, SLOT(plotIsDone()));

    connect(connectionPanel, SIGNAL(frameModeChanged(bool)),
            this, SLOT(frameModeChanged(bool)));

    connect(connectionPanel, SIGNAL(frameModeChanged(bool)),
            plot, SLOT(setFrameMode(bool)));

    timeRange = sidePanel->getTimeRangeMillis();
    timeInterval = sidePanel->getTimeIntervalMillis();

    setNullVoltage();
    statusBar()->show();
}

void MainWindow::timerTick() {
    if (isFrameMode) {
        emit doDownloadFrame(timeInterval);
    } else {
        emit doDownloadMeasurement();
    }
}

void MainWindow::plotIsDone() {
    if (isFrameMode)
        timer->start(0);
}

void MainWindow::socketStateChanged(QAbstractSocket::SocketState state) {
    connectionPanel->setConnectState(state);

    if (state == QAbstractSocket::ConnectedState) {
        QString serverAddress = toPrettyPeerAddress(clientSocket);
        statusBar()->showMessage("Połączono z " + serverAddress);

        plot->setTimeRange(timeRange / 1000);
        timer->start(timeInterval);
    } else if (state == QAbstractSocket::UnconnectedState) {
        timer->stop();
        plot->clearAllChannel();
        setNullVoltage();

        if (!isSocketError)
            statusBar()->showMessage("Rozłączono z serwerem");
    } else if (state == QAbstractSocket::ConnectingState) {
        statusBar()->showMessage("Łączenie z " + toPrettyPeerAddress(clientSocket) + "...");
        isSocketError = false;
    }
}

void MainWindow::socketError(QAbstractSocket::SocketError error) {
    QString errorString = socketErrorToString(error).toLower();
    statusBar()->showMessage("Wystąpił błąd: " + errorString);
    isSocketError = true;
}

QString MainWindow::toPrettyPeerAddress(QAbstractSocket *socket) {
    QString host = socket->peerAddress().toString();
    quint16 port = socket->peerPort();
    return toPrettyAddress(host, port);
}

QString MainWindow::toPrettyAddress(QString host, quint16 port) {
    QString address;
    address.append(host);
    address.append(":");
    address.append(QString::number(port));
    return address;
}

void MainWindow::voltageChanged(Measurement &data) {
    channelPanel[1]->setVoltage(data.channel[1].voltage);
    channelPanel[2]->setVoltage(data.channel[2].voltage);
    channelPanel[3]->setVoltage(data.channel[3].voltage);
    channelPanel[4]->setVoltage(data.channel[4].voltage);
}

void MainWindow::setNullVoltage() {
    channelPanel[1]->setNullVoltage();
    channelPanel[2]->setNullVoltage();
    channelPanel[3]->setNullVoltage();
    channelPanel[4]->setNullVoltage();
}

void MainWindow::timeRangeChanged(QTime time) {
    int millis = time.msecsSinceStartOfDay();
    plot->setTimeRange((double) millis / 1000);
    timeRange = millis;
}

void MainWindow::timeIntervalChanged(QTime time) {
    int millis = time.msecsSinceStartOfDay();
    timer->setInterval(millis);
    timeInterval = millis;
}

void MainWindow::frameModeChanged(bool isFrameMode) {
    this->isFrameMode = isFrameMode;

    if (isFrameMode) {
        timer->setSingleShot(true);
    } else {
        timer->setSingleShot(false);
        timer->setInterval(timeInterval);
    }

    if (clientSocket->isConnected())
        timer->start();
}

QString MainWindow::socketErrorToString(QAbstractSocket::SocketError error) {
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
