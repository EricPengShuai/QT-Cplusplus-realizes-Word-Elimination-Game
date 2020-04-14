#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QTcpSocket>
#include <QMainWindow>
#include <QMessageBox>
#include <QFile>
#include <QTableWidget>
#include <QTime>
#include <QTimer>
#include <QString>
#include "player.h"
#include "tester.h"
#include "battle.h"

extern Player player;
extern Tester tester;
extern QTcpSocket *tcpsocket;
extern QString words;
extern QString all_data;
extern QString battle_str;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void Mytimer();
    void ScreenInfo(QTableWidget *source, QString text);

public slots:
    void handleTimeout();//处理单词定时消失槽

private slots:
    void on_submit_clicked();
    void on_Quit_clicked();
    void on_pushButton_player_clicked();//查询玩家信息
    void on_pushButton_tester_clicked();//查询出题者信息
    void on_pushButton_index_clicked();//索引查询

    void on_battle_clicked();  //对战按钮
    void accept_battle(QString);    //接收对战槽
    void make_battle(QString);  //发起对战槽
    void cannot_battle();   //不可对战槽
    void ready_make();  //准备对战槽
    void victory_make();    //对战获胜槽
    void fail_make();   //对战失败槽
    void quit_make();   //退出对战槽

signals:
    void ready_signal();    //准备信号
    void v_signal();    //对战获胜信号
    void f_signal();    //对战失败信号
    void quit_signal(); //对战退出信号

private:
    Ui::MainWindow *ui;
    QTimer *m_pTimer1;
    QTime *m_pTime;
    QStringList strlist_word;//存放每个单词
    QString ans_word;
    QTableWidget *table;
    Battle *battle;
};

#endif // MAINWINDOW_H
