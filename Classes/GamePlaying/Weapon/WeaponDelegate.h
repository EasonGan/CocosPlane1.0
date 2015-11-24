#ifndef WeaponDelegate_h__
#define WeaponDelegate_h__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "GameCommon\GameDefine.h"
#include "GameCommon\GameConfig.h"
//#include "..\Layer\UIController.h"
//#include "..\Layer\PhysicsLayer.h"

USING_NS_CC;
USING_NS_CC_EXT;

class WeaponDelegate
{
public:
	WeaponDelegate()
	{
		m_BltResTag = ResTag::defaultType,				
		m_BltScaleX = 1.0f,				
		m_BltScaleY = 1.0f,				
		m_ConfigWeaponAngle = 0.0f,			
		m_BltSpeed = 500.0f,				
		m_ConfigShootInterval = 0.15f,			
		m_ConfigWeaponDuration = -1.0f,			
		m_ConfigWeaponOffsetX = 0.0f,			
		m_ConfigWeaponOffsetY = 0.0f,			
		m_ConfigWeaponRotationEachShoot = 0.0f,
		m_ConfigWeaponMaxRotattion = 0.0f,		


		m_BltMass = 0.00001;
		m_BltLinearDamping = 0.0f;
		m_BltZorder = EnemyBulletZorder;
		m_BltHurts = 0;
		m_BltGravityAble = false;
		m_BltIsToPlane = false;
		m_BltBelongTo = UnitId::DefaultIdx;
		m_BltOtherData = nullptr;
		m_BltCategorybitmask = -1;
		m_BltCollisionbitmask =	-1;
		m_BltContacttestbitmask = -1;
		m_OriginVelocity = Vec2(0,1);

		m_ConfigWpNum = 1;
		m_ConfigMinDis = 30;	
		m_ConfigExDis = 0;
		m_ConfigZhenFu = 0;
		m_ConfigIsAmmi = false;
		m_BltFrameName.clear();
		m_BltTailName = "null";
		m_BltSelfRotate = 0;
		m_TailZorder = 0;
		m_BulletLayer = nullptr;
	}

	void setBltTailName(const std::string& file, int Zorder = 0)
	{
		m_BltTailName = file;
		m_TailZorder = Zorder;
	}

	CC_SYNTHESIZE(ResTag,	m_BltResTag,					BltResTag);			 //子弹资源ID
	CC_SYNTHESIZE(float,	m_BltScaleX,					BltScaleX);			 //子弹X轴缩放（默认1.0）
	CC_SYNTHESIZE(float,	m_BltScaleY,					BltScaleY);			 //子弹Y轴缩放（默认1.0）
	CC_SYNTHESIZE(float,	m_ConfigWeaponAngle,			ConfigWeaponAngle);			 //武器开火时的角度
	CC_SYNTHESIZE(float,	m_ConfigShootInterval,			ConfigShootInterval);		 //发射子弹的时间间隔
	CC_SYNTHESIZE(float,	m_ConfigWeaponDuration,			ConfigWeaponDuration);		 //当达到开火持续时间时，武器停止发射子弹
	CC_SYNTHESIZE(float,	m_ConfigWeaponOffsetX,			ConfigWeaponOffsetX);		 //武器与飞机的偏移量（X轴）
	CC_SYNTHESIZE(float,	m_ConfigWeaponOffsetY,			ConfigWeaponOffsetY);		 //武器与飞机的偏移量（Y轴）
	CC_SYNTHESIZE(float,	m_ConfigWeaponRotationEachShoot,	ConfigWeaponRotationEachShoot);	//一次开火后改变的角度
	CC_SYNTHESIZE(float,	m_ConfigWeaponMaxRotattion,		ConfigWeaponMaxRotattion);	//改变的角度最大值

	CC_SYNTHESIZE(float,	m_BltSpeed,			BltSpeed);				//子弹的飞行速度
	CC_SYNTHESIZE(float,	m_BltMass,			BltMass);				//子弹质量
	CC_SYNTHESIZE(float,	m_BltLinearDamping, BltLinearDamping);		//子弹阻尼
	CC_SYNTHESIZE(int,		m_BltZorder,		BltZorder);				//子弹Zorder
	CC_SYNTHESIZE(int,		m_BltHurts,			BltHurts);				//子弹伤害
	CC_SYNTHESIZE(bool,		m_BltGravityAble,	BltGravityAble);		//子弹是否受重力
	CC_SYNTHESIZE(bool,		m_BltIsToPlane,		BltIsToPlane);			//是否addchild在飞机上
	CC_SYNTHESIZE(UnitId,	m_BltBelongTo,		BltBelongTo);			//子弹归属
	CC_SYNTHESIZE(void*,	m_BltOtherData,		BltOtherData);			//其他子弹数据
	CC_SYNTHESIZE_PASS_BY_REF(Vec2, m_OriginVelocity,BltOriginVelocity);//初始速度向量

	
	CC_SYNTHESIZE(int,		m_BltCategorybitmask,	BltCategorybitmask);
	CC_SYNTHESIZE(int,		m_BltCollisionbitmask,	BltCollisionbitmask);
	CC_SYNTHESIZE(int,		m_BltContacttestbitmask,	BltContacttestbitmask);
	CC_SYNTHESIZE(float,	m_BltSelfRotate,	BltSelfRotate);	//子弹自转

	CC_SYNTHESIZE(int,		m_ConfigWpNum,		ConfigWpNum);
	CC_SYNTHESIZE(float,	m_ConfigMinDis,		ConfigMinDis);
	CC_SYNTHESIZE(float,	m_ConfigExDis,		ConfigExDis);
	CC_SYNTHESIZE(float,	m_ConfigZhenFu,		ConfigZhenFu);

	CC_SYNTHESIZE(float,	m_ConfigIsAmmi,		ConfigIsAmmi);
	CC_SYNTHESIZE_PASS_BY_REF(std::string,	m_BltFrameName,		BltFrameName);
	CC_SYNTHESIZE(Layer*,	m_BulletLayer,	BulletLayer);

	std::string				m_BltTailName;
	int						m_TailZorder;
};

#endif // WeaponDelegate_h__