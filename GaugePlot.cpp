#include "GaugePlot.hpp"

GaugePlot::GaugePlot() : QCustomPlot() {

    graph[0] = nullptr; // Null channel
    graph[1] = createNewChannel(Qt::blue);
    graph[2] = createNewChannel(Qt::red);
    graph[3] = createNewChannel(Qt::darkGreen);
    graph[4] = createNewChannel(Qt::magenta);

    setupAxis();
    createTriggersPointsAndLines();
    setupFrequencyText();
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
    points->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 4));
    points->setPen(QPen(Qt::black));
    return points;
}

void GaugePlot::setupFrequencyText() {
    frequencyText = new QCPItemText(this);
    frequencyText->setPositionAlignment(Qt::AlignTop | Qt::AlignRight);
    frequencyText->position->setType(QCPItemPosition::ptAxisRectRatio);
    frequencyText->position->setCoords(1.0, 0.0); // Prawy górny róg
    frequencyText->setFont(QFont("Courier New", 12));
    frequencyText->setColor(Qt::black);
    frequencyText->setPen(QPen(Qt::black));
    frequencyText->setBrush(Qt::white);
    frequencyText->setLayer("legend");
    addItem(frequencyText);
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
        bool calledTrigger = applyTrigger(data);
        updateTriggerLinesPosition();

        if (calledTrigger && triggerOptions.oneShot)
            emit oneShotActive();
    }

    replot();
    emit isDone();
}

bool GaugePlot::applyTrigger(Connection::Frame &data) {
    ShiftFrameTrigger frameTrigger(triggerOptions, data);
    bool calledTrigger = moveGraphForTrigger(frameTrigger);
    if (triggerOptions.isShowCalls)
        updateTriggersPoints(frameTrigger);

    updateFrequency(frameTrigger);
    return calledTrigger;
}

bool GaugePlot::moveGraphForTrigger(ShiftFrameTrigger &frameTrigger) {
    double graphShift = frameTrigger.getShift();
    moveGraph(graphShift);
    return graphShift != 0.0;
}

void GaugePlot::moveGraph(double shift) {
    xAxis->moveRange(shift);
}

void GaugePlot::updateFrequency(FrameTrigger &frameTrigger) {
    double averageDiffKeys = calculateAverageDiffBetweenTriggerCalls(frameTrigger);
    double secondsForOneKey = timeFrame / frameTrigger.getFrameSize();
    double timePeriod = averageDiffKeys * secondsForOneKey;
    double frequency = 1 / timePeriod;
    updateFrequency(frequency);
}

double GaugePlot::calculateAverageDiffBetweenTriggerCalls(FrameTrigger &frameTrigger) {
    QList<FrameTrigger::TriggerCall> callsList = frameTrigger.getTriggerCallsList();
    QListIterator<FrameTrigger::TriggerCall> i(callsList);

    if (callsList.empty())
        return NAN;

    double total = 0.0;
    int count = 0;
    while (i.hasNext()) {
        FrameTrigger::TriggerCall call = i.next();
        if (!i.hasNext()) break;
        FrameTrigger::TriggerCall nextCall = i.peekNext();

        if (call.edge == nextCall.edge) {
            total += nextCall.key - call.key;
            count++;
        }
    }
    return total / count;
}

void GaugePlot::updateFrequency(double frequency) {
    if (std::isnan(frequency))
        frequency = 0;

    double timePeriod = 1 / frequency * 1000;
    QString text = QString().sprintf("T=%6.2f [ms]\nf=%6.2f [Hz]", timePeriod, frequency);
    frequencyText->setText(text);
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
    frequencyText->setVisible(false);

    setTimeRange(timeRange);
    xAxis->setTicks(true);
}

void GaugePlot::setupFrameMode() {
    setTriggerLinesVisible(triggerOptions.isActive);
    setupTriggersPoints();
    frequencyText->setVisible(true);

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
    setupTriggersPoints();
    updateTriggerHorizontalLinePosition();
}

void GaugePlot::setTriggerLinesVisible(bool visible) {
    triggerVLine->setVisible(visible);
    triggerHLine->setVisible(visible);
}

void GaugePlot::setupTriggersPoints() {
    bool triggerIsActive = triggerOptions.isActive;
    bool isShowCalls = triggerOptions.isShowCalls;
    bool isShowChannel = isChannelVisible(triggerOptions.channel);
    bool visible = triggerIsActive && isShowCalls && isShowChannel;
    triggersPoints->setVisible(visible);

    QColor channelColor = getChannelColor(triggerOptions.channel);
    QColor pointsColor = channelColor.darker();
    QPen pointsPen = triggersPoints->pen();
    pointsPen.setColor(pointsColor);
    triggersPoints->setPen(pointsPen);
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

bool GaugePlot::isChannelVisible(int channel) {
    return graph[channel]->visible();
}

void GaugePlot::setChannelVisible(int channel, bool on) {
    graph[channel]->setVisible(on);
    setupTriggersPoints();
}

QColor GaugePlot::getChannelColor(int channel) {
    return graph[channel]->pen().color();
}

void GaugePlot::setVoltageRange(double min, double max) {
    yAxis->setRange(min, max);
    replot();
}