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

    setConnectionState(Connection::Disconnected);

    connect(connectBtn, SIGNAL(clicked()),
            this, SLOT(handleConnectBtn()));

    connect(frameModeCheck, SIGNAL(stateChanged(int)),
            this, SLOT(handleFrameModeStateChanged(int)));
}

void ConnectionPanel::handleConnectBtn() {
    if (connectionState == Connection::Disconnected) {
        QString host = hostEdit->text();
        quint16 port = (quint16) portEdit->value();

        Connection::Params params;
        params.append(host.toUtf8());
        params.append(QString::number(port).toUtf8());
        emit doConnect(params);
    } else if (connectionState == Connection::Connected){
        emit doDisconnect();
    } else {
        // Nic
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

