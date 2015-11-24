#include "GameData.h"
#include "AudioEngine.h"
#include "../GamePlaying/Bullet/BulletFactory.h"
#include "../GamePlaying/Weapon/WeaponFactory.h"
#include "../GamePlaying/BasePlane/BasePlane.h"
#include "../GamePlaying/BasePlane/PlaneAction.h"
#include "../GamePlaying/DropGoods.h"
#include "../GamePlaying/BasePlane/PlaneManager.h"
#include "../GamePlaying/Layer/UIController.h"
#include "../GameUI/GameMainLayer.h"
#include "../GamePlaying/Layer/PhysicsLayer.h"
#include "EffectsManager.h"
#include "../GamePlaying/BasePlane/PlaneFactory.h"
#include "../GamePlaying/AI.h"
#include "NewDataMgr.h"
#include "GameConfig.h"
#include "GameSystem.h"
#include "../GamePlaying/Layer/RollingBg.h"
#include "../GamePlaying/RapidParticle/ParticleManager.h"
#include "../GamePlaying/Layer/CloudLayer.h"

GameData::GameData()
{
	m_winSize = Director::getInstance()->getWinSize();

	m_bulletFactory = BulletFactory::create();
	m_weaponFactory = WeaponFactory::create();
	m_planefactory = PlaneFactory::create();
	m_planeAiFactory = PlaneAIFactory::create();
	m_PlaneAction = EnterAction::create();
	m_scoreFactory = DropsFactory::create();
	m_planeManager = PlaneManager::create();

	m_mainPlane = nullptr;

	m_Playing = false;
	m_gamePause = false;
	m_GameResult = false;
	m_IsHurtByBoss = false;

	resetGameData();
}

GameData::~GameData()
{
	m_bulletFactory->purge();
	m_weaponFactory->purge();
	m_planefactory->purge();
	m_planeAiFactory->purge();
	m_planeManager->purge();
	m_PlaneAction->purge();
	m_scoreFactory->purge();
	ParticleManager::getInstance()->purge();

}

void GameData::addScore(const int& num )
{
	m_Score += num;
	UIController::getInstance()->getMainUILayer()->updateUIScore( m_Score );

}

void GameData::AddEnemyPlane( PhysicsUnit* pu )
{
	m_EnemyPlaneVec.push_back(pu);
}

void GameData::DelEnemyPlane( PhysicsUnit* pu )
{
	std::list<PhysicsUnit*>::iterator it = m_EnemyPlaneVec.begin();
	for (it; it != m_EnemyPlaneVec.end();)
	{
		PhysicsUnit* physicsunit = (*it);
		if (physicsunit == pu)
		{
			m_EnemyPlaneVec.erase(it);
			//log(" enemyPlane del ");
			break;
		}
		else
		{
			++it;
		}
	}
}

void GameData::SortByDis()
{
	std::list<PhysicsUnit*>::iterator it_i = m_EnemyPlaneVec.begin();
	std::list<PhysicsUnit*>::iterator it_j = m_EnemyPlaneVec.begin();
	for (it_i; it_i != m_EnemyPlaneVec.end(); ++it_i)
	{
		for (it_j = it_i; it_j != m_EnemyPlaneVec.end(); ++it_j)
		{
			Point pos_i = (*it_i)->getPosition();
			Point pos_j = (*it_j)->getPosition();
			float dis_i = getDistance(pos_i);
			float dis_j = getDistance(pos_j);
			if (dis_i > dis_j)
			{
				PhysicsUnit* temp = (*it_i);
				(*it_i) = (*it_j);
				(*it_j) = temp;

			}
		}
	}
}

float GameData::getDistance( const Point& pos )
{
	Vec2 vec = Vec2(m_winSize.width * 0.5, 0);
	BasePlane* plane = GameData::getInstance()->getMainPlane();
	if (plane)
	{
		vec = plane->getPosition();
	}
	float dis = pos.distance(vec);
	return dis;
}

void GameData::setMainPlane( MainPlane* plane )
{
	m_mainPlane = plane;
}

MainPlane* GameData::getMainPlane()
{
	return m_mainPlane;
}

void GameData::GameStart()
{
	resetGameData();
}

void GameData::GameOver(bool iswin)
{
	setGameResult(iswin);

	if (m_Playing)
	{
		m_Playing = false;
		if (!iswin)
		{
			UIController::getInstance()->getMainUILayer()->showBuyFuHuoWidget();
		}
		else
		{
			auto pl = GameData::getInstance()->getMainPlane();
			if (pl)
			{
				auto mianpl = (MainPlane*)pl;
				mianpl->FlyUp();
			}
		}
	}
}

void GameData::resetGameData()
{
	m_Golds = 0;
	m_Score = 0;
	m_updateSpeed = 5.0;

	m_mainPlane = nullptr;
	m_Playing = true;
	m_IsBossDead = false;
	m_IsJustShowMainPlane = false;
	m_IsRewardValid = true;
	m_planeManager->resetData();

	m_EnemyPlaneVec.clear();
	m_BossGrade.clear();

}

void GameData::SortByHpLimit()
{
	//log("begin target");
	std::list<PhysicsUnit*>::iterator it_i = m_EnemyPlaneVec.begin();
	std::list<PhysicsUnit*>::iterator it_j = m_EnemyPlaneVec.begin();
	for (it_i; it_i != m_EnemyPlaneVec.end(); ++it_i)
	{
		for (it_j = it_i; it_j != m_EnemyPlaneVec.end(); ++it_j)
		{
			PhysicsUnit* pu_i = (*it_i);
			PhysicsUnit* pu_j = (*it_j);
			if (pu_i || pu_j)
			{
				continue;
			}
			float blood_i = (*it_i)->getBasePro().getMaxHp();
			float blood_j = (*it_j)->getBasePro().getMaxHp();
			if (blood_i < blood_i)
			{
				PhysicsUnit* temp = (*it_i);
				(*it_i) = (*it_j);
				(*it_j) = temp;
			}
		}
	}
}

PhysicsUnit* GameData::getEnemyPlaneByMaxHp()
{
	if (m_EnemyPlaneVec.empty())
	{
		return nullptr;
	}
	//log("********************** size = %d",m_EnemyPlaneVec.size());
	auto it = m_EnemyPlaneVec.begin();

	//for (it; it != m_EnemyPlaneVec.end(); ++it)
	//{
	//	PhysicsUnit* pu = (*it);
	//	if (pu->getBasePro().isTarget && pu->isLiving())
	//	{
	//		//log("+++locked+++");
	//		return pu;
	//	}
	//}


	SortByHpLimit();
	it = m_EnemyPlaneVec.begin();
	for (it; it != m_EnemyPlaneVec.end(); ++it)
	{
		PhysicsUnit* pu = (*it); 

		bool isWudi = pu->getBasePro().getIsInvincible();
		if (!isWudi)
		{
			//BasePro pro = pu->getBasePro();
			//rs.isTarget = true;
			//pu->setBasePro(pro);
			return pu;
		}
	}
	return nullptr;
}

const Vec2& GameData::getNearTargetByPos( const Vec2& curPos )
{
	Vec2 nearPos = Vec2(0,0);
	float dis = 100000.0f;
	auto plVec = m_EnemyPlaneVec;
	for (auto it = plVec.begin(); it != plVec.end(); it++)
	{
		auto pl = (*it);
		if (pl)
		{
			Vec2 pos = pl->convertToWorldSpaceAR(Vec2::ZERO);
			float TempDis = pos.distance(curPos);
			if (TempDis < dis)
			{
				dis = TempDis;
				nearPos = pos;
			}
		}
	}
	return nearPos;
}

void GameData::GamePause()
{
	if (m_gamePause)
	{
		return;
	}
	m_gamePause = true;

	m_TargetSet = Director::getInstance()->getScheduler()->pauseAllTargets();
	m_AllNodes = Director::getInstance()->getActionManager()->pauseAllRunningActions();
	auto world = Director::getInstance()->getRunningScene()->getPhysicsWorld();
	if (world)
	{
		world->setSpeed(0);
	}

	Director::getInstance()->getScheduler()->schedule(schedule_selector(GameData::update),this, 0.0f, false);//一定要设为false
}

void GameData::GameResume()
{
	if (!m_gamePause)
	{
		return;
	}
	m_gamePause = false;
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(GameData::update),this);

	Director::getInstance()->getScheduler()->resumeTargets(m_TargetSet);
	Director::getInstance()->getActionManager()->resumeTargets(m_AllNodes);
	Director::getInstance()->getRunningScene()->getPhysicsWorld()->setSpeed(1);
	m_TargetSet.clear();
	m_AllNodes.clear();
}

void GameData::GameSlowDown(float duration, bool isToBlack)
{
	float rate = 0.3;

	Director::getInstance()->getScheduler()->setTimeScale(rate);

	ActionInterval* delay = DelayTime::create(duration*rate);
	CallFunc* func = CallFunc::create(this,callfunc_selector(GameData::GameResumeScale));
	ActionInterval* seq = Sequence::create(delay,func,nullptr);

	Node* node = UIController::getInstance()->getPhysicsLayer();
	node->runAction(seq);

	if (isToBlack)
	{
		UIController::getInstance()->getPhysicsLayer()->ToBlack(duration);
	}
}

void GameData::GameResumeScale()
{
	Director::getInstance()->getScheduler()->setTimeScale(1.0f);
}

void GameData::addGoldNum(const int& num )
{
	this->m_Golds+= num;

	UIController::getInstance()->getMainUILayer()->updateUIGold( num );
}

void GameData::GameSpeedUp()
{
	//Director::getInstance()->getScheduler()->setTimeScale(1.5f);
	m_updateSpeed = 30.0f;
	UIController::getInstance()->getCloudLayer()->setIsSpeedUp();
}

int GameData::getGoldNum()
{
	return this->getGolds();
}

int GameData::getEmemyNums()
{
	return m_EnemyPlaneVec.size();
}

bool GameData::isContact( int mask1,int mask2,int mask3, int mask4, int mask5,int mask6 )
{
	bool b = false;
	int v1 = mask1 & mask5;
	int v2 = mask1 & mask6;
	int v3 = mask4 & mask2;
	int v4 = mask4 & mask3;
	int v = mask1 & mask1;
	if (v1 && v2 && v3 && v4)
	{
		b = true;
	}
	return b;
}

bool GameData::isContact_P( int mask1,int mask2,int mask3, int mask4, int mask5,int mask6 )
{
	bool b = false;
	int v1 = mask1 & mask5;
	int v2 = mask1 & mask6;
	int v3 = mask4 & mask2;
	int v4 = mask4 & mask3;

	if (!v1 && v2 && !v3 && v4)
	{
		b = true;
	}
	return b;
}

bool GameData::isGameEnd()
{
	bool b = m_planeManager->getIsGroupOver();

	if (b && m_EnemyPlaneVec.empty())
	{
		return true;
	}

	return false;
}

void GameData::addBossRecord(int bossType, const BossRating& rating, float SurvivlaTime)
{
	BossGrade bg;
	bg.bossType = bossType;
	bg.rateing = rating;

	m_BossGrade.push_back(bg);

	setSurvivlaTime(SurvivlaTime);
}

std::vector<BossGrade>& GameData::getBossRecords()
{
	return m_BossGrade;
}

void GameData::addProp()
{

}

int GameData::getBulletHurtsOfEnemyPl(float userdata)
{
	int curMapId = UIController::getInstance()->getGameBgMgr()->getCurBgId();
	int curLvl = m_planeManager->getCurLvl();

	float val1 = NewDataMgr::getInstance()->getEnemyPlaneWithKey()->AtkCoe1;
	float val2 = NewDataMgr::getInstance()->getEnemyPlaneWithKey()->AtkCoe2;
	int ammoHurts = -(pow((userdata * curLvl * curMapId), val1) + val2 * userdata);

	return ammoHurts;
}

void GameData::update( float t )
{
	Director::getInstance()->getActionManager()->update(t);
}

void GameData::setAllEnemyPlaneInvincible( bool isInvincible )
{
	for (auto it = m_EnemyPlaneVec.begin(); it != m_EnemyPlaneVec.end(); it++)
	{
		PhysicsUnit* pl = (*it);
		pl->setInvincible(isInvincible);
	}
}

