#include "main_window.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile styleFile("://style/dark.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        a.setStyleSheet(styleFile.readAll());
        styleFile.close();
    }

    a.setWindowIcon(QIcon("://style/logo.svg"));

    MainWindow w;
    w.show();
    return a.exec();
}
