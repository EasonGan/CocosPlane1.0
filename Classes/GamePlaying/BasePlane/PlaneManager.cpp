#include "PlaneManager.h"
#include "../../GameCommon/GameDefine.h"
#include "../../GameCommon/GameData.h"
#include "PlaneFactory.h"
#include "BasePlane.h"
#include "../../GameUtil/TmxUtil.h"
#include "PlaneAction.h"
#include "../Weapon/WeaponFactory.h"
#include "../AI.h"
#include "../Layer/UIController.h"
#include "../Layer/PhysicsLayer.h"
#include "../../GameUI/GameMainLayer.h"
#include "../../../cocos2d/cocos/editor-support/spine/extension.h"


using namespace experimental;

PlaneManager::PlaneManager()
	:m_GroupTmx(nullptr)
	,m_IsGroupOver(false)
	,m_restTime(-1)
	,m_restCount(0)
{

}

PlaneManager::~PlaneManager()
{
	if (m_GroupTmx)
	{
		m_GroupTmx->release();
	}
}

PlaneManager* PlaneManager::create()
{
	PlaneManager* pm = new (std::nothrow) PlaneManager;
	if (pm && pm->init())
	{
		pm->retain();
		pm->autorelease();
		return pm;
	}
	CC_SAFE_DELETE(pm);
	return nullptr;
}

void PlaneManager::purge()
{
	release();
}

bool PlaneManager::init()
{
	do 
	{
	} while (0);

	return true;
}

void PlaneManager::InitLvlData(const int& lvl )
{
	m_curLvl = lvl;

	GameData::getInstance()->setIsBossDead(false);
 	bool isExsit = readGroupTmx(lvl);
	if (!isExsit)
	{
		//û����һ���ˣ���ô��Ϸ����
		m_IsGroupOver = true;
		return;
	}

	m_GruopVec.clear();
	m_PlaneGroup.clear();

	m_GruopVec = m_GroupTmx->getObjectGroups();
	m_GruopIt = m_GruopVec.begin();
	if (m_curLvl > 1)
	{
		Rest(10.0f);
	}
	refreshEnemyPlane();
}

bool PlaneManager::readGroupTmx(const int& gameLvl)
{
	bool isExsit = true;
	if (m_GroupTmx)
	{
		m_GroupTmx->release();
		m_GroupTmx = nullptr;
	}
	std::string fileName = __String::createWithFormat("LevelData/Level_%d.tmx",gameLvl)->getCString();

	auto tmx = cocos2d::experimental::TMXTiledMap::create(fileName);
	if (!tmx)
	{
		//����ļ������ڣ���ô����false
		isExsit = false;
	}
	else
	{
		m_GroupTmx = tmx;
		m_GroupTmx->retain();
	}
	return isExsit;
}

void PlaneManager::AnalyseOneGroup( TMXObjectGroup* group )
{
	int	ActionId = getNamePro(group,1);						//�ҵ�����ID

	ValueVector& objects = group->getObjects();
	for (auto& obj : objects)
	{
		ValueMap& dict = obj.asValueMap();

		//λ��
		Point pos = getPosByTmx(dict);

		std::string name = dict["name"].asString();
		std::string type = dict["type"].asString();

		//��#�ָ��?���ַ�������
		std::vector<std::string> idStrVec = TmxUtil::getStrVec(name);
		std::vector<std::string> rateStrVec = TmxUtil::getStrVec(type);

		//�л�
		int plId = TmxUtil::getIntDataByRate(idStrVec.at(0),rateStrVec.at(0));			//����Ϣ�������ĵл������У�������ȡ��ID
		
		//AI
		int AiId = TmxUtil::getIntDataByRate(idStrVec.at(1),rateStrVec.at(1));
		
		//����
		int weaponId = TmxUtil::getIntDataByRate(idStrVec.at(2),rateStrVec.at(2));

		//�ӵ���Դ
		int bulletRes = TmxUtil::getIntDataByRate(idStrVec.at(3),rateStrVec.at(3));

		//�û�����
		float userdata = TmxUtil::getFloatDataByRate(idStrVec.at(4),rateStrVec.at(4));
		if (userdata <= 0.0f)
		{
			userdata = 1.0f;
		}

		//log("---- ammo id = %d -------",weaponId);
		auto plane = CreateWithPro(pos, plId, weaponId, bulletRes, ActionId, AiId, m_curLvl-1, userdata);

		//������
		AddDropsByTmx(dict,plane);			//����Ϣ�������ĵ����������У�������ȡ��ID

		AddOnePlane(plane);
	}
}

void PlaneManager::refreshEnemyPlane()
{
	if (m_IsGroupOver)
	{
		return;
	}
	if (m_restTime > 0 )
	{
		m_restCount++;

		//��Ϣ
		auto delay = DelayTime::create(m_restTime);
		auto rest = CallFunc::create(this,callfunc_selector(PlaneManager::RestCallback));
		auto seq = Sequence::create(delay, rest, nullptr);
		UIController::getInstance()->getPhysicsLayer()->runAction(seq);
		return;
	}
	if (m_GruopVec.empty() || m_GruopIt == m_GruopVec.end())
	{
		//��ǰ�ؿ����ȫ���������
		return;
	}
	addNextNormalGroup();
}

void PlaneManager::addNextNormalGroup()
{
	if (m_restCount > 0)
	{
		return;
	}
	//���صл����?
	TMXObjectGroup* group = (*m_GruopIt);
	AnalyseOneGroup(group);
	m_GruopIt++;
}

cocos2d::Point PlaneManager::getPosByTmx( ValueMap& dict )
{
	//λ��
	float x = dict["x"].asFloat();
	float y = dict["y"].asFloat();
	Point pos = ccp(x,y);

	return pos;
}

void PlaneManager::AddDropsByTmx( ValueMap& dict,BasePlane* pl )
{
	std::string drop = dict["drop"].asString();
	std::string rate = dict["dropRate"].asString();

	//������
	int dropId = TmxUtil::getIntDataByRate(drop.c_str(),rate.c_str());			//����Ϣ�������ĵ����������У�������ȡ��ID
	DropReward dropReward;
	dropReward.DropTp = (DropGoodsType)dropId;
	pl->getPlanePro().DropVec.push_back(dropReward);
}

int PlaneManager::getNamePro(TMXObjectGroup* group,const int& idx)
{
	std::string name = group->getGroupName();
	auto vec = TmxUtil::SplitToInt(name,',');		
	int	val = vec.at(idx);

	return val;
}

void PlaneManager::AddNextLevel()
{
	//������һTMX�ļ�
	InitLvlData(++m_curLvl);
}

BasePlane* PlaneManager::CreateWithPro(const Vec2& pos, int plId, int weaponId, int bulletRes, int actionId, int AIid, int gameLvl, float userdata )
{
	auto gd = GameData::getInstance();
	auto pl = gd->getPlaneFactory()->createEnemyPlane(plId, userdata, gameLvl);	//�з��ɻ�
	pl->setPosition(pos);

	EnemyPlane* norPl = dynamic_cast<EnemyPlane*>(pl);
	if (norPl)
	{
		norPl->setWeaponPro(WeaponPro( weaponId, (ResTag)bulletRes, userdata ));
	}

	gd->getPlaneAction()->AddEnemyActionByIdx(actionId,pl);			//�ɻ���������
	gd->getPlaneAiFactory()->AddEnemyAiByIdx(pl,AIid);				//�ɻ��Լ���AI
	UIController::getInstance()->getPhysicsLayer()->addChild(pl,-1000);

	return pl;
}

void PlaneManager::Rest( float dur )
{
	m_restTime = dur;
}

void PlaneManager::RestCallback()
{
	m_restCount--;
	m_restTime = -1;
	if (m_restCount < 1)
	{
		refreshEnemyPlane();
	}
}

void PlaneManager::AddOnePlane( PhysicsUnit* pl)
{
	m_PlaneGroup.push_back(pl);
}

void PlaneManager::DelOnePlane(PhysicsUnit* pl)
{
	auto it = m_PlaneGroup.begin();
	for (it; it != m_PlaneGroup.end();++it)
	{
		PhysicsUnit* plane = (*it);
		if (plane == pl)
		{
			m_PlaneGroup.erase(it);
			break;
		}
	}

	//�����ǰ�ɻ�ȫ�������? ��ô������һ��
	if (m_PlaneGroup.empty())
	{
		if (m_restCount < 1)		//�������Ϣ�׶Σ��Ͳ�����?
		{
			refreshEnemyPlane();
		}
	}
}

void PlaneManager::AddGiftPlaneGroup()
{
	GiftPlaneManager* giftPlMgr = GiftPlaneManager::create();
	UIController::getInstance()->getPhysicsLayer()->addChild(giftPlMgr);
}

void PlaneManager::resetData()
{
	//��������
	m_restCount = 0;		
	m_restTime = -1;
	m_IsGroupOver = false;
}


//---------------------------------

void GiftPlaneManager::onEnter()
{
	Node::onEnter();
	addNextGiftGroup();
}

void GiftPlaneManager::addGiftPlaneGroup()
{
	int lvl = GameData::getInstance()->getPlaneManager()->getCurLvl();
	lvl = lvl%4;
	switch (lvl)
	{
	case 1:
		m_PlaneId = 1;
		break;
	case 2:
		m_PlaneId = 6;
		break;
	case 3:
		m_PlaneId = 12;
		break;
	case 0:
		m_PlaneId = 16;
		break;
	default:
		break;
	}
	m_ActionId = random(9,18);

	{
		auto node = Node::create();
		addChild(node);

		auto delay = DelayTime::create(0.3f);
		auto next = CallFunc::create(this, callfunc_selector(GiftPlaneManager::addOneGiftPlane));
		auto seq = Sequence::create(next, delay, nullptr);
		auto rep = Repeat::create(seq, 5);
		auto rm = RemoveSelf::create(true);
		auto seq1 = Sequence::create(rep, rm, nullptr);
		node->runAction(seq1);
	}
}

void GiftPlaneManager::addNextGiftGroup()
{
	addGiftPlaneGroup();

	auto node = Node::create();
	addChild(node);

	auto delay = DelayTime::create(5.0f + rand_0_1() * 3.0f);
	auto next = CallFunc::create(this, callfunc_selector(GiftPlaneManager::addNextGiftGroup));
	auto rm = RemoveSelf::create(true);
	auto seq = Sequence::create(delay, next, rm, nullptr);
	node->runAction(seq);
}

void GiftPlaneManager::addOneGiftPlane()
{
	int lvl = GameData::getInstance()->getPlaneManager()->getCurLvl();
	auto pl = GameData::getInstance()->getPlaneFactory()->createEnemyPlane(m_PlaneId, 0.5f, lvl);

	GameData::getInstance()->getPlaneAction()->AddEnemyActionByIdx(m_ActionId,pl);

	UIController::getInstance()->getPhysicsLayer()->addChild(pl);

	//��ӵ�����?
	int randNum = random(1,10);
	if (randNum <= 3)
	{
		DropReward dropReward;
		dropReward.DropTp = (DropGoodsType)randNum;
		pl->getPlanePro().DropVec.push_back(dropReward);
	}
}

