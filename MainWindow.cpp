#include "MainWindow.hpp"

MainWindow::MainWindow() : QMainWindow() {
    client = new ClientSocket;
    plot = new GaugePlot();
    timer = new QTimer();

    channelPanel[0] = nullptr;
    channelPanel[1] = new ChannelPanel(1, "Kanał 1", plot->getChannelColor(1));
    channelPanel[2] = new ChannelPanel(2, "Kanał 2", plot->getChannelColor(2));
    channelPanel[3] = new ChannelPanel(3, "Kanał 3", plot->getChannelColor(3));
    channelPanel[4] = new ChannelPanel(4, "Kanał 4", plot->getChannelColor(4));

    timeRangeEdit = new QTimeEdit();
    timeRangeEdit->setDisplayFormat("HH:mm:ss.zzz");
    timeRangeEdit->setCurrentSectionIndex(2);
    timeRangeEdit->setMinimumTime(QTime(0, 0, 0, 1));
    timeRangeEdit->setTime(QTime(0, 0, 8, 0));;
    timeRangeEdit->setFixedWidth(120);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(plot);

    QVBoxLayout *panelLayout = new QVBoxLayout();
    panelLayout->setAlignment(Qt::AlignTop);
    panelLayout->addWidget(channelPanel[1]);
    panelLayout->addWidget(channelPanel[2]);
    panelLayout->addWidget(channelPanel[3]);
    panelLayout->addWidget(channelPanel[4]);
    panelLayout->addWidget(timeRangeEdit);
    layout->addLayout(panelLayout);

    QWidget *centerWidget = new QWidget();
    centerWidget->setLayout(layout);
    setCentralWidget(centerWidget);

    connect(timer, SIGNAL(timeout()),
            this, SLOT(timerTick()));

    connect(client, SIGNAL(measurementDownloaded(Measurement&)),
            plot, SLOT(appendMeasurement(Measurement&)));
    connect(client, SIGNAL(measurementDownloaded(Measurement&)),
            this, SLOT(voltageChanged(Measurement&)));

    for (int i = 1; i <= 4; i++) {
        connect(channelPanel[i], SIGNAL(channelVisibleChanged(int, bool)),
                plot, SLOT(setChannelVisible(int, bool)));
    }
    channelPanel[1]->setChannelVisible(true);
    channelPanel[2]->setChannelVisible(false);
    channelPanel[3]->setChannelVisible(false);
    channelPanel[4]->setChannelVisible(false);

    connect(timeRangeEdit, SIGNAL(timeChanged(QTime)),
            this, SLOT(timeRangeChanged(QTime)));

    client->connectToHost(QHostAddress("192.168.1.8"), 22444);
    if (!client->waitForConnected()) {
        std::cerr << "Serwer jest niedostępny" << std::endl;
        exit(1);
    }

    timer->start(10);
}

void MainWindow::timerTick() {
    client->downloadMeasurement();
}


void MainWindow::voltageChanged(Measurement &data) {
    channelPanel[1]->setVoltage(data.channel[1].voltage);
    channelPanel[2]->setVoltage(data.channel[2].voltage);
    channelPanel[3]->setVoltage(data.channel[3].voltage);
    channelPanel[4]->setVoltage(data.channel[4].voltage);
}

void MainWindow::timeRangeChanged(QTime time) {
    double millis = time.msecsSinceStartOfDay();
    timer->setInterval((int) millis);
    plot->setTimeRange(millis / 1000);
}
