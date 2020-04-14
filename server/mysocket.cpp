#include "mysocket.h"
#include <QMessageBox>
#include <QThread>
#include <QDebug>

extern QList<MySocket *> mysocketlist;
extern QList<QString> gamelist;
extern QList<qintptr> labellist;
extern Mysql sql;

MySocket::MySocket(QWidget *parent, qintptr socket) : QTcpSocket(parent)
{
    //指定该套接字的描述符
    this->setSocketDescriptor(socket);    

    connect(this, SIGNAL(readyRead()), this, SLOT(on_connected()));
    qDebug() << QThread::currentThread()<<"socket";
}

void MySocket::on_connected()
{
    //读到数据的操作
    QString msg = this->readAll();
    emit update_serve(msg,this->socketDescriptor());
}

void MySocket::slot_disconnect()
{
    for(int i = 0; i < mysocketlist.count(); i++)
    {
        MySocket *item = mysocketlist.at(i);
        if(item->socketDescriptor() == this->socketDescriptor())
        {
            mysocketlist.removeAt(i);
            labellist.removeAt(i);
            gamelist.removeAt(i);
        }
    }
}

void MySocket::slot_update(QString msg, qintptr descriptor)
{
    qDebug() << msg ;
    QStringList str_info = msg.split(' ');
    //处理玩家登录信息********************
    if(str_info.at(0) == "player_login")
    {
        QString s = "login_back ";
        if(sql.get_player_entry(str_info.at(1)) == "    ")
        {
            s += "11";  //11--用户名不存在
            this->write(s.toUtf8().data()); //写入管道
            return;
        }
        QStringList entry = sql.get_player_entry(str_info.at(1)).split(' ');
        if(str_info.at(2) == entry.at(0))
        {
            if(entry.at(4) == "1")
                s += "00";  //00--已经登录
            else    //01--登录授权
            {
                mysocketlist.append(this);//登录成功就加入玩家列表
                labellist.append(descriptor);
                gamelist.append(str_info.at(1));
                qDebug() << mysocketlist.count();
                s += "01 " + entry.at(1) + ' ' + entry.at(2) + ' ' + entry.at(3);
                sql.update_player_island(str_info.at(1), "1");  //登录状态
            }
        }
        else
            s += "10";  //10--用户名或密码错误
        this->write(s.toUtf8().data()); //写入管道
    }
    //处理玩家注册信息*****************
    else if(str_info.at(0) == "player_register")
    {
        QString s = "register_back ";
        if(sql.same_name_player(str_info.at(1)) == true)    //0--出现重名
            s += "0";
        else
        {
            s += "1";  //1--成功注册
            sql.insert_player(str_info.at(1), str_info.at(2));
        }
        this->write(s.toUtf8().data()); //写入管道
    }
    //处理出题者登录信息********************
    else if(str_info.at(0) == "tester_login")
    {
        QString s = "login_back ";
        if(sql.get_tester_entry(str_info.at(1)) == "    ")
        {
            s += "11";  //11--用户名不存在
            this->write(s.toUtf8().data()); //写入管道
            return;
        }
        QStringList entry = sql.get_tester_entry(str_info.at(1)).split(' ');
        if(str_info.at(2) == entry.at(0))
        {
            if(entry.at(3) == "1")
                s += "00";  //00--已经登录
            else    //01--登录授权
            {
                s += "01 " + entry.at(1) + ' ' + entry.at(2);
                sql.update_tester_island(str_info.at(1), "1");  //登录状态
            }
        }
        else
            s += "10";  //10--用户名或密码错误
        this->write(s.toUtf8().data()); //写入管道
    }
    //处理出题者注册信息********************
    else if(str_info.at(0) == "tester_register")
    {
        QString s = "register_back ";
        if(sql.same_name_tester(str_info.at(1)) == true)    //0--出现重名
            s += "0";
        else
        {
            s += "1";  //1--成功注册
            sql.insert_tester(str_info.at(1), str_info.at(2));
        }
        this->write(s.toUtf8().data()); //写入管道
    }
    //处理单词请求********************
    else if(str_info.at(0) == "request_word")
    {
        QString s = "word_back ";
        s += sql.get_word_asc();
        this->write(s.toUtf8().data());
    }
    //处理更新玩家请求*****************
    else if(str_info.at(0) == "update_player")
    {
        sql.update_player(str_info.at(1), str_info.at(2), str_info.at(3), str_info.at(4));
    }
    //处理玩家退出或者游戏结束行为********************
    else if(str_info.at(0) == "player_quit")
    {
        sql.update_player_island(str_info.at(1), "0");
        slot_disconnect();  //移除相应的玩家列表
    }
    //查询玩家请求********************
    else if(str_info.at(0) == "check_player")
    {
        QString s = "check_play_back ";
        s += sql.get_player();
        this->write(s.toUtf8().data());
    }
    //查询出题者请求**********************
    else if(str_info.at(0) == "check_tester")
    {
        QString s = "check_test_back ";
        s += sql.get_tester();
        this->write(s.toUtf8().data());
    }
    //提交单词请求***********************
    else if(str_info.at(0) == "submit_word")
    {
        QString s = "submit_back ";
        if(sql.same_word(str_info.at(1)) == true)
            s += '1';   //1--出题成功，同时更新词库
        else
            s += '0';    //0--相同单词
        this->write(s.toUtf8().data());
    }
    //处理更新出题者请求*******************
    else if(str_info.at(0) == "update_tester")
    {
        sql.update_tester(str_info.at(1), str_info.at(2), str_info.at(3));
    }
    //处理出题者退游**********************
    else if(str_info.at(0) == "tester_quit")
    {
        sql.update_tester_island(str_info.at(1), "0");
    }

    //battle就绪************************
    /*else if(str_info.at(0) == "ready")
    {
        if(ready == 0)
        {
            ready ++;
            playerA = str_info.at(1);
        }
        else if(ready == 1)
        {
            ready = 0;
            playerB = str_info.at(1);
            QString msg = "ready_back ps";
            int i,j = -1;
            //先找到要battle的人
            for(i = 0; i < gamelist.count() ; i++)
            {
                if(gamelist.at(i) == playerA)
                {
                    j = i;
                    break;
                }
            }
            if(j == -1)
            {
                msg = "error";
                item->write(msg.toUtf8().data());
                return;
            }
            //找到对应的套接字
            for(i = 0;i < mysocketlist.count(); i++)
            {
                TcpClientSocket *item_battle = mysocketlist.at(i);
                if(item_battle->socketDescriptor() == labellist.at(j))
                {
                    //给那个被挑战者发送信息
                    item->write(msg.toUtf8().data());
                    item_battle->write(msg.toUtf8().data());
                    break;
                }
            }
        }
    }
    //battle结果*********************
    else if(str_info.at(0) == "battle_res")
    {
        if(bat_cnt == 0)
        {
            bat_cnt ++;
            playerA = str_info.at(1);
            A = str_info.at(2).toInt();
        }
        else if(bat_cnt == 1)
        {
            bat_cnt = 0;
            playerB = str_info.at(1);
            B = str_info.at(2).toInt();
            QString msg_vic = "battle_back victory";
            QString msg_fail = "battle_back fail";
            int i,j;
            //先找到要battle的人
            for(i = 0; i < gamelist.count() ; i++)
            {
                if(gamelist.at(i) == playerA)
                {
                    j = i;
                    break;
                }
            }
            //找到对应的套接字
            for(i = 0;i < mysocketlist.count(); i++)
            {
                TcpClientSocket *item_battle = mysocketlist.at(i);
                if(item_battle->socketDescriptor() == labellist.at(j))
                {
                    if(A > B)
                    {
                        item->write(msg_vic.toUtf8().data());
                        item_battle->write(msg_fail.toUtf8().data());
                    }
                    else if(A < B)
                    {
                        item->write(msg_fail.toUtf8().data());
                        item_battle->write(msg_vic.toUtf8().data());
                    }
                    else    //全错就都fail
                    {
                        item->write(msg_fail.toUtf8().data());
                        item_battle->write(msg_fail.toUtf8().data());
                    }
                    break;
                }
            }
        }
    }
    //有一个退出时
    else if(str_info.at(0) == "battle_quit")
    {
        int j = -1;
        for(int i = 0; i < gamelist.count(); i++)
        {
            if(gamelist.at(i) == str_info.at(2))
            {
                j = i;
                break;
            }
        }
        for(int i = 0;i < mysocketlist.count(); i++)
        {
            TcpClientSocket *item_battle = mysocketlist.at(i);
            if(item_battle->socketDescriptor() == labellist.at(j))
            {
                item_battle->write(QString("error quit").toUtf8().data());
                break;
            }
        }
        ready = 0;
        bat_cnt = 0;
        sql.update_player_island(str_info.at(1), "0");  //改battle玩家登录状态
        slot_disconnect(descriptor);  //移除相应的玩家列表
    }
    //battle提出************************
    else if(str_info.at(1) == "battle")
    {
        int i,j;
        //先判断这个人是否登录或者是否存在
        if(sql.player_is_land(str_info.at(2)) == false
          || sql.get_player_entry(str_info.at(2)) == "    ")  //如果用户已经登录或者用户不存在
        {
            item->write(QString("can't battle").toUtf8().data());
            return;
        }
        //先找到要battle的人
        for(i = 0; i < gamelist.count() ; i++)
        {
            if(gamelist.at(i) == str_info.at(2))
            {
                j = i;
                break;
            }
        }
        //找到对应的套接字
        for(i = 0;i < mysocketlist.count(); i++)
        {
            TcpClientSocket *item_battle = mysocketlist.at(i);
            if(item_battle->socketDescriptor() == labellist.at(j))
            {
                qDebug() << i <<' '<< descriptor << ' ' << labellist.at(j);
                //给那个被挑战者发送信息
                QString msg = "Are you accepting "+ str_info.at(0) +" 's battle?";
                item->write(QString("You are battling " + gamelist.at(j)).toUtf8().data());
                item_battle->write(msg.toUtf8().data());
                break;
            }
        }
    }*/
}
