#ifndef MYSQL_H
#define MYSQL_H

#include <QObject>
#include <QtSql>
#include <QSqlDatabase>

class Mysql: public QObject
{
    Q_OBJECT
public:
    Mysql();
    ~Mysql();
    //玩家
    QString get_player_entry(QString name);
    QString get_player();
    bool same_name_player(QString name);
    void update_player_island(QString, QString);
    void update_player(QString, QString, QString, QString);
    void insert_player(QString, QString);
    bool player_is_land(QString);
    //出题者
    QString get_tester_entry(QString name);
    QString get_tester();
    bool same_name_tester(QString name);
    void update_tester_island(QString, QString);
    void update_tester(QString, QString, QString);
    void insert_tester(QString, QString);

    //单词
    QString get_word_asc();
    bool same_word(QString word);
private:
    QSqlDatabase db;
};

#endif // MYSQL_H
