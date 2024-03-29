#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class ChannelPanel : public QWidget {
    Q_OBJECT

private:
    int channel;

    QLabel *nameLabel;
    QPushButton *toggleBtn;
    QLabel *voltageLabel;

    bool channelVisible;
    QPalette paletteEnabled;
    QPalette paletteDisabled;

public:
    ChannelPanel(int channel, QString name, QColor = Qt::black);

    void setChannelColor(QColor color);
    bool isChannelVisible();
    void setNullVoltage();

public slots:
    void setVoltage(double value);
    void setChannelVisible(bool visible);

signals:
    void channelVisibleChanged(int channel, bool visible);
};