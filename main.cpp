#include <QApplication>
#include "MainWindow.hpp"

#define DEFAULT_SERVER_HOST "192.168.1.8"
#define DEFAULT_SERVER_PORT 22444

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window(DEFAULT_SERVER_HOST, DEFAULT_SERVER_PORT);
    window.setGeometry(0, 0, 720, 480);
    window.show();

    return app.exec();
}