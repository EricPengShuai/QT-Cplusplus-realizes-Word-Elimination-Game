#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QHostAddress>
#include <QList>
#include <QObject>
#include "tcpclientsocket.h"

class Server: public QTcpServer
{
    Q_OBJECT
public:
    Server(QObject *parent = 0,int port = 0);
    QList<TcpClientSocket*> tcpclientsocketlist;

protected:
    //只要出现新的连接，就自动调用这个函数
    void incomingConnection(int socketDescriptor);

protected slots:
    //处理客户端发过来的消息
    void slotupdate_server(QString msg, int descriptor);

    //处理断开消息
    void slotclient_disconnect(int descriptor);
    //玩家battle信息处理
    void slot_disconnect(int des);

signals:
    //发送信号给界面，让它更新信息
    void update_server(QString);

private:
    int ready;
    int bat_cnt;
    QString playerA;
    QString playerB;
    int A = 0, B = 0;
};

#endif // SERVER_H
