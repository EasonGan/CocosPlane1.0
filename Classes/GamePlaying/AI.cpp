#include "AI.h"
#include "BasePlane/BasePlane.h"
#include "Weapon/weapon.h"
#include "Weapon/WeaponFactory.h"
#include "../GameCommon/GameData.h"
#include "../GameCommon/EffectsManager.h"
#include "../GameCommon/GameConfig.h"
#include "../GameCommon/GameDefine.h"
#include "../../cocos2d/cocos/editor-support/cocostudio/CCArmature.h"
#include "../GameCommon/GlobalSchedule.h"
#include "Bullet/BulletFactory.h"
#include "Layer/UIController.h"
#include "Layer/PhysicsLayer.h"
#include "Bullet/Bullet.h"
#include "BasePlane/PlaneManager.h"
#include "Weapon/EditorWeapon.h"
#include "BasePlane/BossHpBar.h"


bool PlaneAI::init()
{
	if (!Node::init())
	{
		return false;
	}
	m_speed = 0.3;
	m_winSize = Director::getInstance()->getWinSize();

	return true;
}

float PlaneAI::getShootInterval()
{
	auto curLvl = GameData::getInstance()->getPlaneManager()->getCurLvl();
	float interval = m_ShootInterval - curLvl * 0.1;		//随着关卡等级提高，发射间隔减少
	return interval;
}

void PlaneAI::StartShoot()
{
	auto func = CallFunc::create(this, callfunc_selector(PlaneAI::shoot));
	auto delay = DelayTime::create(getShootInterval());
	auto seq = Sequence::create(func, delay, nullptr);
	auto rep = RepeatForever::create(seq);
	runAction(rep);
}

void PlaneAI::shoot()
{
	EnemyPlane* pl = dynamic_cast<EnemyPlane*>(m_target);
	if (!pl)
	{
		return;
	}

    //飞机皮肤由图片创建
	auto pic = pl->getPicture();
	if (pic)
	{
		pl->addWeapon();
		return;
	}

    //飞机皮肤由骨骼创建
	auto armat = pl->getPlaneArmature();
	if (armat)
	{
		armat->playShootAnimation();
	}
}

void PlaneAI::FixPositionBeforeNext()
{
	auto mv2Mid = MoveTo::create(0.5f, Vec2(m_winSize.width*0.5, m_winSize.height * 0.75));
	auto next = CallFunc::create(this,callfunc_selector(PlaneAI::NextAction));
	auto seq = Sequence::create(mv2Mid, next, nullptr);
	m_target->runAction(seq);
}

bool EnemyAiOfMoveDown::init()
{
	if (!PlaneAI::init())
	{
		return false;
	}

	m_speed = 0.3;
	m_direc = (cocos2d::random(0,1) - 0.5);
	auto body = m_target->getPhysicsBody();
	body->setLinearDamping(5.0f);
	body->setGravityEnable(true);
	return true;
}

void EnemyAiOfMoveDown::StartAI(int lvl)
{
	m_AIlvl = lvl;
	StartShoot();
	schedule(schedule_selector(EnemyAiOfMoveDown::FloatDown),m_speed);
}

void EnemyAiOfMoveDown::FloatDown(float dt)
{
	auto body = m_target->getPhysicsBody();
	body->applyForce(Vec2(rand_minus1_1()*0.3,-rand_0_1()) * 100,Vec2(0,0));
}

//-----------------------------

bool EnemyAiOfFlyToTarget::init()
{
	if (!PlaneAI::init())
	{
		return false;
	}

	m_speed = 2.0f;

	return true;
}

void EnemyAiOfFlyToTarget::StartAI(int lvl)
{
	m_AIlvl = lvl;
	StartShoot();
	schedule(schedule_selector(EnemyAiOfFlyToTarget::FlyToTarget));
}

void EnemyAiOfFlyToTarget::FlyToTarget( float t )
{
	auto mainPl = GameData::getInstance()->getMainPlane();
	if (mainPl)
	{
        //朝向目标
		Vec2 myPos = m_target->convertToWorldSpaceAR(Vec2::ZERO);
		Vec2 targetPos = mainPl->convertToWorldSpaceAR(Vec2::ZERO);

		Vec2 movePos = MoveStep(targetPos,myPos,m_speed);

		m_target->setPosition(movePos);
	}
	else
	{
        //直线向下
		Vec2 myPos = m_target->convertToWorldSpaceAR(Vec2::ZERO);
		Vec2 targetPos = Vec2(myPos.x, -10000);

		Vec2 movePos = MoveStep(targetPos,myPos,m_speed);
		m_target->setPosition(movePos);
	}
}

//---------------------------
bool EnemyAiOfHoverAttack::init()
{
	if (!PlaneAI::init())
	{
		return false;
	}
	m_aimSp = nullptr;
	m_speed = 2.0f;

	return true;
}

void EnemyAiOfHoverAttack::StartAI(int lvl)
{
	m_AIlvl = lvl;
	Hover();
}

void EnemyAiOfHoverAttack::Hover()
 {
	m_target->stopAllActions();
	float dirct = cocos2d::random(0,1) - 0.5;
	auto pos = m_target->convertToWorldSpaceAR(Vec2::ZERO);
	float disX;
	if (dirct > 0)
	{
        //向右
		 disX = m_winSize.width * 0.75 - pos.x;
	}
	else
	{
		disX = m_winSize.width * 0.25 - pos.x;
	}
	disX = disX * cocos2d::random(0.5, 0.9);
	ActionInterval* moveby = MoveBy::create(1.0f - m_speed * 0.1f,ccp(disX,0));
	ActionInterval* moveback = moveby->reverse();
	ActionInterval* seq = Sequence::create(moveby,moveback,nullptr);
	ActionInterval* repeat = Repeat::create(seq,cocos2d::random(1,2));
	CallFunc* func = CallFunc::create(this,callfunc_selector(EnemyAiOfHoverAttack::AimMove));
	ActionInterval* seq1 = Sequence::create(repeat,func,nullptr);
	m_target->runAction(seq1);
}

void EnemyAiOfHoverAttack::AimMove()
{
	auto pl = GameData::getInstance()->getMainPlane();
	if (!pl)
	{
		Hover();		//主飞机不存在那就继续悬停
		return;
	}

	Size s = m_target->getContentSize();
	if (!m_aimSp)
	{
		m_aimSp = Sprite::create("Image/aim.png");
		m_aimSp->setPosition(s.width*0.5,s.height*0.5);
		m_target->addChild(m_aimSp);
	}

	m_aimSp->setVisible(true);

	//ÐèÒªÒÆ¶¯µÄ¾àÀë
	Vec2 myPos = m_target->convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 plPos = pl->convertToWorldSpaceAR(Vec2::ZERO);

	//Êµ¼ÊÄ¿±êÎ»ÖÃ
	m_aimPos = myPos - plPos;

	//Æ«ÉÏÎ»ÖÃ
	Vec2 offsetPos = Vec2(plPos.x, plPos.y + 100) - myPos;

	ActionInterval* moveby = MoveBy::create(0.7 - m_speed*0.1,offsetPos);
	ActionInterval* fadein = FadeIn::create(0.07f - m_speed*0.01);
	ActionInterval* fadeout = FadeOut::create(0.07f - m_speed*0.01);
	ActionInterval* seq = Sequence::create(fadein,fadeout,nullptr);
	ActionInterval* repeat = Repeat::create(seq,5);
	CallFunc* func = CallFunc::create(this,callfunc_selector(EnemyAiOfHoverAttack::AimMoveCallback));
	ActionInterval* seq1 = Sequence::create(moveby,repeat,func,nullptr);
	m_aimSp->runAction(seq1);
}

void EnemyAiOfHoverAttack::AimMoveCallback()
{
	m_aimSp->removeFromParentAndCleanup(true);
	m_aimSp = nullptr;

	//µÐ»ú³åÏòÄ¿±ê
	MoveToMainPl();
}

void EnemyAiOfHoverAttack::MoveToMainPl()
{
	auto pl = GameData::getInstance()->getMainPlane();
	if (!pl)
	{
		Hover();		//Ö÷·É»ú²»´æÔÚÄÇ¾Í¼ÌÐøÐüÍ£
		return;
	}

	//·É»úÆ¤·ôÓÉ¹Ç÷À´´½¨
	EnemyPlane* enePl = dynamic_cast<EnemyPlane*>(m_target);
	auto armat = enePl->getPlaneArmature();
	if (armat)
	{
		armat->playHackAnimation();
	}

	ActionInterval* moveTo = MoveBy::create(0.7f - m_speed*0.1,-m_aimPos);
	ActionInterval* moveback = moveTo->reverse();
	CallFunc* func = CallFunc::create(this,callfunc_selector(EnemyAiOfHoverAttack::Hover));
	ActionInterval* seq = Sequence::create(moveTo,moveback,func,nullptr);
	m_target->runAction(seq);
}

//------------------------
bool EnemyAiOfFloat::init()
{
	if (!PlaneAI::init())
	{
		return false;
	}
	m_speed = 2.0f;

	return true;
}

void EnemyAiOfFloat::StartAI(int lvl)
{
	m_AIlvl = lvl;
	m_OriginPos = m_target->convertToWorldSpaceAR(Vec2::ZERO);

	StartShoot();
	RandFloat();

}

void EnemyAiOfFloat::RandFloat()
{
	Vec2 myPos = m_target->convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 endPos = myPos;
	if (myPos.x > m_winSize.width * 0.85)
	{
		endPos.x = m_winSize.width * 0.5  - rand_0_1() * m_winSize.width * 0.35;
	}
	else if(myPos.x < m_winSize.width * 0.15)
	{
		endPos.x = m_winSize.width * 0.5  + rand_0_1() * m_winSize.width * 0.35;
	}
	else
	{
		int randNum = cocos2d::random(0,1);
		if (randNum) //ÏòÓÒ
		{
			endPos.x = endPos.x + ( m_winSize.width * 0.9 - endPos.x ) * cocos2d::random(0.5f,1.0f);
		}
		else
		{
			endPos.x = endPos.x - ( endPos.x - m_winSize.width * 0.1 ) * cocos2d::random(0.5f,1.0f);
		}
	}

	ActionInterval* moveby = MoveTo::create(2.0f - m_speed * 0.1f,Vec2(endPos.x, m_OriginPos.y));
	CallFunc* next = CallFunc::create(this,callfunc_selector(EnemyAiOfFloat::RandFloat));
	ActionInterval* seq = Sequence::create(moveby,next,nullptr);
	m_target->runAction(seq);
}

//-----------------------------------

ActionInterval* BossAI::randMove()
{
	m_target->stopAllActions();
	auto mypos = m_target->convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 endpos;
	if (m_direct < 0)
	{
		//Ïò×ó
		endpos = Vec2((m_winSize.width * 0.25 - mypos.x) * random(0.5f, 1.0f),0);
	}
	else
	{
		//ÏòÓÒ
		endpos = Vec2((m_winSize.width * 0.75 - mypos.x) * random(0.5f, 1.0f),0);
	}

	ActionInterval* move = MoveBy::create( 3.0f, endpos);

	return move;
}

ActionInterval* BossAI::MoveToMidRight()
{
	Vec2 pos = Vec2(m_winSize.width * 0.63, getBossMidPos().y);
	auto dis = getDis(pos);

	ActionInterval* mvto = MoveTo::create(0.5f + dis/300 * 0.5, pos );
	return mvto;
}

ActionInterval* BossAI::MoveToMid()
{
	Vec2 pos = getBossMidPos();
	auto dis = getDis(pos);

	ActionInterval* mvto = MoveTo::create(0.5f + dis/300 * 0.5, pos );
	return mvto;
}

ActionInterval* BossAI::MoveToEdge()
{
	//ÏÈ¿ìËÙÒÆ¶¯µ½Æðµã
	int RandNum = cocos2d::random(0,1);
	float Randf = RandNum * 1.0f;
	Randf -= 0.5f;
	Vec2 startPos = Vec2( m_winSize.width*(0.5 + Randf * 0.4), m_winSize.height * 0.8);
	ActionInterval* mvStartPos = MoveTo::create(0.5f,startPos);

	//ÔÙÒÆ¶¯µ½ÆÁÄ»µÄÒ»¶Ë
	ActionInterval* mvToEdge = MoveTo::create(2.5f, Vec2(m_winSize.width*(0.5 - Randf * 0.4),
		m_winSize.height * 0.8));
	ActionInterval* seq = Sequence::create(mvStartPos,mvToEdge,nullptr);

	return seq;
}

ActionInterval* BossAI::MoveDown()
{
	//ÕÒµ½Ö÷½ÇµÄÎ»ÖÃ
	auto mypos = m_target->convertToWorldSpaceAR(Vec2::ZERO);
	ActionInterval* mv = MoveBy::create(0.5f,Vec2(0, -mypos.y * 0.8));
	ActionInterval* back = mv->reverse();
	ActionInterval* seq = Sequence::create(mv, back, nullptr);

	return seq;
}

ActionInterval* BossAI::MoveToMainPl()
{
	//ÕÒµ½Ö÷½ÇµÄÎ»ÖÃ
	auto pl = GameData::getInstance()->getMainPlane();
	Vec2 pos;
	if (!pl)
	{
		pos = Vec2(360,100);
	}
	else
	{
		pos = pl->convertToWorldSpaceAR(Vec2::ZERO);
		pos.y -= 50;
	}
	auto mypos = m_target->convertToWorldSpaceAR(Vec2::ZERO);
	ActionInterval* mv = MoveTo::create(0.5f,pos);
	ActionInterval* back = MoveTo::create(0.5f,mypos);
	ActionInterval* seq = Sequence::create(mv, back, nullptr);

	return seq;
}

float BossAI::getDis( const Vec2& pos )
{
	auto mypos = m_target->convertToWorldSpaceAR(Vec2::ZERO);
	return mypos.distance(pos);
}

void BossAI::playArmat( int idx,Ref *target, SEL_FrameEventCallFunc callFunc,float speedScale /*= 1.0f*/ )
{
	BaseBoss* boss = (BaseBoss*)m_target;
	auto armat = boss->getArmature();
	armat->getAnimation()->playWithIndex(idx);
	armat->getAnimation()->setFrameEventCallFunc(target,callFunc);  
	armat->getAnimation()->setSpeedScale(speedScale);
}

void BossAI::playArmat( int idx, float speedScale /*= 1.0f*/ )
{
	BaseBoss* boss = (BaseBoss*)m_target;
	auto armat = boss->getArmature();
	armat->getAnimation()->playWithIndex(idx);
	armat->getAnimation()->setSpeedScale(speedScale);
}

void BossAI::changeDirect()
{
	ActionInterval* delay = DelayTime::create(0.8 + rand_0_1()*1.5f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(BossAI::changeDirCB));
	ActionInterval* seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void BossAI::changeDirCB()
{
	m_direct *= -1.0f;

	changeDirect();
}

void BossAI::DeathAction()
{
	playArmat(0);
	PauseArmat(100000);
}

int BossAI::getBossHurts()
{
	int hurts = m_target->getBasePro().getHurts();
	return hurts;
}

int BossAI::getTagWithRate( const std::vector<int>& vec )
{
	if (vec.empty())
	{
		return 0;
	}
	int size = vec.size();
	int randnum = random(0, size - 1);

	return vec.at(randnum);
}

void BossAI::MoveLR()
{
	m_target->stopAllActions();
	//×óÓÒËæ»úÒÆ¶¯
	float t = 4.0 + rand_0_1() * 1.0;

	auto mypos = m_target->convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 mvDirect;
	if (mypos.x < m_winSize.width * 0.5)
	{
		mvDirect = Vec2(m_winSize.width * 0.85 - mypos.x, 0);
	}
	else
	{
		mvDirect = Vec2(m_winSize.width * 0.15 - mypos.x, 0);
	}
	auto mv = MoveBy::create(t, mvDirect);
	auto next = CallFunc::create(this, callfunc_selector(BossAI::MoveLR));
	auto seq = Sequence::create(mv, next, nullptr);

	m_target->runAction(seq);
}

Armature* BossAI::getArmat()
{
	BaseBoss* boss = (BaseBoss*)m_target;
	auto armat = boss->getArmature();
	return armat;
}

void BossAI::PauseArmat( float t )
{
	getArmat()->getAnimation()->pause();
	auto delay = DelayTime::create(t);
	auto func = CallFunc::create(this, callfunc_selector(BossAI::ResumeArmat));
	auto seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void BossAI::ResumeArmat()
{
	getArmat()->getAnimation()->resume();
}

void BossAI::NextActionDelay( float t )
{
	playArmat(0);
	auto delay = DelayTime::create(t);
	auto next = CallFunc::create(this, callfunc_selector(BossAI::NextAction));
	auto seq = Sequence::create(delay, next, nullptr);
	runAction(seq);
}

void BossAI::InitActionVec()
{
	m_ActionVec = initAction();
	m_it = m_ActionVec.begin();
}

std::vector<int> BossAI::initAction()
{
	std::vector<int> vec;
	vec.push_back(1);		//Ëæ±ã¸ø¸ö³õÊ¼Öµ
	return vec;
}

void BossAI::StartAI( int lvl /*= 1*/ )
{
	m_AIlvl = lvl;
	m_direct = cocos2d::random(0,1) - 0.5;
	changeDirect();
	MoveToMidRight();

	InitActionVec();
	NextAction();
}

int BossAI::getTag()
{
	int tag = 0;
	if (m_it == m_ActionVec.end())
	{
		auto vec = initAction();
		tag = getTagWithRate(vec);
	}
	else
	{
		tag = (*m_it);
		m_it++;
	}
	return tag;
}

void BossAI::SelectAction(int ActionId)
{

}

void BossAI::NextAction()
{
	playArmat(0);
	int tag = getTag();
	SelectAction(tag);
}

cocos2d::Vec2 BossAI::getBossMidPos()
{
	BaseBoss* boss = (BaseBoss*)m_target;

	return boss->getMidPos();
}

int BossAI::getLeftHpNum()
{
	BaseBoss* boss = (BaseBoss*)m_target;
	return boss->getBossHpBar()->getCurHpNum();
}

//----------------------------------
PlaneAIFactory* PlaneAIFactory::create()
{
	PlaneAIFactory* pf = new (std::nothrow) PlaneAIFactory;
	if (pf && pf->init())
	{
		pf->retain();
		pf->autorelease();
		return pf;
	}
	else
	{
		CC_SAFE_DELETE(pf);
		return nullptr;
	}
}

void PlaneAIFactory::purge()
{
	this->release();
}

bool PlaneAIFactory::init()
{
	return true;
}

PlaneAI* PlaneAIFactory::AddEnemyAiByIdx(BasePlane* pl, int id )
{
	PlaneAI* ai = nullptr;
	switch (id)
	{
	case 1:
		{
			//ÂýËÙÆ®Âä
			ai = EnemyAiOfMoveDown::create(pl);
			ai->setSpeed(0.4f);
		}
		break;
	case 2:
		{
			//¿ìËÙÆ®Âä
			ai = EnemyAiOfMoveDown::create(pl);
			ai->setSpeed(0.2f);
		}
		break;
	case 3:
		{
			//ÂýËÙ¸úËæÖ÷·É»ú
			ai = EnemyAiOfFlyToTarget::create(pl);
			ai->setSpeed(2.0f);
		}
		break;
	case 4:
		{
			//¿ìËÙ¸úËæÖ÷·É»ú
			ai = EnemyAiOfFlyToTarget::create(pl);
			ai->setSpeed(3.0f);
		}
		break;
	case 5:
		{
			//ÂýËÙÐü¸¡×óÓÒÒÆ¶¯
			ai = EnemyAiOfFloat::create(pl);
			ai->setSpeed(2.0f);
		}
		break;
	case 6:
		{
			//¿ìËÙÐü¸¡×óÓÒÒÆ¶¯
			ai = EnemyAiOfFloat::create(pl);
			ai->setSpeed(4.0f);
		}
		break;
	case 9:
		{
			ai = EnemyAiofStay::create(pl);
		}
		break;
	case 10:
		{
			ai = EnemyAiofFree::create(pl);
		}
		break;
	case 10000:
		{
			//ÂýËÙÐü¸¡×óÓÒÒÆ¶¯ + ¹¥»÷
			ai = EnemyAiOfHoverAttack::create(pl);
			ai->setSpeed(1.0f);
		}
		break;
	case 10001:
		{
			//¿ìËÙÐü¸¡×óÓÒÒÆ¶¯ + ¹¥»÷
			ai = EnemyAiOfHoverAttack::create(pl);
			ai->setSpeed(2.0f);
		}
		break;
	case 10002:
		{
			//ÂýËÙÐü¸¡×óÓÒÒÆ¶¯ + ¹¥»÷
			ai = EnemyAiOfHoverAttack::create(pl);
			ai->setSpeed(3.0f);
		}
		break;
	case 10003:
		{
			//¿ìËÙÐü¸¡×óÓÒÒÆ¶¯ + ¹¥»÷
			ai = EnemyAiOfHoverAttack::create(pl);
			ai->setSpeed(4.0f);
		}
		break;
	default:
		break;

	}
	if (ai)
	{
		pl->addChild(ai);
		ai->StartAI();
	}
	return ai;
}

std::vector<int> IronManBossAI::initAction()
{
	int HpNums = getLeftHpNum();
	std::vector<int>	tagVec;
	//Ëæ»ú
	switch (m_AIlvl)
	{
	case 1:
		{
			if (HpNums < 1)
			{
				tagVec.push_back(4);
				tagVec.push_back(1);
				tagVec.push_back(3);
			}
			else
			{
				tagVec.push_back(5);
				tagVec.push_back(3);
				tagVec.push_back(1);
			}
		}
		break;
	case 2:
		{
			if (HpNums < 1)
			{
				tagVec.push_back(6);
				tagVec.push_back(2);
				tagVec.push_back(7);
				tagVec.push_back(1);
			}
			else
			{
				tagVec.push_back(2);
				tagVec.push_back(3);
				tagVec.push_back(4);
				tagVec.push_back(6);
			}
		}
		break;
	default:
		break;
	}

	return tagVec;
}

void IronManBossAI::SelectAction(int ActionId)
{
	switch (ActionId)
	{
	case 1:
		Action1();
		break;
	case 2:
		Action2();
		break;
	case 3:
		Action3();
		break;
	case 4:
		Action4();
		break;
	case 5:
		Action5();
		break;
	case 6:
		Action6();
		break;
	case 7:
		Action7();
		break;
	default:
		break;
	}
}

void IronManBossAI::Action1()
{
	auto action = MoveToMidRight();
	CallFunc* shangtang = CallFunc::create(this,callfunc_selector(IronManBossAI::PlayArmatWithReady));
	ActionInterval* seq = Sequence::create(action,shangtang,nullptr);

	//×Óµ¯
	ActionInterval* ready = DelayTime::create(1.2f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(IronManBossAI::PlayArmatWith1Sword));
	ActionInterval* delay2 = DelayTime::create(0.3f);
	ActionInterval* seq1 = Sequence::create(func,delay2,nullptr);
	ActionInterval* rep = Repeat::create(seq1,10);
	CallFunc* func1 = CallFunc::create(this,callfunc_selector(IronManBossAI::NextAction));
	ActionInterval* seq2 = Sequence::create(seq,ready,rep,func1,nullptr);

	m_target->runAction(seq2);
	
}

void IronManBossAI::Action2()
{
	//ÒÆ¶¯
	auto action = MoveToMidRight();
	PlayArmatWithReady();

	//×Óµ¯
	ActionInterval* ready = DelayTime::create(0.6f);
	ActionInterval* delay = DelayTime::create(2.0f);
	CallFunc* func1 = CallFunc::create(this,callfunc_selector(IronManBossAI::PlayArmatWith3Dart));
	ActionInterval* shootDelay = DelayTime::create(0.35);
	CallFunc* func2 = CallFunc::create(this,callfunc_selector(IronManBossAI::PlayArmatWith2Dart));

	ActionInterval* seq = Sequence::create(func1,shootDelay,func2,delay,nullptr);
	ActionInterval* rep = Repeat::create(seq,2);

	CallFunc* func3 = CallFunc::create(this,callfunc_selector(IronManBossAI::NextAction));
	ActionInterval* seq2 = Sequence::create(action, ready, rep, func3, nullptr);
	m_target->runAction(seq2);
}

void IronManBossAI::Action3()
{
	auto action = randMove();

	PlayArmatWithReady();

	//×Óµ¯
	ActionInterval* ready = DelayTime::create(1.5f);
	ActionInterval* delay = DelayTime::create(1.0f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(IronManBossAI::PlayArmatWith3Sword));
	ActionInterval* seq = Sequence::create(func, delay, nullptr);
	ActionInterval* rep = Repeat::create(seq,3);
	ActionInterval* spa = Spawn::create(action, rep, nullptr);

	CallFunc* next = CallFunc::create(this,callfunc_selector(IronManBossAI::NextAction));
	ActionInterval* seq2 = Sequence::create(ready, spa, next, nullptr);
	m_target->runAction(seq2);
}

void IronManBossAI::Action4()
{
	//³åÏòÄ¿±ê1
	ActionInterval* action = randMove();
	CallFunc* attack = CallFunc::create(this,callfunc_selector(IronManBossAI::PlayArmatWithMvAttack1));
	ActionInterval* seq = Sequence::create(action, attack, nullptr);
	m_target->runAction(seq);
}

void IronManBossAI::Action4_1()
{
	//³åÏòÄ¿±ê2
	ActionInterval* action = randMove();
	CallFunc* attack = CallFunc::create(this,callfunc_selector(IronManBossAI::PlayArmatWithMvAttack2));
	ActionInterval* seq = Sequence::create(action, attack, nullptr);
	m_target->runAction(seq);
}

void IronManBossAI::Action5()
{
	//³¯ÏÂ³å
	ActionInterval* action = randMove();
	CallFunc* attack = CallFunc::create(this,callfunc_selector(IronManBossAI::PlayArmatWithMvDown));
	ActionInterval* seq = Sequence::create(action, attack, nullptr);
	m_target->runAction(seq);
}

void IronManBossAI::Action6()
{
	auto action = MoveToMidRight();

	PlayArmatWithReady();

	//×Óµ¯
	ActionInterval* ready = DelayTime::create(1.5f);
	ActionInterval* delay = DelayTime::create(1.0f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(IronManBossAI::PlayArmatWithMidExpl));
	ActionInterval* seq = Sequence::create(func, delay, nullptr);
	ActionInterval* rep = Repeat::create(seq,3);
	ActionInterval* spa = Spawn::create(action, rep, nullptr);

	CallFunc* next = CallFunc::create(this,callfunc_selector(IronManBossAI::NextAction));
	ActionInterval* seq2 = Sequence::create(ready, spa, next, nullptr);
	m_target->runAction(seq2);
}

void IronManBossAI::Action7()
{
	ActionInterval* action = MoveToMidRight();
	CallFunc* shangtang = CallFunc::create(this,callfunc_selector(IronManBossAI::PlayArmatWithReady));
	ActionInterval* seq = Sequence::create(action,shangtang,nullptr);

	//×Óµ¯
	ActionInterval* ready = DelayTime::create(1.2f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(IronManBossAI::PlayArmatWithRandExpl));
	ActionInterval* delay2 = DelayTime::create(2.0f);
	ActionInterval* seq1 = Sequence::create(func,delay2,nullptr);
	ActionInterval* rep = Repeat::create(seq1,6);
	CallFunc* func1 = CallFunc::create(this,callfunc_selector(IronManBossAI::NextAction));
	ActionInterval* seq2 = Sequence::create(seq,ready,rep,func1,nullptr);

	m_target->runAction(seq2);
}

void IronManBossAI::shoot1Sword()
{
	NewAudio::getInstance()->playEffect(Music_Boss1Shoot1);
	Vec2 mypos = m_target->convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 gunPos = mypos + getGunPos();

	Vec2 AmiPos = Vec2(mypos.x, -1280);
	auto pl = GameData::getInstance()->getMainPlane();
	if (pl)
	{
		AmiPos = pl->convertToWorldSpaceAR(Vec2::ZERO);
	}

	//¼ÆËãÐý×ª½Ç¶È
	Vec2 newVec = AmiPos - mypos;
	float angle = 270 - newVec.getAngle() * Pi2Angle;
	setGunRotate(angle);

	Weapon* wp = Weapon::create(m_target);
	wp->setBullstType(BulletType::Enemy_BossDart);
	wp->setShootTimes(1);
	wp->setBltBelongTo(UnitId::ePlaneBoss);
	wp->setBltHurts(getBossHurts());
	m_target->AddOneWp(wp,WeaponType::Addition);						
	wp->setOffset(getShootPos());

	float rot = rand_minus1_1() * 25 + angle;
	wp->setRotation(180 + rot);

}

void IronManBossAI::shoot3Sword()
{
	NewAudio::getInstance()->playEffect(Music_Boss1Shoot1);

	Vec2 mypos = m_target->convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 gunPos = mypos + getGunPos();
	
	Vec2 AmiPos = Vec2(mypos.x, -1280);
	auto pl = GameData::getInstance()->getMainPlane();
	if (pl)
	{
		AmiPos = pl->convertToWorldSpaceAR(Vec2::ZERO);
	}
	
	//¼ÆËãÐý×ª½Ç¶È
	Vec2 newVec = AmiPos - mypos;
	float angle = 270 - newVec.getAngle() * Pi2Angle;
	setGunRotate(angle);

	//¼ÆËãÐý×ªºóµÄ·¢Éäµã
	newVec.normalize();
	newVec *= 100;
	Vec2 shootPos = getGunPos() + newVec;

	for(int i = 0; i < 3; ++i)
	{
		Weapon* wp = Weapon::create(m_target);
		wp->setBullstType(BulletType::Enemy_BossDart);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setShootTimes(1);
		wp->setBltHurts(getBossHurts());
		m_target->AddOneWp(wp,WeaponType::Addition);
		auto offset = Vec2((1-i) * 20, -i%2 * 50) + shootPos;
		wp->setOffset(offset);
		wp->setRotation(180 + angle + (i-1)*10);
	}
}

void IronManBossAI::shoot3Dart()
{
	NewAudio::getInstance()->playEffect(Music_Boss1Shoot1);
	auto GunPos = getShootPos();
	for(int i = 0; i < 3; ++i)
	{
		Weapon* wp = Weapon::create(m_target);
		wp->setBullstType(BulletType::Enemy_BossDart);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setShootTimes(1);
		wp->setBltSpeed(1500);
		wp->setBltLinearDamping(1.2f);
		wp->setBltHurts(getBossHurts());
		wp->setRotation((i-1)*20 + 180);
		
		Vec2 offset = Vec2(Vec2((1-i) * 15, 0) + GunPos);
		if (i == 1)
		{
			offset =(Vec2((1-i) * 15, 0) + GunPos);
		}
		wp->setPosition(offset);
		m_target->AddOneWp(wp, WeaponType::Addition);
	}
}

void IronManBossAI::shoot2Dart()
{
	NewAudio::getInstance()->playEffect(Music_Boss1Shoot1);
	
	for(int i = 0; i < 2; ++i)
	{
		Weapon* wp = Weapon::create(m_target);
		wp->setBullstType(BulletType::Enemy_BossDart);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setShootTimes(1);
		wp->setBltSpeed(1500);
		wp->setBltLinearDamping(1.2f);
		wp->setBltHurts(getBossHurts());
		wp->setRotation((i-0.5)*25 + 180);

		Vec2 offset = Vec2(Vec2((i-0.5) * 25, 0) + getShootPos());
		wp->setPosition(offset);

		m_target->AddOneWp(wp, WeaponType::Addition);
	}
}

void IronManBossAI::shootExplosionInMid()
{
	auto mypos = m_target->convertToWorldSpaceAR(Vec2::ZERO);

	setGunRotate(0);

	ExplosionBullet* bullet = ExplosionBullet::create(0);
	bullet->setBelongTo(UnitId::ePlaneBoss);
	bullet->setPosition(getShootPos() + mypos);
	UIController::getInstance()->getPhysicsLayer()->addChild(bullet);
}

void IronManBossAI::shootExplosionRand()
{
	auto mypos = m_target->convertToWorldSpaceAR(Vec2::ZERO);

	float angle = random(-1,1)*30;
	setGunRotate(angle);

	ExplosionBullet* bullet = ExplosionBullet::create(angle);
	bullet->setBelongTo(UnitId::ePlaneBoss);
	bullet->setPosition(getShootPos() + mypos);
	UIController::getInstance()->getPhysicsLayer()->addChild(bullet);
}

void IronManBossAI::onFrameEventOfIdex0( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	
}

void IronManBossAI::PlayArmatWith1Sword()
{
	playArmat(1,this,frameEvent_selector(IronManBossAI::onFrameEventOf1Sword),3.0);
}

void IronManBossAI::PlayArmatWith3Sword()
{
	setGunRotate(0);
	playArmat(1,this,frameEvent_selector(IronManBossAI::onFrameEventOf3Sword));
}

void IronManBossAI::PlayArmatWith3Dart()
{
	setGunRotate(0);
	playArmat(1,this,frameEvent_selector(IronManBossAI::onFrameEventOf3Dart),1.0f);
}

void IronManBossAI::PlayArmatWith2Dart()
{
	setGunRotate(0);
	playArmat(1,this,frameEvent_selector(IronManBossAI::onFrameEventOf2Dart),1.0f);
}

void IronManBossAI::PlayArmatWithReady()
{
	NewAudio::getInstance()->playEffect(Music_Boss1Ready);
	setGunRotate(0);
	playArmat(2,this,nullptr,1.2f);
}

void IronManBossAI::PlayArmatWithMvAttack1()
{
	setGunRotate(0);
	playArmat(3,this,frameEvent_selector(IronManBossAI::onFrameEventOfAttack1));
}

void IronManBossAI::PlayArmatWithMvAttack2()
{
	playArmat(3,this,frameEvent_selector(IronManBossAI::onFrameEventOfAttack2));
}

void IronManBossAI::PlayArmatWithMvDown()
{
	playArmat(3,this,frameEvent_selector(IronManBossAI::onFrameEventOfMoveDown));
}

void IronManBossAI::PlayArmatWithMidExpl()
{
	playArmat(1,this,frameEvent_selector(IronManBossAI::onFrameEventOfMidExpl));
}

void IronManBossAI::PlayArmatWithRandExpl()
{
	playArmat(1,this,frameEvent_selector(IronManBossAI::onFrameEventOfRandExpl));
}

void IronManBossAI::onFrameEventOf3Dart( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		shoot3Dart();
	}
	else if(evt == "end")  
	{  
		playArmat(0);
	}  
}

void IronManBossAI::onFrameEventOf2Dart( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		shoot2Dart();
	}
	else if(evt == "end")  
	{  
		playArmat(0);
	}  
}

void IronManBossAI::onFrameEventOf1Sword( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		shoot1Sword();
	}
	else if(evt == "end")  
	{  
		playArmat(0);
	}  
}

void IronManBossAI::onFrameEventOf3Sword( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		shoot3Sword();
	}
	else if(evt == "end")  
	{  
		playArmat(0);
	}  
}

void IronManBossAI::onFrameEventOfAttack1( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "attack")
	{
		NewAudio::getInstance()->playEffect(Music_Boss1Shoot2);
		ActionInterval* mvtoPl = MoveToMainPl();
		CallFunc* func = CallFunc::create(this,callfunc_selector(IronManBossAI::Action4_1));
		ActionInterval* seq = Sequence::create(mvtoPl,func,nullptr);
		m_target->runAction(seq);
	}
	else if(evt == "end")  
	{  
		playArmat(0);
	}  
}

void IronManBossAI::onFrameEventOfAttack2( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "attack")
	{
		NewAudio::getInstance()->playEffect(Music_Boss1Shoot2);
		ActionInterval* mvtoPl = MoveToMainPl();
		CallFunc* func = CallFunc::create(this,callfunc_selector(IronManBossAI::NextAction));
		ActionInterval* seq = Sequence::create(mvtoPl,func,nullptr);
		m_target->runAction(seq);
	}
	else if(evt == "end")  
	{  
		playArmat(0);
	}  
}

void IronManBossAI::onFrameEventOfMoveDown( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "attack")
	{
		NewAudio::getInstance()->playEffect(Music_Boss1Shoot2);
		ActionInterval* mvtoPl = MoveDown();
		CallFunc* func = CallFunc::create(this,callfunc_selector(IronManBossAI::NextAction));
		ActionInterval* seq = Sequence::create(mvtoPl,func,nullptr);
		m_target->runAction(seq);
	}
	else if(evt == "end")  
	{  
		playArmat(0);
	}  
}

void IronManBossAI::onFrameEventOfMidExpl( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		shootExplosionInMid();
	}
	else if(evt == "end")  
	{  
		playArmat(0);
	}  
}

void IronManBossAI::onFrameEventOfRandExpl( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		shootExplosionRand();
	}
	else if(evt == "end")  
	{  
		playArmat(0);
	}  
}

void IronManBossAI::setGunRotate( float r )
{
	BaseBoss* boss = (BaseBoss*)m_target;
	auto armat = boss->getArmature();
	Bone* bone = armat->getBone("Layer4");
	bone->setRotation(r);
}

cocos2d::Vec2 IronManBossAI::getShootPos()
{
	return Vec2(-100, -100);
}

cocos2d::Vec2 IronManBossAI::getGunPos()
{
	return Vec2(-100, 50);
}


//----------------------------
void BoneWarriorAI::StartAI(int lvl)
{
	m_times = 0;
	FixPos();
	BossAI::StartAI(lvl);
}

ActionInterval* BoneWarriorAI::randMove()
{
	auto mypos = m_target->convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 endpos;
	if (m_direct < 0)
	{
		//Ïò×ó
		endpos = Vec2(m_winSize.width * 0.3 - mypos.x,0);
	}
	else
	{
		//ÏòÓÒ
		endpos = Vec2(m_winSize.width * 0.6 - mypos.x,0);
	}

	ActionInterval* move = MoveBy::create( 3.0f, endpos);

	return move;
}

void BoneWarriorAI::changeDirect()
{
	ActionInterval* delay = DelayTime::create(0.8 + rand_0_1()*1.5f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(BoneWarriorAI::changeDirCB));
	ActionInterval* seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void BoneWarriorAI::changeDirCB()
{
	m_direct *= -1.0f;

	changeDirect();
}

void BoneWarriorAI::shoot2BezierBullet()
{
	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = Weapon::create(m_target);
		wp->setBullstType(BulletType::Enemy_Bezier);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltScaleX(1.5);
		wp->setBltScaleY(1.5);
		wp->setShootTimes(1);
		wp->setBltHurts(getBossHurts());
		wp->setOffset(Vec2(120,0));
		wp->setRotation(180 + (i-0.5) * 60);
		m_target->AddOneWp(wp, WeaponType::Addition);
	}
}

void BoneWarriorAI::shoot1JumpBullet()
{
	Weapon* wp = Weapon::create(m_target);
	wp->setBullstType(BulletType::Enemy_BossRebound);
	wp->setBltBelongTo(UnitId::ePlaneBoss);
	wp->setBltScaleX(1.5);
	wp->setBltGravityAble(true);
	wp->setBltScaleY(1.5);
	wp->setShootTimes(1);
	wp->setBltHurts(getBossHurts());
	wp->setOffset(Vec2(120,0));
	wp->setRotation(180 + m_direct * (90 + rand_minus1_1() * 60));
	//wp->setRotation(rand_minus1_1() * 360);
	m_target->AddOneWp(wp, WeaponType::Addition);

}

void BoneWarriorAI::Action1()
{
	auto mv = randMove();
	m_target->runAction(mv);

	ActionInterval* delay = DelayTime::create(1.5f + rand_minus1_1() * 0.5);
	ActionInterval* delay1 = DelayTime::create(5.0);
	CallFunc* func = CallFunc::create(this,callfunc_selector(BoneWarriorAI::ShootReboundDown));
	CallFunc* func1 = CallFunc::create(this,callfunc_selector(BoneWarriorAI::NextAction));
	ActionInterval* seq = Sequence::create(delay,func,delay1,func1,nullptr);
	runAction(seq);
}

void BoneWarriorAI::Action2()
{
	MoveLR();
	if (m_times < 4)
	{
		auto delay = DelayTime::create(3.0f);
		auto shoot = CallFunc::create(this,callfunc_selector(BoneWarriorAI::ShootReboundRandom));
		auto next = CallFunc::create(this,callfunc_selector(BoneWarriorAI::Action2));
		ActionInterval* seq2 = Sequence::create(shoot, delay, next, nullptr);
		runAction(seq2);

		m_times++;
	}
	else
	{
		//ÏÂÒ»²½
		m_times = 0;
		NextAction();
	}
	
}

void BoneWarriorAI::Action3()
{
	//Ò»±ßÒÆ¶¯Ò»±ß·¢Éä
	auto action = randMove();
	m_target->runAction(action);

	ActionInterval* delay1 = DelayTime::create(1.5f + rand_minus1_1() * 0.5f);
	ActionInterval* delay2 = DelayTime::create(7.0f);
	CallFunc* shoot = CallFunc::create(this,callfunc_selector(BoneWarriorAI::Shoot6Rebound));
	CallFunc* next = CallFunc::create(this,callfunc_selector(BoneWarriorAI::NextAction));
	ActionInterval* seq = Sequence::create(delay1, shoot, delay2, next, nullptr);
	runAction(seq);
}

void BoneWarriorAI::Action4()
{
	auto mv = MoveToMid();
	m_target->runAction(mv);

	//´ÓÁ½±ß·¢ÉäÁ÷µ¯
	if (m_times < 10)
	{
		int yushu = m_times%2;
		if (yushu == 0)
		{
			//×ó±ß
			auto func = CallFunc::create(this,callfunc_selector(BoneWarriorAI::ShootLiudanL));
			auto delay = DelayTime::create(0.5);
			auto next = CallFunc::create(this,callfunc_selector(BoneWarriorAI::Action4));
			auto seq = Sequence::create(func, delay, next, nullptr);
			runAction(seq);
		}
		else
		{
			//ÓÒ±ß
			auto func = CallFunc::create(this,callfunc_selector(BoneWarriorAI::ShootLiudanR));
			auto delay = DelayTime::create(0.5);
			auto next = CallFunc::create(this,callfunc_selector(BoneWarriorAI::Action4));
			auto seq = Sequence::create(func, delay, next, nullptr);
			runAction(seq);
		}
		m_times++;
	}
	else
	{
		//ÏÂÒ»²½
		m_times = 0;
		NextAction();
	}
	
}

void BoneWarriorAI::Action5()
{
	MoveLR();

	if (m_times < 5)
	{
		auto func = CallFunc::create(this,callfunc_selector(BoneWarriorAI::ShootReboundToMainPlane));
		auto delay = DelayTime::create(3.0);
		auto next = CallFunc::create(this,callfunc_selector(BoneWarriorAI::Action5));
		auto seq = Sequence::create(func, delay, next, nullptr);
		runAction(seq);
	}
	else
	{
		//ÏÂÒ»²½
		m_times = 0;
		NextAction();
	}
}

void BoneWarriorAI::ActionRest()
{
	auto action = randMove();
	m_target->runAction(action);

	ActionInterval* delay = DelayTime::create(1.0f + rand_0_1() * 2);
	CallFunc* next = CallFunc::create(this,callfunc_selector(BoneWarriorAI::NextAction));
	ActionInterval* seq = Sequence::create(delay, next, nullptr);
	runAction(seq);
}

void BoneWarriorAI::SelectAction(int ActionId)
{
	switch (ActionId)
	{
	case 0:
		ActionRest();	//ÐÝÏ¢¶¯×÷
		break;
	case 1:
		Action1();
		break;
	case 2:
		Action2();
		break;
	case 3:
		Action3();
		break;
	case 4:
		Action4();
		break;
	case 5:
		Action5();
		break;
	default:
		break;
	}
}

std::vector<int> BoneWarriorAI::initAction()
{
	int HpNums = getLeftHpNum();
	std::vector<int>  tagVec;
	//Ëæ»ú
	switch (m_AIlvl)
	{
	case 1:
		{
			if (HpNums < 1)
			{
				tagVec.push_back(1);
				tagVec.push_back(5);
			}
			else
			{
				tagVec.push_back(2);
				tagVec.push_back(4);
			}
		}
		break;
	case 2:
		{
			if (HpNums < 1)
			{
				tagVec.push_back(4);
				tagVec.push_back(3);
			}
			else
			{
				tagVec.push_back(3);
				tagVec.push_back(2);
			}
		}
		break;

	default:
		break;
	}
	return tagVec;
}

void BoneWarriorAI::FixPos()
{
	m_target->stopAllActions();
	auto mv = MoveToMidRight();
	m_target->runAction(mv);
}

void BoneWarriorAI::ShootReboundDown()
{
	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = Weapon::create(m_target);
		wp->setBullstType(BulletType::Enemy_BossRebound);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltScaleX(1.5);
		wp->setBltGravityAble(true);
		wp->setBltScaleY(1.5);
		wp->setBltOtherData((void*)2);
		wp->setShootTimes(1);
		wp->setBltHurts(getBossHurts());
		wp->setRotation(-45 + i * 90);
		m_target->AddOneWp(wp, WeaponType::Addition);
	}

}

void BoneWarriorAI::ShootReboundRandom()
{
	Weapon* wp = Weapon::create(m_target);
	wp->setBullstType(BulletType::Enemy_BossRebound);
	wp->setBltBelongTo(UnitId::ePlaneBoss);
	wp->setBltGravityAble(true);
	wp->setBltScaleX(1.5);
	wp->setBltScaleY(1.5);
	wp->setShootTimes(1);
	wp->setBltHurts(getBossHurts());
	wp->setRotation(360 * rand_0_1());
	m_target->AddOneWp(wp, WeaponType::Addition);
}

void BoneWarriorAI::Shoot6Rebound()
{
	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = Weapon::create(m_target);
		wp->setBullstType(BulletType::Enemy_BossRebound);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltGravityAble(true);
		wp->setBltScaleX(1.5);
		wp->setBltScaleY(1.5);
		wp->setBltOtherData((void*)3);
		wp->setShootTimes(1);
		wp->setBltHurts(getBossHurts());
		wp->setRotation(-45 * (i+1));
		m_target->AddOneWp(wp, WeaponType::Addition);
	}
	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = Weapon::create(m_target);
		wp->setBullstType(BulletType::Enemy_BossRebound);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltOtherData((void*)3);
		wp->setBltScaleX(1.5);
		wp->setBltScaleY(1.5);
		wp->setBltGravityAble(true);
		wp->setShootTimes(1);
		wp->setBltHurts(getBossHurts());
		wp->setRotation(45 * (i+1));
		m_target->AddOneWp(wp, WeaponType::Addition);
	}
}

void BoneWarriorAI::ShootReboundToMainPlane()
{
	Vec2 endPos = Vec2(m_winSize.width * 0.5, 0);
	auto mainPl = GameData::getInstance()->getMainPlane();
	if (mainPl)
	{
		endPos = mainPl->convertToWorldSpaceAR(Vec2::ZERO);
	}

	Weapon* wp = Weapon::create(m_target);
	wp->setBullstType(BulletType::Enemy_BossRebound);
	wp->setBltBelongTo(UnitId::ePlaneBoss);
	wp->setBltScaleX(1.5);
	wp->setBltScaleY(1.5);
	wp->setBltGravityAble(true);
	wp->setShootTimes(1);
	wp->setBltHurts(getBossHurts());
	m_target->AddOneWp(wp, WeaponType::Addition);

	auto myPos = wp->convertToWorldSpaceAR(Vec2::ZERO);
	auto newVec = endPos - myPos;
	float angle = newVec.getAngle() * RAD_DEG;
	wp->setRotation(-angle + 90);
}

void BoneWarriorAI::ShootLiudanL()
{
	auto pos = m_target->convertToWorldSpaceAR(Vec2::ZERO);

	Bullet* bullet = GameData::getInstance()->getBulletFactory()->createBullet(nullptr,BulletType::Enemy_Grenade);
	bullet->setPosition(Vec2(pos.x - 200,pos.y));
	bullet->setBelongTo(UnitId::ePlaneBoss);
	UIController::getInstance()->getPhysicsLayer()->addChild(bullet);
}

void BoneWarriorAI::ShootLiudanR()
{
	auto pos = m_target->convertToWorldSpaceAR(Vec2::ZERO);

	Bullet* bullet = GameData::getInstance()->getBulletFactory()->createBullet(nullptr,BulletType::Enemy_Grenade);
	bullet->setPosition(Vec2(pos.x + 200,pos.y));
	bullet->setBelongTo(UnitId::ePlaneBoss);
	UIController::getInstance()->getPhysicsLayer()->addChild(bullet);
}

//
//void BoneWarriorAI::DeathAction()
//{
//	m_target->stopAllActions();
//	DeadArmature();
//}
//
//void BoneWarriorAI::DeadArmature()
//{
//	BaseBoss* boss = dynamic_cast<BaseBoss*>(m_target);
//	auto armt = boss->getArmature();
//	armt->getAnimation()->pause();
//	stopAllActions();
//	boss->setHpBarVisible(false);
//
//	//ÒÆ³öÆÁÄ» + ±äÐ¡
//	auto s = Director::getInstance()->getWinSize();
//	auto myPos = this->convertToWorldSpaceAR(Vec2::ZERO);
//	_ccBezierConfig bezc;
//
//	if (myPos.x > 360)
//	{
//		bezc.controlPoint_1 = Vec2(-100, 50);
//		bezc.controlPoint_2 = Vec2(- 400, -200 );
//		bezc.endPosition = Vec2(- 800, -300 );
//	}
//	else
//	{
//		bezc.controlPoint_1 = Vec2(100, 50);
//		bezc.controlPoint_2 = Vec2(400, -200 );
//		bezc.endPosition = Vec2(800, -300 );
//	}
//
//	float stay = 2.5f;
//	ActionInterval* delay = DelayTime::create(stay);
//	ActionInterval* bezier = BezierBy::create(2.0f,bezc);
//	ActionInterval* scale = ScaleTo::create(2.0f,0.2f);
//	CallFunc* realDead = CallFunc::create(m_target,callfunc_selector(BaseBoss::RealDead));//Ü³£¬ÕâÀï²»Òª´óÒâ£¬Ò»¶¨Òª´«BOSS¶ÔÏó£¬¶ø·Çthis
//	ActionInterval* rotate = RotateBy::create(2.0,180);
//	ActionInterval* seq = Sequence::create(DelayTime::create(stay),bezier,realDead,nullptr);
//	ActionInterval* seq1 = Sequence::create(delay,scale,nullptr);
//	ActionInterval* seq2 = Sequence::create(DelayTime::create(stay),rotate,nullptr);
//	ActionInterval* fadeOut = FadeOut::create(7.0f);
//	ActionInterval* tint = TintTo::create(2.0f,150,30,30);
//
//	m_target->runAction(seq);
//	m_target->runAction(seq2);
//	armt->runAction(seq1);
//	//armt->runAction(fadeOut);
//	armt->runAction(tint);
//}


//-------------------------------------
void RenzheBossAI::StartAI( int lvl )
{
	m_Dart = CreateDart(true);
	m_Dart->setPosition(getDartLocalPos());
	m_target->addChild(m_Dart);
	m_Dart->setVisible(false);
	BossAI::StartAI(lvl);
}

cocos2d::Vec2 RenzheBossAI::getDartLocalPos()
{
	return Vec2(-190,145);
}

cocos2d::Vec2 RenzheBossAI::getDartWorldPos()
{
	auto bossPos = m_target->convertToWorldSpaceAR(Vec2::ZERO);
	return bossPos + getDartLocalPos();
}

PhysicsUnit* RenzheBossAI::CreateDart(bool isWudi)
{
	int hurt = m_target->getBasePro().getHurts();

	auto bullet = DartOfBoss3::create();
	bullet->setBelongTo(UnitId::ePlaneBoss);
	bullet->setInvincibleAtEdge(isWudi);
	bullet->getBasePro().setHurts(hurt);

	auto rot = RotateBy::create(0.6f,-720);
	auto rep = RepeatForever::create(rot);
	bullet->runAction(rep);

	return bullet;
}

PointArray* RenzheBossAI::getPointArray1()
{
	auto dartPos = getDartWorldPos();
	auto endPos = Vec2(m_winSize.width*0.5, 150);

	//¼ÆËã³öÏà¶ÔÎ»ÒÆ
	auto relativePos = endPos - dartPos;
	float dis = dartPos.distance(endPos);

	auto arr = PointArray::create(10);
	arr->addControlPoint(Vec2(0, 0));
	arr->addControlPoint(Vec2(relativePos.x - dis * 0.27,		relativePos.y * 0.45));
	arr->addControlPoint(Vec2(relativePos.x + dis * 0.28,		relativePos.y));
	arr->addControlPoint(Vec2(relativePos.x - dis * 0.27,		relativePos.y));
	arr->addControlPoint(Vec2(relativePos.x + dis * 0.28,		relativePos.y * 0.45));
	arr->addControlPoint(Vec2(0, 0));

	return arr;
}

PointArray* RenzheBossAI::getPointArray2()
{
	auto arr = PointArray::create(10);
	return arr;
}

void RenzheBossAI::DartAction1()
{
	m_Dart->setVisible(true);

	auto mainPl = GameData::getInstance()->getMainPlane();
	Vec2 mainplPos = Vec2(m_winSize.width*0.5, 100);
	if (mainPl)
	{
		mainplPos = mainPl->convertToWorldSpaceAR(Vec2::ZERO);
		//mainplPos.y += 150;
	}

	auto dartPos = getDartWorldPos();

	//¼ÆËã³öÏà¶ÔÎ»ÒÆ
	auto relativePos = mainplPos - dartPos;
	float dis = dartPos.distance(mainplPos);

	//auto midPos = dartPos.getMidpoint(mainplPos);
	auto midPos = Vec2((mainplPos.x + dartPos.x)*0.75, (mainplPos.y + dartPos.y)*0.75);
	auto midPosVec = mainplPos - midPos;
	auto faxianL = midPosVec;		//Ë³Ê±Õë·¨Ïß
	auto faxianR = midPosVec;		//ÄæÊ±Õë·¨Ïß
	faxianL.rotate(Vec2(0,0),-90 * DEG_RAD);
	faxianR.rotate(Vec2(0,0),90 * DEG_RAD);

	faxianL.normalize();
	faxianR.normalize();

	faxianL *= (dis * 0.5);
	faxianR *= (dis * 0.5);

	ccBezierConfig bezier;

	{
		bezier.controlPoint_1 = Vec2(0, 0);
		//bezier.controlPoint_2 = Vec2(relativePos.x * 0.75 - dis * 0.7,		relativePos.y * 0.75);
		//bezier.controlPoint_2 = faxianL;
		bezier.controlPoint_2 = Vec2(relativePos.x - 450,							relativePos.y*0.75);
		bezier.endPosition = Vec2(relativePos.x,							relativePos.y);
	}
	auto flyout = BezierBy::create(0.6, bezier);
	auto easeout = CCEaseSineIn::create(flyout);

	{
		bezier.controlPoint_1 = Vec2(0, 0);
		//bezier.controlPoint_2 = Vec2(-(relativePos.x * 0.25 - dis * 0.7),	-relativePos.y * 0.25);
		bezier.controlPoint_2 = faxianR;
		bezier.endPosition =	Vec2(-relativePos.x,						-relativePos.y);
	}
	auto flyback = BezierBy::create(0.8, bezier);
	auto easeback = CCEaseSineIn::create(flyback);

	auto delay = DelayTime::create(0.5f);
	auto seq = Sequence::create(flyout, delay, easeback, nullptr);
	m_Dart->runAction(seq);

	m_action1Time = 0.5 + 0.5 + 1.0f;
}

void RenzheBossAI::DartAction2()
{
	m_Dart->setVisible(true);

	auto arr = getPointArray1();
	auto mv = CardinalSplineBy::create(2.5f, arr, 0.5);
	m_Dart->runAction(mv);

	m_action2Time = 2.0f;
}

void RenzheBossAI::DartAction3()
{
	m_Dart->setVisible(true);

	auto mainPl = GameData::getInstance()->getMainPlane();
	Vec2 mainplPos = Vec2(m_winSize.width*0.5, 100);
	if (mainPl)
	{
		mainplPos = mainPl->convertToWorldSpaceAR(Vec2::ZERO);
	}

	auto dartPos = getDartWorldPos();

	//¼ÆËã³öÏà¶ÔÎ»ÒÆ
	auto relativePos = mainplPos - dartPos;

	auto mv = MoveBy::create(0.6f, relativePos);
	auto ease = EaseSineIn::create(mv);
	auto back = ease->reverse();
	auto seq = Sequence::create(ease, back, nullptr);
	m_Dart->runAction(seq);

	m_action3Time = 0.6*2 + 1.0f;
}

void RenzheBossAI::DartAction4()
{
	auto dartL = CreateDart();
	auto Lpos = Vec2(150, m_winSize.height*0.5);
	dartL->setPosition(Lpos);
	dartL->setOpacity(0);
	dartL->runAction(FadeIn::create(1.0f));
	UIController::getInstance()->getEffectsLayer()->addChild(dartL);

	auto dartR = CreateDart();
	auto Rpos = Vec2(m_winSize.width - 150, m_winSize.height*0.5);
	dartR->setPosition( Rpos );
	dartR->setOpacity(0);
	dartR->runAction(FadeIn::create(1.0f));
	UIController::getInstance()->getEffectsLayer()->addChild(dartR);

	ccBezierConfig bezier;

	{
		bezier.controlPoint_1 = Lpos;
		bezier.controlPoint_2 = Vec2(m_winSize.width * 0.5, 150);
		bezier.endPosition = Vec2(m_winSize.width * 1.2, -150);

		auto fly = BezierTo::create(1.0, bezier);
		auto delay = DelayTime::create(2.0f);
		auto seq = Sequence::create(delay, fly, nullptr); 
		dartL->runAction(seq);
	}
	{
		bezier.controlPoint_1 = Rpos;
		bezier.controlPoint_2 = Vec2(m_winSize.width * 0.5, 150);
		bezier.endPosition = Vec2(-m_winSize.width * 0.2, -150);

		auto fly = BezierTo::create(1.0, bezier);
		auto delay = DelayTime::create(2.0f);
		auto seq = Sequence::create(delay, fly, nullptr); 
		dartR->runAction(seq);
	}

	m_action4Time = 1.0+ 2.0;
}

void RenzheBossAI::ShootSandan()
{
	if (m_AIlvl == 2)
	{
		float dur = 3.0f;
		for (int i = 0; i < 2; i++)
		{
			Weapon* wp = Weapon::create(m_target);
			wp->setBullstType(BulletType::Enemy_default);
			wp->setBltResTag(ResTag::Bullet_BossSmallDart);
			wp->setBltBelongTo(UnitId::ePlaneBoss);
			wp->setBltScaleX(0.8);
			wp->setBltScaleY(0.8);
			wp->setBltSpeed(300);
			wp->setShootTimes(40);
			wp->setShootSpeed(0.08f);
			wp->setBltHurts(getBossHurts());
			wp->setPosition(getExecrationPos());
			wp->setBltLinearDamping(0.08 + i * 0.08);
			wp->setRotation(180 * i);
			m_target->AddOneWp(wp, WeaponType::Addition);

			ActionInterval* rot = RotateBy::create(1.0f,300);
			ActionInterval* rep = RepeatForever::create(rot);
			wp->runAction(rep);

			ActionInterval* delay = DelayTime::create(dur);
			CallFunc* func = CallFunc::create(this,callfunc_selector(RenzheBossAI::resumeArmat));
			ActionInterval* seq = Sequence::create(delay, func, nullptr);
			runAction(seq);
		}

		return;
	}

	if (m_sandanCount >= m_sandanCountMax)
	{
		m_sandanCount = 0;
		resumeArmat();
		return;
	}
	m_sandanCount++;

	if (m_shootDirect == -1)
	{
		m_shootDirect = 1;
	}
	else
	{
		m_shootDirect = -1;
	}
	int randRot = random(-10,10);

	
	for (int i = 0; i < m_sandanNum; i++)
	{
		Weapon* wp = Weapon::create(m_target);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setBltResTag(ResTag::Bullet_BossSmallDart);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltScaleX(0.8);
		wp->setBltScaleY(0.8);
		wp->setBltSpeed(m_sandanSpeed);
		wp->setDelayShoot(i * 0.08);
		wp->setShootTimes(1);
		wp->setBltHurts(getBossHurts());
		m_target->AddOneWp(wp, WeaponType::Addition);

		wp->setRotation(180 + m_shootDirect * (i-1) * 9 + randRot);

	}

	ActionInterval* delay = DelayTime::create(0.5);
	CallFunc* func = CallFunc::create(this,callfunc_selector(RenzheBossAI::ShootSandan));
	ActionInterval* seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void RenzheBossAI::addCloud()
{
	{
		auto sp = Sprite::create("Effect/cloud/cloud.png");
		sp->setPosition(m_winSize.width * 0.5, m_winSize.height * 0.5);
		sp->setScale(5);
		UIController::getInstance()->getEffectsLayer()->addChild(sp,10000);

		sp->setOpacity(0);
		auto fadein = FadeIn::create(1.0f);
		auto delay = DelayTime::create(5.0f);
		auto fadeout = FadeOut::create(3.0f);
		auto rm = RemoveSelf::create();
		auto seq = Sequence::create(fadein, delay, fadeout, rm, nullptr);
		sp->runAction(seq);
	}

	{
		auto delay = DelayTime::create(20.0f);
		auto func = CallFunc::create(this,callfunc_selector(RenzheBossAI::addCloud));
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}
}

void RenzheBossAI::onFrameEventOfShoot1( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		DartAction1();
	}
	else if (evt == "end")
	{
		PlayArmatKeep(1.1);	//±£³Ö×ËÊÆ
	}
}

void RenzheBossAI::onFrameEventOfShoot2( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		DartAction2();
	}
	else if (evt == "end")
	{
		PlayArmatKeep(0.8);	//±£³Ö×ËÊÆ
	}
}

void RenzheBossAI::onFrameEventOfShoot3( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		DartAction3();
	}
	else if (evt == "end")
	{
		PlayArmatKeep(2.5);	//±£³Ö×ËÊÆ
	}
}

void RenzheBossAI::onFrameEventOfCall2Darts( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		DartAction4();
	}
	else if (evt == "end")
	{
		playArmat(1,this, frameEvent_selector(RenzheBossAI::onFrameEventOfShoot3));
	}
}

void RenzheBossAI::onFrameEventOfExecration( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		ExecrationParticle();
		pauseArmat();
	}
	else if (evt == "end")
	{
		//log("nianznou over");
		removeExecrationParticle();
		NextAction();
	}
}

void RenzheBossAI::onFrameEventOfHuishou( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "end")
	{
		//log("huishou over");
		if (m_Dart)
		{
			m_Dart->setVisible(false);
		}
		NextAction();
	}
}

void RenzheBossAI::onFrameEventOfKeep( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "end")
	{
		//log("keep over");
		PlayArmatHuishou();
	}
}

void RenzheBossAI::onFrameEventOfRest( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	//²»×öÈÎºÎÊÂ£¬Ö»ÎªÌæ»»Ö¡¼àÌýÊÂ¼þ
}

void RenzheBossAI::onFrameEventOfReady( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "pause")
	{
		//ÔÝÍ£
		pauseArmat();
	}
}

void RenzheBossAI::PlayArmatRest()
{
	playArmat(0,this, frameEvent_selector(RenzheBossAI::onFrameEventOfRest));
}

void RenzheBossAI::PlayArmatKeep(float speed)
{
	playArmat(3,this, frameEvent_selector(RenzheBossAI::onFrameEventOfKeep),speed);
}

void RenzheBossAI::PlayArmatShoot1()
{
	setArmatFunc(this, frameEvent_selector(RenzheBossAI::onFrameEventOfShoot1));
	resumeArmat();
}

void RenzheBossAI::PlayArmatShoot2()
{
	setArmatFunc(this, frameEvent_selector(RenzheBossAI::onFrameEventOfShoot2));
	resumeArmat();
}

void RenzheBossAI::PlayArmatShoot3()
{
	setArmatFunc(this, frameEvent_selector(RenzheBossAI::onFrameEventOfShoot3));
	resumeArmat();
}

void RenzheBossAI::PlayArmatCall2Darts()
{
	playArmat(2,this, frameEvent_selector(RenzheBossAI::onFrameEventOfCall2Darts),2.0);
}

void RenzheBossAI::PlayArmatExecration()
{
	//ÔÚ·¢ÉäÉ¢µ¯µÄ¹ý³ÌÖÐÒª±£Ö¤Ö»²¥Ò»´Î£¬ÄÇÃ´¾ÍÒª¼õ»º²¥·ÅËÙÂÊ
	playArmat(2,this, frameEvent_selector(RenzheBossAI::onFrameEventOfExecration),3.0f);
}

void RenzheBossAI::PlayArmatHuishou()
{
	playArmat(4,this, frameEvent_selector(RenzheBossAI::onFrameEventOfHuishou));
}

void RenzheBossAI::ActionRest()
{
	PlayArmatRest();

	auto action = randMove();
	auto next = CallFunc::create(this,callfunc_selector(RenzheBossAI::NextAction));
	auto seq = Sequence::create(action, next, nullptr);
	m_target->runAction(seq);

}

void RenzheBossAI::ActionShootDart1()
{
	auto action = MoveToMidRight();
	auto func = CallFunc::create(this,callfunc_selector(RenzheBossAI::Ready1));
	auto seq  = Sequence::create(action, func, nullptr);
	m_target->runAction(seq);
}

void RenzheBossAI::ActionShootDart2()
{
	auto action = MoveToMidRight();
	auto func = CallFunc::create(this,callfunc_selector(RenzheBossAI::Ready2));
	auto seq  = Sequence::create(action, func, nullptr);
	m_target->runAction(seq);
}

void RenzheBossAI::ActionShootDart3()
{
	auto action = MoveToMidRight();
	auto func = CallFunc::create(this,callfunc_selector(RenzheBossAI::Ready3));
	auto seq  = Sequence::create(action, func, nullptr);
	m_target->runAction(seq);
}

void RenzheBossAI::ActionShootDart4()
{
	auto action = MoveToMidRight();
	auto func = CallFunc::create(this,callfunc_selector(RenzheBossAI::PlayArmatCall2Darts));
	auto seq  = Sequence::create(action, func, nullptr);
	m_target->runAction(seq);
}

void RenzheBossAI::ActionShootExecration()
{
	auto action = MoveToMid();
	auto func = CallFunc::create(this,callfunc_selector(RenzheBossAI::PlayArmatExecration));
	auto seq  = Sequence::create(action, func, nullptr);
	m_target->runAction(seq);
}

void RenzheBossAI::pauseArmat()
{
	BaseBoss* boss = (BaseBoss*)m_target;
	auto armat = boss->getArmature();
	armat->getAnimation()->pause();	
}

void RenzheBossAI::resumeArmat()
{
	m_Dart->runAction(MoveBy::create(0.1f,-getReadyOffset()));	//´Ó×¼±¸Î»ÖÃÔË¶¯µ½·¢ÉäÎ»ÖÃ
	m_Dart->setZOrder(1);

	BaseBoss* boss = (BaseBoss*)m_target;
	auto armat = boss->getArmature();
	armat->getAnimation()->resume();
}

void RenzheBossAI::Ready( Ref* obj, SEL_CallFunc func )
{
	playArmat(1,this, frameEvent_selector(RenzheBossAI::onFrameEventOfReady));

	//ÐÞÕý·ÉïÚÎ»ÖÃ£¬ÈÃÆä½¥ÏÖ
	m_Dart->setPosition(getDartLocalPos() + getReadyOffset());
	m_Dart->setZOrder(-1);
	m_Dart->setOpacity(0);
	m_Dart->setVisible(true);
	ActionInterval* fadein = FadeIn::create(1.0f);
	m_Dart->runAction(fadein);

	ActionInterval* delay = DelayTime::create(2.0f);
	CallFunc* begin = CallFunc::create(obj,func);
	ActionInterval* seq = Sequence::create(delay, begin, nullptr);
	runAction(seq);
}

void RenzheBossAI::Ready1()
{
	Ready(this,callfunc_selector(RenzheBossAI::PlayArmatShoot1));
}

void RenzheBossAI::Ready2()
{
	Ready(this,callfunc_selector(RenzheBossAI::PlayArmatShoot2));
}

void RenzheBossAI::Ready3()
{
	Ready(this,callfunc_selector(RenzheBossAI::PlayArmatShoot3));
}

void RenzheBossAI::Ready4()
{

}

void RenzheBossAI::setArmatFunc(cocos2d::Ref *target, SEL_FrameEventCallFunc callFunc, float speed)
{
	BaseBoss* boss = (BaseBoss*)m_target;
	auto armat = boss->getArmature();
	armat->getAnimation()->setFrameEventCallFunc(target,callFunc);  
	armat->getAnimation()->setSpeedScale(speed);
}

cocos2d::Vec2 RenzheBossAI::getReadyOffset()
{
	return Vec2(32,35);
}

ActionInterval* RenzheBossAI::MoveToMidRight()
{
	Vec2 pos = Vec2(m_winSize.width * 0.75, getBossMidPos().y);
	auto dis = getDis(pos);

	ActionInterval* mvto = MoveTo::create(0.5f + dis/300 * 0.5, pos );
	return mvto;
}

ActionInterval* RenzheBossAI::MoveToMid()
{
	Vec2 pos = getBossMidPos();
	auto dis = getDis(pos);

	ActionInterval* mvto = MoveTo::create(0.5f + dis/300 * 0.5, pos );
	return mvto;
}

void RenzheBossAI::ExecrationParticle()
{
	auto node = m_target->getChildByTag(15611940);
	if (!node)
	{
		node = Node::create();
		node->setPosition(getExecrationPos());
		m_target->addChild(node,0,15611940);
	}
	
	auto partc1 = EffectsManager::addParticle("particla/BossDeadExpl/2/test1.plist",node,Vec2(0,0),false);
	auto partc2 = EffectsManager::addParticle("particla/BossDeadExpl/2/test2.plist",node,Vec2(0,0),false);
	auto partc3 = EffectsManager::addParticle("particla/BossDeadExpl/2/test3.plist",node,Vec2(0,0),false);
	if (partc1)
	{
		partc1->setScale(2.5);
	}
	if (partc2)
	{
		partc2->setScale(2.5);
	}
	if (partc3)
	{
		partc3->setScale(2.5);
	}

	auto delay = DelayTime::create(0.7);
	auto func = CallFunc::create(this,callfunc_selector(RenzheBossAI::ShootSandan));
	auto seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void RenzheBossAI::removeExecrationParticle()
{
	auto node = m_target->getChildByTag(15611940);
	if (node)
	{
		node->removeFromParentAndCleanup(true);
	}
}

void RenzheBossAI::DeathAction()
{
	BossAI::DeathAction();

	//·ÉïÚÒªÉ¾³ý
	m_Dart->DeathForce();
}

cocos2d::Vec2 RenzheBossAI::getExecrationPos()
{
	return Vec2(-55,75);
}

void RenzheBossAI::SelectAction( int ActionId )
{
	switch (ActionId)
	{
	case 0:
		ActionRest();
		break;
	case 1:
		ActionShootDart1();
		break;
	case 2:
		ActionShootDart2();
		break;
	case 3:
		ActionShootDart3();
		break;
	case 4:
		ActionShootDart4();
		break;
	case 5:
		ActionShootExecration();
		break;
	default:
		break;
	}
}

std::vector<int> RenzheBossAI::initAction()
{
	int HpNums = getLeftHpNum();
	std::vector<int>	tagVec;

	switch (m_AIlvl)
	{
	case 1:
		{
			m_sandanCountMax = 2;
			m_sandanSpeed = 400;
			m_sandanNum= 4;

			if (HpNums < 1)
			{
				tagVec.push_back(5);
				tagVec.push_back(2);
			}
			else
			{
				tagVec.push_back(1);
				tagVec.push_back(5);
				tagVec.push_back(3);
			}
		}
		break;
	case 2:
		{
			m_sandanCountMax = 6;
			m_sandanSpeed = 600;
			m_sandanNum= 6;

			if (HpNums < 1)
			{
				tagVec.push_back(1);
				tagVec.push_back(5);
				tagVec.push_back(3);
				tagVec.push_back(4);
			}
			else
			{
				tagVec.push_back(2);
				tagVec.push_back(5);
				tagVec.push_back(3);
			}
		}
		break;

	default:
		break;
	}
	return tagVec;
}

void PoseidonBossAI::StartAI( int lvl )
{
	m_bezierTimes = 0;
	BossAI::StartAI(lvl);
}

void PoseidonBossAI::ShootOneLineBullet()
{
	for (int i = 0; i < 6; i++)
	{
		Bullet1OfBoss4* bullet = Bullet1OfBoss4::create(ResTag::Bullet_BossShuiPaoSmall, i * 1.0 + 2.0);
		bullet->setPosition(Vec2(100 + 105 * i, 750));
		bullet->setBelongTo(UnitId::ePlaneBoss);
		bullet->resetHurt(getBossHurts());
		UIController::getInstance()->getPhysicsLayer()->addChild(bullet);
	}
}

void PoseidonBossAI::ShootTwoBezierBulletL()
{
	Weapon* wp = Weapon::create(m_target);
	wp->setBullstType(BulletType::Enemy_Boss4Bezier);
	wp->setBltBelongTo(UnitId::ePlaneBoss);
	wp->setPosition(-90, 0);
	wp->setRotation(-45);
	wp->setShootTimes(10);
	wp->setShootSpeed(0.2f);
	wp->setBltHurts(getBossHurts());
	m_target->AddOneWp(wp, WeaponType::Normal);
	wp->setOffset(getLeftHandPos());
}

void PoseidonBossAI::ShootTwoBezierBulletR()
{
	Weapon* wp = Weapon::create(m_target);
	wp->setBullstType(BulletType::Enemy_Boss4Bezier);
	wp->setBltBelongTo(UnitId::ePlaneBoss);
	wp->setRotation(45);
	wp->setShootTimes(10);
	wp->setShootSpeed(0.2f);
	wp->setBltHurts(getBossHurts());
	m_target->AddOneWp(wp,WeaponType::Normal);
	wp->setOffset(getRightHandPos());

}

void PoseidonBossAI::ShootOneShuiPaoBullet()
{
	Bullet3OfBoss4* bullet = Bullet3OfBoss4::create();
	bullet->setBelongTo(UnitId::ePlaneBoss);
	bullet->resetHurt(getBossHurts());
	UIController::getInstance()->getPhysicsLayer()->addChild(bullet);
}

void PoseidonBossAI::Shoot3ShuiPaoBullet()
{
	Vec2 offset = Vec2(0,-100);
	{
		Bullet4OfBoss5* bullet = Bullet4OfBoss5::create(0.0);
		bullet->setPosition(Vec2(0, 0) + offset);
		bullet->setBelongTo(UnitId::ePlaneBoss);
		bullet->resetHurt(getBossHurts());
		m_target->addChild(bullet);
	}
	{
		Bullet4OfBoss5* bullet = Bullet4OfBoss5::create(1.0);
		bullet->setPosition(Vec2(-200, 350) + offset);
		bullet->setBelongTo(UnitId::ePlaneBoss);
		bullet->resetHurt(getBossHurts());
		m_target->addChild(bullet);
	}
	{
		Bullet4OfBoss5* bullet = Bullet4OfBoss5::create(2.0);
		bullet->setPosition(Vec2(200, 350) + offset);
		bullet->setBelongTo(UnitId::ePlaneBoss);
		bullet->resetHurt(getBossHurts());
		m_target->addChild(bullet);
	}
}

void PoseidonBossAI::ShootRotateBullet()
{
	for (int i = 0; i < 8; i++)
	{
		Weapon* wp = Weapon::create(m_target);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setBltResTag(ResTag::Bullet_BossShuiPaoSmall);
		wp->setBltScaleX(0.5);
		wp->setBltScaleY(0.5);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setShootTimes(25);
		wp->setBltHurts(getBossHurts());
		wp->setBltSpeed(400);
		wp->setRotation(i * 8 );
		m_target->AddOneWp(wp,WeaponType::Normal);
		wp->setOffset(getRightHandPos());
		wp->runAction(RepeatForever::create(RotateBy::create(2.0,800)));
	}
}

void PoseidonBossAI::SHootTriangleBullet(float angle)
{
	Vec2 shootPos = getLeftHandPos() + Vec2(-85, -20);
	int count = 0;
	{
		Weapon* wp = Weapon::create(m_target);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setBltResTag(ResTag::Bullet_BossShuiPaoSmall);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setShootTimes(1);
		wp->setBltHurts(getBossHurts());
		wp->setBltScaleX(0.5);
		wp->setBltScaleY(0.5);
		wp->setBltSpeed(500);
		wp->setDelayShoot(0.0001);
		wp->setRotation(angle);
		m_target->AddOneWp(wp,WeaponType::Normal);
		wp->setOffset(shootPos);
	}
	count++;

	for (int i = 0; i < 2; i++)
	{
		Weapon* wp = Weapon::create(m_target);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setBltResTag(ResTag::Bullet_BossShuiPaoSmall);
		wp->setShootTimes(1);
		wp->setBltHurts(getBossHurts());
		wp->setBltSpeed(500);
		wp->setBltScaleX(0.5);
		wp->setBltScaleY(0.5);
		wp->setDelayShoot(count * 0.1);
		wp->setRotation(angle + 8 * (i-0.5));
		m_target->AddOneWp(wp,WeaponType::Normal);
		wp->setOffset(shootPos);
	}
	count++;

	for (int i = 0; i < 3; i++)
	{
		Weapon* wp = Weapon::create(m_target);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setBltResTag(ResTag::Bullet_BossShuiPaoSmall);
		wp->setShootTimes(1);
		wp->setBltHurts(getBossHurts());
		wp->setBltSpeed(500);
		wp->setBltScaleX(0.5);
		wp->setBltScaleY(0.5);
		wp->setDelayShoot(count * 0.1);
		wp->setRotation(angle + 10 * (i-1));
		m_target->AddOneWp(wp,WeaponType::Normal);
		wp->setOffset(shootPos);
	}
}

void PoseidonBossAI::PlayAnimation1()
{
	playArmat(1,this,frameEvent_selector(PoseidonBossAI::onFrameEventOfShoot1));
}

void PoseidonBossAI::PlayAnimation2L()
{
	playArmat(1,this,frameEvent_selector(PoseidonBossAI::onFrameEventOfShoot2R),1.5);
}

void PoseidonBossAI::PlayAnimation2R()
{
	playArmat(2,this,frameEvent_selector(PoseidonBossAI::onFrameEventOfShoot2L),1.5);
}

void PoseidonBossAI::PlayAnimation3()
{
	playArmat(1,this,frameEvent_selector(PoseidonBossAI::onFrameEventOfShoot3));
}

void PoseidonBossAI::PlayAnimation4()
{
	playArmat(1,this,frameEvent_selector(PoseidonBossAI::onFrameEventOfShoot4));
}

void PoseidonBossAI::PlayAnimation5()
{
	playArmat(1,this,frameEvent_selector(PoseidonBossAI::onFrameEventOfShoot5));
}

void PoseidonBossAI::PlayAnimation6()
{
	playArmat(2,this,frameEvent_selector(PoseidonBossAI::onFrameEventOfShoot6));
}

void PoseidonBossAI::PlayAnimationRest()
{
	playArmat(0);
}

void PoseidonBossAI::PlayAnimationRest( float dur )
{
	playArmat(0);
	auto delay = DelayTime::create(dur);
	auto next = CallFunc::create(this,callfunc_selector(PoseidonBossAI::NextAction));
	auto seq = Sequence::create(delay, next, nullptr);
	runAction(seq);
}

void PoseidonBossAI::onFrameEventOfShoot1( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		//PauseDur(2.0f);
		ShootOneLineBullet();
	}
	else if(evt == "end")
	{
		PlayAnimationRest(5.5f + rand_0_1() * 0.5);
	}
}

void PoseidonBossAI::onFrameEventOfShoot2L( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		ShootTwoBezierBulletL();
		PauseDur(1.5f);
	}
	else if(evt == "end")
	{
		Action2();
	}
}

void PoseidonBossAI::onFrameEventOfShoot2R( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		ShootTwoBezierBulletR();
		PauseDur(1.5f);
	}
	else if(evt == "end")
	{
		Action2();
	}
}

void PoseidonBossAI::onFrameEventOfShoot3( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		PauseDur(2.0f);

		ShootOneShuiPaoBullet();
		auto delay = DelayTime::create(0.5f);
		auto func = CallFunc::create(this, callfunc_selector(PoseidonBossAI::ShootOneShuiPaoBullet));
		auto seq = Sequence::create(delay, func, delay, func, nullptr);
		runAction(seq);
	}
	else if(evt == "end")
	{
		PlayAnimationRest(0.5f + rand_0_1() * 0.5);
	}
}

void PoseidonBossAI::onFrameEventOfShoot4( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		PauseDur(2.0f);
		Shoot3ShuiPaoBullet();
	}
	else if(evt == "end")
	{
		PlayAnimationRest(3.5f + rand_0_1() * 0.5);
	}
}

void PoseidonBossAI::onFrameEventOfShoot5( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		ShootRotateBullet();
	}
	else if(evt == "end")
	{
		PlayAnimationRest(1.5f + rand_0_1() * 0.5);
	}
}

void PoseidonBossAI::onFrameEventOfShoot6( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		auto delay = DelayTime::create(0.5);
		auto shoot = CallFunc::create(this,callfunc_selector(PoseidonBossAI::ShootTriangBulletSomeTimes));
		auto seq = Sequence::create(shoot, delay, nullptr);
		auto rep = Repeat::create(seq, 5);
		auto next = CallFunc::create(this,callfunc_selector(PoseidonBossAI::NextAction));
		auto seq1 = Sequence::create(rep, next, nullptr);
		m_target->runAction(seq1);
		PauseDur(3.0);
	}
	else if(evt == "end")
	{
		Action0();
	}
}

void PoseidonBossAI::pauseArmat()
{
	BaseBoss* boss = (BaseBoss*)m_target;
	auto armat = boss->getArmature();
	armat->getAnimation()->pause();	
}

void PoseidonBossAI::resumeArmat()
{
	BaseBoss* boss = (BaseBoss*)m_target;
	auto armat = boss->getArmature();
	armat->getAnimation()->resume();	
}

void PoseidonBossAI::changeSpeed( float scale )
{
	BaseBoss* boss = (BaseBoss*)m_target;
	auto armat = boss->getArmature();
	armat->getAnimation()->setSpeedScale(scale);	
}
void PoseidonBossAI::PauseDur( float dt )
{
	auto dur = DelayTime::create(dt);
	auto pause = CallFunc::create(this,callfunc_selector(PoseidonBossAI::pauseArmat));
	auto resume = CallFunc::create(this,callfunc_selector(PoseidonBossAI::resumeArmat));
	auto seq = Sequence::create(pause, dur, resume, nullptr);
	runAction(seq);
}

void PoseidonBossAI::Action0()
{
	PlayAnimationRest(2.0 + rand_0_1()*1.0);
}

void PoseidonBossAI::Action1()
{
	PlayAnimation1();
}

void PoseidonBossAI::Action2()
{
	if (m_bezierTimes > 1)
	{
		m_bezierTimes = 0;
		PlayAnimationRest(1.5f + rand_0_1() * 0.5);
		return;
	}
	int yushu = m_bezierTimes%2;
	if ( yushu == 0)
	{
		PlayAnimation2L();
	}
	else
	{
		PlayAnimation2R();
	}
	m_bezierTimes++;
}

void PoseidonBossAI::Action3()
{
	PlayAnimation3();
}

void PoseidonBossAI::Action4()
{
	PlayAnimation4();
}

void PoseidonBossAI::Action5()
{
	PlayAnimation5();
}

void PoseidonBossAI::Action6()
{
	if (m_bezierTimes > 5)
	{
		m_bezierTimes = 0;
		PlayAnimationRest(3.5f + rand_0_1() * 0.5);
		return;
	}
	PlayAnimation6();
	m_bezierTimes++;
}

void PoseidonBossAI::ShootTriangBulletSomeTimes()
{
	float randAngle = random(0,60);
	SHootTriangleBullet(0 + randAngle);
	SHootTriangleBullet(60 + randAngle);
	SHootTriangleBullet(120 + randAngle);
	SHootTriangleBullet(180 + randAngle);
	SHootTriangleBullet(240 + randAngle);
	SHootTriangleBullet(300 + randAngle);
}

cocos2d::Vec2 PoseidonBossAI::getLeftHandPos()
{
	return Vec2(-80, 240);
}

cocos2d::Vec2 PoseidonBossAI::getRightHandPos()
{
	return Vec2(135, 170);
}

std::vector<int> PoseidonBossAI::initAction()
{
	int HpNums = getLeftHpNum();
	std::vector<int>	tagVec;
	//Ëæ»ú
	switch (m_AIlvl)
	{
	case 1:
		{
			if (HpNums < 1)
			{
				tagVec.push_back(4);
				tagVec.push_back(5);
				tagVec.push_back(3);
			}
			else
			{
				tagVec.push_back(2);
				tagVec.push_back(1);
				tagVec.push_back(5);
			}
		}
		break;
	case 2:
		{
			if (HpNums < 1)
			{
				tagVec.push_back(4);
				tagVec.push_back(3);
				tagVec.push_back(6);
			}
			else
			{
				tagVec.push_back(4);
				tagVec.push_back(5);
				tagVec.push_back(6);
			}
		}
		break;

	default:
		break;
	}
	return tagVec;
}

void PoseidonBossAI::SelectAction( int ActionId )
{
	switch (ActionId)
	{
	case 0:
		Action0();
		break;
	case 1:
		Action1();
		break;
	case 2:
		Action2();
		break;
	case 3:
		Action3();
		break;
	case 4:
		Action4();
		break;
	case 5:
		Action5();
		break;
	case 6:
		Action6();
		break;

	default:
		break;
	}
}



//----------------------
bool EnemyAiofStay::init()
{
	return PlaneAI::init();
}

void EnemyAiofStay::StartAI( int lvl )
{
	m_AIlvl = lvl;
	StartShoot();
	Stay();
}

void EnemyAiofStay::Stay()
{
	//ÏÈÒÆ¶¯µ½ÆÁÄ»µÄÄ³¸öÎ»ÖÃ
	auto mypos = m_target->convertToWorldSpaceAR(Vec2::ZERO);

	auto mv = MoveTo::create(1.0 + rand_0_1() * 0.1, Vec2(m_winSize.width * (0.5 + rand_minus1_1()*0.35), mypos.y));
	auto ease = EaseSineOut::create(mv);
	auto swing = MoveBy::create(0.5 + rand_0_1() * 0.2, Vec2(20, 0));
	auto back = swing->reverse();
	auto seq = Sequence::create(swing, back, nullptr);
	auto rep = RepeatForever::create(seq);
	auto seq1 = Sequence::create(ease, rep, nullptr);
	m_target->runAction(seq1);
}

//----------------------
bool EnemyAiofFree::init()
{
	return PlaneAI::init();
}

void EnemyAiofFree::StartAI( int lvl )
{
	m_AIlvl = lvl;
	StartShoot();
	MoveFree();
}

void EnemyAiofFree::MoveFree()
{
	PointArray* arr = PointArray::create(100);
	auto mypos = m_target->convertToWorldSpaceAR(Vec2::ZERO);
	arr->addControlPoint(mypos);

	for (int i = 0; i < 99; ++i)
	{
		Vec2 pos = Vec2(m_winSize.width * (0.5 + rand_minus1_1() * 0.4),  m_winSize.height * (0.5 + rand_0_1() * 0.4));
		arr->addControlPoint(pos);
	}

	auto mv = CardinalSplineTo::create(200, arr, 0);
	auto next = CallFunc::create(this,callfunc_selector(EnemyAiofFree::MoveFree));
	auto seq = Sequence::create(mv, next, nullptr);
	m_target->runAction(seq);
}

//---------------------------
void FemaleGunBossAi::StartAI( int lvl )
{
	m_shootDownTimes = 0;
	m_shootBulletCounts = 0;
	m_SaoSheTimes = 0;
	m_WingId = 0;
	m_bulletFrameName = "jiqiangBoss_ammo_4.png";

	CreateJiqiang();
	CreateWing();

	BossAI::StartAI(lvl);
}

void FemaleGunBossAi::ActionShootDown()
{
	MoveLR();
	//²¥·ÅÏòÏÂ·¢Éä¶¯×÷
	playArmatShootDown();
}

void FemaleGunBossAi::ActionSaoShe()
{
	MoveLR();

	//²¥·ÅÉ¨Éä¶¯×÷
	playArmatSaoShe();

}

void FemaleGunBossAi::ActionShootAmmi()
{
	MoveLR();

	//²¥·ÅÏòÏÂ·¢Éä¶¯×÷£¬ÇÒÊµÊ±ÐÞÕýÇ¹¿Ú³¯Ïò
	playArmat(1);

	auto delay = DelayTime::create(1.5f);
	auto shoot = CallFunc::create(this, callfunc_selector(FemaleGunBossAi::JiqiangShoot));
	auto seq = Sequence::create(shoot, delay, nullptr);
	auto rep = Repeat::create(seq, 3);
	auto stop = CallFunc::create(this, callfunc_selector(FemaleGunBossAi::StopJiqiang));
	auto seq1 = Sequence::create(rep, stop, nullptr);
	runAction(seq1);

}

void FemaleGunBossAi::addWeaponShootDown(bool isleft)
{
	Weapon* wp = Weapon::create(m_target);
	wp->setBullstType(BulletType::Enemy_default);
	//wp->setBltResTag(ResTag::Bullet_EnemyNor3);
	wp->setBltFrameName(m_bulletFrameName);
	wp->setBltBelongTo(UnitId::ePlaneBoss);
	wp->setBltSpeed(1000);
	wp->setShootTimes(1);
	wp->setBltHurts(getBossHurts());
	wp->setPosition(Vec2(-20 + 40 * isleft, -150));
	wp->setRotation(180);
	m_target->AddOneWp(wp, WeaponType::Addition);
}

void FemaleGunBossAi::addWeaponSaoShe()
{
	float shootSpeed = 0.2;
	float rotSpeed = 0.7;
	float flySpeed = 900;
	if (m_AIlvl == 2)
	{
		shootSpeed = 0.08;
		rotSpeed = 0.46;
		flySpeed = 1100;
	}
	Node* node = Node::create();
	node->setPosition(-30,70);
	addChild(node);
	node->setTag(15081219);
	node->setRotation(-20);

	//»ð»¨
	/*{
		auto huo = Sprite::createWithSpriteFrameName("nps24.png");
		huo->setPosition(0,-215);
		node->addChild(huo);
		huo->setOpacity(0);
		huo->setScale(0.8);
		auto fadeout = FadeOut::create(0.01f);
		auto fadein = FadeIn::create(0.01f);
		auto wait = DelayTime::create(0.2f);
		auto seq = Sequence::create(fadeout, fadein, wait, nullptr);
		auto rep = RepeatForever::create(seq);
		huo->runAction(rep);
	}*/

	Weapon* wp = Weapon::create(node);
	wp->setBullstType(BulletType::Enemy_default);
	//wp->setBltResTag(ResTag::Bullet_EnemyNor3);
	wp->setBltFrameName(m_bulletFrameName);
	wp->setBltBelongTo(UnitId::ePlaneBoss);
	wp->setBltSpeed(flySpeed);
	wp->setBltScaleY(2.0f);
	wp->setShootSpeed(shootSpeed);
	wp->setBltHurts(getBossHurts());
	wp->setPosition(0,-200);
	wp->setRotation(180);
	wp->StartShoot();
	node->addChild(wp);

	auto rot = RotateBy::create(rotSpeed, 47);
	auto back = rot->reverse();
	auto seq = Sequence::create(rot, back, nullptr);
	auto rep = RepeatForever::create(seq);
	node->runAction(rep);
}

void FemaleGunBossAi::CreateJiqiang()
{
	m_Gun = Sprite::createWithSpriteFrameName("nps15.png");
	m_Gun->setAnchorPoint(Vec2(0.5, 0.75));
	m_Gun->setVisible(false);
	m_Gun->setPosition(-45,35);
	m_target->addChild(m_Gun,100);

	//»ð»¨
	auto s = m_Gun->getContentSize();
	Sprite* huohua = Sprite::createWithSpriteFrameName("nps24.png");
	huohua->setPosition(s.width * 0.5, -55);
	huohua->setName("huohua");
	m_Gun->addChild(huohua);
	huohua->setOpacity(0);
}

Bone* FemaleGunBossAi::getGun()
{
	BaseBoss* boss = (BaseBoss*)m_target;
	auto armat = boss->getArmature();
	auto gun = armat->getBone("Layer17");

	return gun;
}

void FemaleGunBossAi::onFrameEventOfShootDown( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		m_shootBulletCounts++;
		addWeaponShootDown(m_shootBulletCounts%2);
	}
	else if(evt == "end")
	{
		if (m_shootBulletCounts > 5)
		{
			//ÐÝÏ¢Ò»ÏÂ£¬¼ÌÐøÉä»÷£¬Ö±µ½ÖØ¸´5´Î
			playArmat(1);
			if (m_shootDownTimes > 1)
			{
				m_shootBulletCounts = 0;
				m_shootDownTimes = 0;
				NextActionDelay(0.5 + rand_0_1() * 0.5);
				return;
			}
			MoveLR();
			auto wait = DelayTime::create(1.0f + rand_0_1() * 1.0);
			auto goOn = CallFunc::create(this, callfunc_selector(FemaleGunBossAi::playArmatShootDown));
			auto seq = Sequence::create(wait, goOn, nullptr);
			runAction(seq);

			m_shootBulletCounts = 0;
			m_shootDownTimes++;
		}
		else
		{
			playArmatShootDown();
		}
	}
}

void FemaleGunBossAi::playArmatShootDown()
{
	m_target->stopAllActions();
	playArmat(3,this,frameEvent_selector(FemaleGunBossAi::onFrameEventOfShootDown),5.0);
}

void FemaleGunBossAi::playArmatSaoShe()
{
	float speed = 1.0;
	if (m_AIlvl == 2)
	{
		speed = 1.5;
	}
	playArmat(2,this,frameEvent_selector(FemaleGunBossAi::onFrameEventOfSaoShe),speed);
}

void FemaleGunBossAi::onFrameEventOfSaoShe( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "start")
	{
		addWeaponSaoShe();
		m_SaoSheTimes++;
	}
	else if (evt == "stop")
	{
		StopSaoShe();

		if (m_SaoSheTimes > 1)
		{
			m_SaoSheTimes = 0;
			playArmat(1);
			NextActionDelay( 0.5 + rand_0_1() * 1.0f);
		}
	}
}

void FemaleGunBossAi::StopSaoShe()
{
	auto node = getChildByTag(15081219);
	if (node)
	{
		node->removeFromParentAndCleanup(true);
	}
}

void FemaleGunBossAi::JiqiangShoot()
{
	auto gun = getGun();
	gun->getDisplayRenderNode()->setVisible(false);

	m_Gun->setVisible(true);
	auto s = m_Gun->getContentSize();

	float t = 0.4;
	//¼ÓÉÏÎäÆ÷
	for (int i = 0; i < 5; ++i)
	{
		Weapon* wp = Weapon::create(m_Gun);
		wp->setBullstType(BulletType::Enemy_default);
		//wp->setBltResTag(ResTag::Bullet_EnemyNor3);
		wp->setBltFrameName(m_bulletFrameName);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltSpeed(1000);
		wp->setDelayShoot(t);
		wp->setShootTimes(1);
		wp->setBltHurts(getBossHurts());
		wp->setPosition(s.width * 0.5, -10);
		wp->setRotation(180 + 15 * (i - 2));
		wp->setBltLinearDamping(0.8);
		wp->StartShoot();
		m_Gun->addChild(wp);
	}

	for (int i = 0; i < 4; ++i)
	{
		Weapon* wp = Weapon::create(m_Gun);
		wp->setBullstType(BulletType::Enemy_default);
		//wp->setBltResTag(ResTag::Bullet_EnemyNor3);
		wp->setBltFrameName(m_bulletFrameName);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltSpeed(900);
		wp->setDelayShoot(0.2 + t);
		wp->setShootTimes(1);
		wp->setBltHurts(getBossHurts());
		wp->setPosition(s.width * 0.5, -10);
		wp->setRotation(180 + 15 * (i - 1.5));
		wp->setBltLinearDamping(0.8);
		wp->StartShoot();
		m_Gun->addChild(wp);
	}
	for (int i = 0; i < 3; ++i)
	{
		Weapon* wp = Weapon::create(m_Gun);
		wp->setBullstType(BulletType::Enemy_default);
		//wp->setBltResTag(ResTag::Bullet_EnemyNor3);
		wp->setBltFrameName(m_bulletFrameName);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltSpeed(800);
		wp->setDelayShoot(0.4 + t);
		wp->setShootTimes(1);
		wp->setBltHurts(getBossHurts());
		wp->setPosition(s.width * 0.5, -10);
		wp->setRotation(180 + 15 * (i - 1));
		wp->setBltLinearDamping(0.8);
		wp->StartShoot();
		m_Gun->addChild(wp);
	}

	auto mainPl = GameData::getInstance()->getMainPlane();
	if (mainPl)
	{
		//ÐÞÕý½Ç¶È£¬Ê¹Æä³¯ÏòÖ÷·É»ú
		auto myPos = m_Gun->convertToWorldSpaceAR(Vec2::ZERO);
		auto plPos = mainPl->convertToWorldSpaceAR(Vec2::ZERO);

		auto newVec = plPos - myPos;
		auto angle = newVec.getAngle() * RAD_DEG;

		newVec.normalize();
		{
			auto rot = RotateTo::create(t, -angle - 90);
			auto mv = MoveBy::create(0.08, newVec * 20);
			auto back = mv->reverse();
			auto seq = Sequence::create(mv, back, nullptr);
			auto rep = Repeat::create(seq,4);
			auto seq1 = Sequence::create(rot, rep, nullptr);
			m_Gun->runAction(seq1);
		}

		{
			//»ð»¨ÉÁË¸
			auto huohua = m_Gun->getChildByName("huohua");

			auto delay = DelayTime::create(0.45f);
			auto fadein = FadeIn::create(0.02);
			auto fadeout = FadeOut::create(0.02);
			auto seq = Sequence::create(fadein, fadeout, nullptr);
			auto rep = Repeat::create(seq, 12);
			auto seq1 = Sequence::create(delay, rep, nullptr);
			huohua->runAction(seq1);
		}
	}
}

void FemaleGunBossAi::StopJiqiang()
{
	m_Gun->setVisible(false);
	getGun()->getDisplayRenderNode()->setVisible(true);

	NextAction();
}

Bone* FemaleGunBossAi::getWing( int idx )
{
	std::string name;
	switch (idx)
	{
	case 0:
		name = "Layer2";
		break;
	case 1:
		name = "Layer2_Copy28_Copy31";
		break;
	case 2:
		name = "Layer2_Copy30";
		break;
	case 3:
		name = "Layer2_Copy28_Copy29_Copy32";
		break;
	case 4:
		name = "Layer2_Copy28_Copy29";
		break;
	case 5:
		name = "Layer2_Copy28";
		break;

	default:
		break;
	}
	BaseBoss* boss = (BaseBoss*)m_target;
	auto armat = boss->getArmature();
	auto bone = armat->getBone(name);

	return bone;
}

void FemaleGunBossAi::CreateWing()
{
	{
		//1
		DirectBullet* wing = DirectBullet::create("nps1.png",5,true);
		wing->setPosition(-180,150);
		wing->getPhysicsBody()->setRotation(0);
		wing->setGlobalZOrder(1000);
		wing->setVisible(false);
		m_target->addChild(wing,-1);
		m_WingVec.push_back(wing);
	}
	{
		//2
		DirectBullet* wing = DirectBullet::create("nps1.png",5,true);
		wing->setPosition(-165,110);
		wing->getPhysicsBody()->setRotation(-10);
		wing->setGlobalZOrder(1000);
		wing->setVisible(false);
		m_target->addChild(wing,-1);
		m_WingVec.push_back(wing);
	}
	{
		//3
		DirectBullet* wing = DirectBullet::create("nps1.png",5,true);
		wing->setPosition(-140,75);
		wing->getPhysicsBody()->setRotation(-20);
		wing->setGlobalZOrder(1000);
		wing->setVisible(false);
		m_target->addChild(wing,-1);
		m_WingVec.push_back(wing);
	}
	{
		//4
		DirectBullet* wing = DirectBullet::create("nps1.png",5,false, true);
		wing->setFlippedX(true);
		wing->setPosition(180,150);
		wing->getPhysicsBody()->setRotation(0);
		wing->setGlobalZOrder(1000);
		wing->setVisible(false);
		m_target->addChild(wing,-1);
		m_WingVec.push_back(wing);
	}
	{
		//5
		DirectBullet* wing = DirectBullet::create("nps1.png",5,false, true);
		wing->setFlippedX(true);
		wing->setPosition(165,110);
		wing->getPhysicsBody()->setRotation(10);
		wing->setGlobalZOrder(1000);
		wing->setVisible(false);
		m_target->addChild(wing,-1);
		m_WingVec.push_back(wing);
	}
	{
		//6
		DirectBullet* wing = DirectBullet::create("nps1.png",5,false, true);
		wing->setFlippedX(true);
		wing->setPosition(140,75);
		wing->getPhysicsBody()->setRotation(20);
		wing->setGlobalZOrder(1000);
		wing->setVisible(false);
		m_target->addChild(wing,-1);
		m_WingVec.push_back(wing);
	}
}

void FemaleGunBossAi::ActionShootWingOneByOne()
{
	playArmat(1);
	{
		m_target->stopAllActions();
		auto mv = MoveToMidRight();
		m_target->runAction(mv);
	}
	setArmatWingsHide(true);
	ShootWingOneByOne();
}

void FemaleGunBossAi::setArmatWingsHide( bool b )
{
	if (b)
	{
		auto delay = DelayTime::create(1.0f);
		auto func = CallFunc::create(this, callfunc_selector(FemaleGunBossAi::HideArmatWings));
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}
	else
	{
		auto delay = DelayTime::create(1.0f);
		auto func = CallFunc::create(this, callfunc_selector(FemaleGunBossAi::ShowArmatWings));
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}
}

void FemaleGunBossAi::ShootWingOneByOne()
{
	if (m_WingId > 5)
	{
		m_WingId = 0;
		setArmatWingsHide(false);
		NextAction();
		return;
	}
	DirectBullet* wing = m_WingVec.at(m_WingId);
	wing->setBelongTo(UnitId::ePlaneBoss);
	wing->setFlyTime(1.0);
	wing->Start();
	wing->resetHurt(-1);

	auto delay = DelayTime::create(1.0f);
	auto goOn = CallFunc::create(this, callfunc_selector(FemaleGunBossAi::ShootWingOneByOne));
	auto seq = Sequence::create(delay, goOn, nullptr);
	runAction(seq);
	m_WingId++;
}

void FemaleGunBossAi::ShootWing2By2()
{
	if (m_WingId > 2)
	{
		m_WingId = 0;
		setArmatWingsHide(false);
		NextAction();
		return;
	}
	{
		DirectBullet* wing = m_WingVec.at(m_WingId);
		wing->setBelongTo(UnitId::ePlaneBoss);
		wing->Start();
		wing->setFlyTime(0.6);
	}
	{
		DirectBullet* wing = m_WingVec.at(m_WingId + 3);
		wing->setBelongTo(UnitId::ePlaneBoss);
		wing->Start();
		wing->setFlyTime(0.6);
	}
	auto delay = DelayTime::create(1.0f);
	auto goOn = CallFunc::create(this, callfunc_selector(FemaleGunBossAi::ShootWing2By2));
	auto seq = Sequence::create(delay, goOn, nullptr);
	runAction(seq);
	m_WingId++;
}

void FemaleGunBossAi::ActionShootWing2By2()
{
	MoveLR();
	playArmat(1);
	
	setArmatWingsHide(true);
	ShootWing2By2();
}

void FemaleGunBossAi::ActionCombine()
{
	MoveLR();
	playArmat(1);
	setArmatWingsHide(true);
	ShootWing2By2WithSandan();
}

void FemaleGunBossAi::ShootWing2By2WithSandan()
{
	if (m_WingId > 2)
	{
		m_WingId = 0;
		setArmatWingsHide(false);
		NextAction();
		return;
	}
	{
		DirectBullet* wing = m_WingVec.at(m_WingId);
		wing->setBelongTo(UnitId::ePlaneBoss);
		wing->Start();
		wing->setFlyTime(0.6);
	}
	{
		DirectBullet* wing = m_WingVec.at(m_WingId + 3);
		wing->setBelongTo(UnitId::ePlaneBoss);
		wing->Start();
		wing->setFlyTime(0.6);
	}
	JiqiangShoot();
	auto delay = DelayTime::create(2.5);
	auto goOn = CallFunc::create(this, callfunc_selector(FemaleGunBossAi::ShootWing2By2WithSandan));
	auto seq = Sequence::create(delay, goOn, nullptr);
	runAction(seq);
	m_WingId++;
}

void FemaleGunBossAi::NextActionDelay( float t )
{
	auto delay = DelayTime::create(t);
	auto next = CallFunc::create(this, callfunc_selector(FemaleGunBossAi::NextAction));
	auto seq = Sequence::create(delay, next, nullptr);
	runAction(seq);
}

void FemaleGunBossAi::ShowArmatWings()
{
	for (int i = 0; i < 6; i++)
	{
		auto wing = getWing(i);
		wing->getDisplayRenderNode()->setVisible(true);

		auto weapon = m_WingVec.at(i);
		weapon->setVisible(false);
	}
}

void FemaleGunBossAi::HideArmatWings()
{
	for (int i = 0; i < 6; i++)
	{
		auto wing = getWing(i);
		wing->getDisplayRenderNode()->setVisible(false);

		auto weapon = m_WingVec.at(i);
		weapon->setVisible(true);
	}
}

std::vector<int> FemaleGunBossAi::initAction()
{
	int HpNums = getLeftHpNum();
	std::vector<int>	tagVec;

	//Ëæ»ú
	switch (m_AIlvl)
	{
	case 1:
		{
			//tagVec.push_back(1);
			tagVec.push_back(2);
			tagVec.push_back(4);
		}
		break;
	case 2:
		{
			tagVec.push_back(2);
			tagVec.push_back(3);
			tagVec.push_back(5);
			tagVec.push_back(6);
		}
		break;

	default:
		break;
	}
	return tagVec;
}

void FemaleGunBossAi::SelectAction( int ActionId )
{
	{
		//Ö´ÐÐÃ¿¸ö¶¯×÷Ö®Ç°
		//1.Òþ²Ø¶îÍâµÄ»úÇ¹£¬ÏÔÊ¾¹Ç÷À»úÇ¹
		//2.Òþ²Ø¶îÍâµÄ³á°ò£¬ÏÔÊ¾¹Ç÷À³á°ò
		getGun()->getDisplayRenderNode()->setVisible(true);
		m_Gun->setVisible(false);

		setArmatWingsHide(false);
	}
	switch (ActionId)
	{
	case 1:
		ActionShootDown();
		break;
	case 2:
		ActionSaoShe();
		break;
	case 3:
		ActionShootAmmi();
		break;
	case 4:
		ActionShootWingOneByOne();
		break;
	case 5:
		ActionShootWing2By2();
		break;
	case 6:
		ActionCombine();
		break;
	default:
		break;
	}
}

void FemaleGunBossAi::FuncWhenTargetHpEmpry()
{
	StopSaoShe();
	for (int i = 0; i < 6; i++)
	{
		auto weapon = m_WingVec.at(i);
		weapon->DeathForce();
	}
}

//----------------------
void EyeBossAi::StartAI( int lvl )
{
	m_TelescopicLaserTimes = 0;
	m_BezierBulletTimes = 0;

	InitEyes();
	BossAI::StartAI(lvl);
}

void EyeBossAi::shootFallingBullet()
{
	auto armat = getArmature();

	int count = 0;
	float interval = 1.0f;
	{
		float delayshoot = interval * count;
		auto eye = getEye(0);
		eye->OpenEyeWithFunc(delayshoot,1,nullptr, nullptr);

		Weapon* wp = Weapon::create(armat);
		wp->setBullstType(BulletType::FallBullet_EyeBoss);
		wp->setBltResTag(ResTag::Bullet_BossEye1);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltSpeed(100);
		wp->setBltTailName("particla/eyes_bullet/test1.plist", -1);
		wp->setShootTimes(2);
		wp->setDelayShoot(delayshoot);
		wp->setShootSpeed(interval);
		wp->setBltGravityAble(true);
		wp->setBltLinearDamping(3.0);
		wp->setPosition(eye->getPosition());
		wp->setRotation(eye->getWpAngle() + 90);
		wp->StartShoot();
		wp->setBltHurts(-1);
		armat->addChild(wp);
		wp->setName("weapon");
		count += 2;
	}
	{
		float delayshoot = interval * count;
		auto eye = getEye(1);
		eye->OpenEyeWithFunc(delayshoot,1,nullptr, nullptr);

		Weapon* wp = Weapon::create(armat);
		wp->setBullstType(BulletType::FallBullet_EyeBoss);
		wp->setBltResTag(ResTag::Bullet_BossEye1);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltTailName("particla/eyes_bullet/test1.plist", -1);
		wp->setBltSpeed(100);
		wp->setShootTimes(2);
		wp->setDelayShoot(delayshoot);
		wp->setShootSpeed(interval);
		wp->setBltGravityAble(true);
		wp->setBltLinearDamping(3.0);
		wp->setPosition(eye->getPosition());
		wp->setRotation(eye->getWpAngle() + 90);
		wp->StartShoot();
		wp->setBltHurts(-1);
		armat->addChild(wp);
		wp->setName("weapon");
		count += 2;
	}
	{
		float delayshoot = interval * count;
		auto eye = getEye(2);
		eye->OpenEyeWithFunc(delayshoot,1,nullptr, nullptr);

		Weapon* wp = Weapon::create(armat);
		wp->setBullstType(BulletType::FallBullet_EyeBoss);
		wp->setBltResTag(ResTag::Bullet_BossEye1);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltTailName("particla/eyes_bullet/test1.plist", -1);
		wp->setBltSpeed(100);
		wp->setShootTimes(2);
		wp->setDelayShoot(delayshoot);
		wp->setShootSpeed(interval);
		wp->setBltGravityAble(true);
		wp->setBltLinearDamping(3.0);
		wp->setPosition(eye->getPosition());
		wp->setRotation(eye->getWpAngle() + 90);
		wp->StartShoot();
		wp->setBltHurts(-1);
		armat->addChild(wp);
		wp->setName("weapon");
		count += 2;
	}
	{
		float delayshoot = interval * count;
		auto eye = getEye(3);
		eye->OpenEyeWithFunc(delayshoot,1,nullptr, nullptr);

		Weapon* wp = Weapon::create(armat);
		wp->setBullstType(BulletType::FallBullet_EyeBoss);
		wp->setBltResTag(ResTag::Bullet_BossEye1);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltTailName("particla/eyes_bullet/test1.plist", -1);
		wp->setBltSpeed(100);
		wp->setShootTimes(2);
		wp->setDelayShoot(delayshoot);
		wp->setShootSpeed(interval);
		wp->setBltGravityAble(true);
		wp->setBltLinearDamping(3.0);
		wp->setPosition(eye->getPosition());
		wp->setRotation(eye->getWpAngle() + 90);
		wp->setBltHurts(-1);
		wp->StartShoot();
		wp->setName("weapon");
		armat->addChild(wp);
	}
	{
		auto delay = DelayTime::create(8.0f);
		auto next = CallFunc::create(this, callfunc_selector(EyeBossAi::NextAction));
		auto seq = Sequence::create(delay, next, nullptr);
		runAction(seq);
	}
}

void EyeBossAi::shootTelescopicLaser()
{
	if (m_TelescopicLaserTimes > 3)
	{
		m_TelescopicLaserTimes = 0;
		NextAction();
		return;
	}
	auto armat = getArmature();

	int count = 0;
	float interval = 0.5;

	{
		float delayshoot = interval * count;
		auto eye = getEye(0);
		eye->OpenEyeWithFunc(delayshoot,1,nullptr, nullptr);

		Weapon* wp = Weapon::create(armat);
		wp->setBullstType(BulletType::TelescopicLaser_EyeBoss);
		wp->setBltResTag(ResTag::Bullet_BossEye2);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltSpeed(100);
		wp->setShootTimes(1);
		wp->setDelayShoot(delayshoot);
		wp->setBltGravityAble(false);
		wp->setPosition(eye->getPosition());
		wp->setRotation(eye->getWpAngle() + 90);
		wp->setBltHurts(-1);
		wp->StartShoot();
		armat->addChild(wp);
		count += 2;
	}
	{
		float delayshoot = interval * count;
		auto eye = getEye(1);
		eye->OpenEyeWithFunc(delayshoot,1,nullptr, nullptr);

		Weapon* wp = Weapon::create(armat);
		wp->setBullstType(BulletType::TelescopicLaser_EyeBoss);
		wp->setBltResTag(ResTag::Bullet_BossEye2);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltSpeed(100);
		wp->setShootTimes(1);
		wp->setDelayShoot(delayshoot);
		wp->setBltGravityAble(false);
		wp->setPosition(eye->getPosition());
		wp->setRotation(eye->getWpAngle() + 90);
		wp->StartShoot();
		armat->addChild(wp);
		count += 2;
	}
	{
		float delayshoot = interval * count;
		auto eye = getEye(2);
		eye->OpenEyeWithFunc(delayshoot,1,nullptr, nullptr);

		Weapon* wp = Weapon::create(armat);
		wp->setBullstType(BulletType::TelescopicLaser_EyeBoss);
		wp->setBltResTag(ResTag::Bullet_BossEye3);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltSpeed(100);
		wp->setShootTimes(1);
		wp->setDelayShoot(delayshoot);
		wp->setBltGravityAble(false);
		wp->setPosition(eye->getPosition());
		wp->setRotation(eye->getWpAngle() + 90);
		wp->StartShoot();
		armat->addChild(wp);
		count += 2;
	}
	{
		float delayshoot = interval * count;
		auto eye = getEye(3);
		eye->OpenEyeWithFunc(delayshoot,1,nullptr, nullptr);

		Weapon* wp = Weapon::create(armat);
		wp->setBullstType(BulletType::TelescopicLaser_EyeBoss);
		wp->setBltResTag(ResTag::Bullet_BossEye4);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltSpeed(100);
		wp->setShootTimes(1);
		wp->setDelayShoot(delayshoot);
		wp->setBltGravityAble(false);
		wp->setPosition(eye->getPosition());
		wp->setRotation(eye->getWpAngle() + 90);
		wp->StartShoot();
		armat->addChild(wp);
	}
	{
		auto delay = DelayTime::create(5.0f);
		auto next = CallFunc::create(this, callfunc_selector(EyeBossAi::shootTelescopicLaser));
		auto seq = Sequence::create(delay, next, nullptr);
		runAction(seq);
	}
	m_TelescopicLaserTimes++;
}

void EyeBossAi::shootBezierBullet()
{
	if (m_BezierBulletTimes > 0)
	{
		m_BezierBulletTimes = 0;
		NextAction();
		return;
	}
	auto armat = getArmature();

	int count = 0;
	float interval = 0.5;

	{
		float delayshoot = interval * count;
		auto eye = getEye(0);
		eye->OpenEyeWithFunc(delayshoot,1,nullptr, nullptr);

		Weapon* wp = Weapon::create(armat);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setBltResTag(ResTag::Bullet_BossEye1);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltTailName("particla/eyes_bullet/test1.plist", -1);
		wp->setBltSpeed(500);
		wp->setShootTimes(12);
		wp->setDelayShoot(delayshoot);
		wp->setBltGravityAble(true);
		wp->setPosition(eye->getPosition());
		wp->setRotation(eye->getWpAngle() + 90);
		wp->setBltLinearDamping(1.0);
		wp->StartShoot();
		wp->setBltHurts(-1);
		armat->addChild(wp);
		wp->runAction(RotateBy::create(10,3600));
		count += 2;
	}
	{
		float delayshoot = interval * count;
		auto eye = getEye(1);
		eye->OpenEyeWithFunc(delayshoot,1,nullptr, nullptr);

		Weapon* wp = Weapon::create(armat);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setBltResTag(ResTag::Bullet_BossEye1);
		wp->setBltTailName("particla/eyes_bullet/test1.plist", -1);
		wp->setBltSpeed(500);
		wp->setShootTimes(12);
		wp->setDelayShoot(delayshoot);
		wp->setBltGravityAble(true);
		wp->setPosition(eye->getPosition());
		wp->setRotation(eye->getWpAngle() + 90);
		wp->setBltLinearDamping(1.0);
		wp->StartShoot();
		wp->setBltHurts(-1);
		armat->addChild(wp);
		wp->runAction(RotateBy::create(10,3600));
		count += 2;
	}
	{
		float delayshoot = interval * count;
		auto eye = getEye(2);
		eye->OpenEyeWithFunc(delayshoot,1,nullptr, nullptr);

		Weapon* wp = Weapon::create(armat);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setBltResTag(ResTag::Bullet_BossEye1);
		wp->setBltTailName("particla/eyes_bullet/test1.plist", -1);
		wp->setBltSpeed(500);
		wp->setShootTimes(12);
		wp->setDelayShoot(delayshoot);
		wp->setBltGravityAble(true);
		wp->setPosition(eye->getPosition());
		wp->setRotation(eye->getWpAngle() + 90);
		wp->setBltLinearDamping(1.0);
		wp->StartShoot();
		wp->setBltHurts(-1);
		armat->addChild(wp);
		wp->runAction(RotateBy::create(10,3600));
		count += 2;
	}
	{
		float delayshoot = interval * count;
		auto eye = getEye(3);
		eye->OpenEyeWithFunc(delayshoot,1,nullptr, nullptr);

		Weapon* wp = Weapon::create(armat);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setBltResTag(ResTag::Bullet_BossEye1);
		wp->setBltTailName("particla/eyes_bullet/test1.plist", -1);
		wp->setBltSpeed(500);
		wp->setShootTimes(12);
		wp->setDelayShoot(delayshoot);
		wp->setBltGravityAble(true);
		wp->setPosition(eye->getPosition());
		wp->setRotation(eye->getWpAngle() + 90);
		wp->setBltLinearDamping(1.0);
		wp->setBltHurts(-1);
		wp->StartShoot();
		wp->runAction(RotateBy::create(10,3600));
		armat->addChild(wp);
	}
	{
		auto delay = DelayTime::create(5.0f);
		auto next = CallFunc::create(this, callfunc_selector(EyeBossAi::shootBezierBullet));
		auto seq = Sequence::create(delay, next, nullptr);
		runAction(seq);
	}
	m_BezierBulletTimes++;
}

void EyeBossAi::shootSaosheLaser()
{
	float dur = 5.0f;
	auto eye = getEye(4);
	auto s = eye->getContentSize();
	DanxiangLaserOfEyeBoss* laser = DanxiangLaserOfEyeBoss::create(dur);
	laser->setBelongTo(UnitId::ePlaneBoss);
	laser->setLaserLength(1300);
	laser->setPosition(s.width * 0.5, s.height * 0.5);
	laser->setLaserWidth(4.0f);
	eye->addChild(laser);
	laser->LaserSaoshe(1.0f, 90, 15);

}

void EyeBossAi::shootMidLaser()
{
	float dur = 2.0f;
	auto eye = getEye(4);
	auto s = eye->getContentSize();
	DanxiangLaserOfEyeBoss* laser = DanxiangLaserOfEyeBoss::create(dur);
	laser->setBelongTo(UnitId::ePlaneBoss);
	laser->setLaserLength(1300);
	laser->setPosition(s.width * 0.5, s.height * 0.5);
	laser->setLaserWidth(4.0f);
	eye->addChild(laser);
	laser->LaserSaoshe(1.0f, 90, 0);
}

void EyeBossAi::shootMidLaser1()
{
	float dur = 10.0f;
	auto eye = getEye(4);
	auto s = eye->getContentSize();
	DanxiangLaserOfEyeBoss* laser = DanxiangLaserOfEyeBoss::create(dur);
	laser->setBelongTo(UnitId::ePlaneBoss);
	laser->setLaserLength(1300);
	laser->setPosition(s.width * 0.5, s.height * 0.5);
	laser->setLaserWidth(4.0f);
	eye->addChild(laser);
//	eye->setRotation(90);
	laser->LaserSaoshe(1.0f, 90, 0);
}

void EyeBossAi::shootJiaochaEye1()
{
	float dur = 6.7;
	auto eye = getEye(1);
	auto s = eye->getContentSize();
	DanxiangLaserOfEyeBoss* laser = DanxiangLaserOfEyeBoss::create(dur);
	laser->setBelongTo(UnitId::ePlaneBoss);
	laser->setLaserLength(1300);
	laser->setPosition(s.width * 0.5, s.height * 0.5);
	eye->addChild(laser);
	laser->LaserSaoshe(2.0f, 120, -30);
}

void EyeBossAi::shootJiaochaEye2()
{
	float dur = 6.7f;
	auto eye = getEye(2);
	auto s = eye->getContentSize();
	DanxiangLaserOfEyeBoss* laser = DanxiangLaserOfEyeBoss::create(dur);
	laser->setBelongTo(UnitId::ePlaneBoss);
	laser->setLaserLength(1300);
	laser->setPosition(s.width * 0.5, s.height * 0.5);
	eye->addChild(laser);
	laser->LaserSaoshe(2.0f, 60, 30);
}

void EyeBossAi::shootZhixianEye1()
{
	float dur = 4.0;
	auto eye = getEye(1);
	auto s = eye->getContentSize();
	DanxiangLaserOfEyeBoss* laser = DanxiangLaserOfEyeBoss::create(dur);
	laser->setBelongTo(UnitId::ePlaneBoss);
	laser->setLaserLength(1300);
	laser->setPosition(s.width * 0.5, s.height * 0.5);
	eye->addChild(laser);
	laser->LaserSaoshe(2.0f, 95, 0);
}

void EyeBossAi::shootZhixianEye2()
{
	float dur = 4.0;
	auto eye = getEye(2);
	auto s = eye->getContentSize();
	DanxiangLaserOfEyeBoss* laser = DanxiangLaserOfEyeBoss::create(dur);
	laser->setBelongTo(UnitId::ePlaneBoss);
	laser->setLaserLength(1300);
	laser->setPosition(s.width * 0.5, s.height * 0.5);
	eye->addChild(laser);
	laser->LaserSaoshe(2.0f, 85, 0);
}

void EyeBossAi::DelayShootZhixianEye1()
{
	auto eye = getEye(1);
	eye->OpenEyeWithFunc(0.0f,5.0f,this, callfunc_selector(EyeBossAi::shootZhixianEye1),2);
}

void EyeBossAi::DelayShootZhixianEye2()
{
	auto eye = getEye(2);
	eye->OpenEyeWithFunc(0.0f,5.0f,this, callfunc_selector(EyeBossAi::shootZhixianEye2),2);
}

void EyeBossAi::shootZhixianEye0()
{
	float dur = 7.0f;
	auto eye = getEye(0);
	auto s = eye->getContentSize();
	DanxiangLaserOfEyeBoss* laser = DanxiangLaserOfEyeBoss::create(dur);
	laser->setBelongTo(UnitId::ePlaneBoss);
	laser->setLaserLength(1300);
	laser->setPosition(s.width * 0.5, s.height * 0.5);
	eye->addChild(laser);
//	eye->setRotation(65);
	laser->LaserSaoshe(2.0f, 65, 0);
}

void EyeBossAi::shootZhixianEye3()
{
	float dur = 7.0f;
	auto eye = getEye(3);
	auto s = eye->getContentSize();
	DanxiangLaserOfEyeBoss* laser = DanxiangLaserOfEyeBoss::create(dur);
	laser->setBelongTo(UnitId::ePlaneBoss);
	laser->setLaserLength(1300);
	laser->setPosition(s.width * 0.5, s.height * 0.5);
	eye->addChild(laser);
//	eye->setRotation(115);
	laser->LaserSaoshe(2.0f, 115, 0);
}

void EyeBossAi::SwitchEyeState( int id, bool isOpen )
{
	if (id > 5 || id < 0)
	{
		return;
	}
	BaseBoss* boss = (BaseBoss*)m_target;
	auto armat = boss->getArmature();

	std::string BoneName = String::createWithFormat("EyeClose_%d",id)->getCString();
	Bone* eye = armat->getBone(BoneName);
	bool EyeState = isOpen;
	if (id == 0)
	{
		//´óÑÛµÄÕö¿ª±ÕºÏ£¬ÌØÊâ´¦Àí
		EyeState = !isOpen;
	}
	
	eye->getDisplayRenderNode()->setVisible(!EyeState);
}

void EyeBossAi::InitEyes()
{
	auto armat = getArmature();
	auto offset = Vec2(-16, 41);
	{
		auto pos = getBonePos("Eye_1");
		EyeOfBoss* eye = EyeOfBoss::create("siyan3.png","siyan-.png");
		eye->setPosition(pos + offset);
		eye->setWpAngle(-180);
		armat->addChild(eye,100);
		m_EyeVec.push_back(eye);
	}
	{
		auto pos = getBonePos("Eye_2");
		EyeOfBoss* eye = EyeOfBoss::create("siyan2.png","siyan-.png");
		eye->setPosition(pos + offset);
		eye->setWpAngle(-165);
		armat->addChild(eye,100);
		m_EyeVec.push_back(eye);
	}
	{
		auto pos = getBonePos("Eye_3");
		EyeOfBoss* eye = EyeOfBoss::create("siyan.png","siyan-.png");
		eye->setPosition(pos + offset);
		eye->setWpAngle(-135);
		armat->addChild(eye,100);
		m_EyeVec.push_back(eye);
	}
	{
		auto pos = getBonePos("Eye_4");
		EyeOfBoss* eye = EyeOfBoss::create("siyan1.png","siyan-.png");
		eye->setPosition(pos + offset);
		eye->setWpAngle(-150);
		armat->addChild(eye,100);
		m_EyeVec.push_back(eye);
	}
	{
		auto pos = getBonePos("EyeClose_0");
		EyeOfBoss* eye = EyeOfBoss::create("siyan4.png","siyan5.png");
		eye->setPosition(pos + offset);
		eye->setWpAngle(90);
		armat->addChild(eye,100);
		m_EyeVec.push_back(eye);
	}
}

Bone* EyeBossAi::getBone( const std::string& boneName )
{
	BaseBoss* boss = (BaseBoss*)m_target;
	auto armat = boss->getArmature();
	auto bone = armat->getBone(boneName);
	return bone;
}

Armature* EyeBossAi::getArmature()
{
	BaseBoss* boss = (BaseBoss*)m_target;
	auto armat = boss->getArmature();
	return armat;
}

cocos2d::Vec2 EyeBossAi::getBonePos( const std::string& boneName )
{
	auto rootPos = getBone("Layer30")->getDisplayRenderNode()->convertToWorldSpaceAR(Vec2::ZERO);
	auto pos = getBone(boneName)->getDisplayRenderNode()->convertToWorldSpaceAR(Vec2::ZERO);
	return pos - rootPos;
}

EyeOfBoss* EyeBossAi::getEye( int idx )
{
	return m_EyeVec.at(idx);
}

void EyeBossAi::ActionRest()
{
	MoveLR();
	playArmat(1);

	{
		auto delay = DelayTime::create(3.0f + rand_minus1_1() * 1.5f);
		auto next = CallFunc::create(this, callfunc_selector(EyeBossAi::NextAction));
		auto seq = Sequence::create(delay, next, nullptr);
		runAction(seq);
	}
}

void EyeBossAi::ActionShootFallingBullet()
{
	playArmat(0);
	MoveLR();
	shootFallingBullet();
}

void EyeBossAi::ActionShootTelescopicLaser()
{
	playArmat(0);
	MoveLR();

	shootTelescopicLaser();
}

void EyeBossAi::ActionShootBezierBullet()
{
	playArmat(0);
	MoveLR();
	shootBezierBullet();
}

void EyeBossAi::ActionShootSaosheLaser()
{
	playArmat(0);
	MoveLR();

	auto eye = getEye(4);
	eye->OpenEyeWithFunc(1.0,5.0f,this, callfunc_selector(EyeBossAi::shootSaosheLaser),3);

	{
		auto delay = DelayTime::create(10.0f);
		auto next = CallFunc::create(this, callfunc_selector(EyeBossAi::NextAction));
		auto seq = Sequence::create(delay, next, nullptr);
		runAction(seq);
	}
}

void EyeBossAi::Action3LaserAnd2Bullet()
{
	playArmat(0);
	MoveLR();

	{
		auto eye = getEye(4);
		eye->OpenEyeWithFunc(0.5,2.0f,this, callfunc_selector(EyeBossAi::shootMidLaser),3);
		eye->OpenEyeWithFunc(10.8,2.0f,this, callfunc_selector(EyeBossAi::shootMidLaser),3);
	}

	{
		auto eye = getEye(1);
		eye->OpenEyeWithFunc(3.0,5.0f,this, callfunc_selector(EyeBossAi::shootJiaochaEye1),3);
	}

	{
		auto eye = getEye(2);
		eye->OpenEyeWithFunc(3.0,5.0f,this, callfunc_selector(EyeBossAi::shootJiaochaEye2),3);
	}

	/*auto armat = getArmature();
	float delayshoot = 3.0f;
	float dur = 3.0f;
	{
		auto eye = getEye(0);
		eye->OpenEyeWithFunc(delayshoot,dur,nullptr, nullptr, 2);

		Weapon* wp = Weapon::create(armat);
		wp->setBullstType(BulletType::BezierBullet_EyeBoss);
		wp->setBltResTag(ResTag::Bullet_BossEye1);
		wp->setBltSpeed(100);
		wp->setShootSpeed(0.5f);
		wp->setShootTimes(dur / 0.5);
		wp->setDelayShoot(delayshoot);
		wp->setBltGravityAble(false);
		wp->setPosition(eye->getPosition());
		wp->setRotation(eye->getWpAngle() + 90);
		wp->StartShoot();
		armat->addChild(wp);
	}
	{
		auto eye = getEye(3);
		eye->OpenEyeWithFunc(delayshoot,dur,nullptr, nullptr, 2);

		Weapon* wp = Weapon::create(armat);
		wp->setBullstType(BulletType::BezierBullet_EyeBoss);
		wp->setBltResTag(ResTag::Bullet_BossEye1);
		wp->setBltSpeed(100);
		wp->setShootSpeed(0.5f);
		wp->setShootTimes(dur / 0.5);
		wp->setDelayShoot(delayshoot);
		wp->setBltGravityAble(false);
		wp->setPosition(eye->getPosition());
		wp->setRotation(eye->getWpAngle() + 90);
		wp->StartShoot();
		armat->addChild(wp);
	}*/

	{
		auto delay = DelayTime::create(15.0f);
		auto next = CallFunc::create(this, callfunc_selector(EyeBossAi::NextAction));
		auto seq = Sequence::create(delay, next, nullptr);
		runAction(seq);
	}
}

void EyeBossAi::Action5Laser()
{
	playArmat(0);
	MoveLRSlowly();

	{
		auto eye = getEye(4);
		eye->OpenEyeWithFunc(8.0f,5.0f,this, callfunc_selector(EyeBossAi::shootMidLaser1),2);
	}

	{
		auto eye = getEye(1);
		eye->OpenEyeWithFunc(0.5,5.0f,this, callfunc_selector(EyeBossAi::shootJiaochaEye1),2);
		auto delay = DelayTime::create(14);
		auto func = CallFunc::create(this, callfunc_selector(EyeBossAi::DelayShootZhixianEye1));
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}

	{
		auto eye = getEye(2);
		eye->OpenEyeWithFunc(0.5f,5.0f,this, callfunc_selector(EyeBossAi::shootJiaochaEye2),2);
		auto delay = DelayTime::create(14);
		auto func = CallFunc::create(this, callfunc_selector(EyeBossAi::DelayShootZhixianEye2));
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}

	{
		auto eye = getEye(0);
		eye->OpenEyeWithFunc(11.0f,5.0f,this, callfunc_selector(EyeBossAi::shootZhixianEye0),2);
	}

	{
		auto eye = getEye(3);
		eye->OpenEyeWithFunc(11.0f,5.0f,this, callfunc_selector(EyeBossAi::shootZhixianEye3),2);
	}

	{
		auto delay = DelayTime::create(25.0f);
		auto next = CallFunc::create(this, callfunc_selector(EyeBossAi::NextAction));
		auto seq = Sequence::create(delay, next, nullptr);
		runAction(seq);
	}
}

void EyeBossAi::ActionWujiaoxing()
{
	playArmat(0);
	DrawShapeOfEyeBoss* layer = DrawShapeOfEyeBoss::create();
	UIController::getInstance()->getPhysicsLayer()->addChild(layer);

	{
		auto delay = DelayTime::create(15.0f);
		auto next = CallFunc::create(this, callfunc_selector(EyeBossAi::NextAction));
		auto seq = Sequence::create(delay, next, nullptr);
		runAction(seq);
	}

}

void EyeBossAi::MoveLR()
{
	m_target->stopAllActions();
	//×óÓÒËæ»úÒÆ¶¯
	float t = 2.0 + rand_0_1() * 1.0;

	auto mypos = m_target->convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 mvDirect;
	if (mypos.x < m_winSize.width * 0.5)
	{
		mvDirect = Vec2(m_winSize.width * 0.7 - mypos.x, 0);
	}
	else
	{
		mvDirect = Vec2(m_winSize.width * 0.3 - mypos.x, 0);
	}
	auto mv = MoveBy::create(t, mvDirect);
	auto next = CallFunc::create(this, callfunc_selector(BossAI::MoveLR));
	auto seq = Sequence::create(mv, next, nullptr);

	m_target->runAction(seq);
}

void EyeBossAi::MoveLRSlowly()
{
	m_target->stopAllActions();
	//×óÓÒËæ»úÒÆ¶¯
	float t = 4.0 + rand_0_1() * 1.0;

	auto mypos = m_target->convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 mvDirect;
	if (mypos.x < m_winSize.width * 0.5)
	{
		mvDirect = Vec2(m_winSize.width * 0.7 - mypos.x, 0);
	}
	else
	{
		mvDirect = Vec2(m_winSize.width * 0.3 - mypos.x, 0);
	}
	auto mv = MoveBy::create(t, mvDirect);
	auto next = CallFunc::create(this, callfunc_selector(EyeBossAi::MoveLRSlowly));
	auto seq = Sequence::create(mv, next, nullptr);

	m_target->runAction(seq);
}

void EyeBossAi::setEyesVisible( bool b )
{
	for (int i = 0; i < m_EyeVec.size(); i++)
	{
		EyeOfBoss* eye = m_EyeVec.at(i);
		eye->setVisible(b);
	}
}

std::vector<int> EyeBossAi::initAction()
{
	int HpNums = getLeftHpNum();
	std::vector<int>	tagVec;

	//Ëæ»ú
	switch (m_AIlvl)
	{
	case 1:
		{
			tagVec.push_back(1);
			tagVec.push_back(2);
			tagVec.push_back(3);
		}
		break;
	case 2:
		{
			tagVec.push_back(4);
			tagVec.push_back(5);
			tagVec.push_back(6);
			tagVec.push_back(7);
		}
		break;

	default:
		break;
	}
	return tagVec;
}

void EyeBossAi::SelectAction( int ActionId )
{
	setEyesVisible(ActionId);
	switch (ActionId)
	{
	case 0:
		ActionRest();
		break;
	case 1:
		ActionShootFallingBullet();
		break;
	case 2:
		ActionShootSaosheLaser();
		break;
	case 3:
		Action3LaserAnd2Bullet();
		break;
	case 4:
		ActionShootBezierBullet();
		break;
	case 5:
		Action5Laser();
		break;
	case 6:
		ActionShootTelescopicLaser();
		break;
	case 7:
		ActionWujiaoxing();
		break;

	default:
		break;
	}
}

void EyeBossAi::FuncWhenTargetHpEmpry()
{
	auto armat = getArmature();
	int num = armat->getChildrenCount();
	for (int i = 0; i < num; i++)
	{
		auto child = armat->getChildByName("weapon");
		if (child)
		{
			child->removeFromParentAndCleanup(true);
		}
	}
}


//-------------------------
void YizhiBossAI::StartAI( int lvl )
{
	m_ShootAutoExplTimes = 0;
	m_ShootMissileTimes = 0;
	m_ShootMissileMaxTimes = 1;
	m_ShootExplAndMissile = 0;
	m_action3Count = 0;
	CreateGun();
	BossAI::StartAI(lvl);
}

void YizhiBossAI::ActionShoot5AutoExpl()
{
	MoveLR();
	shootOneAutoExposion();
}

void YizhiBossAI::ActionShoot4Missile()
{
	MoveToMid();
	m_ShootMissileTimes = 0;
	m_ShootMissileMaxTimes = 1;
	playArmat(2,this,frameEvent_selector(YizhiBossAI::onFrameEventOfSpiralMissile1),1.5f);
}

void YizhiBossAI::ActionShoot5SpeedUpBullet()
{
	MoveToMid();
	playArmat(6,this,frameEvent_selector(YizhiBossAI::onFrameEventOfSpeedUpBullet),0.25f);
}

void YizhiBossAI::ActionSootAutoExplAndMissile()
{
	MoveToMid();
	shootOneAutoExposion();
	m_ShootMissileMaxTimes = 1;
	m_ShootMissileTimes = 0;
	playArmat(2,this,frameEvent_selector(YizhiBossAI::onFrameEventOfSpiralMissile1),1.0f);
}

void YizhiBossAI::ActionDanmu()
{
	MoveLR();
	shootDanmu("Eye_1");
	m_ShootMissileTimes = 0;
	playArmat(2,this,frameEvent_selector(YizhiBossAI::onFrameEventOfDanmu1),1.0f);
}

void YizhiBossAI::ActionHuiXuan()
{
	MoveLR();
	playArmat(0);
	shootHuiXuan();
}

void YizhiBossAI::shootOneAutoExposion()
{
	if (m_ShootAutoExplTimes > 2)
	{
		m_ShootAutoExplTimes = 0;
		NextAction();
		return;
	}

	//¸ù¾ÝÄ¿±êµÄÎ»ÖÃÐý×ªÏàÓ¦µÄ½Ç¶È
	{
		auto mainPl = GameData::getInstance()->getMainPlane();
		if (mainPl)
		{
			auto plPos = mainPl->convertToWorldSpaceAR(Vec2::ZERO);
			auto gunPos = m_Gun->convertToWorldSpaceAR(Vec2::ZERO);
			auto newVec = plPos - gunPos;
			float r = newVec.getAngle() * RAD_DEG;
			GunRotation(-r);
		}
	}

	Weapon* wp = Weapon::create(m_Gun);
	wp->setBullstType(BulletType::YizhiBoss_AutoExpl);
	wp->setBltBelongTo(UnitId::ePlaneBoss);
	wp->setBltFrameName("renzhe17.png");
	wp->setBltTailName("particla/MissileTail/penwei01.plist");
	wp->setBltSpeed(500);
	wp->setShootTimes(1);
	wp->setBltScaleX(2.5);
	wp->setBltScaleY(2.5);
	wp->setDelayShoot(0.1f);
	wp->setBltGravityAble(false);
	wp->setPosition(getGunShootPos());
	wp->setRotation(185);
	wp->StartShoot();
	m_Gun->addChild(wp);	

	//ÖØ¸´5´Î
	auto delay = DelayTime::create(3.0f);
	auto goOn = CallFunc::create(this, callfunc_selector(YizhiBossAI::shootOneAutoExposion));
	auto seq = Sequence::create(delay, goOn, nullptr);
	runAction(seq);

	m_ShootAutoExplTimes++;
}

void YizhiBossAI::shootOneSpiralMissile(const std::string& boneName)
{
	auto armat = getArmature();
	Weapon* wp = Weapon::create(armat);
	wp->setBullstType(BulletType::YizhiBoss_SpiralMissile);
	wp->setBltBelongTo(UnitId::ePlaneBoss);
	wp->setBltFrameName("renzhe17.png");
	wp->setBltTailName("particla/MissileTail/penwei01.plist");
	wp->setBltSpeed(300);
	wp->setShootTimes(1);
	wp->setBltScaleX(1.5);
	wp->setBltScaleY(1.5);
	wp->setBltLinearDamping(0.0);
	wp->setRotation(180);
	wp->setPosition(getBonePos(boneName));
	wp->StartShoot();
	wp->setBltHurts(-1);
	armat->addChild(wp);
}

void YizhiBossAI::shootOneAutoExposion( const std::string& boneName, float angle, float t )
{
	auto armat = getArmature();
	AutoExplosionOfYizhiBoss* bullet = AutoExplosionOfYizhiBoss::create("renzhe17.png");
	bullet->setBelongTo(UnitId::ePlaneBoss);
	bullet->setPosition(getBone(boneName)->getDisplayRenderNode()->convertToWorldSpaceAR(Vec2::ZERO));
	bullet->setExplosionTime(t);
	bullet->setScale(1.5f);
	bullet->getPhysicsBody()->setLinearDamping(0.7);
	bullet->setBitmask(ENEMY_AMMO_CATEGORYBITMASK, 0, ENEMY_AMMO_CONTACTTESTBITMASK);
	EffectsManager::addParticle("particla/MissileTail/penwei01.plist",bullet,Vec2(bullet->getContentSize().width*0.5,5),false);
	UIController::getInstance()->getPhysicsLayer()->addChild(bullet);
	bullet->getPhysicsBody()->setRotation(180 + angle);
	bullet->setBltFlySpeed(700 - t * 100);
}

void YizhiBossAI::shootDanmu( const std::string& boneName )
{
	auto armat = getArmature();
	int randNum = random(0,1);
	for (int i = 0; i < 10; i++)
	{
		Weapon* wp = Weapon::create(armat);
		wp->setBullstType(BulletType::YizhiBoss_SpeedDownAndUniform);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltFrameName("yizhiBoss_ammo.png");
		wp->setBltSpeed(1000);
		wp->setBltSelfRotate(360);
		wp->setShootTimes(4);
		wp->setShootSpeed(0.15f);
		wp->setBltLinearDamping(2.0);
		wp->setRotation(36 * i + 18);
		wp->setPosition(getBonePos(boneName));
		wp->StartShoot();
		armat->addChild(wp);
		wp->runAction(RotateBy::create(10.0, 360 - randNum * 720));
	}
	
}

void YizhiBossAI::shootHuiXuan()
{
	//¸ù¾ÝÄ¿±êµÄÎ»ÖÃÐý×ªÏàÓ¦µÄ½Ç¶È
	{
		auto mainPl = GameData::getInstance()->getMainPlane();
		if (mainPl)
		{
			auto plPos = mainPl->convertToWorldSpaceAR(Vec2::ZERO);
			auto gunPos = m_Gun->convertToWorldSpaceAR(Vec2::ZERO);
			auto newVec = plPos - gunPos;
			float r = newVec.getAngle() * RAD_DEG;
			GunRotation(-r);
		}
	}
	Weapon* wp = Weapon::create(m_Gun);
	wp->setBullstType(BulletType::YizhiBoss_ExplosionCyclotron);
	wp->setBltBelongTo(UnitId::ePlaneBoss);
	wp->setBltFrameName("renzhe17.png");
	wp->setBltTailName("particla/MissileTail/penwei01.plist");
	wp->setBltSpeed(600);
	wp->setShootTimes(1);
	wp->setRotation(185);
	wp->setBltScaleX(1.5);
	wp->setBltScaleY(1.5);
	wp->setPosition(getGunShootPos());
	wp->StartShoot();
	m_Gun->addChild(wp);

	if (m_ShootExplAndMissile > 2)
	{
		m_ShootExplAndMissile = 0;
		NextActionDelay(5);
		return;
	}

	{
		auto delay = DelayTime::create(2.0f);
		auto func = CallFunc::create(this, callfunc_selector(YizhiBossAI::shootHuiXuan));
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}

	m_ShootExplAndMissile++;

}

void YizhiBossAI::CreateGun()
{
	auto armat = getArmature();
	m_Gun = Sprite::createWithSpriteFrameName("renzhe3.png");
	m_Gun->setAnchorPoint(Vec2(0.55, 0.8));
	m_Gun->setPosition(-20, -20);
	armat->addChild(m_Gun,100);
	m_Gun->setVisible(false);
}

void YizhiBossAI::GunRotation( float r )
{
	float angle = r - 95;
	m_Gun->setRotation(angle);
	auto bone = getBone("Gun");
	//bone->setRotation(angle);
	bone->runAction(RotateTo::create(0.1f, angle - 10));

	{
		//ºó×øÁ¦
		auto mv = MoveBy::create(0.03, Vec2(-50, 0));
		auto back = mv->reverse();
		auto seq = Sequence::create(mv, back, nullptr);
		bone->runAction(seq);
	}
}

Armature* YizhiBossAI::getArmature()
{
	BaseBoss* boss = (BaseBoss*)m_target;
	auto armat = boss->getArmature();
	return armat;
}

Bone* YizhiBossAI::getBone( const std::string& boneName )
{
	BaseBoss* boss = (BaseBoss*)m_target;
	auto armat = boss->getArmature();
	auto bone = armat->getBone(boneName);
	return bone;
}

cocos2d::Vec2 YizhiBossAI::getGunShootPos()
{
	auto s = m_Gun->getContentSize();
	return Vec2(s.width * 0.5, -45);
}

void YizhiBossAI::onFrameEventOfSpiralMissile1( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		shootOneSpiralMissile("Eye_1");
	}
	else if(evt == "end")
	{
		//ÏÂÒ»Ö»ÑÛ¾¦
		playArmat(3,this,frameEvent_selector(YizhiBossAI::onFrameEventOfSpiralMissile2),1.5f);
	}
}

void YizhiBossAI::onFrameEventOfSpiralMissile2( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		shootOneSpiralMissile("Eye_2");
	}
	else if(evt == "end")
	{
		//ÏÂÒ»Ö»ÑÛ¾¦
		playArmat(4,this,frameEvent_selector(YizhiBossAI::onFrameEventOfSpiralMissile3),1.5f);
	}
}

void YizhiBossAI::onFrameEventOfSpiralMissile3( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		shootOneSpiralMissile("Eye_3");
	}
	else if(evt == "end")
	{
		//ÏÂÒ»Ö»ÑÛ¾¦
		playArmat(5,this,frameEvent_selector(YizhiBossAI::onFrameEventOfSpiralMissile4),1.5f);
	}
}

void YizhiBossAI::onFrameEventOfSpiralMissile4( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		shootOneSpiralMissile("Eye_4");
	}
	else if(evt == "end")
	{
		if (m_ShootMissileTimes >= m_ShootMissileMaxTimes)
		{
			//ÏÂÒ»¸ö¶¯×÷
			NextActionDelay(5.0f);
			return;
		}
		playArmat(2,this,frameEvent_selector(YizhiBossAI::onFrameEventOfSpiralMissile1),1.0f);
		m_ShootMissileTimes++;
	}
}

void YizhiBossAI::onFrameEventOfSpeedUpBullet( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		m_action3Count++;
		shootOneAutoExposion("Eye_1",10,2.5);
		shootOneAutoExposion("Eye_2",6,1.0f);
		shootOneAutoExposion("Eye_3",-13,2.5);
		shootOneAutoExposion("Eye_4",-3,1.0);
		//shootOneAutoExposion("Gun",0,0.5);
	}
	else if(evt == "end")
	{
		if (m_action3Count > 2)
		{
			m_action3Count = 0;
			NextActionDelay(1.5f);
		}
	}
}

cocos2d::Vec2 YizhiBossAI::getBonePos( const std::string& boneName )
{
	auto rootPos = getBone("Layer13")->getDisplayRenderNode()->convertToWorldSpaceAR(Vec2::ZERO);
	auto pos = getBone(boneName)->getDisplayRenderNode()->convertToWorldSpaceAR(Vec2::ZERO);
	return pos - rootPos;
}

void YizhiBossAI::NextActionDelay( float t )
{
	playArmat(0);
	auto delay = DelayTime::create(t);
	auto next = CallFunc::create(this, callfunc_selector(YizhiBossAI::NextAction));
	auto seq = Sequence::create(delay, next, nullptr);
	runAction(seq);
}

void YizhiBossAI::onFrameEventOfDanmu1( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		shootDanmu("Eye_1");
	}
	else if(evt == "end")
	{
		//ÏÂÒ»Ö»ÑÛ¾¦
		playArmat(3,this,frameEvent_selector(YizhiBossAI::onFrameEventOfDanmu2),1.0f);
	}
}

void YizhiBossAI::onFrameEventOfDanmu2( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		shootDanmu("Eye_2");
	}
	else if(evt == "end")
	{
		playArmat(4,this,frameEvent_selector(YizhiBossAI::onFrameEventOfDanmu3),1.0f);
	}
}

void YizhiBossAI::onFrameEventOfDanmu3( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		shootDanmu("Eye_3");
	}
	else if(evt == "end")
	{
		playArmat(5,this,frameEvent_selector(YizhiBossAI::onFrameEventOfDanmu4),1.0f);
	}
}

void YizhiBossAI::onFrameEventOfDanmu4( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		shootDanmu("Eye_4");
	}
	else if(evt == "end")
	{
		if (m_ShootMissileTimes >= 0)
		{
			//ÏÂÒ»¸ö¶¯×÷
			NextActionDelay(5.0f);
			return;
		}
		playArmat(2,this,frameEvent_selector(YizhiBossAI::onFrameEventOfDanmu1),1.0f);
		m_ShootMissileTimes++;
	}
}

std::vector<int> YizhiBossAI::initAction()
{
	std::vector<int>	tagVec;

	//Ëæ»ú
	switch (m_AIlvl)
	{
	case 1:
		{
			tagVec.push_back(1);
			tagVec.push_back(2);
			tagVec.push_back(5);
		}
		break;
	case 2:
		{
			tagVec.push_back(6);
			tagVec.push_back(3);
			tagVec.push_back(4);
			tagVec.push_back(5);
		}
		break;

	default:
		break;
	}
	return tagVec;
}

void YizhiBossAI::SelectAction( int ActionId )
{
	switch (ActionId)
	{
	case 1:
		ActionShoot5AutoExpl();
		break;
	case 2:
		ActionShoot4Missile();
		break;
	case 3:
		ActionShoot5SpeedUpBullet();
		break;
	case 4:
		ActionSootAutoExplAndMissile();
		break;
	case 5:
		ActionDanmu();
		break;
	case 6:
		ActionHuiXuan();
		break;
	default:
		break;
	}
}

void DoubleGunBossAI::StartAI( int lvl )
{
	m_MaxTimes = 0;
	m_CurTimes = 0;
	m_DanmuId = "0";

	BossAI::StartAI(lvl);

}

void DoubleGunBossAI::Action1()
{
	m_CurTimes = 0;
	m_MaxTimes = 4;
	m_DanmuId = "10090";

	playArmat(1,this,frameEvent_selector(DoubleGunBossAI::onFrameEventOfReady),1.8f);
}

void DoubleGunBossAI::Action2()
{
	m_CurTimes = 0;
	m_MaxTimes = 10;
	m_DanmuId = "10092";
	MoveLR();
	playArmat(4,this,frameEvent_selector(DoubleGunBossAI::onFrameEventOfReady2),1.8f);
}

void DoubleGunBossAI::Action3()
{
	m_CurTimes = 0;
	m_MaxTimes = 10;
	m_DanmuId = "10093";
	MoveLR();
	playArmat(4,this,frameEvent_selector(DoubleGunBossAI::onFrameEventOfReady2),1.8f);

}

void DoubleGunBossAI::Action4()
{
	playArmat(6,this,frameEvent_selector(DoubleGunBossAI::onFrameEventOfTwoGuns),1.8f);
}

void DoubleGunBossAI::Action5()
{
	m_CurTimes = 0;
	m_MaxTimes = 4;
	m_DanmuId = "10089";

	playArmat(1,this,frameEvent_selector(DoubleGunBossAI::onFrameEventOfReady),1.8f);
}

void DoubleGunBossAI::Action6()
{
	m_CurTimes = 0;
	m_MaxTimes = 4;
	MoveLR();
	playArmat(4,this,frameEvent_selector(DoubleGunBossAI::onFrameEventOfReady2_1),1.8f);
}

void DoubleGunBossAI::Action7()
{

}

void DoubleGunBossAI::Action8()
{

}

void DoubleGunBossAI::addDanmuWithConfig( const std::string& id, const Vec2& pos, bool isAmmi /*= false*/ )
{
	WeaponInfoObjList* list = EnemyPlaneController::getInstance()->getWeaponConfigDataFromId(id);
	Vector<WeaponInfoObj*> wpVec = list->weapon_vector;
	if(list != nullptr) 
	{
		for(auto it = wpVec.begin(); it < wpVec.end(); it++) 
		{
			auto weapon = EditorWeaponFactory::createWeaponWithType(*it,true);
			weapon->setParticalPath("particla/shuangqiang/test1.plist");
			weapon->setBelongTo(UnitId::ePlaneBoss);
			weapon->isAim = isAmmi;
			weapon->setPosition(pos);
			getArmat()->addChild(weapon);
		}
	}
}

void DoubleGunBossAI::onFrameEventOfReady( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		
	}
	else if(evt == "end")
	{
		playArmat(2,this,frameEvent_selector(DoubleGunBossAI::onFrameEventOfShoot),1.8f);
		addDanmuWithConfig(m_DanmuId, Vec2(-8,-120));
	}
}

void DoubleGunBossAI::onFrameEventOfShoot( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		m_CurTimes++;
	}
	else if(evt == "end")
	{
		if (m_CurTimes >= m_MaxTimes)
		{
			m_CurTimes = 0;
			playArmat(3,this,frameEvent_selector(DoubleGunBossAI::onFrameEventOfEnd),1.8f);
			return;
		}
	}
}

void DoubleGunBossAI::onFrameEventOfEnd( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		
	}
	else if(evt == "end")
	{
		playArmat(0);
		NextActionDelay(2.0f);
	}
}

void DoubleGunBossAI::onFrameEventOfReady2( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		
	}
	else if(evt == "end")
	{
		playArmat(5,this,frameEvent_selector(DoubleGunBossAI::onFrameEventOfShoot2),3.0f);
	}
}

void DoubleGunBossAI::onFrameEventOfShoot2( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shootL")
	{
		addDanmuWithConfig(m_DanmuId, Vec2(-3 - 30,-120));
		m_CurTimes++;
	}
	else if (evt == "shootR")
	{
		addDanmuWithConfig(m_DanmuId, Vec2(-3 + 30,-120));
		m_CurTimes++;
	}
	else if(evt == "end")
	{
		if (m_CurTimes >= m_MaxTimes)
		{
			m_CurTimes = 0;
			playArmat(6,this,frameEvent_selector(DoubleGunBossAI::onFrameEventOfEnd2),1.8f);
			return;
		}
		else
		{
			PauseArmat(1.0f);
		}
	}
}

void DoubleGunBossAI::onFrameEventOfEnd2( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		
	}
	else if(evt == "end")
	{
		NextActionDelay(2.0f);
	}
}

void DoubleGunBossAI::onFrameEventOfTwoGuns( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		addDanmuWithConfig("10095", Vec2(-3 - 30,-120));
		addDanmuWithConfig("10094", Vec2(-3 + 30,-120));
	}
	else if(evt == "end")
	{
		NextActionDelay(2.0f);
	}
}

void DoubleGunBossAI::onFrameEventOfReady2_1( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{

	}
	else if(evt == "end")
	{
		addDanmuWithConfig("10089", Vec2(-3 - 30,-120));
		addDanmuWithConfig("10089", Vec2(-3 + 30,-120));
		playArmat(5,this,frameEvent_selector(DoubleGunBossAI::onFrameEventOfShoot2_1),3.0f);
	}
}

void DoubleGunBossAI::onFrameEventOfShoot2_1( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shootL")
	{
		//addDanmuWithConfig(m_DanmuId, Vec2(-3 - 30,-120));
		//m_CurTimes++;
	}
	else if (evt == "shootR")
	{
		//addDanmuWithConfig(m_DanmuId, Vec2(-3 + 30,-120));
		m_CurTimes++;
	}
	else if(evt == "end")
	{
		if (m_CurTimes >= m_MaxTimes)
		{
			m_CurTimes = 0;
			playArmat(6,this,frameEvent_selector(DoubleGunBossAI::onFrameEventOfEnd2_1),1.8f);
			return;
		}
	}
}

void DoubleGunBossAI::onFrameEventOfEnd2_1( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{

	}
	else if(evt == "end")
	{
		NextActionDelay(2.0f);
	}
}

std::vector<int> DoubleGunBossAI::initAction()
{
	std::vector<int>	tagVec;

	//Ëæ»ú
	switch (m_AIlvl)
	{
	case 1:
		{
			tagVec.push_back(1);
			tagVec.push_back(2);
			tagVec.push_back(5);
		}
		break;
	case 2:
		{
			tagVec.push_back(3);
			tagVec.push_back(4);
			tagVec.push_back(5);
		}
		break;

	default:
		break;
	}
	return tagVec;
}

void DoubleGunBossAI::SelectAction( int ActionId )
{
	//ActionId = 6;
	switch (ActionId)
	{
	case 1:
		Action1();
		break;
	case 2:
		Action2();
		break;
	case 3:
		Action3();
		break;
	case 4:
		Action4();
		break;
	case 5:
		Action5();
		break;
	case 6:
		Action6();
		break;
	default:
		break;
	}
}
