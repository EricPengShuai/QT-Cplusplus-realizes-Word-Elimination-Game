#include "logindialog.h"
#include "ui_logindialog.h"

#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    serverIP = new QHostAddress();
    tcpsocket = NULL;
    flag_login = 0;//哪种类型的登录 1-玩家，2-出题者
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::link_error()
{
    QMessageBox::warning(this,"Error","无法连接到服务器");
    tcpsocket = NULL;
    return;
}

//玩家登录按钮
void LoginDialog::on_loginBtn_play_clicked()
{
    if(tcpsocket == NULL)
    {
        QString ip = "127.0.0.1";
        int port = 8888;
        if(!serverIP->setAddress(ip))//判断是否可以被正确解析
        {
            QMessageBox::warning(this,"Error","无法解析此IP");
            return;
        }
        //创建一个通信套接字，用来和服务器通信
        tcpsocket = new QTcpSocket(this);
        //和服务器进行连接
        connect(tcpsocket, &QTcpSocket::connected, this, &LoginDialog::play_connected_success);
        connect(tcpsocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(link_error()));
        tcpsocket->connectToHost(*serverIP,port);
        //和服务器连接成功就会出发connected信号
        status = 1;  //登录标志
        //接收到服务器的信息就会出发readyRead信号
        connect(tcpsocket, &QTcpSocket::readyRead, this, &LoginDialog::slot_play_receive);
    }
    else    //第一次密码不对，第二次点击
    {
        if(status == 3 || status == 4)  //避免角色混乱
        {
            QMessageBox::warning(this,"Information","由于之前已经进行了某类操作，所以在未完成之前无法进行下一类操作！");
            return;
        }
        status = 1;
        play_connected_success();
    }
}
//玩家注册按钮
void LoginDialog::on_register_play_clicked()
{
    if(tcpsocket == NULL)
    {
        QString ip = "127.0.0.1";
        int port = 8888;
        if(!serverIP->setAddress(ip))//判断是否可以被正确解析
        {
            QMessageBox::warning(this,"Error","无法解析此IP");
            return;
        }
        //创建一个通信套接字，用来和服务器通信
        tcpsocket = new QTcpSocket(this);
        //和服务器进行连接
        connect(tcpsocket,&QTcpSocket::connected,this,&LoginDialog::play_connected_success);
        connect(tcpsocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(link_error()));
        tcpsocket->connectToHost(*serverIP,port);
        status = 2; //注册标志
        //接收到服务器的信息就会出发readyRead信号
        connect(tcpsocket,&QTcpSocket::readyRead,this,&LoginDialog::slot_play_receive);
    }
    else
    {
        if(status == 3 || status == 4)  //避免多角色混乱
        {
            QMessageBox::warning(this,"Information","由于之前已经进行了某类操作，所以在未完成之前无法进行下一类操作！");
            return;
        }
        status = 2;
        play_connected_success();
    }
}

//玩家连接成功
void LoginDialog::play_connected_success()
{
    QString msg ;
    if(status == 1)
       msg = "player_login "+ ui->usrLineEdit->text() +' '+ ui->pwdLineEdit->text();
    else if(status == 2)
       msg = "player_register "+ ui->usrLineEdit->text() +' '+ ui->pwdLineEdit->text();
    //将登录消息发送给服务器
    tcpsocket->write(msg.toUtf8().data());
}
//读取玩家反馈信息
void LoginDialog::slot_play_receive()
{
    QString array = tcpsocket->readAll();
    //qDebug() << array;
    QStringList str_info = array.split(' ');
    //登录反馈信息******************************
    if(str_info.at(0) == "login_back")
    {
        if(str_info.at(1) == "00")
            QMessageBox::critical(NULL, "提示", "该用户已经登录");
        else if(str_info.at(1) == "01")
        {
            player.name = ui->usrLineEdit->text();
            player.code = ui->pwdLineEdit->text();
            player.level = (str_info.at(2)).toInt();
            player.exp = (str_info.at(3)).toInt();
            player.counted = (str_info.at(4)).toInt();
            flag_login = 1; //玩家登录
            accept();
        }
        else if(str_info.at(1) == "10")
            QMessageBox::critical(NULL, "提示", "密码错误");
        else if(str_info.at(1) == "11")
            QMessageBox::critical(NULL, "提示", "用户名不存在");
    }
    //注册反馈信息******************************
    else if(str_info.at(0) == "register_back")
    {
        if(str_info.at(1) == "0")
            QMessageBox::information(this, tr("Tips"),tr("用户名已存在"));
        else if(str_info.at(1) == "1")
            QMessageBox::information(this, tr("Tips"),tr("Register success!"));
    }
    //单词反馈信息******************************
    else if(str_info.at(0) == "word_back")
    {
        words = array.section(' ',1);
    }
    //查询玩家反馈信息**************************
    else if(str_info.at(0) == "check_play_back")
    {
        all_data = array.section(' ', 1);
    }
    //查询出题者反馈信息************************
    else if(str_info.at(0) == "check_test_back")
    {
        all_data = array.section(' ', 1);
    }

    else if(str_info.at(0) == "can't")
    {
        emit cannot();
    }
    else if(str_info.at(0) == "ready_back")
    {
        emit ready_signal();
    }
    else if(str_info.at(0) == "battle_back")
    {
        if(str_info.at(1) == "victory")
            emit victory_signal();
        else
            emit fail_signal();
    }
    else if(str_info.at(0) == "error")
    {
        emit quit();
    }
    //读取玩家battle反馈信息********************
    else if(str_info.at(2) == "battling")
    {
        emit emit_battle(array);
    }
    else if(str_info.at(2) == "accepting")
    {
        emit battle_signal(array);
    }
}

//出题者登录按钮
void LoginDialog::on_loginBtn_test_clicked()
{
    if(tcpsocket==NULL)
    {
        QString ip = "127.0.0.1";
        int port = 8888;
        if(!serverIP->setAddress(ip))//判断是否可以被正确解析
        {
            QMessageBox::warning(this,"Error","无法连接到服务器");
            return;
        }
        //创建一个通信套接字，用来和服务器通信
        tcpsocket = new QTcpSocket(this);
        //和服务器连接成功就会出发connected信号
        connect(tcpsocket,&QTcpSocket::connected,this,&LoginDialog::test_connected_success);
        connect(tcpsocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(link_error()));
        tcpsocket->connectToHost(*serverIP,port);
        status = 3;  //登录标志
        //接收到服务器的信息就会出发readyRead信号
        connect(tcpsocket,&QTcpSocket::readyRead,this,&LoginDialog::slot_test_receive);
    }
    else
    {
        if(status == 1 || status == 2)
        {
            QMessageBox::warning(this,"Information","由于之前已经进行了某类操作，所以在未完成之前无法进行下一类操作！");
            return;
        }
        status = 3;
        test_connected_success();
    }
}
//出题者注册按钮
void LoginDialog::on_register_test_clicked()
{
    if(tcpsocket == NULL)
    {
        QString ip = "127.0.0.1";
        int port = 8888;
        if(!serverIP->setAddress(ip))//判断是否可以被正确解析
        {
            QMessageBox::warning(this,"Error","无法连接到服务器");
            return;
        }
        //创建一个通信套接字，用来和服务器通信
        tcpsocket = new QTcpSocket(this);
        //和服务器进行连接
        connect(tcpsocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(link_error()));
        connect(tcpsocket, &QTcpSocket::connected, this, &LoginDialog::test_connected_success);
        tcpsocket->connectToHost(*serverIP,port);
        //接收到服务器的信息就会出发readyRead信号
        connect(tcpsocket,&QTcpSocket::readyRead,this,&LoginDialog::slot_test_receive);
        status = 4;
    }
    else
    {
        if(status == 1 || status == 2)
        {
            QMessageBox::warning(this,"Information","由于之前已经进行了某类操作，所以在未完成之前无法进行下一类操作！");
            return;
        }
        status = 4;
        test_connected_success();
    }
}

//出题者连接成功
void LoginDialog::test_connected_success()
{
    QString msg;
    if(status == 3)
       msg = "tester_login "+ ui->usrLineEdit->text() +' '+ ui->pwdLineEdit->text();
    else if(status == 4)
       msg = "tester_register "+ ui->usrLineEdit->text() +' '+ ui->pwdLineEdit->text();
    //将登录消息发送给服务器
    tcpsocket->write(msg.toUtf8().data());
}
//读取出题者反馈信息
void LoginDialog::slot_test_receive()
{
    QString array = tcpsocket->readAll();
    QStringList str_info = array.split(' ');
    //登录反馈信息******************************
    if(str_info.at(0) == "login_back")
    {
        if(str_info.at(1) == "00")
            QMessageBox::critical(NULL, "提示", "该用户已经登录");
        else if(str_info.at(1) == "01")
        {
            tester.name = ui->usrLineEdit->text();
            tester.code = ui->pwdLineEdit->text();
            tester.level = (str_info.at(2)).toInt();
            tester.counted = (str_info.at(3)).toInt();
            flag_login = 2; //玩家登录
            accept();
        }
        else if(str_info.at(1) == "10")
            QMessageBox::critical(NULL, "提示", "密码错误");
        else if(str_info.at(1) == "11")
            QMessageBox::critical(NULL, "提示", "用户名不存在");
    }
    //注册反馈信息******************************
    else if(str_info.at(0) == "register_back")
    {
        if(str_info.at(1) == "0")
            QMessageBox::information(this, tr("Tips"),tr("用户名已存在"));
        else if(str_info.at(1) == "1")
            QMessageBox::information(this, tr("Tips"),tr("Register success!"));
    }
    //提交单词反馈信息***************************
    else if(str_info.at(0) == "submit_back")
    {
        submit = str_info.at(1).toInt(0);
    }
    //查询玩家反馈信息**************************
    else if(str_info.at(0) == "check_play_back")
    {
        all_data = array.section(' ', 1);
    }
    //查询出题者反馈信息************************
    else if(str_info.at(0) == "check_test_back")
    {
        all_data = array.section(' ', 1);
    }
}
