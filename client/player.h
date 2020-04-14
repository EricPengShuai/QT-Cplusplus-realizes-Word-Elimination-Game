#ifndef PLAYER_H
#define PLAYER_H
#include "member.h"
#include <QObject>
class Player:public Member
{
    Q_OBJECT
public:
    int exp;//经验值
    Player() {}
    ~Player() {}
    void Level_up() {
        //看经验值每增加5就会升一级
        int i = 0;
        int temp_exp = exp;
        while(temp_exp>=0){
            temp_exp -= 5;
            if(temp_exp>=0) i++;
            else break;
        }
        level = i;
    }
    void Add_exp(int time,int length) { //增加经验
        exp += 5 - time + length;
    }
    void Rem_exp(int time,int length) { //减少经验（battle过程中使用）
        exp -= 5 - time + length;
    }
};
#endif // PLAYER_H
