#include "GaugePlot.hpp"

GaugePlot::GaugePlot() : QCustomPlot() {

    graph[0] = nullptr; // Null channel
    graph[1] = createNewChannel(Qt::blue);
    graph[2] = createNewChannel(Qt::red);
    graph[3] = createNewChannel(Qt::darkGreen);
    graph[4] = createNewChannel(Qt::magenta);

    setupAxis();
    createTriggersPointsAndLines();
    setVoltageRange(-0.05, 5.15);
    setTimeRange(8.0);
    setTriggerOptions(DEFAULT_TRIGGER_OPTIONS);
    setShowMode(ShowMode::RealTimeMode);
}

QCPGraph *GaugePlot::createNewChannel(QColor color) {
    QCPGraph *newGraph = addGraph();
    newGraph->setPen(QPen(color));
    newGraph->setLineStyle(QCPGraph::lsLine);
    return newGraph;
}

void GaugePlot::setupAxis() {
    xAxis->setTickLabelType(QCPAxis::ltDateTime);
    xAxis->setDateTimeFormat("hh:mm:ss");
    xAxis->setAutoTickStep(false);
    xAxis->grid()->setZeroLinePen(Qt::NoPen);
    axisRect()->setupFullAxesBox();

    yAxis->setAutoTicks(false);
    yAxis->setTickVector({0.0, 0.6, 1.5, 2.4, 3.3, 4.2, 5.1});
}

void GaugePlot::createTriggersPointsAndLines() {
    triggersPoints = createTriggersPoints();

    triggerVLine = createNewTriggerLine();
    addItem(triggerVLine);

    triggerHLine = createNewTriggerLine();
    addItem(triggerHLine);
}

QCPItemLine *GaugePlot::createNewTriggerLine() {
    QCPItemLine *newLine = new QCPItemLine(this);
    newLine->start->setType(QCPItemPosition::ptPlotCoords);
    newLine->end->setType(QCPItemPosition::ptPlotCoords);
    newLine->start->setAxes(xAxis, yAxis);
    newLine->end->setAxes(xAxis, yAxis);
    newLine->setLayer("grid");
    newLine->setPen(QPen(Qt::gray, 1));
    newLine->setAntialiased(false);
    return newLine;
}

QCPGraph *GaugePlot::createTriggersPoints() {
    QCPGraph *points = addGraph();
    points->setLineStyle(QCPGraph::lsNone);
    points->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 3));
    points->setPen(QPen(Qt::black));
    return points;
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

    if (triggerOptions.isActive) {
        applyTrigger(data);
        updateTriggerLinesPosition();
    }

    replot();
    emit isDone();
}


void GaugePlot::applyTrigger(Connection::Frame &data) {
    ShiftFrameTrigger frameTrigger(triggerOptions, data);
    moveGraphForTrigger(frameTrigger);
    if (triggerOptions.isShowCalls)
        updateTriggersPoints(frameTrigger);
}

void GaugePlot::moveGraphForTrigger(ShiftFrameTrigger &frameTrigger) {
    double graphShift = frameTrigger.getShift();
    moveGraph(graphShift);
}

void GaugePlot::moveGraph(double shift) {
    xAxis->moveRange(shift);
}

void GaugePlot::setShowMode(ShowMode mode) {
    this->showMode = mode;
    if (mode == RealTimeMode)
        setupRealTimeMode();
    else
        setupFrameMode();
}

void GaugePlot::setupRealTimeMode() {
    setTriggerLinesVisible(false);
    triggersPoints->setVisible(false);

    setTimeRange(timeRange);
    xAxis->setTicks(true);
}

void GaugePlot::setupFrameMode() {
    setTriggerLinesVisible(triggerOptions.isActive);
    triggersPoints->setVisible(triggerOptions.isActive && triggerOptions.isShowCalls);

    setTimeFrame(timeFrame);
    xAxis->setTickStep(1.0);
    xAxis->setTicks(false);
}

void GaugePlot::clearAllChannel() {
    for (int i = 1; i <= 4; i++) {
        graph[i]->clearData();
    }
    triggersPoints->clearData();
    xAxis->setRange(0, 0);
    replot();
}

void GaugePlot::setTriggerOptions(TriggerOptions options) {
    this->triggerOptions = options;
    setTriggerLinesVisible(options.isActive);
    triggersPoints->setVisible(options.isActive && triggerOptions.isShowCalls);
    updateTriggerHorizontalLinePosition();
}

void GaugePlot::setTriggerLinesVisible(bool visible) {
    triggerVLine->setVisible(visible);
    triggerHLine->setVisible(visible);
}

void GaugePlot::updateTriggersPoints(FrameTrigger &frameTrigger) {
    triggersPoints->clearData();
    QList<FrameTrigger::TriggerCall> calls = frameTrigger.getTriggerCallsList();
    for (FrameTrigger::TriggerCall trigger : calls) {
        triggersPoints->addData(trigger.key, trigger.voltage);
    }
}

void GaugePlot::updateTriggerLinesPosition() {
    updateTriggerVerticalLinePosition();
    updateTriggerHorizontalLinePosition();
}

void GaugePlot::updateTriggerVerticalLinePosition() {
    double xTrigger = xAxis->range().center();
    double yStart = yAxis->range().lower;
    double yEnd = yAxis->range().upper;

    triggerVLine->start->setCoords(xTrigger, yStart);
    triggerVLine->end->setCoords(xTrigger, yEnd);
}

void GaugePlot::updateTriggerHorizontalLinePosition() {
    double yTrigger = triggerOptions.voltage;
    double xStart = xAxis->range().lower;
    double xEnd = xAxis->range().upper;

    triggerHLine->start->setCoords(xStart, yTrigger);
    triggerHLine->end->setCoords(xEnd, yTrigger);
}

void GaugePlot::setTimeRange(double time) {
    timeRange = time;
    xAxis->setTickStep(time / 4.0);
    if (time >= 1.0)
        xAxis->setDateTimeFormat("hh:mm:ss");
    else
        xAxis->setDateTimeFormat("zzz");
}

void GaugePlot::setTimeFrame(double time) {
    timeFrame = time;
}

void GaugePlot::setChannelVisible(int channel, bool on) {
    graph[channel]->setVisible(on);
}

QColor GaugePlot::getChannelColor(int channel) {
    return graph[channel]->pen().color();
}

void GaugePlot::setVoltageRange(double min, double max) {
    yAxis->setRange(min, max);
    replot();
}