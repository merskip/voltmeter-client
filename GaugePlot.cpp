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
    xAxis->setTickStep(2);
    axisRect()->setupFullAxesBox();

    setVoltageRange(-0.2, 5.3);
    setTimeRange(8.0);
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
    yAxis2->setRange(min, max);
}

void GaugePlot::setTimeRange(double time) {
    timeRange = time;
}

void GaugePlot::setChannelVisible(int channel, bool on) {
    graph[channel]->setVisible(on);
    dot[channel]->setVisible(on);
}

QColor GaugePlot::getChannelColor(int channel) {
    return graph[channel]->pen().color();
}

void GaugePlot::appendMeasurement(Measurement &data) {

    updateGraphChannel(1, data.time, data.channel[1].voltage);
    updateGraphChannel(2, data.time, data.channel[2].voltage);
    updateGraphChannel(3, data.time, data.channel[3].voltage);
    updateGraphChannel(4, data.time, data.channel[4].voltage);

    this->xAxis->setRange(data.time, timeRange, Qt::AlignRight);
    this->replot();
}

void GaugePlot::updateGraphChannel(int channel, double time, double voltage) {
    graph[channel]->addData(time, voltage);
    graph[channel]->removeDataBefore(time - timeRange);

    dot[channel]->clearData();
    dot[channel]->addData(time, voltage);
}
