#ifndef MYSERVER_H
#define MYSERVER_H
#include "mythread.h"
#include <QTcpServer>

class MyThread;

class MyServer : public QTcpServer
{
    Q_OBJECT
public:
    MyServer(QWidget *parent,int port);
    ~MyServer();
    virtual void incomingConnection(qintptr socketDescriptor);
};

#endif // MYSERVER_H
