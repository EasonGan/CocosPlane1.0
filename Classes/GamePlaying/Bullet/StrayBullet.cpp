#include "StrayBullet.h"
#include "Bullet.h"
#include "BulletFactory.h"
#include "../Layer/UIController.h"
#include "../Layer/PhysicsLayer.h"
#include "../../GameCommon/GameData.h"
#include "GameCommon/EffectsManager.h"




StrayBulletLayer* StrayBulletLayer::create(const float& t )
{
	StrayBulletLayer* sbl = new (std::nothrow) StrayBulletLayer;
	if (sbl && sbl->init())
	{
		sbl->setDuration(t);
		sbl->autorelease();
		return sbl;
	}
	else
	{
		CC_SAFE_DELETE(sbl);
		return nullptr;
	}
}

bool StrayBulletLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	m_duration = 0.0f;
	m_winSize = Director::getInstance()->getWinSize();
	return true;
}

void StrayBulletLayer::onEnter()
{
	Layer::onEnter();

	//持续时间
	ActionInterval* duration = DelayTime::create(m_duration);
	CallFunc* removeFunc = CallFunc::create(this,callfunc_selector(StrayBulletLayer::removeWhenTimeout));
	ActionInterval* seq = Sequence::create(duration,removeFunc,nullptr);
	runAction(seq);

	schedule(schedule_selector(StrayBulletLayer::shootStrayBullet),0.1);

	
}

void StrayBulletLayer::shootStrayBullet( float t )
{
	 auto bullet = GameData::getInstance()->getBulletFactory()->createStrayBullet();
	 bullet->setPosition(30 + (m_winSize.width - 30)* rand_0_1(),-50);
	 UIController::getInstance()->getEffectsLayer()->addChild(bullet,1);
}

void StrayBulletLayer::removeWhenTimeout()
{
	removeFromParentAndCleanup(true);
}

void StrayBulletLayer::setDuration(const float& t )
{
	m_duration = t;
}

