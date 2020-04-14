#include "mythread.h"
#include "mysocket.h"
#include <QFile>
#include <QMetaType>

MyThread::MyThread(QWidget *parent, qintptr socket) : QThread(parent)
{
    qRegisterMetaType<qintptr>("qintptr");
    this->p = socket;//指定标识符
    connect(this, SIGNAL(finished()),this,SLOT(deleteLater()));
    qDebug() << QThread::currentThread()<<"main_thread";
}

MyThread::~MyThread()
{
    this->deleteLater();
}

void MyThread::run()
{
    socket = new MySocket(0, this->p);
    connect(socket, &MySocket::disconnected, this, &MyThread::quit, Qt::DirectConnection);
    connect(socket, &MySocket::update_serve, socket, &MySocket::slot_update);
    this->exec();
}
