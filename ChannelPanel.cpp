#include <QtWidgets/QBoxLayout>
#include "ChannelPanel.hpp"

ChannelPanel::ChannelPanel(int channel, QString name, QColor color)
        : channel(channel) {

    nameLabel = new QLabel(name);

    toggleBtn = new QPushButton("On");
    toggleBtn->setCheckable(true);
    toggleBtn->setChecked(true);
    toggleBtn->setFixedWidth(35);

    voltageLabel = new QLabel();
    voltageLabel->setLineWidth(2);
    voltageLabel->setFrameStyle(QFrame::Box);

    QFont voltageFont("Courier New");
    voltageFont.setPixelSize(18);
    voltageFont.setBold(true);
    voltageLabel->setFont(voltageFont);
    voltageLabel->setAlignment(Qt::AlignRight);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(nameLabel);
    topLayout->addWidget(toggleBtn);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(topLayout);
    layout->addWidget(voltageLabel);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setLayout(layout);

    paletteEnabled.setColor(foregroundRole(), color);
    paletteDisabled.setColor(foregroundRole(), Qt::gray);
    setChannelVisible(true);

    connect(toggleBtn, SIGNAL(toggled(bool)),
            this, SLOT(setChannelVisible(bool)));
}


void ChannelPanel::setChannelColor(QColor color) {
    paletteEnabled.setColor(foregroundRole(), color);
}

void ChannelPanel::setVoltage(double value) {
    if (channelVisible) {
        QString text = QString("%1 V")
                .arg(QString::number(value, 'f', 2));
        voltageLabel->setText(text);
    }
}

void ChannelPanel::setChannelVisible(bool visible) {
    channelVisible = visible;
    if (visible) {
        toggleBtn->setText("On");
        toggleBtn->setChecked(visible);
        setPalette(paletteEnabled);
    } else {
        toggleBtn->setText("Off");
        toggleBtn->setChecked(visible);
        setNullVoltage();
        setPalette(paletteDisabled);
    }
    emit channelVisibleChanged(channel, visible);
}

void ChannelPanel::setNullVoltage() {
    voltageLabel->setText("-.-- V");
}

bool ChannelPanel::isChannelVisible() {
    return channelVisible;
}
