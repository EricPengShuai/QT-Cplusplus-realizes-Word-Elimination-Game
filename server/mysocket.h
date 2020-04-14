#ifndef MYSOCKET_H
#define MYSOCKET_H

#include <QTcpSocket>
#include <mysql.h>

class MySocket : public QTcpSocket
{
    Q_OBJECT
public:
    MySocket(QWidget *parent, qintptr socket);

    //处理新来的链接
public slots:
    void on_connected();
    void slot_update(QString msg, qintptr descriptor);
    void slot_disconnect();
signals:
    void update_serve(QString msg,qintptr descriptor);
};

#endif // MYSOCKET_H
