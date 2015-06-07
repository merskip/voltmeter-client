#include "GaugePlot.hpp"

GaugePlot::GaugePlot() : QCustomPlot() {

    // Null channel
    graph[0] = nullptr;
    dot[0] = nullptr;

    // Channel 1
    graph[1] = addGraph();
    dot[1] = addGraph();
    setupGraphChannel(1, Qt::blue);

    // Channel 2
    graph[2] = addGraph();
    dot[2] = addGraph();
    setupGraphChannel(2, Qt::red);

    // Channel 3
    graph[3] = addGraph();
    dot[3] = addGraph();
    setupGraphChannel(3, Qt::darkGreen);

    // Channel 4
    graph[4] = addGraph();
    dot[4] = addGraph();
    setupGraphChannel(4, Qt::magenta);

    xAxis->setTickLabelType(QCPAxis::ltDateTime);
    xAxis->setDateTimeFormat("hh:mm:ss");
    xAxis->setAutoTickStep(false);
    axisRect()->setupFullAxesBox();

    setVoltageRange(-0.2, 5.3);
    setTimeRange(8.0);
    setTriggerVoltage(2.7);
}


void GaugePlot::setupGraphChannel(int channel, QColor color) {
    graph[channel]->setPen(QPen(color));
    graph[channel]->setLineStyle(QCPGraph::lsLine);

    dot[channel]->setPen(QPen(color));
    dot[channel]->setLineStyle(QCPGraph::lsNone);
    dot[channel]->setScatterStyle(QCPScatterStyle::ssDisc);
}

void GaugePlot::setVoltageRange(double min, double max) {
    yAxis->setRange(min, max);
    replot();
}

void GaugePlot::setTimeRange(double time) {
    timeRange = time;
    xAxis->setTickStep(time / 4.0);
    if (time >= 1.0)
        xAxis->setDateTimeFormat("hh:mm:ss");
    else
        xAxis->setDateTimeFormat("zzz");
}

void GaugePlot::setChannelVisible(int channel, bool on) {
    graph[channel]->setVisible(on);
    dot[channel]->setVisible(on);
}

QColor GaugePlot::getChannelColor(int channel) {
    return graph[channel]->pen().color();
}


void GaugePlot::setFrameMode(bool state) {
    if (state) {
        clearAllChannel();
        xAxis->setTickLabelType(QCPAxis::LabelType::ltNumber);
        xAxis->setTicks(false);
    } else {
        setTimeRange(timeRange);
        xAxis->setTickLabelType(QCPAxis::LabelType::ltDateTime);
        xAxis->setTicks(true);
    }
}

void GaugePlot::clearAllChannel() {
    for (int i = 1; i <= 4; i++) {
        graph[i]->clearData();
        dot[i]->clearData();
    }
    xAxis->setRange(0, 0);
    replot();
}

void GaugePlot::setTriggerVoltage(double voltage) {
    triggerVoltage = voltage;
}

void GaugePlot::appendMeasurement(Measurement &data) {
    updateGraphChannel(1, data.time, data.channel[1].voltage);
    updateGraphChannel(2, data.time, data.channel[2].voltage);
    updateGraphChannel(3, data.time, data.channel[3].voltage);
    updateGraphChannel(4, data.time, data.channel[4].voltage);

    xAxis->setRange(data.time, timeRange, Qt::AlignRight);
    replot();
    emit isDone();
}

void GaugePlot::updateGraphChannel(int channel, double time, double voltage) {
    graph[channel]->addData(time, voltage);
    graph[channel]->removeDataBefore(time - (timeRange * 2));

    dot[channel]->clearData();
    dot[channel]->addData(time, voltage);
}

void GaugePlot::showFrame(Connection::Frame &data) {
    graph[1]->clearData();
    graph[2]->clearData();
    graph[3]->clearData();
    graph[4]->clearData();

    int dataSize = data.size();
    for (int i = 0; i < dataSize; i++) {
        const QVector<double> &item = data.at(i);
        graph[1]->addData(i, item.at(1));
        graph[2]->addData(i, item.at(2));
        graph[3]->addData(i, item.at(3));
        graph[4]->addData(i, item.at(4));
    }

    int shift = getShiftForTrigger(1, triggerVoltage, data);
    xAxis->setRange(0 + shift + (dataSize / 5), dataSize - 1 + shift - (dataSize / 5));

    replot();
    emit isDone();
}


int GaugePlot::getShiftForTrigger(int channel, double voltage, QList<QVector<double>> &data) {

    int dataSize = data.size();
    int sizeHalf = dataSize / 2;
    for (int i = 0; i < sizeHalf; i++) {
        int indexLeft = sizeHalf - i;
        int indexLeftNext = indexLeft - 1;

        int indexRight = sizeHalf + i;
        int indexRightNext = indexRight + 1;

        if (indexLeftNext >= 0) {
            double leftValue = data.at(indexLeft).at(channel);
            double leftNextValue = data.at(indexLeftNext).at(channel);

            if (belongsTo(voltage, leftValue, leftNextValue) && leftValue < leftNextValue)
                return -i;
        }

        if (indexRightNext < dataSize) {
            double rightValue = data.at(indexRight).at(channel);
            double rightNextValue = data.at(indexRightNext).at(channel);

            if (belongsTo(voltage, rightValue, rightNextValue) && rightValue > rightNextValue)
                return i;
        }
    }
    return 0;
}

bool GaugePlot::belongsTo(double value, double a, double b) {
    double min = std::min(a, b);
    double max = std::max(a, b);
    return value >= min && value <= max;
}
