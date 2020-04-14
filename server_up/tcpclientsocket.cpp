#include "tcpclientsocket.h"

TcpClientSocket::TcpClientSocket(QObject *parent)
    :QTcpSocket(parent)
{
    //客户端发送数据过来就会触发readyRead信号
    connect(this, &TcpClientSocket::readyRead, this, &TcpClientSocket::receive_data);

    //客户端断开连接出发disconnected信号
    connect(this, &TcpClientSocket::disconnected, this, &TcpClientSocket::slotclient_disconnected);
}

void TcpClientSocket::receive_data()
{
    QByteArray array = readAll();
    QString msg = array;
    //发送信号给界面，让界面显示登录者信息
    emit update_server(msg,this->socketDescriptor());
}

void TcpClientSocket::slotclient_disconnected()
{
    //发送断开连接信号
    emit client_disconnected(this->socketDescriptor());
}
