//
// Created by 叶飞 on 15/9/23.
//

#ifndef PLANEGAME_DANJI_ITEMREWARDTIPS_H
#define PLANEGAME_DANJI_ITEMREWARDTIPS_H
#include "cocos2d.h"

USING_NS_CC;

class OneItemRewardTips : public cocos2d::Node
{
private:
public:
    CREATE_FUNC(OneItemRewardTips);
    bool initItemTips(int type,void* data,Node* parent);

private:
    std::string getIconPathFromType(int type);
};

class ItemRewardTips : public cocos2d::Ref {

public:
    Node* parent;
    Vec2 startPos;
    int order;
    static ItemRewardTips* getInstance(Node* parent,int order = 1000,Vec2 startPos = Vec2(360,600));
    static void destroyInstance();
    void addItemTips(int type,void* data);
    void stop();
private:

    void moveTips();
    void moveTipsToScreen(OneItemRewardTips* item);
    void upItems(float dt);
    float normal_up_speed;
    float special_up_speed;
    float move_speed;
    float item_padding;
    float item_height;
    float h_max;
    void fadeOutAndRemoveItem(OneItemRewardTips* item);
    void runOutAndRemoveAction(Node* node);
    ItemRewardTips();
    Scheduler* _scheduler;
    Vector<OneItemRewardTips*> vector;
    Vector<OneItemRewardTips*> temp_vector;
    Vector<OneItemRewardTips*> temp_remove;
};


#endif //PLANEGAME_DANJI_ITEMREWARDTIPS_H
