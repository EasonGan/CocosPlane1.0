/********************************************************************
创建于:		2014/11/13  11:29	
文件名: 	E:\Cocos3.3_Project\MyGame\Classes\weapon.h 
作  者:		alex(alex\)
修改于：	2014/11/13  11:29	

目  的:		发射子弹的武器
*********************************************************************/
#ifndef _WEAPON_H_
#define _WEAPON_H_

#include "cocos2d.h"
#include "cocos-ext.h"
#include "../PhysicsUnit.h"
#include "WeaponDelegate.h"

USING_NS_CC;
USING_NS_CC_EXT;

class Bullet;
class BasePlane;

enum class WeaponType
{
	Normal,			//正常
	Replace,		//替换	
	Addition,		//加成
};

enum class CreateType
{
	Native,			//本地创建
	Config,			//配置创建
};

class Weapon : public Node, public WeaponDelegate
{
public:
	Weapon();
	static Weapon* create(Node* pl);
	static Weapon* createByConfig(Node* pl, WeaponDelegate config);
	virtual void onEnter();
	void setOffset(const Vec2& offset);			//设置武器相对飞机的位置偏移
	void setDelayShoot(float dt = 0.0f);		//设置延迟发射时间
	void setShootTimes(int shootTimes);			//设置发射次数,默认无限发射
	void setShootSpeed(float newSpeed);			//设置发射速率
	void setBullstType(const BulletType& tp);	//设置发射的子弹类型
	void StartShoot();							

	void ShootCrisscross(float interval, float offsetX);		//交错发射
	void ShootOneBullet();
private:
	void initWeaponWithConfig(WeaponDelegate config);
	void shoot(float dt);						//子弹发射定时器
	void PauseShoot(float pauseTime);			//暂停发射一段时间
	void ResumeShoot();							//恢复发射
	void removeWeapon();						//删除武器

protected:
	float			m_shootSpeed;
	float			m_delayShoot;		//延迟射击
	CC_SYNTHESIZE(bool,m_isShootAble,ShootAble);		//是否可以发射子弹
	int				m_ShootTimes;		//预定射击次数
	int				m_curShootTimes;	//当前射击次数
	bool			m_isSwing;			//是否执行横向摆动
	CreateType		m_CreateType;
	BulletType		m_bulletType;
	Size			m_winSize;
	CC_SYNTHESIZE(Node*,m_WeaponParent,WeaponParent);
	CC_SYNTHESIZE(WeaponType,m_WeaponType,WeaponType);
};

//自动寻找目标子弹
class AutoWeapon : public Weapon
{
public:
	AutoWeapon();
	static AutoWeapon* create(bool isEnemy = false);
	virtual void onEnter();

	//不断寻找最近目标,旋转自己的角度
	void SeachTarget(float dt);
	void setLeft(bool b){m_isLeft = b;}
private:
	bool		m_isLeft;
	Size		m_winSize;
};





#endif
