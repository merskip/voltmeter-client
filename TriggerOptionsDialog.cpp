#include "TriggerOptionsDialog.hpp"

TriggerOptionsDialog::TriggerOptionsDialog(QWidget *parent)
        : QDialog(parent) {
    setWindowTitle("Ustawienia wyzwalacza");

    QLayout *ui = createLayoutUI();
    setLayout(ui);
    setDialogSize(ui);
    setOptions(DEFAULT_TRIGGER_OPTIONS);
    connectSignalsAndSlots();
}

QLayout *TriggerOptionsDialog::createLayoutUI() {
    activeCheck = new QCheckBox("Wyzwalacz aktywny");

    channelSelect = new QComboBox();
    channelSelect->addItem("Kanał 1", 1);
    channelSelect->addItem("Kanał 2", 2);
    channelSelect->addItem("Kanał 3", 3);
    channelSelect->addItem("Kanał 4", 4);

    voltageEdit = new QDoubleSpinBox();
    voltageEdit->setSuffix(" V");
    voltageEdit->setDecimals(2);
    voltageEdit->setSingleStep(0.1);
    voltageEdit->setRange(0.0, 5.1);
    voltageEdit->setButtonSymbols(QAbstractSpinBox::PlusMinus);

    edgeSelect = new QComboBox();
    edgeSelect->addItem("Dowolne", TriggerOptions::AnyEdge);
    edgeSelect->addItem("Rosnące", TriggerOptions::RisingEdge);
    edgeSelect->addItem("Opadające", TriggerOptions::FallingEdge);

    oneShotCheck = new QCheckBox("OneShot");
    showCallsCheck = new QCheckBox("Pokaż wywołania");

    formLayout = new QFormLayout();
    formLayout->addWidget(activeCheck);
    formLayout->addRow("Kanał: ", channelSelect);
    formLayout->addRow("Napięcie: ", voltageEdit);
    formLayout->addRow("Zbocze: ", edgeSelect);
    return formLayout;
}

void TriggerOptionsDialog::setDialogSize(QLayout *layoutUI) {
    int heightHint = layoutUI->sizeHint().height();
    this->setFixedHeight(heightHint);
    this->setFixedWidth(300);
}

void TriggerOptionsDialog::connectSignalsAndSlots() {
    connect(activeCheck, SIGNAL(stateChanged(int)),
            this, SLOT(handleActiveChanged(int)));

    connect(channelSelect, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleChannelChanged(int)));

    connect(voltageEdit, SIGNAL(valueChanged(double)),
            this, SLOT(handleVoltageChanged(double)));

    connect(edgeSelect, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleEdgeChanged(int)));

    connect(oneShotCheck, SIGNAL(stateChanged(int)),
            this, SLOT(handleOneShotChanged(int)));

    connect(showCallsCheck, SIGNAL(stateChanged(int)),
            this, SLOT(handleShowCallsChanged(int)));
}

void TriggerOptionsDialog::handleActiveChanged(int state) {
    bool isActive = (bool) state;
    currentOptions.isActive = isActive;
    setWidgetsEnable(isActive);
    emit optionsChanged(currentOptions);
}

void TriggerOptionsDialog::handleChannelChanged(int index) {
    int channel = channelSelect->itemData(index).toInt();
    currentOptions.channel = channel;
    emit optionsChanged(currentOptions);
}

void TriggerOptionsDialog::handleVoltageChanged(double value) {
    currentOptions.voltage = value;
    emit optionsChanged(currentOptions);
}

void TriggerOptionsDialog::handleEdgeChanged(int index) {
    QVariant data = edgeSelect->itemData(index);
    TriggerOptions::EdgeType edge = (TriggerOptions::EdgeType) data.toInt();
    currentOptions.edge = edge;
    emit optionsChanged(currentOptions);
}

void TriggerOptionsDialog::handleOneShotChanged(int state) {
    bool showShot = (bool) state;
    currentOptions.oneShot = showShot;
    emit optionsChanged(currentOptions);
}

void TriggerOptionsDialog::handleShowCallsChanged(int state) {
    bool isShowCalls = (bool) state;
    currentOptions.isShowCalls = isShowCalls;
    emit optionsChanged(currentOptions);
}

TriggerOptions TriggerOptionsDialog::getOptions() {
    return currentOptions;
}

void TriggerOptionsDialog::setOptions(TriggerOptions options) {
    activeCheck->setChecked(options.isActive);
    setWidgetsEnable(options.isActive);

    int channelIndex = channelSelect->findData(options.channel);
    channelSelect->setCurrentIndex(channelIndex);

    voltageEdit->setValue(options.voltage);

    int edgeIndex = edgeSelect->findData(options.edge);
    edgeSelect->setCurrentIndex(edgeIndex);

    showCallsCheck->setChecked(options.isShowCalls);

    currentOptions = options;
    emit optionsChanged(currentOptions);
}

void TriggerOptionsDialog::setWidgetsEnable(bool enable) {
    channelSelect->setEnabled(enable);
    voltageEdit->setEnabled(enable);
    edgeSelect->setEnabled(enable);
    showCallsCheck->setEnabled(enable);
}

void TriggerOptionsDialog::showEvent(QShowEvent *event) {
    if (!savedGeometry.isEmpty())
        restoreGeometry(savedGeometry);
}

void TriggerOptionsDialog::closeEvent(QCloseEvent *event) {
    savedGeometry = saveGeometry();
}
