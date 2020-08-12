#include "mainwindow.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setApplicationName("ScreenTestKit");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("Axel_k");
    QApplication::setOrganizationDomain("https://github.com/3dproger");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
