#include "MainWindow.hpp"

MainWindow::MainWindow(QString serverHost, quint16 serverPort) {
    clientSocket = new ClientSocket;
    plot = new GaugePlot();
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTick()));

    connectionPanel = new ConnectionPanel(serverHost, serverPort);

    channelPanel[0] = nullptr;
    channelPanel[1] = new ChannelPanel(1, "Kanał 1", plot->getChannelColor(1));
    channelPanel[2] = new ChannelPanel(2, "Kanał 2", plot->getChannelColor(2));
    channelPanel[3] = new ChannelPanel(3, "Kanał 3", plot->getChannelColor(3));
    channelPanel[4] = new ChannelPanel(4, "Kanał 4", plot->getChannelColor(4));

    timeRangeEdit = new QTimeEdit();
    timeRangeEdit->setDisplayFormat("HH:mm:ss.zzz");
    timeRangeEdit->setCurrentSectionIndex(2);
    timeRangeEdit->setMinimumTime(QTime(0, 0, 0, 1));
    timeRangeEdit->setTime(QTime(0, 0, 8, 0));
    timeRangeEdit->setFixedWidth(120);
    connect(timeRangeEdit, SIGNAL(timeChanged(QTime)),
            this, SLOT(timeRangeChanged(QTime)));

    plot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    plot->setMinimumSize(300, 150);
    connectionPanel->layout()->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *panelLayout = new QVBoxLayout();
    panelLayout->setAlignment(Qt::AlignTop);
    panelLayout->addWidget(channelPanel[1]);
    panelLayout->addWidget(channelPanel[2]);
    panelLayout->addWidget(channelPanel[3]);
    panelLayout->addWidget(channelPanel[4]);
    panelLayout->addWidget(timeRangeEdit);

    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->addWidget(plot);
    contentLayout->addLayout(panelLayout);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(connectionPanel);
    layout->addLayout(contentLayout);

    QWidget *centerWidget = new QWidget();
    centerWidget->setLayout(layout);
    setCentralWidget(centerWidget);

    for (int i = 1; i <= 4; i++) {
        connect(channelPanel[i], SIGNAL(channelVisibleChanged(int, bool)),
                plot, SLOT(setChannelVisible(int, bool)));
    }
    channelPanel[1]->setChannelVisible(true);
    channelPanel[2]->setChannelVisible(false);
    channelPanel[3]->setChannelVisible(false);
    channelPanel[4]->setChannelVisible(false);

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
        timer->start(10);
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
    double millis = time.msecsSinceStartOfDay();
    plot->setTimeRange(millis / 1000);
}
