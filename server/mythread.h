#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QWidget>

class MySocket;

class MyThread : public QThread
{
    Q_OBJECT
public:
    MyThread(QWidget *parent, qintptr socket);
    ~MyThread();
private:
    qintptr p;
    MySocket *socket;
    virtual void run();
};

#endif // MYTHREAD_H

