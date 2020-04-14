#ifndef TCPCLIENTSOCKET_H
#define TCPCLIENTSOCKET_H
#include <QTcpSocket>
#include <QString>

class TcpClientSocket:public QTcpSocket
{
    Q_OBJECT
public:
    TcpClientSocket(QObject *parent = 0);

protected slots:
    //更新客户端消息，处理readyread
    void receive_data();

    //客户端断开连接信号
    void slotclient_disconnected();


signals:
    //用来告诉tcpserver需要更新界面的显示
    void update_server(QString,int);

    //告诉server有客户端断开连接
    void client_disconnected(int);
};

#endif // TCPCLIENTSOCKET_H
