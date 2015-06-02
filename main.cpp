#include <QApplication>
#include "MainWindow.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.setFixedSize(700, 500);
    window.show();

    return app.exec();
}