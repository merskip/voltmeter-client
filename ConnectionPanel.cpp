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
    stateLabel = new QLabel();

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
    layout->addWidget(stateLabel);
    layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    setLayout(layout);

    setConnectState(ConnectionState::Disconnected);

    connect(connectBtn, SIGNAL(clicked()),
            this, SLOT(handleConnectBtn()));
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
            stateLabel->setText("Połączono");
            connectBtn->setText("Rozłącz");
            connectBtn->setEnabled(true);
            break;
        case Disconnected:
            stateLabel->setText("Rozłączono");
            connectBtn->setText("Połącz");
            connectBtn->setEnabled(true);
            break;
        case Connecting:
            stateLabel->setText("Łączenie...");
            connectBtn->setText("Połącz");
            connectBtn->setEnabled(false);
            break;
    }
    connectionState = state;
}
