#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QFile>
#include <QDebug>
#include <QString>
#include <QStringList>

#include <QHostAddress>
#include <QTcpSocket>
#include "player.h"
#include "tester.h"

extern Player player;
extern Tester tester;
extern QTcpSocket *tcpsocket;//交互通信的套间字
extern QString words;//单词信息
extern int flag_login;//哪类人登陆标志1-player 2-tester
extern QString all_data;//所有交互的信息
extern int submit;

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

private slots:
    void on_loginBtn_play_clicked();//玩家登录
    void on_register_play_clicked();//玩家注册
    void on_loginBtn_test_clicked();//出题者登录
    void on_register_test_clicked();//主题者注册
    void link_error();

    //服务器连接slot
    void play_connected_success();//玩家连接服务器
    void test_connected_success();//出题者连接到服务器
    void slot_play_receive();//玩家收到服务器消息
    void slot_test_receive();//出题者收到服务器消息

signals:
    void battle_signal(QString info);//收到对战信号
    void emit_battle(QString info);//发出对战信号
    void ready_signal();//对战就绪信号
    void victory_signal();//对战获胜信号
    void fail_signal();//对战失败信号
    void quit();//结束对战信号
    void cannot();//不可对战信号

private:
    Ui::LoginDialog *ui;
    int status;
    //服务器
    QHostAddress *serverIP;
};

#endif // LOGINDIALOG_H
