#include "MainWindow.hpp"
#include "NetworkConnection.hpp"

MainWindow::MainWindow() {
    connection = new NetworkConnection();
    plot = new GaugePlot();
    timer = new QTimer();

    thread = new QThread();
    ((NetworkConnection*) connection)->getDevice()->moveToThread(thread);
    connection->moveToThread(thread);
    thread->start();
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTick()));

    connectionPanel = new ConnectionPanel();
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

    // Wymagana rejestracja typów,
    // bez tego dalsze połączenia z Connection nie będą działać
    qRegisterMetaType<Connection::Params>("Connection::Params");
    qRegisterMetaType<Connection::State>("Connection::State");
    qRegisterMetaType<Measurement>("Measurement&");
    qRegisterMetaType<Connection::Frame>("Connection::Frame&");

    connect(connectionPanel, SIGNAL(doConnect(Connection::Params)),
            connection, SLOT(connect(Connection::Params)));
    connect(connectionPanel, SIGNAL(doDisconnect()),
            connection, SLOT(disconnect()));

    connect(connection, SIGNAL(stateChanged(Connection::State)),
            this, SLOT(connectionStateChanged(Connection::State)));
    connect(connection, SIGNAL(stateChanged(Connection::State)),
            connectionPanel, SLOT(setConnectionState(Connection::State)));

    connect(connection, SIGNAL(errorOccurred(QString)),
            this, SLOT(connectionErrorOccurred(QString)));

    connect(connection, SIGNAL(connected()),
            this, SLOT(doStart()));

    connect(this, SIGNAL(doDownloadOne()),
            connection, SLOT(downloadOne()));
    connect(this, SIGNAL(doDownloadFrame(int)),
            connection, SLOT(downloadFrame(int)));

    connect(connection, SIGNAL(downloadedOne(Measurement&)),
            plot, SLOT(appendMeasurement(Measurement&)));
    connect(connection, SIGNAL(downloadedOne(Measurement&)),
            this, SLOT(voltageChanged(Measurement&)));

    connect(connection, SIGNAL(downloadedFrame(Connection::Frame&)),
            plot, SLOT(showFrame(Connection::Frame&)));

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
        emit doDownloadOne();
    }
}

void MainWindow::plotIsDone() {
    if (isFrameMode)
        timer->start(0);
}

void MainWindow::doStart() {
    plot->setTimeRange(timeRange / 1000);
    timer->start(timeInterval);
}

void MainWindow::connectionStateChanged(Connection::State state) {

    if (state == Connection::Connected) {
        QString address = connection->toStringAddress();
        statusBar()->showMessage("Połączono z " + address);

        plot->setTimeRange(timeRange / 1000);
        timer->start(timeInterval);
        isSocketError = false;
    } else if (state == Connection::Disconnected) {
        timer->stop();
        plot->clearAllChannel();
        setNullVoltage();

        if (!isSocketError)
            statusBar()->showMessage("Rozłączono z serwerem");
    } else if (state == Connection::Connecting) {
        QString address = connection->toStringAddress();
        statusBar()->showMessage("Łączenie z " + address + "...");
        isSocketError = false;
    }
}

void MainWindow::connectionErrorOccurred(QString error) {
    statusBar()->showMessage("Wystąpił błąd: " + error.toLower());
    isSocketError = true;
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

    if (connection->isConnected())
        timer->start();
}
