#include "MainWindow.hpp"

MainWindow::MainWindow(QString serverHost, quint16 serverPort) {
    clientSocket = new ClientSocket;
    plot = new GaugePlot();
    timer = new QTimer();
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

    connect(connectionPanel, SIGNAL(doConnect(QString&, quint16&)),
            this, SLOT(doConnect(QString&, quint16&)));
    connect(connectionPanel, SIGNAL(doDisconnect()),
            this, SLOT(doDisconnect()));

    connect(clientSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));

    connect(clientSocket, SIGNAL(measurementDownloaded(Measurement&)),
            plot, SLOT(appendMeasurement(Measurement&)));
    connect(clientSocket, SIGNAL(measurementDownloaded(Measurement&)),
            this, SLOT(voltageChanged(Measurement&)));

    setNullVoltage();
}


void MainWindow::doConnect(QString &host, quint16 &port) {
    clientSocket->connectToHost(host, port);
}

void MainWindow::doDisconnect() {
    timer->stop();
    clientSocket->close();
}

void MainWindow::socketStateChanged(QAbstractSocket::SocketState state) {
    connectionPanel->setConnectState(state);

    if (state == QAbstractSocket::ConnectedState) {
        std::cout << "Połączono, pobieranie danych..." << std::endl;

        int timeRange = sidePanel->getTimeRangeMillis();
        plot->setTimeRange(timeRange / 1000);

        int interval = sidePanel->getTimeIntervalMillis();
        timer->start(interval);
    } else if (state == QAbstractSocket::UnconnectedState) {
        std::cout << "Rozłączono: " << clientSocket->errorString().toStdString() << std::endl;
        timer->stop();
        plot->clearAllChannel();
        setNullVoltage();
    }
}

void MainWindow::timerTick() {
    clientSocket->downloadMeasurement();
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
}


void MainWindow::timeIntervalChanged(QTime time) {
    int millis = time.msecsSinceStartOfDay();
    timer->setInterval(millis);
}
