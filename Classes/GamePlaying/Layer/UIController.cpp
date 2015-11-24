#include "UIController.h"
#include "PhysicsLayer.h"
#include "../Bullet/Bullet.h"
#include "../DropGoods.h"
#include "../../GameCommon/GameData.h"
#include "../../GameUI/GameResult.h"
#include "../../GameUI/GameHelp.h"
#include "GameUI/GameSceneMgr.h"
#include "GameUI/DemoBullet.h"




UIController::UIController()
	:m_GameMainUiLayer(nullptr)
	,m_CloudLayer(nullptr)
{
	m_winSize = Director::getInstance()->getWinSize();
}

void UIController::setPhysicsLayer( PhysicsLayer* layer )
{
	m_physicsLayer = layer;
}

PhysicsLayer* UIController::getPhysicsLayer()
{
	return m_physicsLayer;
}

void UIController::addBullets(Bullet* bullet, const Point& pos,const int& Zorder )
{
	bullet->setPosition(pos);
	if ( m_physicsLayer)
	{
		m_physicsLayer->addChild(bullet, Zorder);
	}
	else 
	{
		DemoShowLayer::getDemoShowLayer()->addChild(bullet, Zorder);
	}

}

void UIController::addBulletStreakEff( Bullet* bullet, const Point& pos,const int& Zorder /*= -1*/ )
{
	auto streak = MotionStreak::create(0.5f, 1.0f, 40.0f, Color3B(255, 255, 0), "menuN.png");
	streak->setPosition( pos );
	m_physicsLayer->addChild(streak,Zorder);
//	bullet->setStreakNode( streak );

}

void UIController::setMenuLayer( MenuLayer* layer )
{
	m_menuLayer = layer;
}

MenuLayer* UIController::getMenuLayer()
{
	return m_menuLayer;
}

void UIController::setMainUILayer( GameMainLayer* layer )
{
	m_GameMainUiLayer = layer;
}

GameMainLayer* UIController::getMainUILayer( void )
{
	return m_GameMainUiLayer;
}

void UIController::addScore(DropGold* sc, const Vec2& pos )
{
	sc->setPosition(pos);
	m_physicsLayer->addChild(sc,1000);
}

void UIController::setEffectsLayer( EffectsManager* effectsLayer )
{
	m_EffectsLayer = effectsLayer;
}

EffectsManager* UIController::getEffectsLayer()
{
	return m_EffectsLayer;
}

void UIController::addBulletsForTarget(Sprite* weapon, Bullet* bullet,const Point& pos, const Vec2& vecDirec,bool isleft )
{
	std::list<PhysicsUnit*> Planelist = GameData::getInstance()->getEnemyPlaneVec();
	std::list<PhysicsUnit*>::iterator it = Planelist.begin();
	Point targetPos = Vec2(0,0);
	if (Planelist.empty())
	{
		//addBullets(weapon,bullet,pos,vecDirec);
		return;
	}
	
	if (isleft)
	{
		for (it; it != Planelist.end(); it++)
		{
			targetPos = (*it)->getPosition();
			if (targetPos.y <= m_winSize.width*0.5  )
			{
				break;
			}
		}
	}
	else
	{
		for (it; it != Planelist.end(); it++)
		{
			targetPos = (*it)->getPosition();
			if (targetPos.y >= m_winSize.width*0.5 )
			{
				break;
			}
		}
	}
	

	/*float r = weapon->getParent()->getRotation();
	float bulletR = Weapon::getCCAngleByVect(direct);

	float fixR = -(r + bulletR);
	Vec2 weaponVect = Vec2::forAngle(fixR/180*M_PI);
	*/

	/*Vec2 vec = targetPos - pos;
	float bulletR = -getCCAngleByVect(vec);

	Vec2 weaponVect = Vec2::forAngle(bulletR/180*M_PI);

	int speed = bullet->getFlySpeed();
	bullet->setPosition(pos);
	bullet->setVelocity(weaponVect*speed);

	m_physicsLayer->addChild(bullet,-1);*/
}

void UIController::addMissile( Bullet* bullet,const Point& pos )
{
	std::list<PhysicsUnit*> planeList = GameData::getInstance()->getEnemyPlaneVec();
	if (planeList.empty())
	{
		return;
	}
	bullet->setPosition(pos);
	m_physicsLayer->addChild(bullet,-1);
}

void UIController::ShowResultLayer( bool isWin /*= true*/ )
{
	if ( isWin )
	{
		int goldNum = GameData::getInstance()->getGoldNum();
		int rewardGoldNum = NewDataMgr::getInstance()->mGameGradeMsg->getRewardGoldWithLv(goldNum);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Gold, goldNum + rewardGoldNum);

		GameInterfaceMgr::getInstance()->addInterfaceToList(GameInterfaceMgr::InterfaceTag_ResoutSuc, false);
		int curlevel = NewDataMgr::getInstance()->getCurLevel();
		int curMaxLevel = NewDataMgr::getInstance()->getCurMaxLevel();

		// 关卡等级
		NewDataMgr::getInstance()->conversionLevelGrade(curlevel, GameData::getInstance()->getSurvivlaTime());

		// 设置关卡完成状态
		NewDataMgr::getInstance()->setLevelFinishState(curlevel, true);

		if (curlevel >= curMaxLevel)
		{
			// 关卡+1
			NewDataMgr::getInstance()->setCurMaxLevel(curMaxLevel + 1);
		}
	}
	else
	{
		GameInterfaceMgr::getInstance()->addInterfaceToList(GameInterfaceMgr::InterfaceTag_ResoutFail, false);
	}
	NewDataMgr::getInstance()->mGameTaskSystem->saveGamePropertyData();
	GameSceneMgr::getInstance()->changeScene(GameSceneType::Scene_Type_RoleInterface);

}

void UIController::addCollectModeTips()
{
	
}

void UIController::addKillModeTips()
{
	GameHelp* gh = GameHelp::create();
	gh->setPosition(ccp(m_winSize.width * 0.5, m_winSize.height*0.5));
	gh->setScale(2.0f);
	UIController::getInstance()->getPhysicsLayer()->addChild(gh,1000);
}

void UIController::addEnergyFullTips()
{
	
}

UIController::~UIController()
{
	/*if (m_GameResoult)
		m_GameResoult->release();*/
}

void UIController::setGameBgMgr( RollingBg* layer )
{
	m_GameBgMgr = layer;
}

RollingBg* UIController::getGameBgMgr()
{
	return m_GameBgMgr;
}




