#include "GaugePlot.hpp"
#include "FrameShiftTrigger.hpp"

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
    xAxis->grid()->setZeroLinePen(Qt::NoPen);
    axisRect()->setupFullAxesBox();

    setVoltageRange(-0.05, 5.15);
    setTimeRange(8.0);
    setTriggerOptions(DEFAULT_TRIGGER_OPTIONS);
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


void GaugePlot::setShowMode(ShowMode mode) {
    if (mode == RealTimeMode)
        setupRealTimeMode();
    else
        setupFrameMode();
}

void GaugePlot::setupRealTimeMode() {
    setTimeRange(timeRange);
    xAxis->setTickLabelType(QCPAxis::LabelType::ltDateTime);
    xAxis->setTicks(true);
}

void GaugePlot::setupFrameMode() {
    clearAllChannel();
    xAxis->setTickLabelType(QCPAxis::LabelType::ltNumber);
    xAxis->setTicks(false);
}

void GaugePlot::clearAllChannel() {
    for (int i = 1; i <= 4; i++) {
        graph[i]->clearData();
        dot[i]->clearData();
    }
    xAxis->setRange(0, 0);
    replot();
}

void GaugePlot::setTriggerOptions(TriggerOptions options) {
    this->triggerOptions = options;
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
    xAxis->setRange(0, dataSize);

    if (triggerOptions.isActive)
        moveGraphForTrigger(data);

    replot();
    emit isDone();
}

void GaugePlot::moveGraphForTrigger(Connection::Frame &data) {
    int shift = getShiftForTrigger(data);
    moveGraph(shift, data.size() / 8);
}

int GaugePlot::getShiftForTrigger(Connection::Frame &data) {
    FrameShiftTrigger trigger(triggerOptions, data);
    return trigger.calculateShift();
}

void GaugePlot::moveGraph(int shift, int margin) {
    double lower = xAxis->range().lower;
    double upper = xAxis->range().upper;

    lower += shift + margin;
    upper += shift - margin;

    xAxis->setRange(lower, upper);
}