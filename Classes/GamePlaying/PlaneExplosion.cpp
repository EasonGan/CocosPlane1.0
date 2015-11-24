#include "PlaneExplosion.h"
#include "BasePlane/BasePlane.h"
#include "Layer/UIController.h"
#include "Layer/PhysicsLayer.h"
#include "../GameCommon/EffectsManager.h"
#include "../GameCommon/GameData.h"

//--------------------------------
SingleExpl* SingleExpl::create( float delayTime,bool isParticle )
{
	SingleExpl* se = new (std::nothrow) SingleExpl;
	if (se && se->initWithFile("weapon.png"))
	{
		se->autorelease();
		se->m_delayTime = delayTime;
		se->m_isParticle= isParticle;
		return se;
	}
	CC_SAFE_DELETE(se);
	return nullptr;
}

void SingleExpl::onEnter()
{
	Sprite::onEnter();
	if (m_delayTime > 0)
	{
		auto delay = DelayTime::create(m_delayTime);
		auto func = CallFunc::create(this,callfunc_selector(SingleExpl::Explosion));
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}
	else
	{
		Explosion();
	}
}

void SingleExpl::RemoveMe()
{
	removeFromParentAndCleanup(true);
}

void SingleExpl::Explosion()
{
	if (random(1,5) == 1)
	{
		int randNum = random(1,5);
		switch (randNum)
		{
		case 1:
			NewAudio::getInstance()->playEffect(Music_PlDead1);
			break;
		case 2:
			NewAudio::getInstance()->playEffect(Music_PlDead2);
			break;
		case 3:
			NewAudio::getInstance()->playEffect(Music_PlDead3);
			break;
		case 4:
			NewAudio::getInstance()->playEffect(Music_PlDead4);
			break;
		case 5:
			NewAudio::getInstance()->playEffect(Music_PlDead5);
			break;

		default:
			break;
		}
	}

	if (m_isParticle)
	{
		auto partc = EffectsManager::addParticle("particla/BossExpl/test9.plist", this, Vec2::ZERO,false,-1.0f, 100000);
		partc->setScale(1.5f);
	}
	auto sp = Sprite::create("weapon.png");
	sp->setRotation(rand_0_1() * 360);
	addChild(sp,100);
	sp->setScale(2.0f);

	//序列帧
	auto animat = EffectsManager::getFramesAnimation("PlaneDeadExpl_", 1.0f/60);
	auto rm = CallFunc::create(this, callfunc_selector(SingleExpl::RemoveMe));
	auto seq = Sequence::create(animat, rm, nullptr);
	sp->runAction(seq);
}

//-------------------------------
BaseExplosion::BaseExplosion()
{
	m_BasePlane = nullptr;
}

void BaseExplosion::setPlane( BasePlane* pl )
{
	m_BasePlane = pl;
}

void BaseExplosion::PlaneDeath()
{
	if (m_BasePlane)
	{
		m_BasePlane->DeathForce();
	}
	else
	{
		removeFromParentAndCleanup(true);
	}
}

void BaseExplosion::StartExplosion()
{
	//开始爆炸
	if (m_BasePlane)
	{
		m_BasePlane->stopAllActions();
		m_BasePlane->getPhysicsBody()->setCollisionBitmask(0);
		m_BasePlane->getBasePro().setHurts(0);
		m_BasePlane->getHpBar()->SetHpBarVisible(false);
	}
}

void BaseExplosion::OneExplosionCircle( float radius, float delay, float scale,const Vec2& offset )
{
	//每次旋转一定的角度
	int num = 10;
	float angle = (2*PI)/num;
	for (int i = 0; i < num; i++)
	{
		auto newVec = offset + Vec2(0,radius + rand_minus1_1() * 20);
		auto vec = newVec.rotateByAngle(offset, angle * i);
		auto expl = CommonExpl(vec, delay + rand_0_1() * 0.25);
		expl->setScale(scale + rand_minus1_1() * 0.05);
	}
}

void BaseExplosion::RemovePlaneAfterTime( float t )
{
	auto delay = DelayTime::create(t);
	auto func = CallFunc::create(this, callfunc_selector(BaseExplosion::PlaneDeath));
	auto seq = Sequence::create(delay, func, nullptr);
	seq->setTag(RemoveTag);
	runAction(seq);
}

std::vector<GroupExpl> BaseExplosion::getGroupExpl( int num /*= 4*/ )
{
	auto s = Size(100,100);
	if (m_BasePlane)
	{
		s = m_BasePlane->getSkinSize();
	}

	float disY = s.height/num;
	float interval = 1.0f/num;

	std::vector<GroupExpl> vec;
	for (int i = 0; i < num; ++i)
	{
		GroupExpl group;
		group.pos = Vec2(rand_minus1_1() * 2, -s.height * 0.5 + disY * i);
		group.delayTime = interval * i;

		vec.push_back(group);
	}

	return vec;
}

SingleExpl* BaseExplosion::CommonExpl( const Vec2& pos, float t,bool isParticle,bool isTail )
{
	SingleExpl* expl = SingleExpl::create(t, isParticle);
	expl->setPosition(pos);
	if (isTail)
	{
		UIController::getInstance()->getPhysicsLayer()->addChild(expl);
	}
	else
	{
		addChild(expl,5);
	}
	return expl;
}

void BaseExplosion::onEnter()
{
	Node::onEnter();

}

void BaseExplosion::PlayParticle1( float time, float scale, int Zorder )
{
	Node* node = Node::create();
	node->setScale(scale);
	addChild(node, Zorder);

	auto delay = DelayTime::create(time);
	auto func = CallFuncN::create(this, callfuncN_selector(BaseExplosion::addParticle1));
	auto seq = Sequence::create(delay, func, nullptr);
	node->runAction(seq);
}

void BaseExplosion::PlayParticle2( float time, float scale, int Zorder  )
{
	Node* node = Node::create();
	node->setScale(scale);
	addChild(node, Zorder);

	auto delay = DelayTime::create(time);
	auto func = CallFuncN::create(this, callfuncN_selector(BaseExplosion::addParticle2));
	auto seq = Sequence::create(delay, func, nullptr);
	node->runAction(seq);
}

void BaseExplosion::PlayParticle3( float time, float scale, int Zorder  )
{
	Node* node = Node::create();
	node->setScale(scale);
	addChild(node, Zorder);

	auto delay = DelayTime::create(time);
	auto func = CallFuncN::create(this, callfuncN_selector(BaseExplosion::addParticle3));
	auto seq = Sequence::create(delay, func, nullptr);
	node->runAction(seq);
}

void BaseExplosion::PlayParticle4( float time, float scale, int Zorder  )
{
	Node* node = Node::create();
	node->setScale(scale);
	addChild(node, Zorder);

	auto delay = DelayTime::create(time);
	auto func = CallFuncN::create(this, callfuncN_selector(BaseExplosion::addParticle4));
	auto seq = Sequence::create(delay, func, nullptr);
	node->runAction(seq);
}

void BaseExplosion::PlayParticle5( float time, float scale, int Zorder  )
{
	Node* node = Node::create();
	node->setScale(scale);
	addChild(node, Zorder);

	auto delay = DelayTime::create(time);
	auto func = CallFuncN::create(this, callfuncN_selector(BaseExplosion::addParticle5));
	auto seq = Sequence::create(delay, func, nullptr);
	node->runAction(seq);
}

void BaseExplosion::PlayParticle6( float time, float scale, int Zorder  )
{
	Node* node = Node::create();
	node->setScale(scale);
	addChild(node, Zorder);

	auto delay = DelayTime::create(time);
	auto func = CallFuncN::create(this, callfuncN_selector(BaseExplosion::addParticle6));
	auto seq = Sequence::create(delay, func, nullptr);
	node->runAction(seq);
}

void BaseExplosion::PlayParticle7( float time, float scale, int Zorder  )
{
	Node* node = Node::create();
	node->setScale(scale);
	addChild(node, Zorder);

	auto delay = DelayTime::create(time);
	auto func = CallFuncN::create(this, callfuncN_selector(BaseExplosion::addParticle7));
	auto seq = Sequence::create(delay, func, nullptr);
	node->runAction(seq);
}

void BaseExplosion::PlayParticle8( float time, float scale, int Zorder  )
{
	Node* node = Node::create();
	node->setScale(scale);
	addChild(node, Zorder);

	auto delay = DelayTime::create(time);
	auto func = CallFuncN::create(this, callfuncN_selector(BaseExplosion::addParticle8));
	auto seq = Sequence::create(delay, func, nullptr);
	node->runAction(seq);
}

void BaseExplosion::PlayParticle9( float time, float scale, int Zorder  )
{
	Node* node = Node::create();
	node->setScale(scale);
	addChild(node, Zorder);

	auto delay = DelayTime::create(time);
	auto func = CallFuncN::create(this, callfuncN_selector(BaseExplosion::addParticle9));
	auto seq = Sequence::create(delay, func, nullptr);
	node->runAction(seq);
}

void BaseExplosion::addParticle1(Node* node)
{
	auto patc =  EffectsManager::addParticle("particla/BossExpl/test1.plist", node, Vec2(0,0),false);
}

void BaseExplosion::addParticle2(Node* node)
{
	auto patc =  EffectsManager::addParticle("particla/BossExpl/test2.plist", node, Vec2(0,0),false);
}

void BaseExplosion::addParticle3(Node* node)
{
	auto patc =  EffectsManager::addParticle("particla/BossExpl/test3.plist", node, Vec2(0,0),false);
}

void BaseExplosion::addParticle4(Node* node)
{
	auto patc =  EffectsManager::addParticle("particla/BossExpl/test4.plist", node, Vec2(0,0),false);
}

void BaseExplosion::addParticle5(Node* node)
{
	auto patc =  EffectsManager::addParticle("particla/BossExpl/test5.plist", node, Vec2(0,0),false);
}

void BaseExplosion::addParticle6(Node* node)
{
	auto patc =  EffectsManager::addParticle("particla/BossExpl/test6.plist", node, Vec2(0,0),false);
}

void BaseExplosion::addParticle7(Node* node)
{
	auto patc =  EffectsManager::addParticle("particla/BossExpl/test7.plist", node, Vec2(0,0),false);
}

void BaseExplosion::addParticle8(Node* node)
{
	auto patc =  EffectsManager::addParticle("particla/BossExpl/test8.plist", node, Vec2(0,0),false);
}

void BaseExplosion::addParticle9(Node* node)
{
	auto patc =  EffectsManager::addParticle("particla/BossExpl/test9.plist", node, Vec2(0,0),false);
}

//--------------------------------------
void NorEnemyPlaneExpl::StartExplosion()
{
	BaseExplosion::StartExplosion();
	int randNum = random(1,10);

	if (randNum < 2)
	{
		ExplosionRandMove();
	}
	else if(randNum < 3)
	{
		ExplosionFalling();
	}
	else
	{
		ExplosionNoMove();
	}
}

void NorEnemyPlaneExpl::ExplosionNoMove()
{
	//MusicNoMove();
	auto myPos = convertToWorldSpaceAR(Vec2::ZERO);

	auto expl = CommonExpl(myPos, 0, true, true);
	expl->setScale(0.7 + rand_0_1() * 0.3);

	RemovePlaneAfterTime(0.1f);
}

void NorEnemyPlaneExpl::ExplosionRandMove()
{
	RemovePlaneAfterTime(1.5f);
	setPosition(0,80);
	auto mypos = convertToWorldSpaceAR(Vec2::ZERO);
	auto s = Director::getInstance()->getWinSize();

	Vec2 v = Vec2::ZERO;
	if (mypos.x > s.width * 0.5)
	{
		v = Vec2(-random(1000.0f, 1500.0f) * 0.5,random(1000.0f, 1500.0f)*(random(0,1) - 0.5));
	}
	else
	{
		v = Vec2(random(1000.0f, 1500.0f) * 0.5,random(1000.0f, 1500.0f)*(random(0,1) - 0.5));
	}
	
	if (m_BasePlane)
	{
		auto body = m_BasePlane->getPhysicsBody();
		body->setVelocity(v);

		auto rot = RotateBy::create(1.0f, 720 + random(0,1) * 1440);
		auto rep = RepeatForever::create(rot);
		m_BasePlane->runAction(rep);
	}

	SingleExplRandMove();
}

void NorEnemyPlaneExpl::ExplosionFalling()
{
	if (m_BasePlane)
	{
		auto body = m_BasePlane->getPhysicsBody();
		body->setGravityEnable(true);
		Vec2 v = Vec2(rand_minus1_1(),rand_minus1_1());
		body->setVelocity(v * 200);

		auto rot = RotateBy::create(3.0f, 720 + random(0,1) * 1440);
		auto rep = RepeatForever::create(rot);
		m_BasePlane->runAction(rep);
	}

	SingleExplFalling();
}

void NorEnemyPlaneExpl::SingleExplRandMove()
{
	auto myPos = convertToWorldSpaceAR(Vec2::ZERO);

	auto expl = CommonExpl(myPos, 0.0f, false, true);
	expl->setScale(0.7 + rand_minus1_1() * 0.2);

	auto delay = DelayTime::create(0.05f +rand_0_1() * 0.1);
	auto next = CallFunc::create(this, callfunc_selector(NorEnemyPlaneExpl::SingleExplRandMove));
	auto seq = Sequence::create(delay, next, nullptr);
	runAction(seq);

}

void NorEnemyPlaneExpl::SingleExplFalling()
{
	auto mypos = convertToWorldSpaceAR(Vec2::ZERO);
	auto expl = CommonExpl(mypos, 0.0f, false, true);
	expl->setScale(0.5 + rand_minus1_1() * 0.2);

	auto delay = DelayTime::create(0.1f +rand_0_1() * 0.08);
	auto next = CallFunc::create(this, callfunc_selector(NorEnemyPlaneExpl::SingleExplFalling));
	auto seq = Sequence::create(delay, next, nullptr);
	runAction(seq);
}

void NorEnemyPlaneExpl::MusicNoMove()
{
	int randNum = random(1,3);
	if (randNum == 1)
	{
		//NewAudio::getInstance()->playEffect(Music_Explosion_1);
	}
	else if(randNum == 2)
	{
		//NewAudio::getInstance()->playEffect(Music_Explosion_2);
	}
	else
	{
		NewAudio::getInstance()->playEffect(Music_PlDead2);
	}
	auto delay = DelayTime::create(0.05 + rand_0_1() * 0.05);
	auto func = CallFunc::create(this, callfunc_selector(NorEnemyPlaneExpl::MusicNoMove));
	auto seq = Sequence::create(delay, func, nullptr);
	auto rep = Repeat::create(seq, random(5,6));
	runAction(rep);
}

//--------------------------------------
void SpecialEnemyPlaneExpl::StartExplosion()
{
	BaseExplosion::StartExplosion();
	ExplosionGroups();
	ExplosionRand();
	ExplosionCircle();
	Particle1(2.7);
	{
		auto delay = DelayTime::create(2.7);
		auto func = CallFunc::create(this, callfunc_selector(SpecialEnemyPlaneExpl::HidePlane));
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}
}

void SpecialEnemyPlaneExpl::ExplosionGroups()
{
	auto vec = getGroupExpl(random(4,7));
	for (int i = 0; i < vec.size(); ++i)
	{
		GroupExpl ge = vec.at(i);
		auto expl = CommonExpl(ge.pos, 0.1f * i);
		expl->setScale(0.35);
	}
}

void SpecialEnemyPlaneExpl::ExplosionRand()
{
	float wait = 0.7;
	auto s = Size(100,100);

	float t = 0;

	if (m_BasePlane)
	{
		s = m_BasePlane->getSkinSize();
	}
	for (int i = 0; i < 10; ++i)
	{
		t = wait + random(0.15,0.3) * i;
		auto expl = CommonExpl(Vec2(s.width*0.5 * rand_minus1_1(), s.height * 0.5 * rand_minus1_1()), t);
		expl->setScale(random(0.7,1.0));
	}
}

void SpecialEnemyPlaneExpl::ExplosionCircle()
{
	float wait = 2.0f;

	OneExplosionCircle(30, wait, 0.4);
	OneExplosionCircle(70, wait + 0.25, 0.5);
	OneExplosionCircle(150, wait + 0.8, 0.5);

	auto delay = DelayTime::create(wait + 0.8);
	auto func = CallFunc::create(this, callfunc_selector(SpecialEnemyPlaneExpl::MidExpl));
	auto seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void SpecialEnemyPlaneExpl::Particle1(float t)
{
	PlayParticle7(t,0.5, 6);
	PlayParticle6(t,0.5, 6);
	PlayParticle1(t + 0.2,0.4);
	PlayParticle3(t + 0.2,0.8);

	RemovePlaneAfterTime(t + 1.2f);
}

void SpecialEnemyPlaneExpl::MidExpl()
{
	auto expl = CommonExpl(Vec2::ZERO, 0);
	expl->setScale(2.0f);
}

void SpecialEnemyPlaneExpl::HidePlane()
{
	if (!m_BasePlane)
	{
		return;
	}
	EnemyPlane* plane = (EnemyPlane*)m_BasePlane;
	auto armat = plane->getPlaneArmature();
	auto pic = plane->getPicture();
	if (pic)
	{
		pic->setVisible(false);
	}
	else if(armat)
	{
		armat->setVisible(false);
	}
}

//---------------------------------
void BossExplosion::StartExplosion()
{
	BaseExplosion::StartExplosion();
	ExplosionRandPos();
	ExplosionRandDelay(0.0f);

	ExplosionRandDelay(1.0f);
	ExplosionRandDelay(1.3f);

	ExplosionRandDelay(2.3f);

	/*ExplosionRandDelay(3.0f);
	ExplosionRandDelay(3.5f);
	ExplosionRandDelay(3.7f);
	ExplosionRandDelay(3.9f);*/

	LastExplosionDelay(3.2f);
}

void BossExplosion::ExplosionRandDelay( float t )
{
	if (t > 0)
	{
		auto delay = DelayTime::create(t);
		auto func = CallFunc::create(this, callfunc_selector(BossExplosion::ExplosionRand));
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}
	else
	{
		ExplosionRand();
	}
}

void BossExplosion::ExplosionRand()
{
	EffectsManager::NodeShake(m_BasePlane,0.3);

	float radius = 50;
	float wait = 0;
	float scale = 1.0f;
	Vec2 offset = Vec2::ZERO;

	for (int i = 0; i < 3; ++i)
	{
		radius = 20 + rand_0_1() * 100;
		wait = 0.1 + rand_0_1() * 0.02;
		scale = 0.7 + rand_minus1_1() * 0.3;
		offset = Vec2(random(-2,2) * 50, random(-2,2) * 50);
		OneExplosionCircle(radius, wait, scale, offset);
	}
	PlayParticle5(0.1, 1.0);
	PlayParticle3(0.1, 1.0);
	PlayParticle7(0, 0.4 + rand_0_1() * 0.3, 6);
	PlayParticle6(0, 0.2, 10);
}

void BossExplosion::ExplosionRandPosDelay( float t )
{
	auto delay = DelayTime::create(t);
	auto next = CallFunc::create(this, callfunc_selector(BossExplosion::ExplosionRandPos));
	auto seq = Sequence::create(delay, next, nullptr);
	runAction(seq);
}

void BossExplosion::ExplosionRandPos()
{
	float radius = 50 + rand_0_1() * 20;
	float wait = 0.1;
	float scale = 0.5 + rand_0_1() * 0.3;
	Vec2 offset = Vec2(random(-3,3) * 30, random(-3,3) * 30);

	auto expl = CommonExpl(offset, 0, true);
	expl->setScale(1.2f);

	OneExplosionCircle(radius, 0.0, scale, offset);

	{
		auto delay = DelayTime::create(0.3 + rand_0_1() * 0.2);
		auto next = CallFunc::create(this, callfunc_selector(BossExplosion::ExplosionRandPos));
		auto seq = Sequence::create(delay, next, nullptr);
		runAction(seq);
	}
}

void BossExplosion::LastExplosionDelay( float t )
{
	auto delay = DelayTime::create(t);
	auto next = CallFunc::create(this, callfunc_selector(BossExplosion::LastExplosion));
	auto seq = Sequence::create(delay, next, nullptr);
	runAction(seq);
}

void BossExplosion::LastExplosion()
{
	//GameData::getInstance()->GameSlowDown(1.0f);
	//UIController::getInstance()->getPhysicsLayer()->ToBlack(1.0f);
	NotificationCenter::getInstance()->postNotification(Msg_Shake,nullptr);
	EffectsManager::NodeShake(m_BasePlane,0.3);

	if (m_BasePlane)
	{
		m_BasePlane->DeathImpactOfDropReward();
	}

	ExplosionRandDelay(0.15);

	PlayParticle1(0.2,1.0,1);
	PlayParticle2(0.2,1.0,2);
	PlayParticle3(0.2,1.0,4);
	PlayParticle4(0.3,1.0,6);
	//PlayParticle5(0.15,1.0,3);
	PlayParticle6(0.1,1.0,10);
	PlayParticle7(0.1,1.0,4);

	RemovePlaneAfterTime(1.0f);

	{
		auto delay = DelayTime::create(0.2);
		auto func = CallFunc::create(this, callfunc_selector(BossExplosion::HideBoss));
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}
}

void BossExplosion::HideBoss()
{
	if (!m_BasePlane)
	{
		return;
	}
	BaseBoss* boss = (BaseBoss*)m_BasePlane;
	boss->getArmature()->setVisible(false);
}
