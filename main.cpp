#include <QApplication>
#include "MainWindow.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.setGeometry(0, 0, 720, 480);
    window.show();

    return app.exec();
}