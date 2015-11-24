/********************************************************************
创建于:		2014/12/25  14:56	
文件名: 	E:/Cocos3.3_Project/MyGame/Classes/Buff.h 
作  者:		alex(alex)
修改于：	2014/12/25  14:56	

目  的:		飞机单元的BUFF
*********************************************************************/
#ifndef __Buff_h__
#define __Buff_h__


#include "cocos2d.h"
#include "cocos-ext.h"
#include "../GameCommon/GameDefine.h"

USING_NS_CC;
USING_NS_CC_EXT;

class BuffManager;
class BaseBuff : public Node
{
public:
	static BaseBuff* create(BuffManager* buffmanager);

	/**
	@功能：激活BUFF，如果当前有此BUFF，那么时间重置
	@参数：1.Node：要加BUFF的节点；
			2.BuffId：要激活的buff的Id
			3.float：BUFF持续的时间
	@返回：无
	*/
	void ActiveBuff(const BuffId& sid, const float& duration);

	/**
	@功能：当此BUFF未消失之前，又出发了相同BUFF，那么重置该BUFF的时间为最大值
	@参数：无
	@返回：无
	*/
	void resetTime();

	//取得当前剩余时间
	float getLeftTime();

	//取得当前BUFF的ID
	BuffId getBuffId();

	//BUFF时间到后删除
	void removeBuff();

private:
	//BUFF时间倒数
	void CutDown(float t);

	/**
	@功能：将BUFF管理器传入进来
	@参数：BuffManager：BUFF管理器
	@返回：无
	*/
	void setManager(BuffManager* buffmanager);

	/**
	@功能：根据BUFF的id选择加载BUFF效果的显示,并且传给BUFF管理器
	@参数：BuffId: BUFF的ID
	@返回：无
	*/
	void addBUffById(const BuffId& buffid);

	/**
	@功能：根据BUFF的id选择要移除的BUFF效果的显示，并且从BUFF管理器删除
	@参数：BuffId: BUFF的ID
	@返回：无
	*/
	void removeBuffById(const BuffId& buffid);

private:

	BuffId		m_buffid;
	float		m_LeftTime;
	float		m_maxTime;		

	BuffManager* m_buffManager;
};


/************************************************************************/
/* 
BUFF管理器，由能够加载BUFF的节点类继承，管理多个BUFF
*/
/************************************************************************/
class BuffManager 
{
public:

	/**
	@功能：判断某个buff是否激活
	@参数：BuffId；	BUFF的id
	@返回：bool：如果是激活状态返回true，否则返回false
	*/
	bool isBuffActive(const BuffId& buffid);

	/**
	@功能：重置某个BUFF的时间
	@参数：BuffId：BUFF的id
	@返回：
	*/
	void resetBuff(const BuffId& buffid);

	//增加BUFF
	void addBuff(BaseBuff* buff);

	//删除对应BUFF
	void DelBuff(BaseBuff* buff);

private:

	std::vector<BaseBuff*>		m_buffVec;

};

#endif // Buff_h__