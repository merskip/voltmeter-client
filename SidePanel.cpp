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
    timeRangeEdit->setMinimumTime(QTime(0, 0, 0, 1));
    timeRangeEdit->setTime(QTime(0, 0, 8, 0));

    timeIntervalEdit = new QTimeEdit();
    timeIntervalEdit->setDisplayFormat("HH:mm:ss.zzz");
    timeIntervalEdit->setMinimumTime(QTime(0, 0, 0, 1));
    timeIntervalEdit->setTime(QTime(0, 0, 0, 10));

    timeFrameEdit = new QTimeEdit();
    timeFrameEdit->setDisplayFormat("HH:mm:ss.zzz");
    timeFrameEdit->setCurrentSectionIndex(3);
    timeFrameEdit->setMinimumTime(QTime(0, 0, 0, 1));
    timeFrameEdit->setTime(QTime(0, 0, 0, 100));

    normalLayout = new QVBoxLayout();
    normalLayout->addStretch(1);
    normalLayout->addWidget(new QLabel("Zakres czasu:"));
    normalLayout->addWidget(timeRangeEdit);
    normalLayout->addWidget(new QLabel("Odświerzanie:"));
    normalLayout->addWidget(timeIntervalEdit);
    normalLayout->setContentsMargins(0, 0, 0, 0);

    normalPage = new QWidget();
    normalPage->setLayout(normalLayout);

    frameLayout = new QVBoxLayout();
    frameLayout->addStretch(1);
    frameLayout->addWidget(new QLabel("Czas klatki:"));
    frameLayout->addWidget(timeFrameEdit);
    frameLayout->setContentsMargins(0, 0, 0, 0);

    framePage = new QWidget();
    framePage->setLayout(frameLayout);

    stackedPanel = new QStackedWidget();
    stackedPanel->addWidget(normalPage);
    stackedPanel->addWidget(framePage);
    stackedPanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(channelPanel[1]);
    layout->addWidget(channelPanel[2]);
    layout->addWidget(channelPanel[3]);
    layout->addWidget(channelPanel[4]);
    layout->addSpacerItem(new QSpacerItem(0,0, QSizePolicy::Preferred, QSizePolicy::Expanding));
    layout->addWidget(stackedPanel);
    setLayout(layout);

    connect(frameModeCheck, SIGNAL(stateChanged(int)),
            this, SLOT(handleFrameModeStateChanged(int)));

    connect(timeRangeEdit, SIGNAL(timeChanged(QTime)),
            this, SLOT(handleTimeRangeEdit(QTime)));
    connect(timeIntervalEdit, SIGNAL(timeChanged(QTime)),
            this, SLOT(handleTimeIntervalEdit(QTime)));
    connect(timeFrameEdit, SIGNAL(timeChanged(QTime)),
            this, SLOT(handleTimeFrameEdit(QTime)));

    setFrameMode(false);
}

ChannelPanel *SidePanel::getChannelPanel(int channel) {
    return channelPanel[channel];
}

void SidePanel::handleFrameModeStateChanged(int state) {
    setFrameMode((bool) state);
    frameModeCheck->setFocus();
}

void SidePanel::setFrameMode(bool enabled) {
    stackedPanel->setCurrentWidget(enabled ? framePage : normalPage);

    if (enabled) {
        frameLayout->insertWidget(1, frameModeCheck);
        timeFrameEdit->setCurrentSectionIndex(3);
    } else {
        normalLayout->insertWidget(1, frameModeCheck);
        timeRangeEdit->setCurrentSectionIndex(2);
        timeIntervalEdit->setCurrentSectionIndex(3);
    }

    emit frameModeChanged(enabled);
}

void SidePanel::handleTimeRangeEdit(QTime time) {
    int millis = time.msecsSinceStartOfDay();
    emit timeRangeChanged(millis);
}

void SidePanel::handleTimeIntervalEdit(QTime time) {
    int millis = time.msecsSinceStartOfDay();
    emit timeIntervalChanged(millis);
}

void SidePanel::handleTimeFrameEdit(QTime time) {
    int millis = time.msecsSinceStartOfDay();
    emit timeFrameChanged(millis);
}
