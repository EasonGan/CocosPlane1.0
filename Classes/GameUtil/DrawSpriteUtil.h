//
// Created by 叶飞 on 15/8/31.
//
/********************************************************************
	created:	2015/09/01
	created:	1:9:2015   21:15
	filename: 	E:\cocos3.4_projects\PlaneGame_Danji\Classes\GameUtil\DrawSpriteUtil.h
	file path:	E:\cocos3.4_projects\PlaneGame_Danji\Classes\GameUtil
	file base:	DrawSpriteUtil
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/
#ifndef DrawSpriteUtil_h__
#define DrawSpriteUtil_h__
#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;



class GameResource : public Ref
{
public:
    static GameResource* create(int type,std::string resource_name,int frame_num = 0,float unit = 0.1,float loop = -1,bool restore = false);
    const static unsigned int TYPE_PNG = 1;
    const static unsigned int TYPE_FRAME = 2;
    std::string resource_name;
    int frame_num;
    int resource_type;
    float unit;
    int loop;
    bool restore;
};

class DrawSpriteUtil {
public:
    static void setSpriteWithGameResource(Sprite* sprite,GameResource* resource);
    static SpriteFrame* getSpriteFrameFromResource(GameResource* resource);
    static SpriteFrame* getSpriteFrameFromResource(GameResource* resource,int index);
};


#endif // DrawSpriteUtil_h__

