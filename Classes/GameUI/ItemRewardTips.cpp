//
// Created by Ҷ�� on 15/9/23.
//

#include "ItemRewardTips.h"
#include "../GameCommon/NewDataMgr.h"

static ItemRewardTips *itemRewardTips_singleInstance = nullptr;

ItemRewardTips *ItemRewardTips::getInstance(Node* parent,int order,Vec2 startPos) {
    if (!itemRewardTips_singleInstance) {
        itemRewardTips_singleInstance = new(std::nothrow) ItemRewardTips();
    }
    if(parent == nullptr) {
        itemRewardTips_singleInstance->parent = Director::getInstance()->getRunningScene();
    } else {
        itemRewardTips_singleInstance->parent = parent;
    }
    itemRewardTips_singleInstance->order = order;
    itemRewardTips_singleInstance->startPos = startPos;
    return itemRewardTips_singleInstance;
}

void ItemRewardTips::destroyInstance() {
    CC_SAFE_DELETE(itemRewardTips_singleInstance);
}

void ItemRewardTips::upItems(float dt)
{
    if(this->vector.size() <= 0 && this->temp_vector.size() <= 0) {
        this->_scheduler->pauseTarget(this);
        return;
    }

    if(temp_vector.size() > 0) {
        if(this->vector.size() <= 0) {
            this->moveTips();
        } else {
            OneItemRewardTips* item = this->vector.back();
            if (item->getPositionY() <= this->startPos.y + this->item_height + this->item_padding) {
                this->move_speed = this->special_up_speed;
            } else {
                this->moveTips();
                this->move_speed = this->normal_up_speed;
            }
        }
    }

    for(Vector<OneItemRewardTips*>:: iterator it = this->vector.begin(); it != this->vector.end();it ++) {
        OneItemRewardTips* item = *it;
        if(item->getPosition().y - this->startPos.y >= this->h_max) {
//            this->vector.eraseObject(item);
            this->temp_remove.pushBack(item);
            this->fadeOutAndRemoveItem(item);
        }
        item->setPosition(Vec2(0,this->move_speed * 0.01) + item->getPosition());
    }

    if(this->temp_remove.size() > 0)
    {
        for(Vector<OneItemRewardTips*>:: iterator it = this->temp_remove.begin(); it != this->temp_remove.end();it ++) {
            this->vector.eraseObject(*it);
        }
    }
}
void ItemRewardTips::moveTips()
{
    OneItemRewardTips* item = this->temp_vector.back();
    this->temp_vector.eraseObject(item);
    this->moveTipsToScreen(item);
    this->vector.pushBack(item);
}

void ItemRewardTips::stop()
{
    this->_scheduler->pauseTarget(this);
    this->vector.clear();
    this->temp_vector.clear();
    this->temp_remove.clear();
}

void ItemRewardTips::moveTipsToScreen(OneItemRewardTips* item)
{
    if(this->parent == nullptr || this->parent->getReferenceCount() <= 0) {
        return;
    }
    item->setPosition(this->startPos);
    item->setLocalZOrder(this->order);
    this->parent->addChild(item);
}


ItemRewardTips::ItemRewardTips() {
    this->normal_up_speed = 300;
    this->special_up_speed = 460;
    this->item_padding = 1;
    this->item_height = 80;
    this->h_max = 450;

    this->move_speed = normal_up_speed;

    this->_scheduler = Director::getInstance()->getScheduler();
    this->_scheduler->schedule(CC_SCHEDULE_SELECTOR(ItemRewardTips::upItems), this, 0.01, false);
    this->_scheduler->pauseTarget(this);
}

void ItemRewardTips::fadeOutAndRemoveItem(OneItemRewardTips* item)
{
    this->runOutAndRemoveAction(item);
    Vector<Node*> temp = item->getChildren();
    for(Vector<Node*>::iterator it = temp.begin();it != temp.end();it ++) {
        this->runOutAndRemoveAction(*it);
    }
}

void ItemRewardTips::runOutAndRemoveAction(Node* node)
{
    node->runAction(Sequence::create(FadeOut::create(0.2),RemoveSelf::create(true), nullptr));
}

void ItemRewardTips::addItemTips(int type,void* data)
{
    OneItemRewardTips* item = OneItemRewardTips::create();
    if(item->initItemTips(type,data,this->parent)) {
        item->retain();
        this->temp_vector.pushBack(item);
        this->_scheduler->resumeTarget(this);
    } else {
        log("tips create faild //////////////// ");
    }
}

std::string OneItemRewardTips::getIconPathFromType(int type) {
    std::string imgName = "";
    switch (type) {
        case GoodsID::GoodsID_Gold:
            imgName = "ccsRes/resShopLayer/ImgGold1.png";
            break;
        case GoodsID::GoodsID_HuDun:
            imgName = "ccsRes/resShopLayer/ImgHuDun1.png";
            break;
        case GoodsID::GoodsID_DaZhao:
            imgName = "ccsRes/resShopLayer/mfw37.png";
            break;
        case GoodsID::GoodsID_Life:
            imgName = "ccsRes/resShopLayer/ImgLife1.png";
            break;
        default:
            imgName = "ccsRes/resShopLayer/ImgGold1.png";
            break;
    }
    return imgName;

}

bool OneItemRewardTips::initItemTips(int type, void* data, Node *parent) {
    if(!parent) {
        log("tips can not find the parent %s","");
        return false;
    }

    Sprite* bg = Sprite::create("Image/item_reward_bg.png");
    this->addChild(bg);
    Label* lab1 = Label::createWithSystemFont("获得","Marker Felt",30);
    lab1->setAnchorPoint(Vec2(0,0.5));
    lab1->setPosition(150,bg->getContentSize().height * 0.5);
    bg->addChild(lab1);

	if (type < GoodsID::GoodsID_Role) {
        std::string resourceName = this->getIconPathFromType(type);
        Sprite* itemIcon = nullptr;
        SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(resourceName);
        if(!frame) {
            if(FileUtils::getInstance()->isFileExist(resourceName)) {
                itemIcon = Sprite::create(resourceName);
            } else {
                log("tips can not find the resource %s",resourceName.c_str());
                return false;
            }
        } else {
            itemIcon = Sprite::createWithSpriteFrame(frame);
        }

        itemIcon->setContentSize(Size(70,70));
        itemIcon->setAnchorPoint(Vec2(0,0.5));
        itemIcon->setPosition(lab1->getPositionX() + 100,lab1->getPositionY());
        bg->addChild(itemIcon);

        int num = (intptr_t) data;

        std::string text = __String::createWithFormat("X%d",num)->getCString();
        Label* lab2 = Label::createWithSystemFont(text,"Marker Felt",30);
        lab2->setAnchorPoint(Vec2(0,0.5));
        lab2->setPosition(Vec2(itemIcon->getPositionX() + 100,itemIcon->getPositionY()));
        bg->addChild(lab2);
    } else {
        Label* lab2 = Label::createWithSystemFont("","Marker Felt",30);
        if(GoodsID::GoodsID_Role == type) {
			std::string name = (char*)(data);
            lab2->setString(name);
        } else if(GoodsID::GoodsID_Card == type)
        {
            int starNum = (intptr_t) data;
            std::string text = __String::createWithFormat("%d星卡牌 ",starNum)->getCString();
            lab2->setString(text);
        }
        lab2->setAnchorPoint(Vec2(0,0.5));
        lab2->setPosition(lab1->getPositionX() + 100,lab1->getPositionY());
        bg->addChild(lab2);

        Label* lab3 = Label::createWithSystemFont("X1","Marker Felt",30);
        lab3->setAnchorPoint(Vec2(0,0.5));
        lab3->setPosition(Vec2(lab2->getPositionX() + lab2->getContentSize().width +40,lab2->getPositionY()));
        bg->addChild(lab3);
    }

    return true;

}