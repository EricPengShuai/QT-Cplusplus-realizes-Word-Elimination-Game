#include "mainwindow.h"
#include <QApplication>
#include "logindialog.h"
#include "mainwindow_test.h"

#include <QPalette>

Player player;
Tester tester;
QTcpSocket *tcpsocket = NULL;
QString words = "";
int flag_login = 0;
QString all_data = "";
int submit = -1;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle("Fusion");
    //背景刷成黑色,前景色设为白色
    //QPalette bgpal;
    //bgpal.setColor (QPalette::Background, QColor(255,250,205,255));
    //bgpal.setColor(QPalette::Background,QColor(0x00,0xff,0x00,0x00));
    //bgpal.setColor(QPalette::Foreground,QColor(204,204,204,255));

    MainWindow *play;
    MainWindow_test *test;

    LoginDialog login;
    //login.setPalette(bgpal);
    login.setWindowTitle("Login");
    if( login.exec() == QDialog::Accepted && flag_login == 1){
        play = new MainWindow();
        //信号槽实现battle的传送
        QObject::connect(&login, SIGNAL(battle_signal(QString)), play, SLOT(accept_battle(QString)));
        QObject::connect(&login, SIGNAL(emit_battle(QString)), play, SLOT(make_battle(QString)));
        QObject::connect(&login, SIGNAL(ready_signal()), play, SLOT(ready_make()));
        QObject::connect(&login, SIGNAL(victory_signal()), play, SLOT(victory_make()));
        QObject::connect(&login, SIGNAL(fail_signal()), play, SLOT(fail_make()));
        QObject::connect(&login, SIGNAL(quit()), play, SLOT(quit_make()));
        QObject::connect(&login, SIGNAL(cannot()), play, SLOT(cannot_battle()));
        play->show();
        play->Mytimer();
        return a.exec();
    } else if(flag_login == 2) {
        test = new MainWindow_test();
        test->setWindowTitle("Tester");
        test->show();
        return a.exec();
    }
    return 0;
}
