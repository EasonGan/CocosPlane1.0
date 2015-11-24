/********************************************************************
创建于:		2014/11/14  11:03	
文件名: 	E:\Cocos3.3_Project\MyGame\Classes\RollingBg.h 
作  者:		alex(alex)
修改于：	2014/11/14  11:03	

目  的:		滚动背景
*********************************************************************/
#ifndef _ROLLING_BG_H_
#define _ROLLING_BG_H_


#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class GameBg : public Layer
{
public:
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();
	virtual void update(float delta);
	void refreshSpeed(float dt);

	virtual float SpeedRate();
	virtual bool isFlip();				//是否要翻转,默认不需要
	virtual void SpeedUpEase(){};		//加速时，过渡效果

	void addBgAndRolling();

	void Shake(Ref* obj);

	void SwitchBg(int id);

protected:
	CC_SYNTHESIZE(float, m_curSpeed, CurSpeed);
	CC_SYNTHESIZE(Sprite*, m_Bg1, Bg1);
	CC_SYNTHESIZE(Sprite*, m_Bg2, Bg2);
	CC_SYNTHESIZE(int, m_curBgId, CurBgId);

	Size		m_size;
	Texture2D*	m_BgTexture;
};

//滚动背景
class RollingBg : public GameBg
{
public:
	CREATE_FUNC(RollingBg)

	virtual float SpeedRate();
	virtual bool isFlip();				//是否要翻转,默认不需要
	virtual void SpeedUpEase();		//加速时，过渡效果

	virtual void onEnter();
};


#endif
