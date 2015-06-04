#include <QHBoxLayout>
#include <QLabel>
#include "ConnectionPanel.hpp"

ConnectionPanel::ConnectionPanel(QString host, quint16 port) {
    hostEdit = new QLineEdit();
    hostEdit->setText(host);
    hostEdit->setPlaceholderText("host/ip");
    hostEdit->setFixedWidth(250);

    portEdit = new QSpinBox();
    portEdit->setRange(1, 65535);
    portEdit->setValue(port);

    connectBtn = new QPushButton();

    frameModeCheck = new QCheckBox("Tryb frame:");
    frameModeCheck->setLayoutDirection(Qt::RightToLeft);

    QLabel *hostLabel = new QLabel("Host:");
    QLabel *portLabel = new QLabel("Port:");

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignLeft);
    layout->addWidget(hostLabel);
    layout->addWidget(hostEdit);
    layout->addWidget(portLabel);
    layout->addWidget(portEdit);
    layout->addSpacerItem(new QSpacerItem(20, 0));
    layout->addWidget(connectBtn);
    layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    layout->addWidget(frameModeCheck);
    setLayout(layout);

    setConnectState(ConnectionState::Disconnected);

    connect(connectBtn, SIGNAL(clicked()),
            this, SLOT(handleConnectBtn()));

    connect(frameModeCheck, SIGNAL(stateChanged(int)),
            this, SLOT(handleFrameModeStateChanged(int)));
}

void ConnectionPanel::handleConnectBtn() {
    if (connectionState == Disconnected) {
        QString host = hostEdit->text();
        quint16 port = (quint16) portEdit->value();
        emit doConnect(host, port);
    } else if (connectionState == Connected){
        emit doDisconnect();
    } else {
        // Nic
    }
}


void ConnectionPanel::setConnectState(QAbstractSocket::SocketState state) {
    if (state == QAbstractSocket::ConnectedState)
        setConnectState(Connected);
    else if (state == QAbstractSocket::UnconnectedState)
        setConnectState(Disconnected);
    else if (state == QAbstractSocket::ConnectingState)
        setConnectState(Connecting);
    else
        ; // Ignorowanie reszty stanów
}

void ConnectionPanel::setConnectState(ConnectionState state) {
    switch (state) {
        case Connected:
            connectBtn->setText("Rozłącz");
            connectBtn->setEnabled(true);
            break;
        case Disconnected:
            connectBtn->setText("Połącz");
            connectBtn->setEnabled(true);
            break;
        case Connecting:
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

