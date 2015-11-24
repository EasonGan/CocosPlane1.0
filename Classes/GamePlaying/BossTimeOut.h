/********************************************************************
创建于:		2015/02/04  14:54	
文件名: 	E:/Cocos3.3_Project/PlaneGame/Classes/Common/BossTimeOut.h 
作  者:		alex(alex)
修改于：	2015/02/04  14:54	

目  的:		BOSS超时没有打死
*********************************************************************/
#ifndef __BossTimeOut_H__
#define __BossTimeOut_H__


#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

//BOSS超时掉落陨石
class BossTimeOut : public Node
{
public:
	BossTimeOut();
	CREATE_FUNC(BossTimeOut);

	void setMaxTime(const float& time);
	void setInterval(const float& time);

	virtual void onEnter();

	void StartTiming();

	//倒计时
	void TimeCutdown();

	//时间到了，执行掉落陨石函数
	void TimeOut();

	//掉落陨石
	void Fallingstone();

	Vec2 randPos();

	void speedUp(float dt);

protected:
	Node*		m_delegate;
	float		m_maxTime;
	float		m_interval;
	int			m_curRandPos;
};

#endif // __BossTimeOut_H__