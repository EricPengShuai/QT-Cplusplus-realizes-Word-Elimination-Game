#ifndef BATTLE_H
#define BATTLE_H
#include "player.h"
#include "qtcpsocket.h"
#include <QTimer>
#include <QTime>
#include <QMainWindow>
#include <QMessageBox>

extern Player player;
extern QTcpSocket *tcpsocket;
extern QString words;

namespace Ui {
class Battle;
}

class Battle : public QMainWindow
{
    Q_OBJECT

public:
    explicit Battle(QString plA, QString plB, QWidget *parent = 0);
    ~Battle();
    void make_word();   //产生单词

private slots:
    void on_ready_clicked();    //准备就绪按钮
    void on_submit_clicked();   //答题按钮
    void handleout();   //单词出现和消失的槽函数
    //信号槽机制
    void ready();   //准备就绪
    void victory(); //胜利
    void fail();    //失败
    void quit_info();   //退出

    void on_Quit_clicked(); //结束battle

signals:
    void exit_signal();//结束battle信号

private:
    Ui::Battle *ui;
    QTime *Time;    //battle过程计时
    QTimer *m_pTimer;   //battle过程控制单词消失和出现
    int t;  //答出单词用时
};

#endif // BATTLE_H
