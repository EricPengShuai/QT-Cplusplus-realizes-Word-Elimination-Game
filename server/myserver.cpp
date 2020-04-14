#include "myserver.h"
#include <QDebug>

MyServer::MyServer(QWidget *parent,int port)
    :QTcpServer(parent)
{
    listen(QHostAddress::Any,port);
}

MyServer::~MyServer()
{

}

void MyServer::incomingConnection(qintptr socketDescriptor)
{
    MyThread *thread = new MyThread(0, socketDescriptor);
    connect(thread, &MyThread::finished, [&]{ qDebug() << "thread is over"<<QThread::currentThread(); });
    connect(thread, &MyThread::finished, &MyThread::deleteLater);
    thread->start();
    //qDebug() << QThread::currentThread();
}
