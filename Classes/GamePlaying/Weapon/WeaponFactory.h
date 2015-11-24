/********************************************************************
创建于:		2015/03/11  14:37	
文件名: 	E:\Cocos3.3_Project\PlaneGame2\Classes\GamePlaying\Weapon\WeaponFactory.h 
作  者:		alex()
修改于：	2015/03/11  14:37	

目  的:		
*********************************************************************/
#ifndef __WeaponFactory_H__
#define __WeaponFactory_H__


#include "cocos2d.h"
#include "cocos-ext.h"
#include "GameCommon\GameDefine.h"

USING_NS_CC;
USING_NS_CC_EXT;



#define WeaponInstance GameData::getInstance()->getWeaponFactory()

#define MainAttack 0.6
#define AssistantAttack (1.0 - MainAttack)


struct WeaponPro
{
	int			m_weaponId;
	float		m_userdata;
	ResTag		m_res;
	WeaponPro()
	{
		m_weaponId = 0;
		m_weaponId = 0.0f;
		m_res = ResTag::defaultType;
	};

	WeaponPro(int wpid, ResTag res, float userdata)
	{
		m_res = res;
		m_userdata = userdata;
		m_weaponId = wpid;
	};
};

//武器工厂
class Weapon;
class BasePlane;
class WeaponFactory : public Ref
{
public:
	static WeaponFactory* create();
	void purge();
	bool initFactory();
	
	void AddEnemyWp(const WeaponPro& weapon,  BasePlane* plane);
	void AddMainPlaneWp(int planeType, int plLvl, BasePlane* plane, bool isTemp = false);
	void AddWeaponWithConfig(BasePlane* plane, int ConfigId);


	/****己方飞机武器****/
	void MainPl1_Wp_1(BasePlane* plane,Layer* layer, int hurts = -1, float Shootspeed = -1.0f, bool isTemp = false);
	void MainPl1_Wp_2(BasePlane* plane,Layer* layer, int hurts = -1, float Shootspeed = -1.0f, bool isTemp = false);
	void MainPl1_Wp_3(BasePlane* plane,Layer* layer, int hurts = -1, float Shootspeed = -1.0f, bool isTemp = false);
	void MainPl1_Wp_4(BasePlane* plane,Layer* layer, int hurts = -1, float Shootspeed = -1.0f, bool isTemp = false);
	void MainPl1_Wp_5(BasePlane* plane,Layer* layer, int hurts = -1, float Shootspeed = -1.0f, bool isTemp = false);

	void MainPl2_Wp_1(BasePlane* plane,Layer* layer, int hurts = -1, float Shootspeed = -1.0f, bool isTemp = false);
	void MainPl2_Wp_2(BasePlane* plane,Layer* layer, int hurts = -1, float Shootspeed = -1.0f, bool isTemp = false);
	void MainPl2_Wp_3(BasePlane* plane,Layer* layer, int hurts = -1, float Shootspeed = -1.0f, bool isTemp = false);
	void MainPl2_Wp_4(BasePlane* plane,Layer* layer, int hurts = -1, float Shootspeed = -1.0f, bool isTemp = false);
	void MainPl2_Wp_5(BasePlane* plane,Layer* layer, int hurts = -1, float Shootspeed = -1.0f, bool isTemp = false);

	void MainPl3_Wp_1(BasePlane* plane,Layer* layer, int hurts = -1, float Shootspeed = -1.0f, bool isTemp = false);
	void MainPl3_Wp_2(BasePlane* plane,Layer* layer, int hurts = -1, float Shootspeed = -1.0f, bool isTemp = false);
	void MainPl3_Wp_3(BasePlane* plane,Layer* layer, int hurts = -1, float Shootspeed = -1.0f, bool isTemp = false);
	void MainPl3_Wp_4(BasePlane* plane,Layer* layer, int hurts = -1, float Shootspeed = -1.0f, bool isTemp = false);
	void MainPl3_Wp_5(BasePlane* plane,Layer* layer, int hurts = -1, float Shootspeed = -1.0f, bool isTemp = false);

	/***********新的普通敌机的武器*************/
	void EnemyNorPlWeapon1(int wpId, BasePlane* plane, const ResTag& res, int hurts);
	void EnemyNorPlWeapon2(int wpId, BasePlane* plane, const ResTag& res, int hurts );
	void EnemyNorPlWeapon3(int wpId, BasePlane* plane, const ResTag& res, int hurts );
	void EnemyNorPlWeapon4(int wpId, BasePlane* plane, const ResTag& res, int hurts );
	void EnemyNorPlWeapon5(int wpId, BasePlane* plane, const ResTag& res, int hurts );
	void EnemyNorPlWeapon6(int wpId, BasePlane* plane, const ResTag& res, int hurts );
	void EnemyNorPlWeapon7(int wpId, BasePlane* plane, const ResTag& res, int hurts );
	void EnemyNorPlWeapon8(int wpId, BasePlane* plane, const ResTag& res, int hurts );
	void EnemyNorPlWeapon9(int wpId, BasePlane* plane, const ResTag& res, int hurts );
	void EnemyNorPlWeapon10(int wpId, BasePlane* plane, const ResTag& res, int hurts );
	void EnemyNorPlWeapon11(int wpId, BasePlane* plane, const ResTag& res, int hurts );
	void EnemyNorPlWeapon12(int wpId, BasePlane* plane, const ResTag& res, int hurts );
	void EnemyNorPlWeapon13(int wpId, BasePlane* plane, const ResTag& res, int hurts );
	void EnemyNorPlWeapon14(int wpId, BasePlane* plane, const ResTag& res, int hurts );
	void EnemyNorPlWeapon15(int wpId, BasePlane* plane, const ResTag& res, int hurts );
	void EnemyNorPlWeapon16(int wpId, BasePlane* plane, const ResTag& res, int hurts );
	void EnemyNorPlWeapon17(int wpId, BasePlane* plane, const ResTag& res, int hurts );
	void EnemyNorPlWeapon18(int wpId, BasePlane* plane, const ResTag& res, int hurts );


	/****己方特殊武器****/
	//激光
	Weapon* SpecialWP_Laser_S(BasePlane* appendNode,int hurts = -1, float Shootspeed = -1.0f, float wpLife = -1.0f);

	//猫
	Weapon* SpecialWP_Cat_S(BasePlane* appendNode,int hurts = -1, float Shootspeed = -1.0f, float wpLife = -1.0f);

	//冰导弹
	Weapon* SpecialWP_IceMisile_S(BasePlane* appendNode,int hurts = -1, float Shootspeed = -1.0f, float wpLife = -1.0f);

	//小飞镖
	Weapon* SpecialWp_SmallDart_S(BasePlane* appendNode,int hurts = -1, float Shootspeed = -1.0f, float wpLife = -1.0f);

	Weapon* getWeaponDataWithConfig(BasePlane* plane, int weaponId, bool isToTarget = false );
};


#endif // __WeaponFactory_H__