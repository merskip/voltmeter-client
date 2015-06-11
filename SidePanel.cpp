#include <QVBoxLayout>
#include "SidePanel.hpp"

SidePanel::SidePanel() {
    channelPanel[0] = nullptr;
    channelPanel[1] = new ChannelPanel(1, "Kanał 1");
    channelPanel[2] = new ChannelPanel(2, "Kanał 2");
    channelPanel[3] = new ChannelPanel(3, "Kanał 3");
    channelPanel[4] = new ChannelPanel(4, "Kanał 4");

    frameModeCheck = new QCheckBox("Tryb frame");

    timeRangeEdit = new QTimeEdit();
    timeRangeEdit->setDisplayFormat("HH:mm:ss.zzz");
    timeRangeEdit->setCurrentSectionIndex(2);
    timeRangeEdit->setMinimumTime(QTime(0, 0, 0, 1));
    timeRangeEdit->setTime(QTime(0, 0, 8, 0));

    timeIntervalEdit = new QTimeEdit();
    timeIntervalEdit->setDisplayFormat("HH:mm:ss.zzz");
    timeIntervalEdit->setCurrentSectionIndex(3);
    timeIntervalEdit->setMinimumTime(QTime(0, 0, 0, 1));
    timeIntervalEdit->setTime(QTime(0, 0, 0, 10));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(channelPanel[1]);
    layout->addWidget(channelPanel[2]);
    layout->addWidget(channelPanel[3]);
    layout->addWidget(channelPanel[4]);
    layout->addSpacerItem(new QSpacerItem(0,0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    layout->addWidget(frameModeCheck);
    layout->addWidget(new QLabel("Zakres czasu:"));
    layout->addWidget(timeRangeEdit);
    layout->addWidget(new QLabel("Odświerzanie:"));
    layout->addWidget(timeIntervalEdit);
    setLayout(layout);

    connect(frameModeCheck, SIGNAL(stateChanged(int)),
            this, SLOT(handleFrameModeStateChanged(int)));
}

ChannelPanel *SidePanel::getChannelPanel(int channel) {
    return channelPanel[channel];
}

void SidePanel::handleFrameModeStateChanged(int state) {
    emit frameModeChanged((bool) state);
}

QTimeEdit *SidePanel::getTimeRangeEdit() {
    return timeRangeEdit;
}

QTimeEdit *SidePanel::getTimeIntervalEdit() {
    return timeIntervalEdit;
}

int SidePanel::getTimeRangeMillis() {
    return timeRangeEdit->time().msecsSinceStartOfDay();
}

int SidePanel::getTimeIntervalMillis() {
    return timeIntervalEdit->time().msecsSinceStartOfDay();
}
