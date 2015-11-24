#include "BossTimeOut.h"
#include "../GameCommon/GameData.h"
#include "../GameCommon/EffectsManager.h"
#include "Layer/UIController.h"
#include "Layer/PhysicsLayer.h"


BossTimeOut::BossTimeOut()
{
	m_maxTime = 30.0f;
	m_interval = 1.5f;
	m_curRandPos = 0;
}

void BossTimeOut::onEnter()
{
	Node::onEnter();
	TimeCutdown();

}

void BossTimeOut::setMaxTime(const float& time )
{
	m_maxTime = time;
}

void BossTimeOut::TimeCutdown()
{
	ActionInterval* delay = DelayTime::create(m_maxTime);
	CallFunc* func = CallFunc::create(this,callfunc_selector(BossTimeOut::TimeOut));
	ActionInterval* seq = Sequence::create(delay,func,nullptr);

	runAction(seq);
}

void BossTimeOut::TimeOut()
{
	schedule(schedule_selector(BossTimeOut::speedUp),0.25f);
	StartTiming();
}

void BossTimeOut::Fallingstone()
{
	FallingStone* stone = FallingStone::create();
	stone->setTag(20150924);
	stone->setPosition(randPos());
	UIController::getInstance()->getPhysicsLayer()->addChild(stone);

	StartTiming();
}

cocos2d::Vec2 BossTimeOut::randPos()
{
	int randNum = cocos2d::random(0,20);
	if (randNum == m_curRandPos)
	{
		randPos();
	}
	m_curRandPos = randNum;
	auto ws = Director::getInstance()->getWinSize();
	return Vec2(ws.width/20 * randNum + rand_minus1_1() * 10, ws.height + 70);
}

void BossTimeOut::speedUp(float dt)
{
	m_interval -= 0.01;
	if (m_interval < 0.3f)
	{
		m_interval = 0.3f;
	}
}

void BossTimeOut::setInterval( const float& time )
{
	m_interval = time;
}

void BossTimeOut::StartTiming()
{
	ActionInterval* delay = DelayTime::create(m_interval);
	CallFunc* func = CallFunc::create(this,callfunc_selector(BossTimeOut::Fallingstone));
	ActionInterval* seq = Sequence::create(delay,func,nullptr);
	runAction(seq);
}

