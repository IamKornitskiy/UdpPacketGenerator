#include "main_window.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setOrganizationName("UPG");
    QApplication::setOrganizationDomain("org");
    QApplication::setApplicationName("UdpPacketGenerator");
    a.setWindowIcon(QIcon("://style/logo.svg"));

    MainWindow w;
    w.show();
    return a.exec();
}
