/********************************************************************
创建于:		2015/01/13  14:46	
文件名: 	E:\Cocos3.3_Project\PlaneGame\Classes\Bullet\StrayBullet.h 
作  者:		alex(alex)
修改于：	2015/01/13  14:46	

目  的:		流弹层，从底部持续射出子弹，持续若干秒
*********************************************************************/
#ifndef __StrayBullet_H__
#define __StrayBullet_H__


#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;


class StrayBulletLayer : public Layer
{
public:
	static StrayBulletLayer* create(const float& t);
	CREATE_FUNC(StrayBulletLayer);
	virtual bool init();
	virtual void onEnter();

	//设置持续时间
	void setDuration(const float& t);
	void shootStrayBullet(float t);
	void removeWhenTimeout();

private:
	float		m_duration;
	Size		m_winSize;
};

#endif // __StrayBullet_H__