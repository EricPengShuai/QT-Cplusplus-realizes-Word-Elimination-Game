#ifndef TESTER_H
#define TESTER_H
#include <QObject>
#include "member.h"

class Tester:public Member
{  
    Q_OBJECT
public:
    Tester() {}
    ~Tester() {}
    void Level_up() {
        //看答题数每增加3就会升一级
        int i = 0;
        int temp_count = counted;
        while(temp_count>=0){
            temp_count -= 3;
            if(temp_count>=0) i++;
            else break;
        }
        level = i;
    }
};

#endif // TESTER_H
