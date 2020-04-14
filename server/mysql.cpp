#include "mysql.h"
#include <QMessageBox>

Mysql::Mysql()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("pengshuai");//这里输入你的数据库名
    db.setUserName("root");
    db.setPassword("mysql123456");   //这里输入你的密码
    if (!db.open()) {
        QMessageBox::critical(0, QObject::tr("无法打开数据库"),"无法创建数据库连接", QMessageBox::Cancel);
    }
}

Mysql::~Mysql()
{
    db.close();
    this->deleteLater();
}
//获取某行玩家信息
QString Mysql::get_player_entry(QString name)
{
    QSqlQuery query(db);
    query.exec(QString("select * from player where name = '%1'").arg(name));
    query.seek(0);
    QString str = query.value(1).toString() +' '+ query.value(2).toString()
                +' '+ query.value(3).toString() +' '+ query.value(4).toString()
                +' '+ query.value(5).toString();
    return str;
}
QString Mysql::get_player()
{
    QString s = "";
    QSqlQuery query(db);
    query.exec("select * from player");
    while(query.next())
    {
       s += query.value(0).toString() +' '+ query.value(2).toString()
           +' '+ query.value(3).toString() +' '+ query.value(4).toString()
           +' '+ query.value(5).toString() +'\n';
    }
    return s;
}
QString Mysql::get_tester()
{
    QString s = "";
    QSqlQuery query(db);
    query.exec("select * from tester");
    while(query.next())
    {
       s += query.value(0).toString() +' '+ query.value(2).toString()
           +' '+ query.value(3).toString() +' '+ query.value(4).toString() +'\n';
    }
    return s;
}
//是否已经登录--这个东西太鸡肋了
bool Mysql::same_name_player(QString name)
{
    QSqlQuery query(db);
    query.exec(QString("select count(*) from player where name = '%1'").arg(name));
    query.seek(0);
    if(query.value(0).toInt() == 0) return false;
    else return true;
}
//更新玩家登录信息
void Mysql::update_player_island(QString name, QString land)
{
    QSqlQuery query(db);
    query.exec(QString("update player set is_land = '%1' where name = '%2'").arg(land).arg(name));
}
void Mysql::update_player(QString name, QString level, QString exp, QString counted)
{
    QSqlQuery query(db);
    query.exec(QString("update player set level = '%1' where name = '%2'").arg(level).arg(name));
    query.exec(QString("update player set exp = '%1' where name = '%2'").arg(exp).arg(name));
    query.exec(QString("update player set counted = '%1' where name = '%2'").arg(counted).arg(name));
}
//添加玩家信息
void Mysql::insert_player(QString name, QString code)
{
    QSqlQuery query;
    query.exec(QString("insert into player values('%1', '%2', '0', '0', '0', '0')").arg(name).arg(code));
}

//某行出题者信息
QString Mysql::get_tester_entry(QString name)
{
    QSqlQuery query(db);
    query.exec(QString("select * from tester where name = '%1'").arg(name));
    query.seek(0);
    QString str = query.value(1).toString() +' '+ query.value(2).toString()
                +' '+ query.value(3).toString() +' '+ query.value(4).toString();
    return str;
}
bool Mysql::same_name_tester(QString name)
{
    QSqlQuery query(db);
    query.exec(QString("select count(*) from tester where name = '%1'").arg(name));
    query.seek(0);
    if(query.value(0).toInt() == 0) return false;
    else return true;
}
//更新出题者登录信息
void Mysql::update_tester_island(QString name, QString land)
{
    QSqlQuery query;
    query.exec(QString("update tester set is_land = '%1' where name = '%2'").arg(land).arg(name));
}
void Mysql::update_tester(QString name, QString level, QString counted)
{
    QSqlQuery query(db);
    query.exec(QString("update tester set level = '%1' where name = '%2'").arg(level).arg(name));
    query.exec(QString("update tester set counted = '%1' where name = '%2'").arg(counted).arg(name));
}
//添加出题者信息
void Mysql::insert_tester(QString name, QString code)
{
    QSqlQuery query;
    query.exec(QString("insert into tester values('%1', '%2', '0', '0', '0')").arg(name).arg(code));
}

//单词
QString Mysql::get_word_asc()
{
    QString s = "";
    QSqlQuery query;
    query.exec("select WORD from words order by LENGTH(WORD) asc");
    while(query.next())
    {
        s += query.value(0).toString() + ' ';
    }
    return s;
}

bool Mysql::same_word(QString word)
{
    QSqlQuery query(db);
    query.exec(QString("select count(*) from words where WORD = '%1'").arg(word));
    query.seek(0);
    if(query.value(0).toInt() == 0)
    {
        query.exec(QString("insert into words value('%1')").arg(word)); //不存在就直接更新了
        return true;
    }
    else
        return false; //出现相同单词
}
