#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //国际化
    QTranslator translator;
    if (QLocale::system().name() != "zh_CN")
    {
        translator.load("en_US.qm");
        a.installTranslator(&translator);
    }

    MainWindow w;

//    w.show(); //在构造函数中调用

    return a.exec();
}
