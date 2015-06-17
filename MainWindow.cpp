#include "MainWindow.hpp"

MainWindow::MainWindow() {
    connection = nullptr;
    plot = new GaugePlot();
    timer = new QTimer();

    thread = new QThread();
    thread->start();
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTick()));

    connectionPanel = new ConnectionPanel();
    sidePanel = new SidePanel();

    plot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    plot->setMinimumSize(300, 150);
    connectionPanel->layout()->setContentsMargins(0, 0, 0, 0);
    sidePanel->layout()->setContentsMargins(0, 0, 0, 0);
    sidePanel->setFixedWidth(150);

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

    connect(sidePanel, SIGNAL(showModeChanged(ShowMode)),
            this, SLOT(showModeChanged(ShowMode)));

    connect(sidePanel, SIGNAL(timeRangeChanged(int)),
            this, SLOT(timeRangeChanged(int)));
    connect(sidePanel, SIGNAL(timeIntervalChanged(int)),
            this, SLOT(timeIntervalChanged(int)));
    connect(sidePanel, SIGNAL(timeFrameChanged(int)),
            this, SLOT(timeFrameChanged(int)));

    // Wymagana rejestracja typów,
    // bez tego dalsze połączenia z Connection nie będą działać
    qRegisterMetaType<Connection::State>("Connection::State");
    qRegisterMetaType<Measurement>("Measurement&");
    qRegisterMetaType<Connection::Frame>("Connection::Frame&");

    connect(connectionPanel, SIGNAL(connectionChanged(Connection*)),
            this, SLOT(setConnection(Connection*)));

    connect(plot, SIGNAL(isDone()), this, SLOT(plotIsDone()));

    setNullVoltage();
    statusBar()->show();
}


void MainWindow::setConnection(Connection *connection) {
    if (this->connection != nullptr)
        delete this->connection;

    this->connection = connection;
    this->connection->moveToThread(thread);

    connect(connectionPanel, SIGNAL(doConnect()),
            connection, SLOT(createConnection()));
    connect(connectionPanel, SIGNAL(doDisconnect()),
            connection, SLOT(closeConnection()));

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
}

void MainWindow::timerTick() {
    if (!isBusy) {
        isBusy = true;
        if (isFrameMode()) {
            emit doDownloadFrame(timeFrame);
        } else {
            emit doDownloadOne();
        }
    }
}

void MainWindow::plotIsDone() {
    isBusy = false;
    if (isFrameMode())
        timer->start(0);
}

void MainWindow::doStart() {
    isBusy = false;
    if (isRealTimeMode())
        startRealTimeMode();
    else
        startFrameMode();
}

void MainWindow::startRealTimeMode() {
    int timeRange = sidePanel->getTimeRange();
    int timeInterval = sidePanel->getTimeInterval();
    plot->setTimeRangeMillis(timeRange);
    timer->setSingleShot(false);
    timer->setInterval(timeInterval);
    timer->start();
}

void MainWindow::startFrameMode() {
    this->timeFrame = sidePanel->getTimeFrame();
    setNullVoltage();
    timer->setSingleShot(true);
    timer->setInterval(0);
    timer->start();
}

void MainWindow::connectionStateChanged(Connection::State state) {

    if (state == Connection::Connected) {
        QString address = connection->toStringAddress();
        statusBar()->showMessage("Połączono z " + address);

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

void MainWindow::showModeChanged(ShowMode mode) {
    this->showMode = mode;
    plot->setShowMode(mode);

    if (isConnected())
        doStart();
}

void MainWindow::timeRangeChanged(int timeRange) {
    if (isRealTimeMode())
        plot->setTimeRangeMillis(timeRange);
}

void MainWindow::timeIntervalChanged(int timeInterval) {
    if (isRealTimeMode())
        timer->setInterval(timeInterval);
}

void MainWindow::timeFrameChanged(int timeFrame) {
    this->timeFrame = timeFrame;
}

bool MainWindow::isRealTimeMode() {
    return showMode == RealTimeMode;
}

bool MainWindow::isFrameMode() {
    return showMode == FrameMode;
}

bool MainWindow::isConnected() {
    if (this->connection == nullptr)
        return false;
    return this->connection->isConnected();
}

Connection *MainWindow::getConnection() {
    return connection;
}