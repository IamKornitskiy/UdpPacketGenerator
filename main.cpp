#include "MainWindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile styleFile("://style/style.qss"); // путь из ресурсов
    if (styleFile.open(QFile::ReadOnly)) {
        a.setStyleSheet(styleFile.readAll());
        styleFile.close();
        qDebug() << "ok";
    }

    MainWindow w;
    w.show();
    return a.exec();
}
