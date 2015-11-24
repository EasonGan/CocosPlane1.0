/********************************************************************
创建于:		2014/11/26  11:05	
文件名: 	E:\Cocos3.3_Project\MyGame\Classes\PhysicsUnit.h 
作  者:		alex(alex)
修改于：	2014/11/26  11:05	

目  的:		所有附带物理属性的精灵的基类
*********************************************************************/

#ifndef _PHYSICS_UNIT_H_
#define _PHYSICS_UNIT_H_

#include "cocos2d.h"
#include "Buff.h"
#include "../GameCommon/GameDefine.h"

USING_NS_CC;

#define RemoveTag		150929

//BUFF结构体
typedef struct BuffStruct
{
	BuffId	buffId;
	float	durarion;
	BuffStruct()
	{
		buffId = BuffId::Buff_default;
		durarion = 0.0;
	}
	BuffStruct(BuffId id, float t)
	{
		buffId = id;
		durarion = t;
	}
}BuffStruct;

//物品掉落
struct DropReward
{
	float					HpPercent;	//血量百分比
	int						nums;			//掉落数量
	DropGoodsType			DropTp;			//掉落物品类型
	DropReward()
	{
		HpPercent = 0.0f;
		nums = 1;
		DropTp = DropGoodsType::DropGoods_Default;
	}

	DropReward(float p, int num, const DropGoodsType& tp)
	{
		HpPercent = p;
		nums = num;
		DropTp = tp;
	}
};

typedef struct BasePro
{
	CC_SYNTHESIZE(int,		id,					Id);
	CC_SYNTHESIZE(int,		hurts,				Hurts);
	CC_SYNTHESIZE(int,		defense,			Defense);	
	CC_SYNTHESIZE(int,		curHP,				CurHp);
	CC_SYNTHESIZE(int,		maxHp,				MaxHp);
	CC_SYNTHESIZE(int,		isInvincible,		IsInvincible);
	CC_SYNTHESIZE(int,		isIgnoreScreenEdge,	IsIgnoreScreenEdge);
	CC_SYNTHESIZE(int,		isInvincibleAbsolutely,	IsInvincibleAbsolutely);
	CC_SYNTHESIZE(UnitId,	myId,				MyId);
	CC_SYNTHESIZE(UnitId,	belongTo,			BelongTo);
	CC_SYNTHESIZE_PASS_BY_REF(Vec2,	  contactPos,	ContactPos);

	BasePro()
	{
		id = -1;
		hurts = 0;
		defense = 0;
		curHP = 1;
		maxHp = 1;
		isInvincible = 0;
		isIgnoreScreenEdge = 0;
		isInvincibleAbsolutely = 0;
		myId = UnitId::DefaultIdx;
		belongTo = UnitId::DefaultIdx;
		contactPos = Vec2(360,640);		//初始值设在屏幕中央，便于观测
	}
}UPro;

//死亡类型
enum class DeadType
{
	Dead_default,
	Dead_Nature,		//自然死亡
	Dead_Force,			//强制死亡
	Dead_Kill,			//击杀死亡
};

class PhysicsUnit : public Sprite
{
public:
	PhysicsUnit();
	static PhysicsUnit* createWithFrameName(const std::string name);
	static PhysicsUnit* createWithFile(const std::string file);
	static PhysicsUnit* create();
	virtual void onEnter();
	virtual void onExit();

	virtual void initBody();
	virtual void FuncAfterEnter(){}
	virtual void PlayMusicWhenContact(){}		//碰撞音效
	virtual void PlayMusicWhenDead(){}			//死亡音效
	virtual void AnimationWhenDead(){}			//死亡特效

	//当自己发生碰撞时,改变自己的属性
	void ChangeProperty(const BasePro& pro);
	virtual void OtherDealWhenContact(const BasePro& rs){}	//其他碰撞影响（不能在这个函数中有删除自己的操作）
	virtual void DoSomethingBeforeDead(){}					//死亡之前要做的事

	//碰撞属性影响
	virtual void ImpactOfHp(const BasePro& pro);				//血量影响
	virtual void DeathImpactOfDrops(){}							//死亡得分
	virtual void DeathImpactOfDropReward(){}					//当血量少于指定比例时，掉落指定物品
	virtual void FuncWhenHpChanged(){}
	virtual void FuncHpEmpty(const BasePro& pro);

	float getHpPercent();		//得到当前血量的百分比

	void DeathForce();

	void setInvincible(int invincible );
	void setUnitId(const UnitId& idx);
	void setBelongTo(const UnitId& idx);
	void setCantContact();
	void setMaxHp(int max);
	void setInvincibleAtEdge(int b = 1);
	void setInvincibleAbsolutely(int b = 1);
	bool isScreenEdge(const BasePro& pro);
	BasePro& getBasePro(){return m_BasePro;}
	void setBasePro(const BasePro& pro){m_BasePro = pro;}
	std::vector<DropReward> getDropVec(const std::vector<DropReward>& vec);

protected:
	virtual void autoCheckChance();
public:
	virtual void RemoveMyself();

	//
//	//<<*****************设置色调*********************/
//public:
//	float getHue();
//	void setHue(float hue);	//0~2π
//
//protected:
//	float _hue;
//	GLfloat _mat[3][3];
//
//	bool initWithTexture(cocos2d::Texture2D *texture);
//	bool initWithTexture(cocos2d::Texture2D *texture, const cocos2d::Rect& rect);
//	virtual bool initWithTexture(cocos2d::Texture2D *texture, const cocos2d::Rect &rect, bool rotated);
//	virtual bool initWithSpriteFrame(cocos2d::SpriteFrame *spriteFrame);
//
//	void setupDefaultSettings();
//	void initShader();
//	const GLchar* shaderBody();
//	virtual void updateColor();
//	void updateColorMatrix();
//	void updateAlpha();
//	GLfloat getAlpha();
//
//	void hueUniformCallback(cocos2d::GLProgram *p, cocos2d::Uniform *u);
//
//	//*****************设置色调*********************>>/

protected:
	Size				m_WinSize;
	CC_SYNTHESIZE(DeadType,		m_DeadType,	DeadType);		//死亡方式
	BasePro						m_BasePro;
	int							m_CurChance;
	CC_SYNTHESIZE(float,		m_delayRemoveTime, DelayRemoveTime);
	CC_SYNTHESIZE(int,			m_MaxChance,	MaxChance);
	bool						m_isDead;
	bool						m_isRemoving;
};


#endif