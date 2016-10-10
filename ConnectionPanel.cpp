#include <QHBoxLayout>
#include <QLabel>
#include <QtSerialPort/QSerialPortInfo>
#include "ConnectionPanel.hpp"
#include "NetworkConnection.hpp"
#include "SerialPortConnection.hpp"
#include "EmulatorConnection.hpp"

ConnectionPanel::ConnectionPanel() {
    typeSelect = new QComboBox();
    typeSelect->addItem("Sieć", Type::Network);
    typeSelect->addItem("Port szeregowy", Type::SerialPort);
    typeSelect->addItem("Emulator", Type::Emulator);
    typeSelect->setCurrentIndex(0);

    connectBtn = new QPushButton();

    contentLayout = new QHBoxLayout();
    mainLayout = new QHBoxLayout();
    mainLayout->setAlignment(Qt::AlignLeft);
    mainLayout->addWidget(typeSelect);
    mainLayout->addLayout(contentLayout);
    mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    setLayout(mainLayout);

    setupParamsLayout(Type::Network);
    this->setConnectionState(Connection::Disconnected);

    connect(connectBtn, SIGNAL(clicked()),
            this, SLOT(handleConnectBtn()));

    connect(typeSelect, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleTypeChanged(int)));
}


void ConnectionPanel::handleTypeChanged(int index) {
    QVariant data = typeSelect->itemData(index);
    Type type = (Type) data.toInt();
    this->currentType = type;
    setupParamsLayout(type);
}

void ConnectionPanel::setupParamsLayout(Type type) {
    clearContentLayout();
    delete contentLayout;

    contentLayout = new QHBoxLayout();
    mainLayout->insertLayout(1, contentLayout);

    switch (type) {
        case Network:
            setupNetworkLayout(contentLayout);
            break;
        case SerialPort:
            setupSerialLayout(contentLayout);
            break;
        case Emulator:
            setupEmulatorLayout(contentLayout);
            break;
        default: break; // Nic, pusty layout
    }
}


void ConnectionPanel::clearContentLayout() {
    QLayoutItem *item;
    while ((item = contentLayout->takeAt(0)) != 0) {
        if (item->widget() == connectBtn) {
            contentLayout->removeWidget(connectBtn);
            connectBtn->setParent(nullptr);
            continue; // Usuń tylko z layoutu, samą instancję należy pozostawić
        }

        delete item->layout();
        delete item->widget();
        delete item;
    }
}

void ConnectionPanel::setupNetworkLayout(QHBoxLayout *layout) {
    network.hostLabel = new QLabel("Host:");
    network.portLabel = new QLabel("Port:");

    network.hostEdit = new QLineEdit();
    network.hostEdit->setText(DEFAULT_NETWORK_HOST);

    network.portEdit = new QSpinBox();
    network.portEdit->setRange(1, 65535);
    network.portEdit->setValue(DEFAULT_NETWORK_PORT);

    layout->addWidget(network.hostLabel);
    layout->addWidget(network.hostEdit);
    layout->addWidget(network.portLabel);
    layout->addWidget(network.portEdit);
    layout->addWidget(connectBtn);
}

void ConnectionPanel::setupSerialLayout(QHBoxLayout *layout) {
    serial.portNameLabel = new QLabel("Port:");
    serial.baudRateLabel = new QLabel("Prędkość:");

    serial.portNameSelect = new QComboBox();
    for (auto portInfo : QSerialPortInfo::availablePorts()) {
        QString text = portInfo.systemLocation() + " - " + portInfo.description();
        serial.portNameSelect->addItem(text, portInfo.portName());

        if (portInfo.portName() == DEFAULT_SERIAL_PORT_NAME) {
            int index= serial.portNameSelect->count();
            serial.portNameSelect->setCurrentIndex(index);
        }
    }

    if (serial.portNameSelect->currentIndex() == -1)
        serial.portNameSelect->setCurrentIndex(0);

    serial.baudRateSelect = new QComboBox();
    for (auto baudRate : QSerialPortInfo::standardBaudRates()) {
        QString text = QString::number(baudRate);
        qint32 data = baudRate;
        serial.baudRateSelect->addItem(text, data);

        if (baudRate == DEFAULT_SERIAL_BAUD_RATE) {
            int index= serial.baudRateSelect->count() - 1;
            serial.baudRateSelect->setCurrentIndex(index);
        }
    }

    layout->addWidget(serial.portNameLabel);
    layout->addWidget(serial.portNameSelect);
    layout->addWidget(serial.baudRateLabel);
    layout->addWidget(serial.baudRateSelect);
    layout->addWidget(connectBtn);
}


void ConnectionPanel::setupEmulatorLayout(QHBoxLayout *layout) {
    layout->addWidget(new QLabel("tryb emulatora"));
    layout->addWidget(connectBtn);
}

void ConnectionPanel::handleConnectBtn() {
    if (connectionState == Connection::Disconnected)
        sendDoConnect();
    else if (connectionState == Connection::Connected)
        emit doDisconnect();
    else
        ; // Nic
}


void ConnectionPanel::sendDoConnect() {
    switch (currentType) {
        case Network:
            sendDoConnectNetwork();
            break;
        case SerialPort:
            sendDoConnectSerialPort();
            break;
        case Emulator:
            sendDoConnectEmulator();
            break;
    }
}


void ConnectionPanel::sendDoConnectNetwork() {
    QString host = network.hostEdit->text();
    quint16 port = (quint16) network.portEdit->value();

    NetworkConnection *connection = new NetworkConnection();
    connection->setServerHost(host);
    connection->setServerPort(port);
    emit connectionChanged(connection);

    emit doConnect();
}

void ConnectionPanel::sendDoConnectSerialPort() {
    QString portName = serial.portNameSelect->currentData().toString();
    qint32 baudRate = serial.baudRateSelect->currentData().toInt();

    SerialPortConnection *connection = new SerialPortConnection();
    connection->setPortName(portName);
    connection->setBaudRate(baudRate);
    emit connectionChanged(connection);

    emit doConnect();
}


void ConnectionPanel::sendDoConnectEmulator() {
    EmulatorConnection *connection = new EmulatorConnection();
    emit connectionChanged(connection);

    emit doConnect();
}

void ConnectionPanel::setConnectionState(Connection::State state) {
    connectionState = state;
    switch (state) {
        case Connection::Connected:
            connectBtn->setText("Rozłącz");
            connectBtn->setEnabled(true);
            break;
        case Connection::Disconnected:
            connectBtn->setText("Połącz");
            connectBtn->setEnabled(true);
            break;
        case Connection::Connecting:
            connectBtn->setText("Połącz");
            connectBtn->setEnabled(false);
            break;
    }
}
