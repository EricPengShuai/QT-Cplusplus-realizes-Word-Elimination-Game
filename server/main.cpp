#include "mainwindow.h"
#include <QApplication>
#include <mysql.h>

QList<MySocket *> mysocketlist;
QList<QString> gamelist;
QList<qintptr> labellist;

Mysql sql;  //所有数据文件

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle("Fusion");
    MainWindow w;
    w.show();
    return a.exec();
}
