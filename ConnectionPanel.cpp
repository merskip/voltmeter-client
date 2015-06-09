#include <QHBoxLayout>
#include <QLabel>
#include "ConnectionPanel.hpp"

ConnectionPanel::ConnectionPanel() {
    typeSelect = new QComboBox();
    typeSelect->addItem("Sieć", Type::Network);
    typeSelect->addItem("Port szeregowy", Type::SerialPort);
    typeSelect->addItem("Emulator", Type::Emulator);
    typeSelect->setCurrentIndex(0);

    connectBtn = new QPushButton();

    frameModeCheck = new QCheckBox("Tryb frame:");
    frameModeCheck->setLayoutDirection(Qt::RightToLeft);

    contentLayout = new QHBoxLayout();
    mainLayout = new QHBoxLayout();
    mainLayout->setAlignment(Qt::AlignLeft);
    mainLayout->addWidget(typeSelect);
    mainLayout->addLayout(contentLayout);
    mainLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    mainLayout->addWidget(frameModeCheck);
    setLayout(mainLayout);

    setupParamsLayout(Type::Network);
    setConnectionState(Connection::Disconnected);

    connect(connectBtn, SIGNAL(clicked()),
            this, SLOT(handleConnectBtn()));

    connect(frameModeCheck, SIGNAL(stateChanged(int)),
            this, SLOT(handleFrameModeStateChanged(int)));

    connect(typeSelect, SIGNAL(currentIndexChanged(int)),
            this, SLOT(handleTypeChanged(int)));
}


void ConnectionPanel::handleTypeChanged(int index) {
    QVariant data = typeSelect->itemData(index);
    Type type = (Type) data.toInt();
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
    network.hostEdit->setPlaceholderText("host/ip");
    network.hostEdit->setMaximumWidth(250);

    network.portEdit = new QSpinBox();
    network.portEdit->setRange(1, 65535);
    network.portEdit->setValue(DEFAULT_NETWORK_PORT);

    layout->addWidget(network.hostLabel);
    layout->addWidget(network.hostEdit);
    layout->addWidget(network.portLabel);
    layout->addWidget(network.portEdit);
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
    Type type = typeSelect->currentData().value<Type>();
    if (type == Type::Network) {
        QString host = network.hostEdit->text();
        quint16 port = (quint16) network.portEdit->value();

        Connection::Params params;
        params.append(host.toUtf8());
        params.append(QString::number(port).toUtf8());
        emit doConnect(params);
    } else {
        throw QString("Not implemented yet");
    }
}

void ConnectionPanel::setConnectionState(Connection::State state) {
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
    connectionState = state;
}

bool ConnectionPanel::isFrameMode() {
    return frameModeCheck->isChecked();
}

void ConnectionPanel::handleFrameModeStateChanged(int state) {
    emit frameModeChanged((bool) state);
}

