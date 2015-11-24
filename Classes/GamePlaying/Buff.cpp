#include "Buff.h"
#include "../GameCommon/GameDefine.h"
#include "../GameCommon/GameData.h"
#include "../GameCommon/GameConfig.h"
#include "Weapon/weapon.h"
#include "Weapon/WeaponFactory.h"
#include "Bullet/Bullet.h"
#include "Layer/UIController.h"
#include "Layer/PhysicsLayer.h"

BaseBuff* BaseBuff::create(BuffManager* buffmanager)
{
	auto buff = new (std::nothrow) BaseBuff;
	if (buff && buff->init())
	{
		buff->setManager(buffmanager);
		buff->autorelease();
		return buff;
	}
	else
	{
		CC_SAFE_DELETE(buff);
		return nullptr;
	}
}

void BaseBuff::setManager( BuffManager* buffmanager )
{
	m_buffManager = buffmanager;
}

void BaseBuff::ActiveBuff( const BuffId& buffId, const float& duration )
{
	m_buffid = buffId;
	m_maxTime = duration;
	m_LeftTime = duration;

	//执行激活BUFF函数
	bool isActive = m_buffManager->isBuffActive(buffId);
	if (!isActive)
	{
		addBUffById(buffId);
		schedule(schedule_selector(BaseBuff::CutDown), 0.1f, CC_REPEAT_FOREVER, 1.0f);
	}
}

void BaseBuff::CutDown(float t)
{
	m_LeftTime -= 0.1f;
	//log("--left time = %f",m_LeftTime);
	if (m_LeftTime <= 0.0)
	{
		m_LeftTime = 0.0;
		removeBuff();
	}
}

float BaseBuff::getLeftTime()
{
	return m_LeftTime;
}

BuffId BaseBuff::getBuffId()
{
	return m_buffid;
}

void BaseBuff::removeBuff()
{
	//执行销毁BUFF函数
	m_buffManager->DelBuff(this);
	removeBuffById(m_buffid);

	this->removeFromParentAndCleanup(true);
}

void BaseBuff::addBUffById(const BuffId& buffid )
{
	switch (buffid)
	{
	case BuffId::Buff_default:
		break;
	case BuffId::Buff_Sandan:
		{
			
			break;
		}
	case BuffId::Buff_Burning:
		{
			
			break;
		}
	case BuffId::Buff_TempNorWp1:
	case BuffId::Buff_TempNorWp2:
	case BuffId::Buff_TempNorWp3:
		{
			//主飞机的最终武器
			int planeType = NewDataMgr::getInstance()->mRoleConfig.roleIndex;
			int planeLvl = NewDataMgr::getInstance()->mGameRoleSystem->getRoleGradeWithID( planeType );
			auto mainpl = GameData::getInstance()->getMainPlane();
			auto weaponFact = GameData::getInstance()->getWeaponFactory();
			planeLvl = (planeLvl-1)/12 + 1;
			planeLvl++;
			if (planeLvl > 5 )
			{
				planeLvl = 5;
			}
			weaponFact->AddMainPlaneWp(planeType, planeLvl, mainpl, true);
		}
		break;
	case BuffId::Buff_LightingLvl1:
		{
			auto cardType = NewDataMgr::getInstance()->mRoleConfig.cardArr[2].cardID;
			auto cardLvl = NewDataMgr::getInstance()->mRoleConfig.cardArr[2].cardStarLv;
			cardType = cardType%3;
			int hurt = DataConfig::getCardHurts(DropGoodsType::DropGoods_YellowWp,cardType, cardLvl);
			hurt /= 24;
			NewLightingLayer* layer = NewLightingLayer::create(1, hurt);
			UIController::getInstance()->getPhysicsLayer()->addChild(layer,-2000);
			layer->setTag(5191802);

		}
		break;
	case BuffId::Buff_LightingLvl2:
		{
			auto cardType = NewDataMgr::getInstance()->mRoleConfig.cardArr[2].cardID;
			auto cardLvl = NewDataMgr::getInstance()->mRoleConfig.cardArr[2].cardStarLv;
			cardType = cardType%3;
			int hurt = DataConfig::getCardHurts(DropGoodsType::DropGoods_YellowWp,cardType, cardLvl);
			hurt /= 24;

			NewLightingLayer* layer = NewLightingLayer::create(2,hurt);
			UIController::getInstance()->getPhysicsLayer()->addChild(layer,-2000);
			layer->setTag(5191802);

		}
		break;
	case BuffId::Buff_LightingLvl3:
		{
			auto cardType = NewDataMgr::getInstance()->mRoleConfig.cardArr[2].cardID;
			auto cardLvl = NewDataMgr::getInstance()->mRoleConfig.cardArr[2].cardStarLv;
			cardType = cardType%3;
			int hurt = DataConfig::getCardHurts(DropGoodsType::DropGoods_YellowWp,cardType, cardLvl);
			hurt /= 24;
			NewLightingLayer* layer = NewLightingLayer::create(3, hurt);
			UIController::getInstance()->getPhysicsLayer()->addChild(layer,-2000);
			layer->setTag(5191802);

		}
		break;
	case BuffId::Buff_Magnet:
		{
			auto mainpl = GameData::getInstance()->getMainPlane();
			if (mainpl)
			{
				MainPlane* pl = (MainPlane*) mainpl;
				pl->addMagnet();
			}
		}
		break;
	default:
		break;
	}
	m_buffManager->addBuff(this);
}

void BaseBuff::removeBuffById(const BuffId& buffid )
{
	switch (buffid)
	{
	case BuffId::Buff_default:
		break;
	case BuffId::Buff_Sandan:
		{
		
			break;
		}
	case BuffId::Buff_Burning:
		{
			
			break;
		}
	case BuffId::Buff_TempNorWp1:
	case BuffId::Buff_TempNorWp2:
	case BuffId::Buff_TempNorWp3:
		{
			Node* node = getParent();
			MainPlane* pl = (MainPlane*)node;
			pl->resumeNorWp();
		}
		break;;
	case BuffId::Buff_LightingLvl1:
	case BuffId::Buff_LightingLvl2:
	case BuffId::Buff_LightingLvl3:
		{
			auto node = UIController::getInstance()->getPhysicsLayer()->getChildByTag(5191802);
			if (node)
			{
				node->removeFromParentAndCleanup(true);
			}
		}
		break;
	case BuffId::Buff_Magnet:
		{
			auto mainpl = GameData::getInstance()->getMainPlane();
			if (mainpl)
			{
				MainPlane* pl = (MainPlane*) mainpl;
				pl->removeMagnet();
			}
		}
		break;
	default:
		break;
	}
}

void BaseBuff::resetTime()
{
	m_LeftTime = m_maxTime;
}





//---------------------------------
void BuffManager::addBuff( BaseBuff* buff )
{
	m_buffVec.push_back(buff);
}

void BuffManager::DelBuff( BaseBuff* buff )
{
	std::vector<BaseBuff*>::iterator it = m_buffVec.begin();

	for (it; it != m_buffVec.end(); it++)
	{
		BaseBuff* bb = (*it);
		if (bb->getBuffId() == buff->getBuffId())
		{
			m_buffVec.erase(it);
			break;
		}
	}
}

bool BuffManager::isBuffActive( const BuffId& buffid )
{
	bool isActive = false;
	std::vector<BaseBuff*>::iterator it = m_buffVec.begin();

	for (it; it != m_buffVec.end(); it++)
	{
		BaseBuff* bb = (*it);
		if (bb->getBuffId() == buffid)
		{
			isActive = true;
			break;
		}
	}

	return isActive;
}

void BuffManager::resetBuff(const BuffId& buffid)
{
	std::vector<BaseBuff*>::iterator it = m_buffVec.begin();

	for (it; it != m_buffVec.end(); it++)
	{
		BaseBuff* bb = (*it);
		if (bb->getBuffId() == buffid)
		{
			bb->resetTime();
			break;
		}
	}

}
