#ifndef MEMBER_H
#define MEMBER_H
#include <QString>
#include <QObject>
class  Member:public QObject
{   
    Q_OBJECT
public:
    QString name;
    QString code;
    int level;
    int counted;
    Member() {}
    ~Member() {}
    virtual void Level_up() {}
};
#endif // MEMBER_H
