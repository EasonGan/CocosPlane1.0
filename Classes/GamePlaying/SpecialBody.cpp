#include "SpecialBody.h"
#include "Layer/UIController.h"
#include "../GameCommon/GameData.h"
#include "../GameCommon/EffectsManager.h"
#include "../GameCommon/GameConfig.h"
#include "ContactManager.h"
#include "../GameUtil/DrawSpriteUtil.h"
#include "../GameUI/GameMainLayer.h"
#include "../GameUtil/StatisticsHelp.h"

Wall* Wall::create(const Size& size,const PhysicsMaterial& pm )
{
	Wall* wall = new (std::nothrow) Wall;
	if (wall && wall->init())
	{
		wall->initBody(size,pm);
		wall->autorelease();
		return wall;
	}
	else
	{
		CC_SAFE_DELETE(wall);
		return nullptr;
	}
	
}

void Wall::initBody(const Size& size,const PhysicsMaterial& pm )
{
	auto body = PhysicsBody::createBox(size,pm,Point::ZERO);
	setPhysicsBody(body);
	body->setGravityEnable(false);
	body->setMass(100000);
	body->setDynamic(false);
	body->setCategoryBitmask(WALL_CATEGORYBITMASK);
	body->setCollisionBitmask(WALL_COLLISIONBITMASK);
	body->setContactTestBitmask(WALL_CONTACTTESTBITMASK);
	setInvincibleAtEdge();
	setInvincible(1);
}



//-----------------------------
ExplisonScreen::ExplisonScreen()
{
	m_duration = 0.6f;
	m_idx = 0;
}

ExplisonScreen* ExplisonScreen::create()
{
	ExplisonScreen* exp = new (std::nothrow) ExplisonScreen;
	if (exp && exp->init())
	{
		exp->autorelease();
		exp->initPro();
		exp->initBody();
		return exp;
	}
	else
	{
		CC_SAFE_DELETE(exp);
		return nullptr;
	}
}

void ExplisonScreen::initBody()
{
	auto body = PhysicsBody::createBox(m_WinSize);
	body->setGravityEnable(false);
	body->setMass(100000);
	body->setCategoryBitmask(SELF_SCOPE_CATEGORYBITMASK);
	body->setCollisionBitmask(0);
	body->setContactTestBitmask(SELF_SCOPE_CONTACTTESTBITMASK);
	setPhysicsBody(body);
}

void ExplisonScreen::initPro()
{
	setInvincible(true);
	m_BasePro.setHurts(-Ene_Plane_Hp_max- 10);
	setUnitId(UnitId::eScreenExplosion);
}

void ExplisonScreen::onEnter()
{
	PhysicsUnit::onEnter();

	//�ȱ�Ȧ����ȫ����ը
	auto pl = GameData::getInstance()->getMainPlane();
	if (!pl)
	{
		DeathForce();
		return;
	}
	auto pos = pl->convertToWorldSpaceAR(Vec2::ZERO);

	NewAudio::getInstance()->playEffect(Music_SpeedOver);

	auto layer = UIController::getInstance()->getEffectsLayer();
	EffectsManager::addParticle("particla/SpeedUpOver/SpeedUpOver.plist",layer,pos);

	ActionInterval* delay = DelayTime::create(0.5f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(ExplisonScreen::StartExp));
	ActionInterval* seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void ExplisonScreen::setBody( float dt )
{
	initBody();
}

void ExplisonScreen::removeExplosion()
{
	ActionInterval* delay = DelayTime::create(m_duration);
	CallFunc* func = CallFunc::create(this,callfunc_selector(ExplisonScreen::removeMe));
	ActionInterval* seq = Sequence::create(delay,func, nullptr);
	runAction(seq);
}

void ExplisonScreen::StartExp()
{
	schedule(schedule_selector(ExplisonScreen::setBody),0.05);
	removeExplosion();
	auto layer = UIController::getInstance()->getEffectsLayer();
	EffectsManager::addParticle("particla/FullScreenExpl/FullScreenExpl.plist",layer,
		Vec2(m_WinSize.width*0.5,m_WinSize.height*0.5),true,m_duration);

	StartExpMusic();
	AddParticle();
}

void ExplisonScreen::AddParticle()
{
	auto layer = UIController::getInstance()->getEffectsLayer();

	Vec2 pos = Vec2(m_WinSize.width * rand_0_1(), m_WinSize.height * rand_0_1());

	EffectsManager::addParticle("particla/FullScreenExpl/Exp1.plist",layer,pos);
	//EffectsManager::addParticle("particla/FullScreenExpl/Exp2.plist",layer,pos);
	//EffectsManager::addParticle("particla/FullScreenExpl/Exp3.plist",layer,pos);

	ActionInterval* delay = DelayTime::create(0.005);
	CallFunc* func = CallFunc::create(this,callfunc_selector(ExplisonScreen::AddParticle));
	ActionInterval* seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void ExplisonScreen::StartExpMusic()
{
	ActionInterval* delay = DelayTime::create(0.05 + rand_0_1() * 0.05);
	CallFunc* func = CallFunc::create(this,callfunc_selector(ExplisonScreen::PlayExpMusic));
	ActionInterval* seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void ExplisonScreen::PlayExpMusic()
{
	NewAudio::getInstance()->playEffect(Music_Boss1Shoot1);
	StartExpMusic();
}

void ExplisonScreen::Zhenlie()
{
	auto layer = UIController::getInstance()->getEffectsLayer();

	for (int i = 1; i <= 9; i++)
	{
		Vec2 pos = Vec2(m_WinSize.width * 0.1 * i, m_WinSize.height*0.1 * m_idx);

		auto partic = EffectsManager::addParticle("particla/FullScreenExpl/Exp1.plist",layer,pos);
		if (partic)
		{
			partic->setScale(0.5f);
		}
		//EffectsManager::addParticle("particla/FullScreenExpl/Exp2.plist",layer,pos);
		//EffectsManager::addParticle("particla/FullScreenExpl/Exp3.plist",layer,pos);
	}
	m_idx++;
	if (m_idx > 10)
	{
		StartExp();
		return;
	}

	ActionInterval* delay = DelayTime::create(0.05f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(ExplisonScreen::Zhenlie));
	ActionInterval* seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void ExplisonScreen::removeMe()
{
	auto mainPlane = GameData::getInstance()->getMainPlane();
	if (mainPlane)
	{
		mainPlane->setInvincible(false);
	}
	UIController::getInstance()->getMainUILayer()->startDaZhaoSD();
	DeathForce();
}


//---------------------
PlaneProtect* PlaneProtect::create()
{
	PlaneProtect* pp = new (std::nothrow) PlaneProtect;
	if (pp && pp->initWithFile("weapon.png"))
	{
		pp->autorelease();
		return pp;
	}
	CC_SAFE_DELETE(pp);
	return nullptr;
}

void PlaneProtect::ContactEff( const Vec2& pos )
{
	ContactEff(this,130.0f, pos);
}

void PlaneProtect::Broken()
{
	NotificationCenter::getInstance()->postNotification(Msg_ProtectRemove, nullptr);
	UIController::getInstance()->getMainUILayer()->consumeHudun();
	ClearScreen();
}

void PlaneProtect::ContactEff( Node* node, float radius, const Vec2& pos, float s, const Vec2& offset )
{
	Vec2 randPos = Vec2(pos.x + rand_minus1_1() * 80, pos.y + rand_minus1_1() * 80);

	Vec2 mypos = node->convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 newVec = randPos - mypos;
	float angle = newVec.getAngle() * Pi2Angle;

	newVec.normalize();
	newVec *= radius;

	auto size = node->getContentSize();

	auto endPos =Vec2( size.width*0.5 + newVec.x, size.height*0.5 + newVec.y);
	endPos += offset;

	auto sp = Sprite::create("Effect/contactEff/contactEff.png");
	sp->setTag(20150922);
	sp->setPosition(endPos);
	sp->setRotation(90 - angle);
	node->addChild(sp);

	auto parti1 = EffectsManager::addParticle("particla/SpeedUpContact/SpeedUpContact.plist",node,endPos);
	if (parti1)
	{
		parti1->setScale(s);
	}

	ActionInterval* scale = ScaleTo::create(0.1f,(7.0 + rand_0_1()) * s,(2.0f + rand_0_1())*s);
	CallFuncN* func = CallFuncN::create(nullptr,callfuncN_selector(PlaneProtect::removeEff));
	ActionInterval* seq = Sequence::create(scale, func, nullptr);
	sp->runAction(seq);
}

void PlaneProtect::removeEff(Node* node)
{
	if (node)
	{
		node->removeFromParentAndCleanup(true);
	}
}

void PlaneProtect::onEnter()
{
	Sprite::onEnter();

	this->protectFrames = GameResource::create(GameResource::TYPE_FRAME, "ani_protect", 12);
    DrawSpriteUtil::setSpriteWithGameResource(this, this->protectFrames);
    this->runAction(Sequence::create(ScaleTo::create(0.1,2.4),ScaleTo::create(0.1,2), nullptr));
}

void PlaneProtect::onExit()
{
	this->protectFrames->release();
    Sprite::onExit();
}

void PlaneProtect::ClearScreen()
{
	auto mainpl = GameData::getInstance()->getMainPlane();
	if (mainpl)
	{
		auto pos = mainpl->convertToWorldSpaceAR(Vec2::ZERO);
		auto layer = UIController::getInstance()->getEffectsLayer();
		EffectsManager::addParticle("particla/SpeedUpOver/SpeedUpOver.plist",layer,
			pos,true,1.0f);
	}
}

void PlaneProtect::removeMe()
{
	StatisticsHelp::use("hudun",1,1);
	Broken();
	removeFromParentAndCleanup(true);
}



//------------------------------------
SpeedUpProtect* SpeedUpProtect::create()
{
	SpeedUpProtect* sup = new (std::nothrow) SpeedUpProtect;
	if (sup && sup->initWithFile("particla/addSpeed/Protect1.png")) 
	{
		sup->initBody();
		sup->autorelease();
		return sup;
	}
	else
	{
		CC_SAFE_DELETE(sup);
		return nullptr;
	}
}

void SpeedUpProtect::initBody()
{
	auto s = getContentSize();
	Vec2 vec[7] = 
	{
		Vec2(0,0),
		Vec2(s.width * 0.1, s.height * 0.4),
		Vec2(s.width * 0.26, s.height * 0.72),
		Vec2(s.width * 0.5, s.height),
		Vec2(s.width * 0.74, s.height * 0.72),
		Vec2(s.width * 0.9, s.height * 0.4),
		Vec2(s.width,0),
	};

	PhysicsBody* body = PhysicsBody::createPolygon(vec, 7, PHYSICSBODY_MATERIAL_DEFAULT, Vec2(-s.width*0.5,-s.height*0.5));
	body->setGravityEnable(false);
	body->setRotationEnable(false);
	body->setDynamic(false);
	body->setMass(10000000);
	body->setCategoryBitmask(SELF_SCOPE_CATEGORYBITMASK);
	body->setCollisionBitmask(SELF_SCOPE_COLLISIONBITMASK);
	body->setContactTestBitmask(SELF_SCOPE_CONTACTTESTBITMASK);
	setPhysicsBody(body);
	setInvincible(true);
	setInvincibleAtEdge(1);
	m_BasePro.setHurts(-Ene_Plane_Hp_max - 10);
}

void SpeedUpProtect::onEnter()
{
	PhysicsUnit::onEnter();
	addProtect();
	addParticle();

	setScale(0.2);
	ActionInterval* scaleBack = ScaleTo::create(1.0,1.1,1.0);
	runAction(scaleBack);

	ActionInterval* scale = ScaleTo::create(0.3,0.85,0.9);
	ActionInterval* back = ScaleTo::create(0.3,1.1,1.0);
	ActionInterval* seq = Sequence::create(scale, back, nullptr);
	ActionInterval* rep = RepeatForever::create(seq);
	runAction(rep);
}

void SpeedUpProtect::onExit()
{
	PhysicsUnit::onExit();

}

void SpeedUpProtect::addProtect()
{
	auto s = getContentSize();

	Sprite* protect2 = Sprite::create("particla/addSpeed/Protect2.png");
	Sprite* protect3 = Sprite::create("particla/addSpeed/Protect3.png");

	protect2->setPosition(s.width * 0.5, s.height * 0.63 -10);
	protect3->setPosition(s.width * 0.5, s.height * 0.1);

	addChild(protect2);
	addChild(protect3);

	{
		ActionInterval* scale = ScaleTo::create(0.3,0.8,1.0);
		ActionInterval* back = ScaleTo::create(0.3,1.1,1.0);
		ActionInterval* seq = Sequence::create(scale, back, nullptr);
		ActionInterval* rep = RepeatForever::create(seq);
		protect2->runAction(rep);
	}
	{
		ActionInterval* scale = ScaleTo::create(0.15,1.2,1.0);
		ActionInterval* back = ScaleTo::create(0.15,0.8,0.8);
		ActionInterval* seq = Sequence::create(scale, back, nullptr);
		ActionInterval* rep = RepeatForever::create(seq);
		protect3->runAction(rep);
	}
	
}

void SpeedUpProtect::addTail(Node* parent)
{
	auto s = parent->getContentSize();
	auto layer = UIController::getInstance()->getEffectsLayer();
	{
		Sprite* sp = Sprite::create("weapon.png");
		sp->setPosition(-20, 30);
		parent->addChild(sp);
		sp->setScale(1.0);

		ActionInterval* animate1 = layer->getFramesAnimation("ProtectTailFirst_", 16, 1.0/15, 0, 1);
		ActionInterval* delay = DelayTime::create(1.5f);
		ActionInterval* seq = Sequence::create(animate1, delay, nullptr);
		ActionInterval* rep = RepeatForever::create(seq);
		sp->runAction(rep);
	}
	{
		Sprite* sp = Sprite::create("weapon.png");
		sp->setPosition(0, 0);
		parent->addChild(sp);
		ActionInterval* animate2 = layer->getFramesAnimation("ProtectTailSec_", 8, 0.1, 0, REPEAT_FORVER);
		sp->runAction(animate2);
	}
}

void SpeedUpProtect::addParticle()
{
	auto s = getContentSize();
	EffectsManager::addParticle("particla/addSpeed/addSpeed_1.plist", this, Vec2(s.width*0.5, s.height*0.5), false);
	EffectsManager::addParticle("particla/addSpeed/addSpeed_2.plist", this, Vec2(s.width*0.5, s.height*0.5), false);
}

//----------------------------
ScreenEdge* ScreenEdge::create(const Size& s)
{
	ScreenEdge* edge = new (std::nothrow) ScreenEdge;
	if (edge && edge->init())
	{
		edge->m_BodySize = s;
		edge->initBody();
		return edge;
	}
	CC_SAFE_DELETE(edge);
	return nullptr;
}

void ScreenEdge::initBody()
{
	auto body = PhysicsBody::createEdgeBox(m_BodySize,PHYSICSBODY_MATERIAL_DEFAULT, 20);
	body->setCategoryBitmask(ScreenEdge_CATEGORYBITMASK);
	body->setCollisionBitmask(ScreenEdge_COLLISIONBITMASK_P);
	body->setContactTestBitmask(ScreenEdge_CONTACTTESTBITMASK);
	body->setRotationEnable(false);
	body->setGravityEnable(false);
	setPhysicsBody(body);

	setInvincible(1);
	getBasePro().setIsIgnoreScreenEdge(1);
}

void ScreenEdge::OtherDealWhenContact( const BasePro& rs )
{
	
}
