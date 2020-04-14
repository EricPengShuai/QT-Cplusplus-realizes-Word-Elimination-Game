#ifndef MYSQL_H
#define MYSQL_H

#include <QObject>
#include <QtSql>
#include <QSqlDatabase>

class Mysql: public QObject
{
    Q_OBJECT
public:
    Mysql();    //连接数据库
    ~Mysql();
    //玩家
    QString get_player_entry(QString name);//获取某行玩家信息
    QString get_player();
    bool same_name_player(QString name);//是否已经登录
    void update_player_island(QString, QString);
    void update_player(QString, QString, QString, QString);//更新玩家登录信息
    void insert_player(QString, QString);//添加玩家信息
    //出题者
    QString get_tester_entry(QString name);//某行出题者信息
    QString get_tester();
    bool same_name_tester(QString name);
    void update_tester_island(QString, QString);//更新出题者登录信息
    void update_tester(QString, QString, QString);
    void insert_tester(QString, QString);//添加出题者信息
    //单词
    QString get_word_asc();//获取排列好了的单词
    bool same_word(QString word);
private:
    QSqlDatabase db;
};

#endif // MYSQL_H
