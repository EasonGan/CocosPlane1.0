#include "WeaponFactory.h"
#include "weapon.h"
#include "../../GameCommon/GameConfig.h"
#include "../../GameCommon/GameDefine.h"
#include "../BasePlane/BasePlane.h"
#include "../Layer/UIController.h"
#include "../Layer/RollingBg.h"
#include "../../GameCommon/GameData.h"
#include "../../../cocos2d/cocos/editor-support/spine/extension.h"
#include "../../Config/WeaponConfig.h"
#include "../Layer/PhysicsLayer.h"
#include "GameUI/GameRoleScene.h"
#include "GameUI/GameSceneMgr.h"



//---------------------------------------
WeaponFactory* WeaponFactory::create()
{
	WeaponFactory* wf = new (std::nothrow) WeaponFactory;
	if (wf && wf->initFactory())
	{
		wf->retain();
		wf->autorelease();
		return wf;
	}
	else
	{
		CC_SAFE_DELETE(wf);
		return nullptr;
	}
}

bool WeaponFactory::initFactory()
{
	do 
	{
	} while (0);

	return true;
}

void WeaponFactory::purge()
{
	this->release();
}

void WeaponFactory::AddEnemyWp( const WeaponPro& weapon, BasePlane* plane )
{
	if (!plane)
	{
		return;
	}
	int wpId = weapon.m_weaponId;
	ResTag res= weapon.m_res;
	float userdata = weapon.m_userdata;

	if (wpId < 1)
	{
		return;
	}

	int ammoHurts = plane->getBasePro().getHurts();

	if (res >= ResTag::Bullet_EnemyNor1 && res <= ResTag::Bullet_EnemyNor4)
	{

	}
	else
	{
		//log("--- ammo res out of range---");
	}
	switch (wpId)
	{
	case 1:
		EnemyNorPlWeapon1(wpId, plane, res, ammoHurts);
		break;
	case 2:
		EnemyNorPlWeapon2(wpId, plane, res, ammoHurts);
		break;
	case 3:
		EnemyNorPlWeapon3(wpId, plane, res, ammoHurts);
		break;
	case 4:
		EnemyNorPlWeapon4(wpId, plane, res, ammoHurts);
		break;
	case 5:
		EnemyNorPlWeapon5(wpId, plane, res, ammoHurts);
		break;
	case 6:
		EnemyNorPlWeapon6(wpId, plane, res, ammoHurts);
		break;
	case 7:
		EnemyNorPlWeapon7(wpId, plane, res, ammoHurts);
		break;
	case 8:
		EnemyNorPlWeapon8(wpId, plane, res, ammoHurts);
		break;
	case 9:
		EnemyNorPlWeapon9(wpId, plane, res, ammoHurts);
		break;
	case 10:
		EnemyNorPlWeapon10(wpId, plane, res, ammoHurts);
		break;
	case 11:
		EnemyNorPlWeapon11(wpId, plane, res, ammoHurts);
		break;
	case 12:
		EnemyNorPlWeapon12(wpId, plane, res, ammoHurts);
		break;
	case 13:
		EnemyNorPlWeapon13(wpId, plane, res, ammoHurts);
		break;
	case 14:
		EnemyNorPlWeapon14(wpId, plane, res, ammoHurts);
		break;
	case 15:
		EnemyNorPlWeapon15(wpId, plane, res, ammoHurts);
		break;
	case 16:
		EnemyNorPlWeapon16(wpId, plane, res, ammoHurts);
		break;
	case 17:
		EnemyNorPlWeapon17(wpId, plane, res, ammoHurts);
		break;
	case 18:
		EnemyNorPlWeapon18(wpId, plane, res, ammoHurts);
		break;

	default:
		break;
	}
}

void WeaponFactory::AddMainPlaneWp(int planeType, int plLvl, BasePlane* plane,bool isTemp )
{
	int hurt = -300;// -NewDataMgr::getInstance()->getRoleAtkWithId(planeType);

	Layer* layer = UIController::getInstance()->getPhysicsLayer();
	//bool isJustShow = GameData::getInstance()->getIsJustShowMainPlane();

	if (GameSceneType::Scene_Type_RoleInterface == GameSceneMgr::getInstance()->getCurScen())
	//if (isJustShow)
	{
		//auto scene = Director::getInstance()->getRunningScene();
		auto scene = GameInterfaceMgr::getInstance()->getParentNode();
		Node* node = scene->getChildByName("MainPlaneShowLayer");
		if (node)
		{
			layer = (Layer*)node;
		}
	}

	switch (planeType)
	{
	case 1:
		{
			if (plLvl == 1)
			{
				MainPl1_Wp_1(plane, layer, hurt, 0.08, isTemp);
			}								    
			else if(plLvl == 2)				    
			{								    
				MainPl1_Wp_2(plane, layer, hurt, 0.15f, isTemp);
			}								    
			else if(plLvl == 3)				    
			{								    
				MainPl1_Wp_3(plane, layer, hurt, 0.15f, isTemp);
			}								    
			else if(plLvl == 4)				    
			{								    
				MainPl1_Wp_4(plane, layer, hurt, 0.12f, isTemp);
			}								    
			else if(plLvl == 5)				    
			{								    
				//MainPl1_Wp_5(plane, layer, hurt, 0.1f, isTemp);
			}
		}
		break;
	case 2:
		{
			if (plLvl == 1)
			{
				MainPl2_Wp_1(plane, layer, hurt, 0.1f, isTemp);
			}
			else if(plLvl == 2)
			{
				MainPl2_Wp_2(plane, layer, hurt, 0.15f,isTemp);
			}
			else if(plLvl == 3)
			{
				MainPl2_Wp_3(plane, layer, hurt, 0.12f, isTemp);
			}
			else if(plLvl == 4)
			{
				MainPl2_Wp_4(plane, layer, hurt, 0.11f, isTemp);
			}
			else if(plLvl == 5)
			{
				//MainPl2_Wp_5(plane, layer, hurt, 0.2f, isTemp);
			}
		}
		break;
	case 3:
		{
			if (plLvl == 1)
			{
				MainPl3_Wp_1(plane, layer, hurt, 0.1f, isTemp);
			}
			else if(plLvl == 2)
			{
				MainPl3_Wp_2(plane, layer, hurt, 0.12f, isTemp);

			}
			else if(plLvl == 3)
			{
				MainPl3_Wp_3(plane, layer, hurt, 0.13f, isTemp);
			}
			else if(plLvl == 4)
			{
				MainPl3_Wp_4(plane, layer, hurt, 0.05f, isTemp);
			}
			else if(plLvl == 5)
			{
				MainPl3_Wp_5(plane, layer, hurt, 0.1f, isTemp);
			}
		}
		break;
	default:
		break;
	}
}

Weapon* WeaponFactory::SpecialWP_Laser_S( BasePlane* appendNode,int hurts /*= -1*/, float Shootspeed /*= -1.0f*/, float wpLife /*= -1.0f*/ )
{
	if (!appendNode)
	{
		return nullptr;
	}
	Weapon* wp = Weapon::create(appendNode);
	wp->setBullstType(BulletType::Self_LaserBullet);
	wp->setBltIsToPlane(true);
	wp->setShootTimes(1);
	wp->setBltHurts(hurts);
	appendNode->AddOneWp(wp, WeaponType::Addition);					//�����µ���������Ӱ��ԭ�е�������
	return wp;
}

Weapon* WeaponFactory::SpecialWP_Cat_S( BasePlane* appendNode,int hurts /*= -1*/, float Shootspeed /*= -1.0f*/, float wpLife /*= -1.0f*/ )
{
	return nullptr;
}

Weapon* WeaponFactory::SpecialWP_IceMisile_S( BasePlane* appendNode,int hurts /*= -1*/, float Shootspeed /*= -1.0f*/, float wpLife /*= -1.0f*/ )
{
	if (!appendNode)
	{
		return nullptr;
	}
	Weapon* wp = Weapon::create(appendNode);
	wp->setBullstType(BulletType::Self_MissleForDis);
	wp->setShootTimes(1);
	wp->setBltHurts(hurts);
	appendNode->AddOneWp(wp, WeaponType::Addition);				//�����µ���������Ӱ��ԭ�е�������
	return wp;
}

Weapon* WeaponFactory::SpecialWp_SmallDart_S( BasePlane* appendNode,int hurts /*= -1*/, float Shootspeed /*= -1.0f*/, float wpLife /*= -1.0f*/ )
{
	if (!appendNode)
	{
		return nullptr;
	}
	Weapon* wp = Weapon::create(appendNode);
	wp->setBullstType(BulletType::Self_SmallDartBullet);
	wp->setShootTimes(1);
	wp->setBltHurts(hurts);
	appendNode->AddOneWp(wp, WeaponType::Addition);				//¼ÓÉÏÐÂµÄÎäÆ÷£¨²»Ó°ÏìÔ­ÓÐµÄÎäÆ÷£©
	return wp;
}

void WeaponFactory::EnemyNorPlWeapon1(int wpId, BasePlane* plane, const ResTag& res, int hurts )
{
	//����1��Ʈ��
	Weapon* wp = Weapon::create(plane);
	wp->setBullstType(BulletType::Enemy_FallingExBullet);
	wp->setBltResTag(res);
	wp->setShootTimes(2);
	wp->setBltHurts(hurts);	
	wp->setRotation(180);
	wp->setPosition( Vec2(0,-50));	
	plane->AddOneWp(wp,WeaponType::Normal);

}

void WeaponFactory::EnemyNorPlWeapon2(int wpId, BasePlane* plane, const ResTag& res, int hurts )
{
	//ͬʱ����2��Ʈ��
	std::vector<Weapon*> vec;
	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Enemy_FallingExBullet);
		wp->setShootTimes(3);
		wp->setBltResTag(res);
		wp->setBltHurts(hurts);	
		wp->setPosition(Vec2(0,-50));	
		wp->setRotation(5 - 10 * i + 180);
		vec.push_back(wp);
	}
	plane->AddWpVec(vec,WeaponType::Normal);

}

void WeaponFactory::EnemyNorPlWeapon3(int wpId, BasePlane* plane, const ResTag& res, int hurts )
{
	//ͬʱ����6��Ʈ��
	int randNum = random(0,1);
	int tag = -1;
	if (randNum == 1)
	{
		tag = 1;
	}
	std::vector<Weapon*> vec;
	for (int i = 1; i <= 6; i++)
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Enemy_FallingExBullet);
		wp->setShootTimes(1);
		wp->setDelayShoot(0.1*i);
		wp->setBltResTag(res);
		wp->setBltHurts(hurts);	
		wp->setPosition(Vec2(0,-50));
		wp->setRotation(tag * (i - 3.5) * 20 + 180);
		vec.push_back(wp);
	}	
	plane->AddWpVec(vec,WeaponType::Normal);
}

void WeaponFactory::EnemyNorPlWeapon4(int wpId, BasePlane* plane, const ResTag& res, int hurts )
{
	//�����Ƿ�����1���ӵ���ÿ��1���ӵ�
	Weapon* wp = Weapon::create(plane);
	wp->setBullstType(BulletType::Enemy_default);
	wp->setShootTimes(3);
	wp->setBltResTag(res);
	wp->setBltHurts(hurts);
	wp->setBltSpeed(500);
	plane->AddOneWp(wp,WeaponType::Normal);

	auto mainpl = GameData::getInstance()->getMainPlane();
	if (mainpl)
	{
		auto plPos = mainpl->convertToWorldSpaceAR(Vec2::ZERO);
		auto myPos = wp->convertToWorldSpaceAR(Vec2::ZERO);
		auto newVec = plPos - myPos;
		auto angle = newVec.getAngle() * RAD_DEG;
		wp->setRotation(-angle + 90);		//�����ӵ�ԭ�������Ϸ���ģ��Ƕ��?90������Ҫ����90������
	}
}

void WeaponFactory::EnemyNorPlWeapon5(int wpId, BasePlane* plane, const ResTag& res, int hurts )
{
	auto mainpl = GameData::getInstance()->getMainPlane();
	auto plPos = Vec2(360,0);
	if (mainpl)
	{
		plPos = mainpl->convertToWorldSpaceAR(Vec2::ZERO);
	}
	float angleOffset = 0.0f;

	std::vector<Weapon*> vec;

	//�����Ƿ�����2���ӵ���ÿ��1���ӵ�
	for (int i = 0; i < 3; ++i)
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setShootTimes(1);
		wp->setDelayShoot(0.1f * i);
		wp->setBltResTag(res);
		wp->setBltHurts(hurts);
		wp->setBltSpeed(700 + (i * 50));
		wp->setBltLinearDamping(0.5f);

		if (mainpl)
		{
			auto parentPos = plane->convertToWorldSpaceAR(Vec2::ZERO);
			auto myPos = wp->convertToWorldSpaceAR(Vec2::ZERO);
			myPos = myPos + parentPos;

			auto newVec = plPos - myPos;
			angleOffset = newVec.getAngle() * RAD_DEG;
		}
		wp->setRotation(3 *i - angleOffset+ 90);
		vec.push_back(wp);
	}
	for (int i = 0; i < 3; ++i)
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setShootTimes(1);
		wp->setDelayShoot(0.1f * i);
		wp->setBltResTag(res);
		wp->setBltHurts(hurts);
		wp->setBltSpeed(700 + (i * 50));
		wp->setBltLinearDamping(0.5f);

		if (mainpl)
		{
			auto parentPos = plane->convertToWorldSpaceAR(Vec2::ZERO);
			auto myPos = wp->convertToWorldSpaceAR(Vec2::ZERO);
			myPos = myPos + parentPos;
			auto newVec = plPos - myPos;
			angleOffset = newVec.getAngle() * RAD_DEG;
		}
		wp->setRotation(-3 *i - angleOffset+ 90);
		vec.push_back(wp);
	}
	plane->AddWpVec(vec,WeaponType::Normal);

}

void WeaponFactory::EnemyNorPlWeapon6(int wpId, BasePlane* plane, const ResTag& res, int hurts )
{
	auto mainpl = GameData::getInstance()->getMainPlane();
	auto plPos = Vec2(360,0);
	if (mainpl)
	{
		plPos = mainpl->convertToWorldSpaceAR(Vec2::ZERO);
	}
	float angleOffset = 0.0f;
	
	std::vector<Weapon*> vec;

	//�����Ƿ�����4���ӵ���ÿ��1���ӵ�
	for (int i = 0; i < 3; ++i)
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setShootTimes(4);
		wp->setBltResTag(res);
		wp->setBltHurts(hurts);	
		wp->setBltSpeed(300);
		wp->setPosition(Vec2(0,0));

		auto parentPos = plane->convertToWorldSpaceAR(Vec2::ZERO);
		auto myPos = wp->convertToWorldSpaceAR(Vec2::ZERO);
		myPos = myPos + parentPos;
		auto newVec = plPos - myPos;
		angleOffset = newVec.getAngle() * RAD_DEG;
		wp->setRotation( (i - 1) * 20 + 90 - angleOffset);

		{
			auto rot = RotateBy::create(1.0f,-30);
			auto rep = RepeatForever::create(rot);
			wp->runAction(rep);
		}
		vec.push_back(wp);
	}	
	plane->AddWpVec(vec,WeaponType::Normal);
}

void WeaponFactory::EnemyNorPlWeapon7(int wpId, BasePlane* plane, const ResTag& res, int hurts )
{
	auto mainpl = GameData::getInstance()->getMainPlane();
	auto plPos = Vec2(360,0);
	if (mainpl)
	{
		plPos = mainpl->convertToWorldSpaceAR(Vec2::ZERO);
	}
	float angleOffset = 0.0f;

	std::vector<Weapon*> vec;

	for (int i = 0; i < 2; ++i)
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setShootTimes(6);
		wp->setShootSpeed(0.1f);
		wp->setBltResTag(ResTag::Bullet_EnemyNor4);
		wp->setBltHurts(hurts);	
		wp->setBltSpeed(500);
		auto parentPos = plane->convertToWorldSpaceAR(Vec2::ZERO);
		auto myPos = wp->convertToWorldSpaceAR(Vec2::ZERO);
		myPos = myPos + parentPos;
		auto newVec = plPos - myPos;
		angleOffset = newVec.getAngle() * RAD_DEG;
		wp->setRotation(15 - 30 *i - angleOffset+ 90);
		
		{
			auto rot = RotateBy::create(1.0f,30);
			auto rep = RepeatForever::create(rot);
			wp->runAction(rep);
		}
		vec.push_back(wp);
	}
	plane->AddWpVec(vec, WeaponType::Normal);

}

void WeaponFactory::EnemyNorPlWeapon8(int wpId, BasePlane* plane, const ResTag& res, int hurts )
{
	//�����Ƿ�����3���ӵ���ÿ��4���ӵ�
	auto mainpl = GameData::getInstance()->getMainPlane();
	auto plPos = Vec2(360,0);
	if (mainpl)
	{
		plPos = mainpl->convertToWorldSpaceAR(Vec2::ZERO);
	}
	float angleOffset = 0.0f;

	std::vector<Weapon*> vec;
	for (int i = 0; i < 3; ++i)
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setShootSpeed(0.15f);
		wp->setShootTimes(4);
		wp->setBltResTag(res);
		wp->setBltHurts(hurts);	
		wp->setPosition(Vec2(0,-50));
		wp->setBltSpeed(500);

		auto parentPos = plane->convertToWorldSpaceAR(Vec2::ZERO);
		auto myPos = wp->convertToWorldSpaceAR(Vec2::ZERO);
		myPos = myPos + parentPos;

		auto newVec = plPos - myPos;
		angleOffset = newVec.getAngle() * RAD_DEG;
		wp->setRotation((i - 1) * 15 - angleOffset + 90);

		vec.push_back(wp);
	}
	plane->AddWpVec(vec, WeaponType::Normal);
}

void WeaponFactory::EnemyNorPlWeapon9(int wpId, BasePlane* plane, const ResTag& res, int hurts )
{
	//�����Ƿ�����1��������ӵ�?
	auto mainpl = GameData::getInstance()->getMainPlane();
	auto plPos = Vec2(360,0);
	if (mainpl)
	{
		plPos = mainpl->convertToWorldSpaceAR(Vec2::ZERO);
	}
	float angleOffset = 0.0f;
	std::vector<Weapon*> vec;

	int count = 0;
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setDelayShoot(count * 0.1);
		wp->setShootTimes(1);
		wp->setBltResTag(res);
		wp->setBltHurts(hurts);	
		wp->setBltSpeed(500);
		wp->setPosition(Vec2(0,-50));

		auto parentPos = plane->convertToWorldSpaceAR(Vec2::ZERO);
		auto myPos = wp->convertToWorldSpaceAR(Vec2::ZERO);
		myPos = myPos + parentPos;

		auto newVec = plPos - myPos;
		angleOffset = newVec.getAngle() * RAD_DEG;
		wp->setRotation(0 - angleOffset + 90);
		vec.push_back(wp);
	}
	count++;

	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setBltResTag(res);
		wp->setBltHurts(hurts);	
		wp->setDelayShoot(count * 0.1);
		wp->setShootTimes(1);
		wp->setBltSpeed(500);
		wp->setPosition(Vec2(0,-50));
		wp->setRotation(-angleOffset + 3 * (i-0.5) + 90);
		vec.push_back(wp);
	}
	count++;

	for (int i = 0; i < 3; i++)
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setDelayShoot(count * 0.1);
		wp->setShootTimes(1);
		wp->setBltResTag(res);
		wp->setBltHurts(hurts);	
		wp->setBltSpeed(500);
		wp->setPosition(Vec2(0,-50));
		wp->setRotation(-angleOffset + 4 * (i-1) + 90);
		vec.push_back(wp);
	}
	plane->AddWpVec(vec, WeaponType::Normal);
}

void WeaponFactory::EnemyNorPlWeapon10(int wpId, BasePlane* plane, const ResTag& res, int hurts )
{
	//����Ȧ���������ƴ�Ȧ��
	Weapon* wp = Weapon::create(plane);
	wp->setBullstType(BulletType::Enemy_HelixBullet);
	wp->setShootTimes(5);
	wp->setBltResTag(res);
	wp->setBltHurts(hurts);	
	wp->setPosition(Vec2(0,-50));
	plane->AddOneWp(wp, WeaponType::Normal);
}

void WeaponFactory::EnemyNorPlWeapon11(int wpId, BasePlane* plane, const ResTag& res, int hurts )
{
	//С��Ȧ����������2Ȧ��Ȼ���ӵ�������λ�������?
	Weapon* wp = Weapon::create(plane);
	wp->setBullstType(BulletType::Enemy_HelixExBullet);
	wp->setShootTimes(3);
	wp->setBltResTag(res);
	wp->setBltHurts(hurts);	
	wp->setPosition(Vec2(0,-50));
	plane->AddOneWp(wp, WeaponType::Normal);
}

void WeaponFactory::EnemyNorPlWeapon12(int wpId, BasePlane* plane, const ResTag& res, int hurts )
{
	float randNum = random(0,1) - 0.5;
	randNum *= 2;

	std::vector<Weapon*> vec;
	for (int i = 0; i < 3; ++i)
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Enemy_Gravity);
		wp->setShootTimes(5);
		wp->setShootSpeed(0.05);
		wp->setBltResTag(res);
		wp->setBltSpeed(500);
		wp->setBltHurts(hurts);	
		wp->setBltLinearDamping(0.2);
		wp->setRotation(120 * i);
		
		{
			auto rot = RotateBy::create(1.0f,180 * randNum);
			auto rep = RepeatForever::create(rot);
			wp->runAction(rep);
		}
		vec.push_back(wp);
	}
	plane->AddWpVec(vec, WeaponType::Normal);
}

void WeaponFactory::EnemyNorPlWeapon13(int wpId, BasePlane* plane, const ResTag& res, int hurts )
{
	std::vector<Weapon*> vec;

	//��
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Enemy_Gravity);
		wp->setShootTimes(4);
		wp->setBltResTag(ResTag::Bullet_EnemyNor4);
		wp->setBltHurts(hurts);	
		wp->setShootSpeed(0.05);
		wp->setOffset(Vec2(-30,0));
		wp->setRotation(175);
		auto rot = RotateBy::create(1.0f,-30);
		auto rep = RepeatForever::create(rot);
		wp->runAction(rep);
		vec.push_back(wp);
	}
	//��
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Enemy_Gravity);
		wp->setShootSpeed(0.05);
		wp->setShootTimes(4);
		wp->setBltResTag(ResTag::Bullet_EnemyNor4);
		wp->setBltHurts(hurts);	
		wp->setOffset(Vec2(30,0));
		wp->setRotation(185);

		auto rot = RotateBy::create(1.0f,30);
		auto rep = RepeatForever::create(rot);
		wp->runAction(rep);

		vec.push_back(wp);
	}

	plane->AddWpVec(vec, WeaponType::Normal);
}

void WeaponFactory::EnemyNorPlWeapon14(int wpId, BasePlane* plane, const ResTag& res, int hurts )
{
	std::vector<Weapon*> vec;

	//���䵯�������ߵ���2�ŷ����ӵ���
	for (int i = 1; i <= 2; ++i)
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Enemy_Rebounce);
		wp->setShootTimes(5);
		wp->setBltResTag(res);
		wp->setBltHurts(hurts);
		wp->setBltGravityAble(true);
		wp->setRotation(-90 + i * 180);
		vec.push_back(wp);
	}
	plane->AddWpVec(vec, WeaponType::Normal);
}

void WeaponFactory::EnemyNorPlWeapon15(int wpId, BasePlane* plane, const ResTag& res, int hurts )
{
	std::vector<Weapon*> vec;

	//���䵯�������ܵ���6�ŷ����ӵ���
	for (int i = 0; i < 6; ++i)
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Enemy_Rebounce);
		wp->setShootTimes(5);
		wp->setBltResTag(res);
		wp->setBltGravityAble(true);
		wp->setBltHurts(hurts);	
		wp->setPosition(Vec2(0,-50));
		wp->setRotation(60 * i);
		vec.push_back(wp);
	}	
	plane->AddWpVec(vec, WeaponType::Normal);
}

void WeaponFactory::EnemyNorPlWeapon16(int wpId, BasePlane* plane, const ResTag& res, int hurts )
{
	auto mainpl = GameData::getInstance()->getMainPlane();
	auto plPos = Vec2(360,0);
	if (mainpl)
	{
		plPos = mainpl->convertToWorldSpaceAR(Vec2::ZERO);
	}

	float angleOffset = 0.0f;
	//���䵯�������ܵ���6�ŷ����ӵ���
	Weapon* wp = Weapon::create(plane);
	wp->setBullstType(BulletType::Enemy_default);
	wp->setShootTimes(20);
	wp->setShootSpeed(0.05);
	wp->setBltResTag(ResTag::Bullet_EnemyNor3);
	//wp->setBltGravityAble(true);
	wp->setBltHurts(hurts);	

	auto parentPos = plane->convertToWorldSpaceAR(Vec2::ZERO);
	auto myPos = wp->convertToWorldSpaceAR(Vec2::ZERO);
	myPos = myPos + parentPos;

	auto newVec = plPos - myPos;
	angleOffset = -newVec.getAngle() * RAD_DEG + 90;
	wp->setRotation(angleOffset);

	{
		auto rot = RotateTo::create(0.5,angleOffset + 45);
		auto back = RotateTo::create(0.5,angleOffset - 45);
		auto seq = Sequence::create(rot, back, nullptr);
		auto rep = RepeatForever::create(seq);
		wp->runAction(rep);
	}
	plane->AddOneWp(wp, WeaponType::Normal);
}

void WeaponFactory::EnemyNorPlWeapon17(int wpId, BasePlane* plane, const ResTag& res, int hurts )
{
	std::vector<Weapon*> vec;

	//����Χ����Բ����ɢ��12��
	int num = 1;
	float angle = 360.0f/5;
	for (int i = 0; i < 5; ++i)
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setShootTimes(3 + num);
		wp->setBltResTag(ResTag::Bullet_EnemyNor3);
		wp->setBltHurts(hurts);	
		wp->setBltSpeed(300);
		wp->setRotation(angle * i);
		if (num == 1)
		{
			num = 0;
		}
		else if(num == 0)
		{
			num = 1;
		}
		{
			auto rot = RotateBy::create(1.0f,90);
			auto rep = RepeatForever::create(rot);
			wp->runAction(rep);
		}
		vec.push_back(wp);
	}	

	plane->AddWpVec(vec, WeaponType::Normal);

}

void WeaponFactory::EnemyNorPlWeapon18(int wpId, BasePlane* plane, const ResTag& res, int hurts )
{
	std::vector<Weapon*> vec;

	//����Χ����Բ����ɢ��12��,������
	int num = 1;
	float angle = 360.0f/8;
	for (int i = 0; i < 8; ++i)
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setShootTimes(3 + num);
		wp->setBltResTag(ResTag::Bullet_EnemyNor3);
		wp->setBltHurts(hurts);	
		wp->setBltSpeed(300);
		wp->setRotation(angle * i);
		if (num == 1)
		{
			num = 0;
		}
		else if(num == 0)
		{
			num = 1;
		}
		{
			auto rot = RotateBy::create(1.0f,90);
			auto rep = RepeatForever::create(rot);
			wp->runAction(rep);
		}
		vec.push_back(wp);
	}	
	/*for (int i = 0; i < 8; ++i)
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setShootTimes(3 + num);
		wp->setBltResTag(ResTag::Bullet_EnemyNor3);
		wp->setBltHurts(hurts);	
		wp->setDelayShoot(0.5f);
		wp->setBltSpeed(300);
		wp->setRotation(angle * i);
		if (num == 1)
		{
			num = 0;
		}
		else if(num == 0)
		{
			num = 1;
		}
		{
			auto rot = RotateBy::create(1.0f,-90);
			auto rep = RepeatForever::create(rot);
			wp->runAction(rep);
		}
		vec.push_back(wp);
	}	*/
	plane->AddWpVec(vec, WeaponType::Normal);

}

void WeaponFactory::MainPl1_Wp_1( BasePlane* plane,Layer* layer, int hurts /*= -1*/, float Shootspeed /*= -1.0f*/,bool isTemp )
{
	if (!plane)
	{
		return;
	}

	std::vector<Weapon*> wpVec;

	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 2, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setBltZorder(SelfBulletZorder);
		wp->setShootSpeed(Shootspeed);
		wp->setOffset(Vec2(-45 + 90 * i, 0));
		wpVec.push_back(wp);
		wp->ShootCrisscross(Shootspeed, 20 - 40 * i);
	}
	/*for (int i = 0; i < 3; ++i)
	{
		Weapon* wp = Weapon::create(plane);
		wp->setBullstType(BulletType::Convolution);
		wp->setPhysicsLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setBltZorder(SelfBulletZorder);
		wp->setBltCategorybitmask(SELF_AMMO_CATEGORYBITMASK);
		wp->setBltCollisionbitmask(SELF_AMMO_COLLISIONBITMASK);
		wp->setBltContacttestbitmask(SELF_AMMO_CONTACTTESTBITMASK);
		wp->setShootSpeed(0.05f);
		wp->setBltSpeed(700);
		wp->setBltResTag(ResTag::Bullet_Self_14);
		wp->setRotation(120 * i);
		wp->setShootTimes(5);
		wpVec.push_back(wp);

		auto rot = RotateBy::create(0.5f, 120);
		wp->runAction(rot);
	}*/
	
	if (isTemp)
	{
		plane->AddWpVec(wpVec, WeaponType::Replace);
	}
	else
	{
		plane->AddWpVec(wpVec, WeaponType::Normal);
	}
}

void WeaponFactory::MainPl1_Wp_2( BasePlane* plane,Layer* layer, int hurts /*= -1*/, float Shootspeed /*= -1.0f*/, bool isTemp )
{
	if (!plane)
	{
		return;
	}
	std::vector<Weapon*> wpVec;

	Weapon* wp = getWeaponDataWithConfig(plane, 2, false);
	wp->setBulletLayer(layer);
	wp->setBltSpeed(MainPlBltSpeed);
	wp->setBltZorder(SelfBulletZorder);
	wp->setShootSpeed(Shootspeed);
	wp->setOffset(Vec2(0, 40));
	wpVec.push_back(wp);

	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 1, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setBltZorder(SelfBulletZorder);
		wp->setShootSpeed(Shootspeed);
		wp->setOffset(Vec2(-45 + 90 * i, 20));
		wpVec.push_back(wp);
	}
	if (isTemp)
	{
		plane->AddWpVec(wpVec, WeaponType::Replace);
	}
	else
	{
		plane->AddWpVec(wpVec, WeaponType::Normal);
	}
}

void WeaponFactory::MainPl1_Wp_3( BasePlane* plane,Layer* layer, int hurts /*= -1*/, float Shootspeed /*= -1.0f*/, bool isTemp )
{
	if (!plane)
	{
		return;
	}
	std::vector<Weapon*> wpVec;

	Weapon* wp = getWeaponDataWithConfig(plane, 4, false);
	wp->setBulletLayer(layer);
	wp->setBltSpeed(MainPlBltSpeed);
	wp->setBltZorder(SelfBulletZorder);
	wp->setShootSpeed(Shootspeed);
	wp->setOffset(Vec2(0, 40));
	wpVec.push_back(wp);

	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 3, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setBltZorder(SelfBulletZorder);
		wp->setShootSpeed(Shootspeed);
		wp->setOffset(Vec2(-45 + 90 * i, 0));
		wpVec.push_back(wp);
	}
	if (isTemp)
	{
		plane->AddWpVec(wpVec, WeaponType::Replace);
	}
	else
	{
		plane->AddWpVec(wpVec, WeaponType::Normal);
	}
}

void WeaponFactory::MainPl1_Wp_4( BasePlane* plane,Layer* layer, int hurts /*= -1*/, float Shootspeed /*= -1.0f*/, bool isTemp )
{
	if (!plane)
	{
		return;
	}

	std::vector<Weapon*> wpVec;

	Weapon* wp = getWeaponDataWithConfig(plane, 6, false);
	wp->setBulletLayer(layer);
	wp->setBltSpeed(MainPlBltSpeed);
	wp->setBltZorder(SelfBulletZorder);
	wp->setShootSpeed(Shootspeed);
	wp->setOffset(Vec2(0, 40));
	wpVec.push_back(wp);

	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 5, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setBltZorder(SelfBulletZorder);
		wp->setShootSpeed(Shootspeed);
		wp->setOffset(Vec2(-45 + 90 * i, 20));
		wpVec.push_back(wp);
		wp->setRotation(-3 + 6 * i);
	}
	if (isTemp)
	{
		plane->AddWpVec(wpVec, WeaponType::Replace);
	}
	else
	{
		plane->AddWpVec(wpVec, WeaponType::Normal);
	}
}

void WeaponFactory::MainPl1_Wp_5( BasePlane* plane,Layer* layer, int hurts /*= -1*/, float Shootspeed /*= -1.0f*/, bool isTemp )
{
	if (!plane)
	{
		return;
	}
	std::vector<Weapon*> wpVec;

	int eachHurts = hurts * MainAttack;
	for (int i = 0; i < 5; i++)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 3, false);
		wp->setBulletLayer(layer);
		wp->setRotation((i - 2) * 1);
		wp->setOffset(Vec2((i - 2) * 15, 50 - fabsf(i-2) * 10));
		wp->setBltZorder(SelfBulletZorder + fabsf(i-2) * 1);
		wp->setShootSpeed(Shootspeed);
		wp->setBltSpeed(MainPlBltSpeed);
		wpVec.push_back(wp);
	}
	//���?��ɢ��
	for (int i = 0; i < 3; i++)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 6, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed*0.8);
		wp->setShootSpeed(Shootspeed * 5);
		wp->setOffset(Vec2(-55 -i * 10, 60*i));		
		wp->setBltZorder(SelfBulletZorder);
		wp->setRotation( -9 + i * 3);
		wpVec.push_back(wp);
	}

	//�ұ�3��ɢ��
	for (int i = 0; i < 3; i++)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 6, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed*0.8);
		wp->setShootSpeed(Shootspeed * 5);
		wp->setOffset(Vec2(55 + i * 10, 60*i));
		wp->setRotation( 9 - i * 3);		
		wp->setBltZorder(SelfBulletZorder);
		wpVec.push_back(wp);
	}
	if (isTemp)
	{
		plane->AddWpVec(wpVec, WeaponType::Replace);
	}
	else
	{
		plane->AddWpVec(wpVec, WeaponType::Normal);
	}
}

void WeaponFactory::MainPl2_Wp_1( BasePlane* plane,Layer* layer, int hurts /*= -1*/, float Shootspeed /*= -1.0f*/, bool isTemp )
{
	if (!plane)
	{
		return;
	}
	std::vector<Weapon*> wpVec;

	Weapon* wp = getWeaponDataWithConfig(plane, 7, false);
	wp->setBulletLayer(layer);
	wp->setShootSpeed(Shootspeed);
	wp->setBltSpeed(MainPlBltSpeed);
	wp->setOffset(Vec2(0, 50));
	wpVec.push_back(wp);

	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 7, false);
		wp->setBulletLayer(layer);
		wp->setShootSpeed(Shootspeed);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setBltZorder(SelfBulletZorder - fabsf(i - 1));
		wp->setOffset(Vec2(-35 + 70 * i, 20));
		wpVec.push_back(wp);
	}
	if (isTemp)
	{
		plane->AddWpVec(wpVec, WeaponType::Replace);
	}
	else
	{
		plane->AddWpVec(wpVec, WeaponType::Normal);
	}
}

void WeaponFactory::MainPl2_Wp_2( BasePlane* plane,Layer* layer, int hurts /*= -1*/, float Shootspeed /*= -1.0f*/, bool isTemp )
{
	if (!plane)
	{
		return;
	}
	std::vector<Weapon*> wpVec;

	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 8, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setBltZorder(SelfBulletZorder);
		wp->setShootSpeed(Shootspeed);
		wp->setOffset(Vec2(-15 + 30 * i, 40));
		wpVec.push_back(wp);
	}

	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 8, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setBltZorder(SelfBulletZorder);
		wp->setShootSpeed(Shootspeed);
		wp->setOffset(Vec2(-45 + 90 * i, 0));
		wpVec.push_back(wp);
	}
	if (isTemp)
	{
		plane->AddWpVec(wpVec, WeaponType::Replace);
	}
	else
	{
		plane->AddWpVec(wpVec, WeaponType::Normal);
	}
}

void WeaponFactory::MainPl2_Wp_3( BasePlane* plane,Layer* layer, int hurts /*= -1*/, float Shootspeed /*= -1.0f*/, bool isTemp )
{
	if (!plane)
	{
		return;
	}
	std::vector<Weapon*> wpVec;

	Weapon* wp = getWeaponDataWithConfig(plane, 9, false);
	wp->setBulletLayer(layer);
	wp->setBltSpeed(MainPlBltSpeed);
	wp->setBltZorder(SelfBulletZorder);
	wp->setShootSpeed(Shootspeed);
	wp->setOffset(Vec2(0, 50));
	wpVec.push_back(wp);

	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 10, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setBltZorder(SelfBulletZorder);
		wp->setShootSpeed(Shootspeed);
		wp->setOffset(Vec2(-20 + 40 * i, 40));
		wpVec.push_back(wp);
	}
	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 10, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setBltZorder(SelfBulletZorder);
		wp->setShootSpeed(Shootspeed);
		wp->setOffset(Vec2(-45 + 90 * i, 0));
		wpVec.push_back(wp);
	}

	if (isTemp)
	{
		plane->AddWpVec(wpVec, WeaponType::Replace);
	}
	else
	{
		plane->AddWpVec(wpVec, WeaponType::Normal);
	}
}

void WeaponFactory::MainPl2_Wp_4( BasePlane* plane,Layer* layer, int hurts /*= -1*/, float Shootspeed /*= -1.0f*/, bool isTemp )
{
	if (!plane)
	{
		return;
	}
	std::vector<Weapon*> wpVec;

	Weapon* wp = getWeaponDataWithConfig(plane, 11, false);
	wp->setBulletLayer(layer);
	wp->setBltSpeed(MainPlBltSpeed);
	wp->setShootSpeed(Shootspeed);
	wp->setBltZorder(SelfBulletZorder);
	wp->setOffset(Vec2(0, 30));
	wpVec.push_back(wp);

	if (isTemp)
	{
		plane->AddWpVec(wpVec, WeaponType::Replace);
	}
	else
	{
		plane->AddWpVec(wpVec, WeaponType::Normal);
	}
}

void WeaponFactory::MainPl2_Wp_5( BasePlane* plane,Layer* layer, int hurts /*= -1*/, float Shootspeed /*= -1.0f*/, bool isTemp )
{
	if (!plane)
	{
		return;
	}
	std::vector<Weapon*> wpVec;

	for (int i = 0; i < 3; ++i)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 11, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setShootSpeed(Shootspeed);
		wp->setBltZorder(SelfBulletZorder);
		wp->setOffset(Vec2((i - 1) * 20, 50 - fabsf(i-1) * 20));
		wpVec.push_back(wp);
	}
	for (int i = 0; i < 2; ++i)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 12, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setShootSpeed(Shootspeed);
		wp->setBltZorder(SelfBulletZorder);
		wp->setOffset(Vec2(-45 + 90 * i, 20));
		wpVec.push_back(wp);
	}
	for (int i = 0; i < 3; ++i)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 10, true);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setShootSpeed(Shootspeed * 3);
		wp->setBltZorder(SelfBulletZorder);
		wp->setOffset(Vec2(-100 + 20 * i, 50 * i));
		wp->setRotation(-20 + 5 * i);
		wpVec.push_back(wp);
	}
	for (int i = 0; i < 3; ++i)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 10, true);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setShootSpeed(Shootspeed * 3);
		wp->setBltZorder(SelfBulletZorder);
		wp->setOffset(Vec2(100 - 20 * i, 50 * i));
		wp->setRotation(20 - 5 * i);
		wpVec.push_back(wp);
	}
	if (isTemp)
	{
		plane->AddWpVec(wpVec, WeaponType::Replace);
	}
	else
	{
		plane->AddWpVec(wpVec, WeaponType::Normal);
	}
}

void WeaponFactory::MainPl3_Wp_1( BasePlane* plane,Layer* layer, int hurts /*= -1*/, float Shootspeed /*= -1.0f*/, bool isTemp )
{
	if (!plane)
	{
		return;
	}
	std::vector<Weapon*> wpVec;

	for (int i = 0; i < 2; ++i)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 14, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setShootSpeed(Shootspeed);
		wp->setBltZorder(SelfBulletZorder);
		wp->setOffset(Vec2(-15 + 30 * i, 80));
		wpVec.push_back(wp);
	}
	for (int i = 0; i < 2; ++i)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 13, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setShootSpeed(Shootspeed);
		wp->setBltZorder(SelfBulletZorder);
		wp->setOffset(Vec2(-45 + 90 * i, 0));
		wpVec.push_back(wp);
	}
	if (isTemp)
	{
		plane->AddWpVec(wpVec, WeaponType::Replace);
	}
	else
	{
		plane->AddWpVec(wpVec, WeaponType::Normal);
	}
}

void WeaponFactory::MainPl3_Wp_2( BasePlane* plane,Layer* layer, int hurts /*= -1*/, float Shootspeed /*= -1.0f*/, bool isTemp )
{
	if (!plane)
	{
		return;
	}
	std::vector<Weapon*> wpVec;

	Weapon* wp = getWeaponDataWithConfig(plane, 16, false);
	wp->setBulletLayer(layer);
	wp->setShootSpeed(Shootspeed);
	wp->setBltSpeed(MainPlBltSpeed);
	wp->setBltZorder(SelfBulletZorder - 1);
	wp->setOffset(Vec2(0, 60));
	wpVec.push_back(wp);

	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 15, false);
		wp->setBulletLayer(layer);
		wp->setShootSpeed(Shootspeed);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setBltZorder(SelfBulletZorder + 1);
		wp->setOffset(Vec2(-55 + 110 * i, 0));
		wpVec.push_back(wp);
	}

	if (isTemp)
	{
		plane->AddWpVec(wpVec, WeaponType::Replace);
	}
	else
	{
		plane->AddWpVec(wpVec, WeaponType::Normal);
	}
}

void WeaponFactory::MainPl3_Wp_3( BasePlane* plane,Layer* layer, int hurts /*= -1*/, float Shootspeed /*= -1.0f*/, bool isTemp )
{
	if (!plane)
	{
		return;
	}
	std::vector<Weapon*> wpVec;

	Weapon* wp = getWeaponDataWithConfig(plane, 17, false);
	wp->setBulletLayer(layer);
	wp->setBltSpeed(MainPlBltSpeed);
	wp->setBltZorder(SelfBulletZorder);
	wp->setShootSpeed(Shootspeed);
	wp->setOffset(Vec2(0, 65));
	wpVec.push_back(wp);

	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 15, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setBltZorder(SelfBulletZorder);
		wp->setShootSpeed(Shootspeed);
		wp->setOffset(Vec2(-40 + 80 * i, 0));
		wpVec.push_back(wp);
	}
	if (isTemp)
	{
		plane->AddWpVec(wpVec, WeaponType::Replace);
	}
	else
	{
		plane->AddWpVec(wpVec, WeaponType::Normal);
	}
}

void WeaponFactory::MainPl3_Wp_4( BasePlane* plane,Layer* layer, int hurts /*= -1*/, float Shootspeed /*= -1.0f*/, bool isTemp )
{
	if (!plane)
	{
		return;
	}
	std::vector<Weapon*> wpVec;

	Weapon* wp = getWeaponDataWithConfig(plane, 18, false);
	wp->setBulletLayer(layer);
	wp->setBltSpeed(MainPlBltSpeed);
	wp->setBltZorder(SelfBulletZorder);
	wp->setShootSpeed(0.15);
	wp->setOffset(Vec2(0, 40));
	wpVec.push_back(wp);

	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 19, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setBltZorder(SelfBulletZorder);
		wp->setShootSpeed(Shootspeed);
		wp->setOffset(Vec2(-45 + 90 * i, 20));
		wpVec.push_back(wp);
		wp->setRotation(-10 + 20 * i);

		auto rot = RotateBy::create(0.5, 15 - 30 * i);
		auto back = rot->reverse();
		auto seq = Sequence::create(rot, back, nullptr);
		auto rep = RepeatForever::create(seq);
		wp->runAction(rep);
	}
	if (isTemp)
	{
		plane->AddWpVec(wpVec, WeaponType::Replace);
	}
	else
	{
		plane->AddWpVec(wpVec, WeaponType::Normal);
	}
}

void WeaponFactory::MainPl3_Wp_5( BasePlane* plane,Layer* layer, int hurts /*= -1*/, float Shootspeed /*= -1.0f*/, bool isTemp )
{
	if (!plane)
	{
		return;
	}
	std::vector<Weapon*> wpVec;

	Weapon* wp = getWeaponDataWithConfig(plane, 21, false);
	wp->setBulletLayer(layer);
	wp->setBltSpeed(MainPlBltSpeed);
	wp->setShootSpeed(Shootspeed);
	wp->setBltZorder(SelfBulletZorder);
	wp->setOffset(Vec2(0, 60));
	wpVec.push_back(wp);

	for (int i = 0; i < 5; ++i)
	{
		if (i == 2)
		{
			continue;
		}
		Weapon* wp = getWeaponDataWithConfig(plane, 20, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setShootSpeed(Shootspeed);
		wp->setBltZorder(SelfBulletZorder);
		wp->setOffset(Vec2((i - 2) * 30, 60 - fabsf(i-2) * 20));
		wpVec.push_back(wp);
	}
	for (int i = 0; i < 2; ++i)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 17, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setShootSpeed(Shootspeed * 0.25);
		wp->setBltZorder(SelfBulletZorder);
		wp->setOffset(Vec2(-65 + i * 130, 40));
		wp->setRotation(-5 + 10 * i);
		wpVec.push_back(wp);

		auto rot = RotateBy::create(0.3f, -20 + 40 * i);
		auto back = rot->reverse();
		auto seq = Sequence::create(rot, back, nullptr);
		auto rep = RepeatForever::create(seq);
		wp->runAction(rep);
	}
	for (int i = 0; i < 2; ++i)
	{
		Weapon* wp = getWeaponDataWithConfig(plane, 22, false);
		wp->setBulletLayer(layer);
		wp->setBltSpeed(MainPlBltSpeed);
		wp->setShootSpeed(Shootspeed * 0.25);
		wp->setBltZorder(SelfBulletZorder);
		wp->setOffset(Vec2(-50 + i * 100, 40));
		wp->setRotation(-25 + 50 * i);
		wpVec.push_back(wp);

		auto rot = RotateBy::create(0.2f, -15 + 30 * i);
		auto back = rot->reverse();
		auto seq = Sequence::create(rot, back, nullptr);
		auto rep = RepeatForever::create(seq);
		wp->runAction(rep);
	}
	if (isTemp)
	{
		plane->AddWpVec(wpVec, WeaponType::Replace);
	}
	else
	{
		plane->AddWpVec(wpVec, WeaponType::Normal);
	}
}

Weapon* WeaponFactory::getWeaponDataWithConfig(BasePlane* plane, int weaponId, bool isToTarget )
{
	int curRole = NewDataMgr::getInstance()->mRoleConfig.roleIndex;
	int baseHurt = DataConfig::getMainPlBaseHurt();
	
	auto data = NewDataMgr::getInstance()->getBulletConfigWithId(weaponId);
	auto core = data->attackCoe;	//����ϵ��
	auto scaleX = data->scaleX;
	auto scaleY = data->scaleY;
	std::string res = String::createWithFormat("bullet_%d.png",data->resId)->getCString();
	auto bulletType = BulletType::Self_Default;
	if (isToTarget)
	{
		bulletType = BulletType::Self_ToTarget;
	}
	int hurt = baseHurt * core;

	Weapon* wp = Weapon::create(plane);
	wp->setBullstType(bulletType);
	wp->setBltResTag(ResTag::defaultType);
	wp->setBltFrameName(res);
	wp->setBltBelongTo(UnitId::eMainPlane);
	wp->setBltScaleX(scaleX);
	wp->setBltScaleY(scaleY);
	wp->setBltHurts(hurt);
	wp->setBltCategorybitmask(SELF_AMMO_CATEGORYBITMASK);
	wp->setBltCollisionbitmask(SELF_AMMO_COLLISIONBITMASK);
	wp->setBltContacttestbitmask(SELF_AMMO_CONTACTTESTBITMASK);

	return wp;
}

void WeaponFactory::AddWeaponWithConfig( BasePlane* plane, int ConfigId )
{
	int bitMask = plane->getPhysicsBody()->getCategoryBitmask();
	BulletType bt = BulletType::Self_Default;
	switch (bitMask)
	{
	case ENEMY_PLANE_CATEGORYBITMASK:
		bt = BulletType::Enemy_default;
		break;
	case SELF_PLANE_CATEGORYBITMASK:
		bt = BulletType::Self_Default;
		break;
	default:
		break;
	}
	int hurt = -100;
	auto config = WeaponConfig::getWeaponConfigWithId(ConfigId);
	for (int i = 0; i < config.size(); i++)
	{
		auto weaponPro = config.at(i);
		int WpNum = weaponPro.getConfigWpNum();
		if (WpNum > 1)
		{
			float DisInterval = 30;
			float minDis = weaponPro.getConfigMinDis();
			if (minDis < DisInterval)
			{
				minDis = DisInterval;
			}
			float ExDis = weaponPro.getConfigExDis();
			float zhenfu = weaponPro.getConfigZhenFu();
			int eachDis = minDis + ExDis;
			int SideNum = WpNum/2;
			float t = ExDis*0.5/zhenfu * weaponPro.getConfigShootInterval();

			//ȷ��ÿ��������λ��
			//���?
			for (int i = 0; i < SideNum; ++i)
			{
				Weapon* wp = Weapon::createByConfig(plane,weaponPro);
				auto pos = Vec2(-(DisInterval * (i + 0.5) + ExDis * (i+0.5)), 0);
				wp->setBullstType(bt);
				wp->setBltHurts(hurt);
				plane->AddOneWp(wp, WeaponType::Normal);
				wp->setPosition(pos);

				auto endPos = Vec2(-(minDis * (i + 0.5)), 0);
				auto mvDis = endPos - pos;

				if (zhenfu > 0)
				{
					auto mv = MoveBy::create(t, mvDis);
					auto back = mv->reverse();
					auto seq = Sequence::create(mv, back, nullptr);
					auto rep = RepeatForever::create(seq);
					wp->runAction(rep);
				}
			}

			//�ұ�
			for (int i = 0; i < SideNum; ++i)
			{
				Weapon* wp = Weapon::createByConfig(plane,weaponPro);
				auto pos = Vec2(DisInterval * (i + 0.5) + ExDis * (i+0.5), 0);
				wp->setBullstType(bt);
				wp->setBltHurts(hurt);
				plane->AddOneWp(wp, WeaponType::Normal);
				wp->setPosition(pos);

				auto endPos = Vec2(-(minDis * (i + 0.5)), 0);
				auto mvDis = endPos - pos;

				if (zhenfu > 0)
				{
					auto mv = MoveBy::create(t, mvDis);
					auto back = mv->reverse();
					auto seq = Sequence::create(mv, back, nullptr);
					auto rep = RepeatForever::create(seq);
					wp->runAction(rep);
				}
			}
		}
		else
		{
			Weapon* wp = Weapon::createByConfig(plane,weaponPro);
			wp->setBullstType(bt);
			wp->setBltHurts(hurt);
			plane->AddOneWp(wp, WeaponType::Normal);
		}
	}
}

