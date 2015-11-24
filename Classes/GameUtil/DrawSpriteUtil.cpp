//
// Created by 叶飞 on 15/8/31.
//

#include "DrawSpriteUtil.h"

GameResource* GameResource::create(int type,std::string resource_name,int frame_num,float unit,float loop,bool restore)
{
    GameResource* sf = new (std::nothrow) GameResource;
    sf->resource_type = type;
    sf->resource_name = resource_name;
    sf->frame_num = frame_num;
    sf->unit = unit;
    sf->loop = loop;
    sf->restore = restore;
    return sf;
}

void DrawSpriteUtil::setSpriteWithGameResource(Sprite* sprite,GameResource *resource) {

    if(GameResource::TYPE_FRAME == resource->resource_type) {
        Animation* animation = AnimationCache::getInstance()->getAnimation(resource->resource_name);
        if(!animation) {
            animation = Animation::create();
            int num = resource->frame_num;
            for (int i = 1; i <= num; i++) {
                SpriteFrame *frame = DrawSpriteUtil::getSpriteFrameFromResource(resource, i);
                if (frame) {
                    if (i == 1) {
                        sprite->setSpriteFrame(frame);
                    }
                    animation->addSpriteFrame(frame);
                }
            }
            AnimationCache::getInstance()->addAnimation(animation,resource->resource_name);
        }
        if(animation->getFrames().size() > 0) {
            animation->setDelayPerUnit(resource->unit);
            animation->setRestoreOriginalFrame(resource->restore);
            if(resource->loop > 0) {
                animation->setLoops(resource->loop);
                sprite->runAction(Animate::create(animation));
            } else {
                sprite->runAction(RepeatForever::create(Animate::create(animation)));
            }
        }
    } else if(GameResource::TYPE_PNG == resource->resource_type) {
        SpriteFrame* frame = DrawSpriteUtil::getSpriteFrameFromResource(resource);
        if(frame) {
            sprite->setSpriteFrame(frame);
        }
    }
}

SpriteFrame* DrawSpriteUtil::getSpriteFrameFromResource(GameResource* resource)
{
    SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(resource->resource_name);
    return frame;
}

SpriteFrame* DrawSpriteUtil::getSpriteFrameFromResource(GameResource* resource,int index) {
    std::string resourceName = resource->resource_name + "_%d.png";
    std::string frameName= __String::createWithFormat(resourceName.c_str(),index)->getCString();
    SpriteFrame* frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
    return frame;
}