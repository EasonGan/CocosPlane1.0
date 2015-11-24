//
// Created by 叶飞 on 15/10/20.
//

#ifndef PLANEGAME_DANJI_STATISTICSHELP_H
#define PLANEGAME_DANJI_STATISTICSHELP_H

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class StatisticsHelp : public cocos2d::Ref
{
public:
    static void buy(int item,int num = 1,int price = 1);
    static void use(int item,int num = 1,int price = 1);
    static void buy(const char* item,int num = 1,int price = 1);
    static void use(const char* item,int num = 1,int price = 1);
    static void startLevel(int level);
    static void failLevel(int level);
    static void finishLevel(int level);
    static void pay(int money,int gameMoney,int payChannel);
    static void callJava(const char* p1,const char* p2,const char* p3,const char* p4);
};

#endif //PLANEGAME_DANJI_STATISTICSHELP_H
