#ifndef MAINWINDOW_TEST_H
#define MAINWINDOW_TEST_H
#include "player.h"
#include "tester.h"
#include <QMainWindow>
#include <QTcpSocket>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QTableWidget>
#include <QTimer>

extern Player player;
extern Tester tester;
extern QTcpSocket *tcpsocket;
extern QString words;
extern int submit;
extern QString all_data;

namespace Ui {
class MainWindow_test;
}

class MainWindow_test : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow_test(QWidget *parent = 0);
    ~MainWindow_test();
    void ScreenInfo(QTableWidget *source, QString text);

private slots:
    void on_submit_clicked();//出题按钮
    void on_quit_clicked();//退游按钮
    void on_pushButton_player_clicked();//闯关者信息查询
    void on_pushButton_tester_clicked();//出题者信息查询
    void on_pushButton_index_clicked();//索引查询

private:
    Ui::MainWindow_test *ui;
    QStringList strlist_word;
    QTableWidget *table;
};

#endif // MAINWINDOW_TEST_H
