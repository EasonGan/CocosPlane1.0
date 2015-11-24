#include "Bullet.h"
#include "../Layer/UIController.h"
#include "../Layer/PhysicsLayer.h"
#include "../../GameCommon/GameData.h"
#include "../../GameCommon/EffectsManager.h"
#include "../../GameCommon/GameConfig.h"
#include "../Layer/MenuLayer.h"
#include "../Weapon/WeaponFactory.h"
#include "../Weapon/weapon.h"
#include "../../GameCommon/GameDefine.h"
#include "BulletFactory.h"
#include "../ContactManager.h"
#include "../../GameUI/GameSceneMgr.h"


Bullet* Bullet::create( const std::string& frameName )
{
	Bullet* bullet = new (std::nothrow) Bullet;
	if (bullet && bullet->initWithSpriteFrameName(frameName))
	{
		bullet->autorelease();
		bullet->initBullet();
		bullet->initBody();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

Bullet* Bullet::createWithPicture( const std::string& file )
{
	Bullet* bullet = new (std::nothrow) Bullet;
	if (bullet && bullet->initWithFile(file))
	{
		bullet->initBullet();
		bullet->autorelease();
		bullet->initBody();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void Bullet::FixAngleWithWeapon( Weapon* weapon )
{
	if (!weapon)
	{
		return;
	}
	m_weapon = weapon;

	//閿熺煫纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹杞敓瑙掑害锝忔嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸帴纰夋嫹閿熸枻鎷疯浆閿熸枻鎷峰簲閿熶茎瑙掕�?
	Node* plane = weapon->getParent();
	float planeRotate = plane->getRotation();
	float wpRotate = weapon->getRotation();
	float sumRotate = planeRotate + wpRotate;
	_physicsBody->setRotation(sumRotate);
	
	Vec2 vec = m_weapon->getBltOriginVelocity().forAngle( (90 -sumRotate) * Angle2pi );
	vec.normalize();
	_physicsBody->setVelocity(vec * m_weapon->getBltSpeed());
}

void Bullet::initBody()
{
	auto s = this->getContentSize();
	auto body = PhysicsBody::createBox(Size(s.width*0.2, s.height*0.6));
	body->setGravityEnable(false);
	setPhysicsBody(body);
}

void Bullet::setBodyPro()
{
	if (!m_WeaponDelegate)
	{
		return;
	}
	//body->setVelocity(m_WeaponDelegate->getBltSpeed() * m_WeaponDelegate->getBltOriginVelocity());
	_physicsBody->setLinearDamping(m_WeaponDelegate->getBltLinearDamping());
	_physicsBody->setGravityEnable(m_WeaponDelegate->getBltGravityAble());
	_physicsBody->setMass(m_WeaponDelegate->getBltMass());
	if (m_WeaponDelegate->getBltCategorybitmask() >= 0)
	{
		_physicsBody->setCategoryBitmask(m_WeaponDelegate->getBltCategorybitmask());
	}
	if (m_WeaponDelegate->getBltCollisionbitmask() >= 0)
	{
		_physicsBody->setCollisionBitmask(m_WeaponDelegate->getBltCollisionbitmask());
	}
	if (m_WeaponDelegate->getBltContacttestbitmask() >= 0)
	{
		_physicsBody->setContactTestBitmask(m_WeaponDelegate->getBltContacttestbitmask());
	}
	
}

void Bullet::setBitmask(const int& cate,const int& collision,const int& test )
{
	_physicsBody->setCategoryBitmask(cate);
	_physicsBody->setCollisionBitmask(collision);
	_physicsBody->setContactTestBitmask(test);
}

void Bullet::onEnter()
{
	PhysicsUnit::onEnter();
	ListeningBossDead();
	ListeningProtectRemove();

	setBodyPro();
}

void Bullet::resetHurt( int hurts )
{
	m_BasePro.setHurts(hurts);
}

void Bullet::ListeningStopWeapon()
{
	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(Bullet::StopBullet),Msg_StopMainPlBullet, nullptr);
}

void Bullet::ListeningBossDead()
{
	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(Bullet::BossDead),Msg_BossDead, nullptr);
}

void Bullet::ListeningProtectRemove()
{
	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(Bullet::ProtectRemove),Msg_ProtectRemove, nullptr);
}

void Bullet::BossDead( Ref* obj )
{
	bool b = false;
	if (m_BasePro.getBelongTo() == UnitId::ePlaneBoss )
	{
		b = true;
	}
	else if (m_BasePro.getBelongTo() >= UnitId::eRedCard && m_BasePro.getBelongTo() <= UnitId::eSuperCard )
	{
		b = true;
	}
	if (b)
	{
		DeathForce();
	}
}

void Bullet::StopBullet( Ref* obj )
{
	DeathForce();
}

void Bullet::ProtectRemove( Ref* obj )
{
	NotificationCenter::getInstance()->removeObserver(this, Msg_ProtectRemove);
	if (m_BasePro.getBelongTo() <= UnitId::ePlaneBoss)
	{
		//_physicsBody->setDynamic(false);
		stopAllActions();
		resetHurt(0);
		setOpacity(0);
		auto fadeOut = FadeOut::create(0.02f);
		auto fadeIn = FadeIn::create(0.02f);
		auto seq = Sequence::create(fadeIn,fadeOut,nullptr);
		auto rep = Repeat::create(seq, 5);
		auto func = CallFunc::create(this, callfunc_selector(PhysicsUnit::DeathForce));
		auto seq1 = Sequence::create(rep, func, nullptr);
		runAction(seq1);
	}

    if(this->couldChange) {
        GameData::getInstance()->m_scoreFactory->createDropItem(this->getPosition(), 5, DropMod::Drop_Mod, true);
    }
}

void Bullet::setWeaponDelegate( WeaponDelegate* wpDlgt )
{
	if (Scene_Type_RoleInterface == GameSceneMgr::getInstance()->getCurScen())
	{
		return;
	}

	m_WeaponDelegate = wpDlgt;

	resetHurt(wpDlgt->getBltHurts());
	setScaleX(wpDlgt->getBltScaleX());
	setScaleY(wpDlgt->getBltScaleY());
	if (wpDlgt->getBltBelongTo() != UnitId::DefaultIdx)
	{
		setBelongTo(wpDlgt->getBltBelongTo());
	}
}

void Bullet::setBltFlySpeed( int speed )
{
	float r = _physicsBody->getRotation();
	m_FlySpeed = speed;
	Vec2 v = m_OriginVelocity;
	v.normalize();
	v.rotate(Vec2::ZERO, -r * DEG_RAD);
	_physicsBody->setVelocity(v * m_FlySpeed);
}

int Bullet::getBltFlySpeed()
{
	return m_FlySpeed;
}

void Bullet::ImpactOfHp( const BasePro& pro )
{
	if (isScreenEdge(pro))
	{
		m_CurChance++;
		if (m_CurChance >= m_MaxChance)
		{
			if (!m_BasePro.getIsIgnoreScreenEdge())
			{
				setDeadType(DeadType::Dead_Nature);
				m_isDead = true;
				return;
			}
		}
	}
	else
	{
		if (m_BasePro.getIsInvincible())
		{
			return;
		}
		
		//鐎涙劕鑴婇崣顖滄箙閸嬫碍鐥呴張澶庮攨闁插骏绱濈悮顐ゎ潾韫囧懏�?
		//FuncHpEmpty(pro);
		//DeathImpactOfDropReward();
		//DeathImpactOfDrops();
		//PlayMusicWhenDead();			//
		//AnimationWhenDead();			//
		//setDeadType(DeadType::Dead_Kill);
		m_isDead = true;
	}
}

void Bullet::initBullet()
{
	m_weapon = nullptr;
	m_WeaponDelegate = nullptr;
	m_FlySpeed = 500;
	m_OriginVelocity = Vec2(0,1);
	couldChange = false;
	//m_MaxChance = 1;
}




//----------------------------------
//閿熸枻鎷烽敓鏂ゆ�?
MissileForDis* MissileForDis::create( const std::string& frameName )
{
	MissileForDis* bullet = new (std::nothrow) MissileForDis;
	if (bullet && bullet->initWithSpriteFrameName(frameName))
	{
		bullet->initBullet();
		bullet->autorelease();
		bullet->initBody();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

MissileForDis* MissileForDis::create()
{
	MissileForDis* bullet = new (std::nothrow) MissileForDis;
	if (bullet && bullet->init())
	{
		bullet->initBullet();
		bullet->autorelease();
		bullet->initBody();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void MissileForDis::initBody()
{	
	auto body = PhysicsBody::createCircle(10);
	body->setGravityEnable(false);
	body->setRotationEnable(false);
	setPhysicsBody(body);
	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
	setInvincible(true);
	setInvincibleAtEdge();
	_physicsBody->setVelocity(Vec2(0,100));
	schedule(schedule_selector(MissileForDis::setBody), 0.1);
}

void MissileForDis::onEnter()
{
	Bullet::onEnter();
	NewAudio::getInstance()->playEffect(Music_IceMissile);

	m_targetPos = Vec2(360, 1500);

	addParticle();

	schedule(schedule_selector(MissileForDis::FlyToTarget));
}

void MissileForDis::FlyToTarget( float dt )
{
	{
	Vec2 curVec = _physicsBody->getVelocity();
	Vec2 myPos = this->getPhysicsBody()->getPosition();
	{

		if (m_particle1)
		{
			Vec2 partcPos = m_particle1->convertToWorldSpaceAR(Vec2::ZERO);
			Vec2 lerpPos = partcPos.lerp(myPos,0.3);
			m_particle1->setPosition(lerpPos);
			if (m_particle2)
			{
				m_particle2->setPosition(lerpPos);
			}
		}
	}

	auto enemyPlaneList = GameData::getInstance()->getEnemyPlaneVec();
	if (!enemyPlaneList.empty())
	{
		m_targetPos = EnemyPlaneController::getInstance()->getTheClosePlane(myPos)->getPhysicsBody()->getPosition();
	}

	Vec2 newVec = m_targetPos - myPos;

	float distance = myPos.distance(m_targetPos);
	curVec.smooth(newVec,100,1 + distance); 
	curVec.normalize();
	_physicsBody->setVelocity(curVec * m_FlySpeed);

	//float bulletR = curVec.getAngle()*Pi2Angle;
		//this->setRotation(90-bulletR);		//��Ϊ�������ӣ����Բ�����ת

	if (m_FlySpeed < 300)
	{
		m_FlySpeed += 5;
	}
	}
}


void MissileForDis::addParticle()
{
	auto layer = UIController::getInstance()->getPhysicsLayer();
	auto mypos = convertToWorldSpaceAR(Vec2::ZERO);

	m_particle1 = EffectsManager::addParticle("particla/IceMissile/IceMissile_1.plist",layer,mypos,false);
	m_particle2 = EffectsManager::addParticle("particla/IceMissile/IceMissile_2.plist",layer,mypos,false);
}

void MissileForDis::Explosion()
{
	//閿熸枻鎷锋挒閿熸枻鎷风偢
	auto layer = UIController::getInstance()->getPhysicsLayer();
	auto pos = convertToWorldSpaceAR(Vec2::ZERO);
	EffectsManager::addParticle("particla/IceMissleExpl/IceMissleExpl.plist",layer,pos);

	auto sp = Sprite::create("particla/IceMissleExpl/IceMissleExpl.png");
	sp->setPosition(pos);
	sp->setScale(0.1);
	layer->addChild(sp,-1);

	ActionInterval* scale = ScaleTo::create(0.3f, 2.0f);
	ActionInterval* fade = FadeOut::create(0.3f);
	CallFuncN* rm = CallFuncN::create(this,callfuncN_selector(MissileForDis::removeExpSp));
	ActionInterval* seq = Sequence::create(scale, fade, rm, nullptr);
	sp->runAction(seq);

	ActionInterval* rotate = RotateBy::create(0.3f, 1000);
	ActionInterval* rep = RepeatForever::create(rotate);
	sp->runAction(rep);

}

void MissileForDis::removeExpSp( Node* node )
{
	node->removeFromParentAndCleanup(true);
}

void MissileForDis::Dying()
{
	unschedule(schedule_selector(MissileForDis::FlyToTarget));
	getPhysicsBody()->setVelocity(Vec2(0.01,0.01));

	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_COLLISIONBITMASK_P);
	m_BasePro.setHurts(0);
	
	ActionInterval* delay = DelayTime::create(0.25f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(MissileForDis::DeathForce));
	ActionInterval* seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void MissileForDis::DoSomethingBeforeDead()
{
	if (m_particle1)
	{
		m_particle1->removeFromParentAndCleanup(true);
		m_particle1 = nullptr;
	}
	if (m_particle2)
	{
		m_particle2->removeFromParentAndCleanup(true);
		m_particle2 = nullptr;
	}
}

void MissileForDis::OtherDealWhenContact( const BasePro& rs )
{
	Dying();
	if (!isScreenEdge(rs))
	{
		Explosion();
	}
}

void MissileForDis::setBody( float t )
{
	auto body = PhysicsBody::createCircle(10);
	body->setGravityEnable(false);
	body->setRotationEnable(false);
	setPhysicsBody(body);
	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
	setInvincible(true);
}




//-----------------------------------------
MissileForBloodlimit::MissileForBloodlimit()
{
	
}

MissileForBloodlimit* MissileForBloodlimit::create( const std::string& frameName,const float& AngleOffet )
{
	MissileForBloodlimit* bullet = new (std::nothrow) MissileForBloodlimit;
	if (bullet && bullet->initWithSpriteFrameName(frameName))
	{
		bullet->initBullet();
		bullet->autorelease();
		bullet->m_AngleOffset = AngleOffet;
		bullet->initBody();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void MissileForBloodlimit::onEnter()
{
	Bullet::onEnter();
	addParticle();

	//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰祵閿?
	FixDirect();

	ActionInterval* delay = DelayTime::create(1.0);
	CallFunc* func = CallFunc::create(this,callfunc_selector(MissileForBloodlimit::Explosion));
	ActionInterval* seq = Sequence::create(delay,func,nullptr);
	runAction(seq);

	schedule(schedule_selector(MissileForBloodlimit::FlyToTarget));
}

void MissileForBloodlimit::FlyToTarget( float dt )
{
	Vec2 curVec = _physicsBody->getVelocity();
	auto target = GameData::getInstance()->getEnemyPlaneByMaxHp();

	if (target)
	{
		Vec2 bulletPos = getPosition();

		Vec2 targetPos = target->convertToWorldSpaceAR(Point::ZERO);

		//log("--target pos = %f, %f",targetPos.x,targetPos.y);
		Vec2 newVec = targetPos - bulletPos;

		//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熶茎澶规枻�?
		float jiajiao = newVec.getAngle(curVec)*Pi2Angle;
		float distance = bulletPos.distance(targetPos);
		curVec.smooth(newVec,100,1 + distance); 
	}

	curVec.normalize();
	float bulletR = curVec.getAngle()*Pi2Angle;
	this->getPhysicsBody()->setRotation(90-bulletR);
	_physicsBody->setVelocity(curVec * m_FlySpeed);
}

void MissileForBloodlimit::FixDirect()
{
	//鍙栭敓鏂ゆ嫹鐩敓鏂ゆ嫹
	//auto target = GameData::getInstance()->getEnemyPlaneByMaxHp();

	//Vec2 curVec = m_weapon->getBltOriginVelocity();

	//if (target)
	//{
	//	//閿熸枻鎷烽敓鏂ゆ�?
	//	BasePro rs = target->getBasePro();
	//	rs.isTarget = true;
	//	target->setContactPro(rs);

	//	Vec2 bulletPos = getPosition();
	//	Vec2 targetPos = target->getPosition();
	//	curVec = targetPos - bulletPos;
	//}

	////閿熸枻鎷峰噯閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓?
	//float bulletR = curVec.getAngle()*Pi2Angle;
	//float sumAngle = 90 - bulletR + m_AngleOffset;
	//this->setRotation(sumAngle);
	//curVec = curVec.forAngle((90 + sumAngle) * Angle2pi);
	//curVec.normalize();
	//_physicsBody->setVelocity(curVec * m_FlySpeed);
}

void MissileForBloodlimit::OtherDealWhenContact( const BasePro& rs )
{
	//閿熸枻鎷烽敓鏂ゆ嫹閿熼樁璇ф嫹閿熸枻鎷锋綔閿熸枻鎷烽敓鏂ゆ嫹鐩敓鏂ゆ嫹,閿熸枻鎷蜂箞閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
	//log("-- isTarget = %d ",rs.isTarget);
	/*if (rs.isTarget)
	{
		Explosion();
	}*/
}

void MissileForBloodlimit::addParticle()
{
	
}

void MissileForBloodlimit::Explosion()
{
	UIController::getInstance()->getEffectsLayer()->displayScopeAmmo(getPosition());

	DeathForce();
}






//----------------------------------
FallingBullet* FallingBullet::create( const std::string& frameName )
{
	FallingBullet* bullet = new (std::nothrow) FallingBullet;
	if (bullet && bullet->initWithSpriteFrameName(frameName))
	{
		bullet->initBullet();
		bullet->autorelease();
		bullet->initBody();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void FallingBullet::onEnter()
{
	Bullet::onEnter();

	setBitmask(
		ENEMY_AMMO_CATEGORYBITMASK,
		ENEMY_AMMO_COLLISIONBITMASK,
		ENEMY_AMMO_CONTACTTESTBITMASK);
	_physicsBody->setLinearDamping(0.8);
	_physicsBody->setGravityEnable(false);
	_physicsBody->setVelocity(Vec2(0,0));
	_physicsBody->setMass(0.008);

	_physicsBody->applyForce(Vec2(rand_minus1_1()*0.3,-rand_0_1()),Vec2(0,0));

	schedule(schedule_selector(FallingBullet::newFalling),1.0);
}

void FallingBullet::newFalling(float dt)
{
	_physicsBody->applyForce(Vec2(rand_minus1_1()*0.3,-rand_0_1()*0.1),Vec2(0,0));
}

//------------------------------
CurveBullet* CurveBullet::create( const std::string& frameName)
{
	CurveBullet* bullet = new (std::nothrow) CurveBullet;
	if (bullet && bullet->initWithSpriteFrameName(frameName))
	{
		bullet->initBullet();
		bullet->autorelease();
		bullet->initBody();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void CurveBullet::onEnter()
{
	Bullet::onEnter();
	float wpRotate = m_weapon->getRotation() - 180;
	setBitmask(
		ENEMY_AMMO_CATEGORYBITMASK,
		ENEMY_AMMO_COLLISIONBITMASK,
		ENEMY_AMMO_CONTACTTESTBITMASK);
	_physicsBody->setMass(0.1);
	//schedule(schedule_selector(CurveBullet::newCurve));

	Point myPos = this->convertToWorldSpaceAR(Point::ZERO);
	
	float offset = rand_0_1() * 300;
	//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷风�?
	ccBezierConfig bzconfig;
	if (wpRotate < 0)	// 閿熸枻鎷?
	{
		bzconfig.endPosition = Vec2(m_WinSize.width*0.5 - 100,-120);
		bzconfig.controlPoint_1 = Vec2(m_WinSize.width - 100 + offset,myPos.y*0.8);
		bzconfig.controlPoint_2 = Vec2(m_WinSize.width - 100 - offset,myPos.y*0.33);
	}
	else    // 閿熸枻鎷?
	{
		bzconfig.endPosition = Vec2(m_WinSize.width*0.5 + 100,-120);
		bzconfig.controlPoint_1 = Vec2(100 - offset,myPos.y*0.8);
		bzconfig.controlPoint_2 = Vec2(100 + offset,myPos.y*0.33);
	}
	
	ActionInterval* bez = BezierTo::create(1.0f,bzconfig);
	runAction(bez);
}

void CurveBullet::newCurve( float dt )
{
	//閿熺煫纰夋嫹閿熸枻鎷峰墠閿熸枻鎷烽敓鏂ゆ嫹閿熷姭搴︼綇鎷烽敓鏂ゆ嫹寮忛敓鏂ゆ�?
	Vec2 curV = _physicsBody->getVelocity();
	//log("curv = %f, %f",curV.x,curV.y);
	curV.normalize();

	//閿熸枻鎷烽敓鎺ョ鎷烽敓鏂ゆ嫹杞敓鏂ゆ嫹浣块敓鎴掓湞閿熸枻鎷烽敓鍓胯鎷烽敓鏂ゆ嫹閿熸枻鎷?
	float r = curV.getAngle(Vec2(1,0)) * Pi2Angle - 90;
	this->getPhysicsBody()->setRotation(r);

	//閿熸彮纰夋嫹閿熷姭璁规嫹閿熸枻鎷烽敓鏂ゆ嫹閿熶茎鍑ゆ嫹閿熺锝忔嫹閿熸枻鎷烽敓鑴氳鍑ゆ嫹閿熸枻鎷烽敓鎻紮鎷烽敓鏂ゆ嫹�?閿熸枻鎷烽敓鏂ゆ嫹涓€閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋�?
	/*Vec2 faxian = curV.getPerp();
	faxian.normalize();
	_physicsBody->applyImpulse(faxian*5,Vec2(0,0));*/

}

void CurveBullet::initBullet()
{
	Bullet::initBullet();
	m_BasePro.setHurts(-1000);
}


//-------------------------- 閿熸枻鎷烽敓鏂ゆ嫹閿熸帴纰夋嫹閿熸枻鎷烽敓鏂ゆ嫹
CurveBullet2* CurveBullet2::create( const std::string& frameName,const BullDirection& dir )
{
	CurveBullet2* bullet = new (std::nothrow) CurveBullet2;
	if (bullet && bullet->initWithSpriteFrameName(frameName))
	{
		bullet->initBullet();
		bullet->autorelease();
		bullet->initBody();
		bullet->_dir = dir;
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void CurveBullet2::onEnter()
{
	Bullet::onEnter();

	setBitmask(
		ENEMY_AMMO_CATEGORYBITMASK,
		ENEMY_AMMO_COLLISIONBITMASK,
		ENEMY_AMMO_CONTACTTESTBITMASK);
	_physicsBody->setMass(0.1);
	//schedule(schedule_selector(CurveBullet::newCurve));
	Point myPos = this->convertToWorldSpaceAR(Point::ZERO);

	//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷风�?
	ccBezierConfig bzconfig;
	if ( Dirct_Right == _dir )	// 閿熸枻鎷?
	{
		bzconfig.endPosition = Vec2( 0,200);
		bzconfig.controlPoint_1 = Vec2(m_WinSize.width + 100,myPos.y*0.5);
		bzconfig.controlPoint_2 = Vec2(m_WinSize.width - 200 ,myPos.y*0.5);
	}
	else    // 閿熸枻鎷?
	{
		bzconfig.endPosition = Vec2(m_WinSize.width,200);
		bzconfig.controlPoint_1 = Vec2(-100,myPos.y*0.5);
		bzconfig.controlPoint_2 = Vec2(200,myPos.y*0.5);
	}

	ActionInterval* bez = BezierTo::create(1.0f,bzconfig);
	runAction(bez);
}

void CurveBullet2::newCurve( float dt )
{
	//閿熺煫纰夋嫹閿熸枻鎷峰墠閿熸枻鎷烽敓鏂ゆ嫹閿熷姭搴︼綇鎷烽敓鏂ゆ嫹寮忛敓鏂ゆ�?
	Vec2 curV = _physicsBody->getVelocity();
	//log("curv = %f, %f",curV.x,curV.y);
	curV.normalize();

	//閿熸枻鎷烽敓鎺ョ鎷烽敓鏂ゆ嫹杞敓鏂ゆ嫹浣块敓鎴掓湞閿熸枻鎷烽敓鍓胯鎷烽敓鏂ゆ嫹閿熸枻鎷?
	float r = curV.getAngle(Vec2(1,0)) * Pi2Angle - 90;
	this->getPhysicsBody()->setRotation(r);
}

void CurveBullet2::initBullet()
{
	Bullet::initBullet();
	m_BasePro.setHurts(-1000);
}



//-------------------------- 閿熸枻鎷烽敓鏂ゆ嫹閿熸帴纰夋�?
ReboundBullect* ReboundBullect::create( const std::string& frameName )
{
	ReboundBullect* bullet = new (std::nothrow) ReboundBullect;
	if (bullet && bullet->initWithSpriteFrameName(frameName))
	{
		bullet->initBullet();
		bullet->autorelease();
		bullet->initBody();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void ReboundBullect::onEnter()
{
	Bullet::onEnter();

	ActionInterval* rot = RotateBy::create(1.0f,360 - cocos2d::random(0,1) * 720 );
	ActionInterval* rep = RepeatForever::create(rot);
	runAction(rep);

	//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓?
	void* otherdata = m_weapon->getBltOtherData();
	if (otherdata != nullptr)
	{
		int times = (intptr_t)otherdata;
		m_MaxTimes = times;
	}
}

void ReboundBullect::initBody()
{
	//鍦嗛敓鏂ゆ嫹
	auto body = PhysicsBody::createCircle( getContentSize().width*0.4, PhysicsMaterial(0.0f,1.0f,0.5f));
	body->setGravityEnable(true);
	body->setRotationEnable(true);
	body->setLinearDamping(0.4f);
	body->setMass(100);
	setPhysicsBody(body);
	setBitmask(
		ENEMY_AMMO2_CATEGORYBITMASK,
		ENEMY_AMMO2_COLLISIONBITMASK_P,
		ENEMY_AMMO2_CONTACTTESTBITMASK);
	setInvincible(true);
	setInvincibleAtEdge();
}

void ReboundBullect::ImpactOfHp( const BasePro& rs )
{
	// 閿熸枻鍒版挒閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹�?
	if ( m_MaxTimes <= _collisionTimes && rs.getMyId() == UnitId::eBottomWall)
	{
		setDeadType(DeadType::Dead_Force);
		m_isDead = true;
		return;
	}

	// 閿熸枻鎷蜂竴閿熸枻鎷锋挒閿熸枻鎷烽敓闃惰鎷烽敓鏂ゆ嫹閿熸枻鎷风粺閿熸枻�?
	if ( rs.getMyId() == UnitId::eBottomWall )
	{
		_collisionTimes ++;
	}
}


//閿熸枻鎷烽敓鏂ゆ�?
GrenadeBullect* GrenadeBullect::create( const std::string& frameName )
{
	auto bullet = new (std::nothrow) GrenadeBullect;
	if (bullet && bullet->initWithSpriteFrameName(frameName))
	{
		bullet->initBullet();
		bullet->autorelease();
		bullet->initBody();
		bullet->RandPos();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void GrenadeBullect::onEnter()
{
	Bullet::onEnter();
	setDelayRemoveTime(1.0f);
	Point myPos = this->convertToWorldSpaceAR(Point::ZERO);

	ccBezierConfig bzconfig;
	if ( cocos2d::random(0,1) )	
	{
		bzconfig.controlPoint_1 = Vec2(m_WinSize.width + 100 - myPos.x,myPos.y*0.8);
		bzconfig.controlPoint_2 = Vec2(m_WinSize.width - 200 - myPos.x ,myPos.y*0.4);
		bzconfig.endPosition = Vec2( myPos.x,- 100);
	}
	else    
	{
		bzconfig.controlPoint_1 = Vec2(-100 + myPos.x,myPos.y*0.8);
		bzconfig.controlPoint_2 = Vec2(200 + myPos.x ,myPos.y*0.4);
		bzconfig.endPosition = Vec2(myPos.x,-100);
	}

	setOpacity( 0 );

	ActionInterval* bez = BezierTo::create(3.0f,bzconfig);
	runAction( bez );

	auto s = getContentSize();

	auto sp = Sprite::create("particla/liudan/kulou.png");
	sp->setPosition(s.width*0.5, s.height*0.5);
	sp->setName("kulou");
	addChild(sp);

	//閿熸枻鎷峰熬閿熸枻鎷烽敓鏂ゆ�?
	EffectsManager::addParticle("particla/liudan/tail_1.plist", this, Vec2(s.width*0.5, s.height*0.1), false);
	//EffectsManager::addParticle("particla/liudan/tail_2.plist", this, Vec2(s.width*0.5, s.height*0.5), false);
	//EffectsManager::addParticle("particla/liudan/tail_3.plist", this, Vec2(s.width*0.5, s.height*0.8), false);

}

void GrenadeBullect::initBullet()
{
	//閿熸枻鎷烽敓鐨嗙鎷峰閿熸枻鎷?
	Bullet::initBullet();
	m_BasePro.setHurts(Ene_Blt_hurt);
	setInvincible(true);
}

void GrenadeBullect::RandPos()
{
	int x = cocos2d::random( 50, 700 );
	setPosition( Vec2(x, 1280) );
}

void GrenadeBullect::OtherDealWhenContact( const BasePro& rs )
{
	if (rs.getMyId() == UnitId::eScreenEdgeSmall)
	{
		return;
	}
	stopAllActions();
	_physicsBody->setCollisionBitmask(0);

	//UIController::getInstance()->getEffectsLayer()->PlaneCantTouchAble(2.0f);

	Node* node = getChildByName("kulou");
	if (node)
	{
		auto fadeout = FadeOut::create(0.5f);
		auto scale = ScaleTo::create(0.1f,10.0f);
		auto spw = Spawn::create(fadeout, scale, nullptr);
		auto func = CallFunc::create(this, callfunc_selector(PhysicsUnit::DeathForce));
		auto seq = Sequence::create(spw, func, nullptr);
		node->runAction(seq);
	}
}


//-------------------
HelixBullet* HelixBullet::create( const std::string& frameName )
{
	HelixBullet* bullet = new (std::nothrow) HelixBullet;
	if (bullet && bullet->initWithSpriteFrameName(frameName))
	{
		bullet->initBullet();
		bullet->autorelease();
		bullet->initBody();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void HelixBullet::onEnter()
{
	Bullet::onEnter();
	setBitmask(
		ENEMY_AMMO_CATEGORYBITMASK,
		ENEMY_AMMO_COLLISIONBITMASK,
		ENEMY_AMMO_CONTACTTESTBITMASK);
	_physicsBody->setLinearDamping(0.8);
	_physicsBody->setGravityEnable(false);
	//_physicsBody->setVelocity(Vec2(0,-10));
	_physicsBody->setMass(0.008);

	m_centerPos = getPosition();
	m_beginPos = m_centerPos;
	m_centerPos.y -= 50;
	schedule(schedule_selector(HelixBullet::Helix));

	{
		ActionInterval* movedown = MoveBy::create(2.0,ccp(0,-250));
		ActionInterval* repeat = RepeatForever::create(movedown);
		//runAction(repeat);
	}
	m_randNun = random(0,1) - 0.5;
	m_randNun /= 5;
}

void HelixBullet::Helix( float dt )
{
	//閿熺煫纰夋嫹閿熸枻鎷峰墠閿熸枻鎷烽敓鏂ゆ嫹閿熷姭搴︼綇鎷烽敓鏂ゆ嫹寮忛敓鏂ゆ�?
	//Vec2 curV = _physicsBody->getVelocity();

	////閿熸彮纰夋嫹閿熷姭璁规嫹閿熸枻鎷烽敓鏂ゆ嫹閿熶茎鍑ゆ嫹閿熺锝忔嫹閿熸枻鎷烽敓鑴氳鍑ゆ嫹閿熸枻鎷烽敓鎻紮鎷烽敓鏂ゆ嫹�?閿熸枻鎷烽敓鏂ゆ嫹涓€閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋�?
	//Vec2 faxian = curV.getPerp();
	//faxian.normalize();
	//_physicsBody->applyImpulse(faxian * 0.1,Vec2(0,0));
	auto nextPos = ccpRotateByAngle(m_beginPos, m_centerPos,0.12); //閿熸枻鎷烽敓鏂ゆ�?
	setPosition(nextPos);
	m_beginPos = nextPos;
	m_centerPos.y -= 3.0;
}

void HelixBullet::initBullet()
{
	Bullet::initBullet();
	m_BasePro.setHurts(-1000);
}


//----------------------
HelixBulletEx* HelixBulletEx::create( const std::string& frameName )
{
	HelixBulletEx* bullet = new (std::nothrow) HelixBulletEx;
	if (bullet && bullet->initWithSpriteFrameName(frameName))
	{
		bullet->initBullet();
		bullet->autorelease();
		bullet->initBody();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void HelixBulletEx::onEnter()
{
	HelixBullet::onEnter();

	float helixTime = 1.0f;
	{
		auto delay = DelayTime::create(helixTime);
		auto func = CallFunc::create(this,callfunc_selector(HelixBulletEx::StopHelix));
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}
}

void HelixBulletEx::StopHelix()
{
	unschedule(schedule_selector(HelixBulletEx::Helix));
	MoveToPl();
}

void HelixBulletEx::MoveToPl()
{
	stopAllActions();
	_physicsBody->setLinearDamping(0.0);
	auto pl = GameData::getInstance()->getMainPlane();
	Vec2 pos;
	if (pl)
	{
		pos = pl->convertToWorldSpaceAR(Vec2::ZERO);
	}
	else
	{
		pos = Vec2(360,-10);
	}

	auto mypos = convertToWorldSpaceAR(Vec2::ZERO);
	auto newVec = pos - mypos;
	newVec.normalize();
	_physicsBody->setVelocity(newVec * 700);
}

void HelixBulletEx::initBody()
{
	Size size = getContentSize();

	auto body = PhysicsBody::createBox(size);
	body->setGravityEnable(false);
	body->setLinearDamping(0.8);
	body->setVelocity(Vec2(0,-10));
	body->setMass(0.008);
	setPhysicsBody(body);
	setBitmask(
		ENEMY_AMMO_CATEGORYBITMASK,
		ENEMY_AMMO_COLLISIONBITMASK,
		ENEMY_AMMO_CONTACTTESTBITMASK);
}



//-------------------------
FallingBulletEX* FallingBulletEX::create( const std::string& frameName )
{
	FallingBulletEX* bullet = new (std::nothrow) FallingBulletEX;
	if (bullet && bullet->initWithSpriteFrameName(frameName))
	{
		bullet->initBullet();
		bullet->autorelease();
		bullet->initBody();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void FallingBulletEX::initBody()
{
	Bullet::initBody();

	setBitmask(
		ENEMY_AMMO_CATEGORYBITMASK,
		ENEMY_AMMO_COLLISIONBITMASK,
		ENEMY_AMMO_CONTACTTESTBITMASK);
	_physicsBody->setLinearDamping(0.5);
	_physicsBody->setMass(0.008);

}

void FallingBulletEX::onEnter()
{
	Bullet::onEnter();

	QuickShoot();
}

void FallingBulletEX::newFalling( float dt )
{
	_physicsBody->applyForce(Vec2(rand_minus1_1()*0.3,-rand_0_1()),Vec2(0,0));
}

void FallingBulletEX::QuickShoot()
{
	//_physicsBody->setVelocity(Vec2(0, -300));
	ActionInterval* delay = DelayTime::create(0.2f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(FallingBulletEX::shootCallback));
	ActionInterval* seq = Sequence::create(delay,func,nullptr);
	runAction(seq);
}

void FallingBulletEX::shootCallback()
{
	schedule(schedule_selector(FallingBullet::newFalling),0.1);
}



//---------------------------------
DartBullet* DartBullet::create( const std::string& frameName )
{
	DartBullet* db = new (std::nothrow) DartBullet;
	if (db && db->initWithSpriteFrameName(frameName))
	{
		db->initBullet();
		db->autorelease();
		db->initBody();
		return db;
	}
	else
	{
		CC_SAFE_DELETE(db);
		return nullptr;
	}
}

void DartBullet::onEnter()
{
	Bullet::onEnter();
	Size s = getContentSize();
	setInvincible(true); 
	PlayAction();
	addParticle();
	setInvincibleAtEdge();//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熺即鎾呮嫹閿熸枻鎷烽敓?

	NewAudio::getInstance()->playEffect(Music_Feibiao);
}

void DartBullet::PlayAction()
{
	//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷疯�?
	ActionInterval* rotate = RotateBy::create(1.0f,-2000);
	ActionInterval* rep = RepeatForever::create(rotate);
	runAction(rep);

	//閿熺嫛璁规嫹閿熸枻鎷烽敓鏂ゆ嫹骞曢敓鏂ゆ嫹閿熸枻�?
	Vec2 endPos = Vec2(m_WinSize.width* 0.5, m_WinSize.height - 100);
	ActionInterval* mv = MoveTo::create(2.0f,endPos);
	ActionInterval* ease = EaseExponentialOut::create(mv);
	runAction(ease);

	ActionInterval* delay = DelayTime::create(1.8f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(DartBullet::addForce));
	ActionInterval* seq = Sequence::create(delay,func,nullptr);
	runAction(seq);
}

void DartBullet::addParticle()
{
	auto size = getContentSize();
	auto pos = Vec2(size.width * 0.5, size.height *0.5);
	EffectsManager::addParticle("particla/Huixuanbiao/Huixuanbiao.plist",this,pos,false);
}

void DartBullet::addForce()
{
	Vec2 mypos = convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 amiPos = Vec2(360,m_WinSize.height*0.15);
	auto mainpl = GameData::getInstance()->getMainPlane();
	if (mainpl)
	{
		amiPos = mainpl->convertToWorldSpaceAR(Vec2::ZERO);
	}

	Vec2 newVec = amiPos - mypos;
	newVec.normalize();
	_physicsBody->applyForce(newVec * 0.3);

	ActionInterval* delay = DelayTime::create(1.5f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(PhysicsUnit::DeathForce));
	ActionInterval* seq = Sequence::create(delay,func,nullptr);
	runAction(seq);
}

void DartBullet::initBullet()
{
	//閿熸枻鎷烽敓鐨嗙鎷峰閿熸枻鎷?
	Bullet::initBullet();
	setInvincible(true);
	m_BasePro.setHurts(-1000);
}

void DartBullet::initBody()
{
	Bullet::initBody();
	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
	_physicsBody->setGravityEnable(false);
}

//-----------------------------------
LaserBullet* LaserBullet::create(int lvl)
{
	LaserBullet* db = new (std::nothrow) LaserBullet;
	if (db && db->init())
	{
		db->initBullet();
		db->m_level = lvl;
		db->autorelease();
		db->initBody();
		return db;
	}
	else
	{
		CC_SAFE_DELETE(db);
		return nullptr;
	}
}

void LaserBullet::initBody()
{
	auto body = PhysicsBody::createBox(Size(25,2000),PHYSICSBODY_MATERIAL_DEFAULT, Vec2(0,1000));
	body->setGravityEnable(false);
	body->setMass(0.0001);
	setPhysicsBody(body);
	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
	schedule(schedule_selector(LaserBullet::setBody),0.1);
}

void LaserBullet::initBullet()
{
	Bullet::initBullet();
	m_dur = 3.0f;
	m_scaleY = 2000;
	m_scaleX = 2.0f;
	m_timeIn = 1.0f;
	m_TimeToScale = 2.0f;
	m_TimeToBack = 2.0f;
	setInvincibleAbsolutely(1);	//閿熸枻鎷烽敓鏂ゆ嫹钖滈敓?
}

void LaserBullet::onEnter()
{
	Bullet::onEnter();
	NewAudio::getInstance()->playEffect(Music_Laser);
	ListeningStopWeapon();

	addShootPoint();
	addExtraLight();
}

void LaserBullet::onExit()
{
	Bullet::onExit();
}

void LaserBullet::removeLaser()
{
	DeathForce();
}

void LaserBullet::removeLaserO( Ref* obj )
{
	DeathForce();
}

void LaserBullet::setBody(float dt)
{
	auto body = PhysicsBody::createBox(Size(25,2000),PHYSICSBODY_MATERIAL_DEFAULT, Vec2(0,1000));
	body->setGravityEnable(false);
	body->setMass(0.0001);
	setPhysicsBody(body);
	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
}

void LaserBullet::addExtraLight()
{
	if (m_level == 3)
	{
		/*auto sp = Sprite::create("weapon.png");
		sp->setPosition(Vec2::ZERO);
		sp->setAnchorPoint(Vec2(0.5,0));
		addChild(sp);
		sp->setScaleX(0.01);
		sp->setScaleY(2.0);
		auto action = getCommonAction(0.8, 3.0);
		sp->runAction(action);

		auto layer = UIController::getInstance()->getEffectsLayer();
		auto animat = layer->getFramesAnimation("Laser3_", 8, 1.0/24, 0, REPEAT_FORVER, true);
		sp->runAction(animat);

		addExtraCircle();*/
	}
}

void LaserBullet::addExtraCircle()
{
	auto delay = DelayTime::create(0.1 + rand_0_1() * 0.1);
	auto next = CallFunc::create(this, callfunc_selector(LaserBullet::ExtraCircle));
	auto seq = Sequence::create(delay, next, nullptr);
	runAction(seq);
}

void LaserBullet::ExtraCircle()
{
	auto sp = Sprite::create("animation/Laser3/quan.png");
	addChild(sp);
	auto curScale = m_ShootSp->getScaleX();
	sp->setScale(0.25 * curScale);

	{
		auto mv = MoveBy::create(0.7 + rand_0_1() * 0.1,Vec2(0, 1500));
		auto ease = EaseSineIn::create(mv);
		auto rm = RemoveSelf::create(true);
		auto seq = Sequence::create(ease, rm, nullptr);
		sp->runAction(seq);
	}

	//addExtraCircle();
}

void LaserBullet::addLaserCore()
{
	auto laerCore = Sprite::create("ammo/laser/laser.png");
	laerCore->setAnchorPoint(Vec2(0.5f,0));
	laerCore->setPosition(Vec2(0, m_ShootSp->getContentSize().height - 25) + getLaserCoreOffset());
	laerCore->setScaleX(0.05);
	laerCore->setScaleY(m_scaleY);
	addChild(laerCore,-1);

	auto action = getCommonAction(m_scaleX, m_scaleY);
	laerCore->runAction(action);
}

void LaserBullet::addShootPoint()
{
	m_ShootSp = Sprite::create("ammo/laser/ShootPos.png");
	m_ShootSp->setPosition(Vec2(0,-25) + getShootPointOffset());
	m_ShootSp->setAnchorPoint(Vec2(0.5f,0.5f));
	m_ShootSp->setScaleX(0.05f);
	m_ShootSp->setScaleY(m_scaleX);
	addChild(m_ShootSp);
	{
		auto action1 = getCommonAction(m_scaleX, m_scaleX);
		auto rm = CallFunc::create(this, callfunc_selector(LaserBullet::removeLaser));
		auto seq = Sequence::create(action1, rm, nullptr);
		m_ShootSp->runAction(seq);
	}
	
	{
		//閿熸枻鎷烽敓鏂ゆ嫹鑺敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹寰敓鏂ゆ嫹閿熸枻鎷?
		auto mv = MoveBy::create(0.1, Vec2(2,0));
		auto mvBack = MoveBy::create(0.1, Vec2(-2,0));
		auto seq = Sequence::create(mv, mvBack, nullptr);
		auto rep = RepeatForever::create(seq);
		runAction(rep);
	}

	addLaserCore();
}

ActionInterval* LaserBullet::getCommonAction(float scaleX, float scaleY)
{
	//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽�?
	auto delay = DelayTime::create(m_timeIn);
	auto scaleto = ScaleTo::create(m_TimeToScale, scaleX, scaleY);
	auto scaleback = ScaleTo::create(m_TimeToBack, 0.05, scaleY);
	auto fadeOut = FadeOut::create(m_TimeToBack);
	auto stay = DelayTime::create(m_dur);
	auto spwOut = Spawn::create(scaleback, fadeOut, nullptr);
	auto seq = Sequence::create(delay, scaleto, stay, spwOut, nullptr);
	return seq;
}

cocos2d::Vec2 LaserBullet::getLaserCoreOffset()
{
	return Vec2(0,0);
}

cocos2d::Vec2 LaserBullet::getShootPointOffset()
{
	return Vec2(0,0);
}

//-------------------------------
OutsideLaser::OutsideLaser()
{
	m_scaleX = 1.0f;
}

OutsideLaser* OutsideLaser::create(const Vec2& pos,int zorder)
{
	OutsideLaser* db = new (std::nothrow) OutsideLaser;
	if (db && db->init())
	{
		db->initBullet();
		db->autorelease();
		db->m_pos = pos;
		db->m_zorder = zorder;
		db->initBody();
		return db;
	}
	else
	{
		CC_SAFE_DELETE(db);
		return nullptr;
	}
}

void OutsideLaser::MoveCircle()
{
	{
		ActionInterval* moveBy = MoveBy::create(0.6f,Vec2(-m_pos.x * 2,0));
		ActionInterval* ease = EaseCircleActionInOut::create(moveBy);
		ActionInterval* back = ease->reverse();
		ActionInterval* seq = Sequence::create(ease,back,nullptr);
		ActionInterval* rep = RepeatForever::create(seq);
		this->runAction(rep);
	}
	
	//schedule(schedule_selector(OutsideLaser::refreshPos));
}

void OutsideLaser::onEnter()
{
	LaserBullet::onEnter();
	addDipan();
	MoveCircle();

	schedule(schedule_selector(LaserBullet::setBody),0.1f);
}

void OutsideLaser::addDipan()
{
	auto sp = Sprite::create("ammo/laser/dipan.png");
	sp->setPosition(0,0);
	sp->setScale(1.5f);
	sp->setOpacity(0);
	addChild(sp,-1);

	auto fadein = FadeIn::create(0.1);
	auto delay = DelayTime::create(m_timeIn + m_dur + m_TimeToScale - 0.1);
	auto fadeOut = FadeOut::create(m_TimeToBack);
	auto seq = Sequence::create(fadein, delay, fadeOut, nullptr);
	sp->runAction(seq);
}

cocos2d::Vec2 OutsideLaser::getLaserCoreOffset()
{
	return Vec2(0,0);
}

cocos2d::Vec2 OutsideLaser::getShootPointOffset()
{
	return Vec2(0,0);
}



//-----------------------------
CatBullet* CatBullet::create( bool isleft, CatType ct )
{
	CatBullet* cat = new (std::nothrow) CatBullet;
	if (cat && cat->init())
	{
		cat->initBullet();
		cat->autorelease();
		cat->initBody();
		cat->m_catType = ct;
		cat->m_direct = 1;
		if (isleft)
		{
			cat->m_direct = -1;
		}
		return cat;
	}
	else
	{
		CC_SAFE_DELETE(cat);
		return nullptr;
	}
}

void CatBullet::onEnter()
{
	Bullet::onEnter();
	setInvincible(true);
	initCat();
	FirstJump();
	setScale(0.7);
	CatLeave();
}

void CatBullet::NextPos()
{
	Vec2 mypos = convertToWorldSpaceAR(Vec2::ZERO);
	float dis = 150 + rand_0_1() * 50 + m_ExDis;

	auto targetPos = GameData::getInstance()->getNearTargetByPos(mypos);
	bool isIn = isTargetInScreen(targetPos);

	//閿熻鍑ゆ嫹閿熸枻鎷风洰閿熺枼锛岄敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鐩敓鏂ゆ嫹閿熸枻鎷烽敓鐨嗘拝鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹璺冮敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鑴氱鎷?
	if (!isIn || m_timeToLeave)
	{
		m_JumpVec = Vec2(dis * m_direct, dis);
	}
	else
	{
		m_JumpVec = targetPos - mypos;
		m_JumpVec.normalize();
		m_JumpVec *= dis;
	}
	FixJumpPos();

	m_NextRunPos = mypos + m_JumpVec;
}

void CatBullet::TurnDirect()
{
	//閿熸枻鎷烽敓鏂ゆ嫹杞敓鏂ゆ�?
	if (m_JumpVec.x > 0) 
	{
		//杞敓鏂ゆ嫹
		m_catStay->setFlipX(true);
	}
	else
	{
		//閿熸枻鎷疯浆閿熸枻鎷?
		m_catStay->setFlipX(false);
	}

	//1閿熸枻鎷烽敓鏂ゆ�?
	if (m_JumpVec.y > 0 && m_JumpVec.x > 0)
	{
		m_catJump->setFlipY(false);
	}
	else if(m_JumpVec.y > 0 && m_JumpVec.x <= 0)
	{
		//2閿熸枻鎷烽敓鏂ゆ�?
		m_catJump->setFlipY(true);
	}
	else if(m_JumpVec.y <= 0 && m_JumpVec.x <= 0)
	{
		//3閿熸枻鎷烽敓鏂ゆ�?
		m_catJump->setFlipY(true);
	}
	else
	{
		m_catJump->setFlipY(false);
	}
	m_catJump->setFlipX(true);

	if (m_direct < 0)
	{
		m_direct = 1;
	}
	else
	{
		m_direct = -1;
	}
}

void CatBullet::AdjustCatAngle()
{
	float angle = m_JumpVec.getAngle() * Pi2Angle;

	m_catJump->setRotation(-angle);
}

void CatBullet::CatRun()
{
	NextPos();
	TurnDirect();
	AdjustCatAngle();
	JumpState();
	ActionInterval* move = MoveTo::create(0.15f + rand_0_1() * 0.03f,m_NextRunPos);
	CallFunc* stay = CallFunc::create(this,callfunc_selector(CatBullet::StayState));
	ActionInterval* delay = DelayTime::create(0.05f + rand_0_1() * 0.2f);
	CallFunc* next = CallFunc::create(this,callfunc_selector(CatBullet::CatRun));
	ActionInterval* seq = Sequence::create(move,stay,delay,next,nullptr);
	runAction(seq);
}

void CatBullet::initBullet()
{
	Bullet::initBullet();
	m_timeToLeave = false;
	m_ExDis = 0.0f;
}

void CatBullet::initBody()
{
	auto body = PhysicsBody::createBox(Size(100,50));
	body->setGravityEnable(false);
	body->setMass(0.0001);
	setPhysicsBody(body);
	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
	_physicsBody->setGravityEnable(false);
}

void CatBullet::addClawL()
{
	CatClaw* claw = CatClaw::create(m_JumpVec);
	claw->setPosition(m_contactPos);
	UIController::getInstance()->getPhysicsLayer()->addChild(claw);
}

void CatBullet::addClawR()
{
	CatClaw* claw = CatClaw::create(m_JumpVec);
	claw->setPosition(m_contactPos);
	UIController::getInstance()->getPhysicsLayer()->addChild(claw);
}

void CatBullet::OtherDealWhenContact( const BasePro& rs )
{
	if (rs.getMyId() == UnitId::eScreenEdgeSmall)
	{
		return;
	}
	m_contactPos = rs.getContactPos();
	if (rs.getMyId() == UnitId::ePlaneBoss)
	{
		m_ExDis = 150.0f;
	}
	else
	{
		m_ExDis = 0.0f;
	}

	//addClawL();	//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷疯�?

	//閿熸枻鎷烽敓鏂ゆ嫹閿熸彮浼欐嫹閿熸彮顏庢嫹鏄犱繀閿熸枻鎷烽敓鏂ゆ嫹閿熼樁锔兼嫹纭敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹?
	ActionInterval* delay = DelayTime::create(0.05);
	CallFunc* func = CallFunc::create(this,callfunc_selector(CatBullet::addClawR));
	ActionInterval* seq = Sequence::create(delay,func,nullptr);
	runAction(seq);

}

void CatBullet::JumpState()
{
	m_catJump->setVisible(true);
	m_catStay->setVisible(false);
}

void CatBullet::StayState()
{
	m_catJump->setVisible(false);
	m_catStay->setVisible(true);
}

void CatBullet::initCat()
{
	std::string Stayres;
	std::string Jumpres;
	switch (m_catType)
	{
	case CatBullet::cat:
		{
			Stayres = "Effect/cat/catStay_b.png";
			Jumpres = "Effect/cat/catJump_b.png";
			if (m_direct < 0)
			{
				Stayres = "Effect/cat/catStay_w.png";
				Jumpres = "Effect/cat/catJump_w.png";
			}
		}
		break;
	case CatBullet::wolf:
		{
			Stayres = "Effect/cat/wolfStay_b.png";
			Jumpres = "Effect/cat/wolfJump_b.png";
			if (m_direct < 0)
			{
				Stayres = "Effect/cat/wolfStay_w.png";
				Jumpres = "Effect/cat/wolfJump_w.png";
			}
		}
		break;
	case CatBullet::tiger:
		{
			Stayres = "Effect/cat/tigerStay_b.png";
			Jumpres = "Effect/cat/tigerJump_b.png";
			if (m_direct < 0)
			{
				Stayres = "Effect/cat/tigerStay_w.png";
				Jumpres = "Effect/cat/tigerJump_w.png";
			}
		}
		break;
	default:
		break;
	}
	
	auto s = getContentSize();
	m_catStay = Sprite::create(Stayres);
	m_catStay->setPosition(s.width * 0.5,s.height*0.5);
	addChild(m_catStay);

	m_catJump = Sprite::create(Jumpres);
	m_catJump->setPosition(s.width * 0.5,s.height*0.5);
	addChild(m_catJump);
}

bool CatBullet::isTargetInScreen( const Vec2& pos )
{
	if (pos.x > m_WinSize.width - 50 || pos.x < 50  )
	{
		return false;
	}
	if (pos.y > m_WinSize.height - 50 || pos.y < 50)
	{
		return false;
	}
	return true;
}

void CatBullet::FirstJump()
{
	Vec2 mypos = convertToWorldSpaceAR(Vec2::ZERO);
	float dis = 150 + rand_0_1() * 50 + m_ExDis;

	m_JumpVec = Vec2(dis * m_direct, dis);

	m_NextRunPos = mypos + m_JumpVec;
	TurnDirect();
	AdjustCatAngle();
	JumpState();

	ActionInterval* move = MoveTo::create(0.2f + rand_0_1() * 0.03f,m_NextRunPos);
	ActionInterval* ease = EaseExponentialOut::create(move);
	CallFunc* stay = CallFunc::create(this,callfunc_selector(CatBullet::StayState));
	ActionInterval* delay = DelayTime::create(0.05f + rand_0_1() * 0.2f);
	CallFunc* next = CallFunc::create(this,callfunc_selector(CatBullet::CatRun));
	ActionInterval* seq = Sequence::create(ease,stay,delay,next,nullptr);
	runAction(seq);
}

void CatBullet::CatLeave()
{
	ActionInterval* delay = DelayTime::create(5.0f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(CatBullet::LeaveCB));
	ActionInterval* seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void CatBullet::LeaveCB()
{
	m_timeToLeave = true;
}

void CatBullet::FixJumpPos()
{
	auto Rpos = Vec2(0,0);	//閿熸枻鎷疯浆閿熸枻鎷烽敓?
	float angle = m_JumpVec.getAngle();
	if (angle > PI/4 && angle < PI/2)
	{
		m_JumpVec.rotate(Rpos,PI/4 - angle );
	}
	else if(angle >= PI/2 && angle < PI * 3/4)
	{
		m_JumpVec.rotate(Rpos,PI * 3/4 - angle );
	}
	else if(angle >= PI * 5/4 && angle < PI * 6/4)
	{
		m_JumpVec.rotate(Rpos,PI * 5/4 - angle );
	}
	else if(angle >= PI * 6/4 && angle < PI * 7/4)
	{
		m_JumpVec.rotate(Rpos,PI * 7/4 - angle );
	}

}

//----------------------
CatClaw* CatClaw::create( const Vec2& direc )
{
	CatClaw* claw = new (std::nothrow) CatClaw;
	if (claw && claw->initWithFile("Effect/cat/claw.png"))
	{
		claw->initBullet();
		claw->m_MvDirect = direc;
		claw->autorelease();
		claw->initBody();
		return claw;
	}
	else
	{
		CC_SAFE_DELETE(claw);
		return nullptr;
	}
}

void CatClaw::initBody()
{
	auto body = PhysicsBody::createBox(getContentSize());
	body->setGravityEnable(false);
	body->setMass(0.001);
	setPhysicsBody(body);
	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
	setInvincible(true);
	setInvincibleAtEdge();
}

void CatClaw::onEnter()
{
	Bullet::onEnter();
	
	float r = m_MvDirect.getAngle() * Pi2Angle;
	setRotation(-r);	//閿熸枻鎷烽敓鏂ゆ嫹鏈ㄩ敓楗轰紮鎷疯柟閿熸枻鎷烽敓鏂ゆ�?

	//runAction(RotateBy::create(0.01,-r));
	NewAudio::getInstance()->playEffect(Music_Clow);
	setScale(3);
	Catch();
}

void CatClaw::Catch()
{
	float mvDis = 100 + rand_0_1() * 20;
	m_MvDirect.normalize();
	m_MvDirect *= mvDis;

	ActionInterval* scale = ScaleTo::create(0.1f,1.0f,1.0f);
	ActionInterval* move = MoveBy::create(0.25f,m_MvDirect);
	ActionInterval* fadein = FadeOut::create(0.25f);
	ActionInterval* spaw = Spawn::create(scale,move,fadein,nullptr);

	CallFunc* func = CallFunc::create(this,callfunc_selector(PhysicsUnit::DeathForce));
	ActionInterval* seq = Sequence::create(spaw,func,nullptr);

	runAction(seq);
}


//----------------------------------------
BossDart* BossDart::create(const std::string& frameName)
{
	BossDart* claw = new (std::nothrow) BossDart;
	if (claw && claw->initWithSpriteFrameName(frameName))
	{
		claw->initBullet();
		claw->autorelease();
		claw->initBody();
		return claw;
	}
	else
	{
		CC_SAFE_DELETE(claw);
		return nullptr;
	}
}

void BossDart::initBody()
{
	//鍦嗛敓鏂ゆ嫹
	auto body = PhysicsBody::createBox( getContentSize());
	body->setGravityEnable(true);
	body->setRotationEnable(false);
	setPhysicsBody(body);
	setBitmask(
		ENEMY_AMMO_CATEGORYBITMASK,
		ENEMY_AMMO_COLLISIONBITMASK,
		ENEMY_AMMO_CONTACTTESTBITMASK);

	addParticle();
}

void BossDart::initBullet()
{
	Bullet::initBullet();
	m_BasePro.setHurts(Ene_Blt_hurt);
}

void BossDart::addParticle()
{
	auto pos = Vec2(getContentSize().width*0.5,getContentSize().height*0.5);
	EffectsManager::addParticle("particla/Boss1ammo/Boss1ammo.plist",this,pos,false);
}


//---------------------------
FlowerBullet* FlowerBullet::create()
{
	FlowerBullet* flower = new (std::nothrow) FlowerBullet;
	if (flower && flower->init())
	{
		flower->initBullet();
		flower->initBody();
		flower->initFLower();
		flower->autorelease();
		return flower;
	}
	else
	{
		CC_SAFE_DELETE(flower);
		return nullptr;
	}
}

void FlowerBullet::initBody()
{
	auto body = PhysicsBody::createCircle(m_radius);;
	body->setGravityEnable(false);
	setPhysicsBody(body);
	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
	setInvincible(true);
	setInvincibleAtEdge(1);

	m_BasePro.setHurts(-700);
}

void FlowerBullet::onEnter()
{
	Bullet::onEnter();
	schedule(schedule_selector(FlowerBullet::setBody),0.1f);
	start();

	NewAudio::getInstance()->playEffect(Music_Flower);
}

void FlowerBullet::setBody(float dt)
{
	auto body = PhysicsBody::createCircle(m_radius);;
	body->setGravityEnable(false);
	setPhysicsBody(body);
	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
}

void FlowerBullet::initFLower()
{
	m_Flower = Sprite::create("particla/Petals/flower.png");
	addChild(m_Flower,-1);

	m_eff1 = Sprite::create("particla/Petals/flowerEff1.png");
	addChild(m_eff1,-1);
	m_eff1->setVisible(false);

	m_eff2 = Sprite::create("particla/Petals/flowerEff2.png");
	addChild(m_eff2,-1);
	m_eff2->setVisible(false);
}

void FlowerBullet::start()
{
	Vec2 pos = convertToWorldSpaceAR(Vec2::ZERO);

	ActionInterval* mv = MoveTo::create(1.0f,Vec2(pos.x, m_WinSize.height * 0.8f));
	runAction(mv);

	ActionInterval* delay = DelayTime::create(0.5f);
	CallFunc* next = CallFunc::create(this,callfunc_selector(FlowerBullet::FLowerAction));
	ActionInterval* seq = Sequence::create(delay, next, nullptr);
	runAction(seq);

	DropFlower();
}

void FlowerBullet::FLowerAction()
{
	{
		ActionInterval* rotate = RotateBy::create(2.0f,-1000);
		ActionInterval* ease = EaseCubicActionIn::create(rotate);
		m_Flower->runAction(ease);
	}

	{
		ActionInterval* scale = ScaleBy::create(2.0f,4.5);
		m_Flower->runAction(scale);
	}

	{
		ActionInterval* delay = DelayTime::create(2.0f);
		CallFunc* next = CallFunc::create(this,callfunc_selector(FlowerBullet::Eff1Action));
		ActionInterval* seq = Sequence::create(delay, next, nullptr);
		m_Flower->runAction(seq);
	}
	
	{
		ActionInterval* delay = DelayTime::create(1.8f);
		ActionInterval* fade = FadeOut::create(0.8f);
		ActionInterval* seq = Sequence::create(delay,fade,nullptr);
		m_Flower->runAction(seq);
	}
}

void FlowerBullet::Eff1Action()
{
	m_eff1->setVisible(true);
	m_eff1->setScale(5.0);

	//閿熸枻鎷疯繙閿熸枻鎷疯浆 + 閿熸枻鎷烽敓鏂ゆ�?
	{
		ActionInterval* rotate = RotateBy::create(1.0f,-1000);
		ActionInterval* rep = RepeatForever::create(rotate);
		m_eff1->runAction(rep);

		ActionInterval* fadein = FadeIn::create(0.5f);
		m_eff1->runAction(fadein);
	}

	//閿熸枻鎷烽敓鏂ゆ�?
	ActionInterval* delay = DelayTime::create(0.8f);

	//閿熻剼杈炬嫹 + 閿熸枻鎷烽敓鏂ゆ嫹閿熼摪闈╂嫹閿熸枻鎷锋�?
	{
		ActionInterval* scale = ScaleTo::create(0.05f,5.0);
		CallFunc* next = CallFunc::create(this,callfunc_selector(FlowerBullet::Eff2Action));
		ActionInterval* spw = Spawn::create(scale, next, nullptr);
		ActionInterval* seq = Sequence::create(delay, spw, nullptr);
		m_eff1->runAction(seq);
	}

	//閿熸枻鎷风偢
	{
		ActionInterval* delay = DelayTime::create(0.8);
		CallFunc* next = CallFunc::create(this,callfunc_selector(FlowerBullet::Explosion));
		ActionInterval* seq = Sequence::create(delay, next, nullptr);
		runAction(seq);
		//Explosion();
	}
}

void FlowerBullet::Eff2Action()
{
	m_eff2->setVisible(true);
	m_eff2->setScale(0.1);
	//EFF1閿熸枻鎷烽敓鏂ゆ�?
	{
		ActionInterval* delay = DelayTime::create(0.4f);
		ActionInterval* fadeout = FadeOut::create(0.2f);
		ActionInterval* seq = Sequence::create(delay,fadeout, nullptr);
		m_eff1->runAction(seq);
	}

	// EFF2閿熸枻鎷烽敓鏂ゆ�?+ EFF2閿熻剼杈炬嫹 + EFF2閿熸枻鎷疯浆
	{
		ActionInterval* fadein = FadeIn::create(0.01);
		ActionInterval* rotate = RotateBy::create(0.8f,-100);
		ActionInterval* scale = ScaleTo::create(0.01f, 5.0);
		ActionInterval* spw = Spawn::create(fadein, rotate, scale, nullptr);
		
		m_eff2->runAction(spw);
	}

	//EFF2閿熸枻鎷烽敓鏂ゆ�?
	{
		ActionInterval* scale = ScaleTo::create(0.01f,5.0);
		ActionInterval* fadeout = FadeOut::create(0.7);
		ActionInterval* spw = Spawn::create(scale, fadeout, nullptr);
		m_eff2->runAction(fadeout);
	}

	
}

void FlowerBullet::Explosion()
{
	unschedule(schedule_selector(FlowerBullet::setBody));
	
	auto body = PhysicsBody::createCircle(150);
	body->setGravityEnable(false);
	setPhysicsBody(body);
	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
	setInvincible(true);
	{
		//閿熸枻鎷风偢閿熸枻鎷烽敓鏂ゆ�?
		Vec2 pos = convertToWorldSpaceAR(Vec2::ZERO);
		auto layer = UIController::getInstance()->getPhysicsLayer();
		EffectsManager::addParticle("particla/Petals/Petals3.plist", layer, pos, true, 5.0f);
	}
	{
		ActionInterval* delay = DelayTime::create(1.0f);
		CallFunc* rm = CallFunc::create(this,callfunc_selector(PhysicsUnit::DeathForce));
		ActionInterval* seq = Sequence::create(delay, rm, nullptr);
		runAction(seq);
		runAction(ScaleBy::create(0.2f,1.3));
	}
}

void FlowerBullet::DropFlower()
{
	EffectsManager::addParticle("particla/Petals/Petals1.plist",this,Vec2(0,0),false);
	EffectsManager::addParticle("particla/Petals/Petals2.plist",this,Vec2(0,0),false);
}

void FlowerBullet::initBullet()
{
	Bullet::initBullet();
	m_radius = 200.0f;
}

//--------------------------------
FireDragon* FireDragon::create(Flaming::FireType ft)
{
	FireDragon* fd = new (std::nothrow) FireDragon;
	if (fd && fd->init())
	{
		fd->m_fireType = ft;
		fd->initBullet();
		fd->addArmature();
		fd->initBody();
		fd->autorelease();
		return fd;
	}
	else
	{
		CC_SAFE_DELETE(fd);
		return nullptr;
	}
}

void FireDragon::initBody()
{
	auto body = PhysicsBody::createBox(getContentSize());
	body->setGravityEnable(false);
	setPhysicsBody(body);
	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
	setInvincible(true);
	m_BasePro.setHurts(-700);
}

void FireDragon::onEnter()
{
	Bullet::onEnter();
	MoveUp();

	NewAudio::getInstance()->playEffect(Music_Dragon);
}

void FireDragon::MoveUp()
{
	auto pos = convertToWorldSpaceAR(Vec2::ZERO);

	{
		ActionInterval* mv = MoveTo::create(6,Vec2(pos.x, m_WinSize.height + 500));
		ActionInterval* ease = CCEaseSineIn::create(mv);
		runAction(ease);
	}

	ShootFire();
}

void FireDragon::ShootFire()
{
	auto s = getContentSize();

	auto fire = Flaming::create(m_fireType);
	fire->setPosition(s.width * 0.5, s.height);

	fire->getBasePro().setHurts(m_BasePro.getHurts());

	addChild(fire);
}

void FireDragon::addArmature()
{
	std::string name;
	
	switch (m_fireType)
	{
	case Flaming::eSmall:
		name = "xiaolong";
		break;
	case Flaming::eMiddle:
		name = "zhonglong";
		break;
	case Flaming::eBig:
		name = "julong";
		break;
	default:
		break;
	}

	auto Armat = Armature::create(name);
	auto s = Armat->getContentSize();
	Armat->setAnchorPoint(Vec2(0.5,0.5));
	Armat->setPosition(s.width*0.5,s.height*0.5);
	addChild(Armat);
	setContentSize(s);
	Armat->getAnimation()->playWithIndex(0);
}



//------------------------
Flaming* Flaming::create(FireType ft)
{
	Flaming* fire = new (std::nothrow) Flaming;
	if (fire && fire->init())
	{
		fire->autorelease();
		fire->m_fireType = ft;
		fire->initBody();
		return fire;
	}
	else
	{
		CC_SAFE_DELETE(fire);
		return nullptr;
	}
}

void Flaming::initBody()
{
	setInvincible(true);
	Size sizeOffset = Size(0,0);
	switch (m_fireType)
	{
	case Flaming::eSmall:
		{

		}
		break;
	case Flaming::eMiddle:
		{
			sizeOffset = Size(20,100);
		}
		break;
	case Flaming::eBig:
		{
			sizeOffset = Size(20,200);
		}
		break;
	default:
		break;
	}

	Vec2 vec[3]=
	{
		Vec2(0,0),
		Vec2(-100 - sizeOffset.width, 250 + sizeOffset.height),
		Vec2(100 + sizeOffset.width, 250 + sizeOffset.height)
	};
	m_FireSize = Size(100 + sizeOffset.width, 250 + sizeOffset.height);

	auto body = PhysicsBody::createPolygon(vec,3,PHYSICSBODY_MATERIAL_DEFAULT,Vec2(0,0));
	body->setGravityEnable(false);
	setPhysicsBody(body);
	_physicsBody->setCategoryBitmask(SELF_AMMO_CATEGORYBITMASK);
	_physicsBody->setCollisionBitmask(SELF_AMMO_COLLISIONBITMASK_P);
	_physicsBody->setContactTestBitmask(SELF_AMMO_CONTACTTESTBITMASK);
	setInvincibleAtEdge();
}

void Flaming::setBody( float dt )
{
	initBody();
}

void Flaming::onEnter()
{
	PhysicsUnit::onEnter();
	schedule(schedule_selector(Flaming::setBody), 0.1);

	std::string particleFile = "particla/FireDragon/FireDragon.plist";
	switch (m_fireType)
	{
	case Flaming::eSmall:
		break;
	case Flaming::eMiddle:
		{
			particleFile = "particla/FireDragon/FireDragonMid.plist";
		}
		break;
	case Flaming::eBig:
		{
			particleFile = "particla/FireDragon/FireDragonBig.plist";
		}
		break;
	default:
		break;
	}

	m_particle = EffectsManager::addParticle(particleFile,this,Vec2(0,0),false);
	if (m_particle)
	{
		m_particle->setPositionType(ParticleSystem::PositionType::GROUPED);
	}

	if (m_fireType == eBig)
	{
		{
			auto next = CallFunc::create(this,callfunc_selector(Flaming::DelayShowSparks));
			auto delay = DelayTime::create(0.7);
			auto seq = Sequence::create(delay, next, nullptr);
			runAction(seq);
		}
		/*{
			auto next = CallFunc::create(this,callfunc_selector(Flaming::DelayShowFireCircle));
			auto delay = DelayTime::create(0.7);
			auto seq = Sequence::create(delay, next, nullptr);
			runAction(seq);
		}*/
	}
}

void Flaming::Sparks()
{
	//閿熸枻鎷烽敓鏂ゆ嫹浣嶉敓鐭殑鍑ゆ嫹�?
	float y = m_FireSize.height * random(0.5f, 1.0f);
	float rate = y/m_FireSize.height;
	float x = rate * m_FireSize.width * 1.2 * rand_minus1_1();
	Vec2 pos = Vec2(x,y);
	
	auto sp = Sprite::create("weapon.png");
	sp->setPosition(pos);
	addChild(sp);
	sp->setRotation(360 * rand_0_1());
	sp->setScale(0.5 + rand_0_1() * 0.3);

	auto layer = UIController::getInstance()->getEffectsLayer();
	auto animat = layer->getFramesAnimation("FireExpl_", 3, 1.0/24);
	auto mv = RemoveSelf::create(true);
	auto seq = Sequence::create(animat, mv, nullptr);
	sp->runAction(seq);
}

void Flaming::FlamCircle()
{
	auto sp = Sprite::create("animation/FireSurround/FireCircle.png");
	sp->setPosition(Vec2(-5,0));
	addChild(sp);
	sp->setScale(1.4);

	//閿熸枻鎷烽敓閾扮鎷烽敓鏂ゆ嫹閿熷壙璁规嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鍒犻敓鏂ゆ�?
	float t = 1.0 + rand_0_1() * 0.2;
	auto mv = MoveBy::create(t, Vec2(0, 150 + rand_0_1() * 20));
	auto ease = EaseSineIn::create(mv);
	auto scale = ScaleTo::create(t, 3.5f + rand_0_1(), 1.0f);
	auto spa = Spawn::create(mv, scale, nullptr);
	auto rm = RemoveSelf::create(true);
	auto fadeout = FadeOut::create(0.5);
	auto seq = Sequence::create(spa, fadeout, rm, nullptr);
	sp->runAction(seq);
}

void Flaming::DelayShowSparks()
{
	auto next = CallFunc::create(this,callfunc_selector(Flaming::Sparks));
	auto interval = DelayTime::create(0.03 + rand_0_1() * 0.02);
	auto seq = Sequence::create(next, interval, nullptr);
	auto rep = RepeatForever::create(seq);

	runAction(rep);
}

void Flaming::DelayShowFireCircle()
{
	auto func = CallFunc::create(this,callfunc_selector(Flaming::FlamCircle));
	auto next = CallFunc::create(this,callfunc_selector(Flaming::DelayShowFireCircle));
	auto interval = DelayTime::create(0.3 + rand_0_1() * 0.1);
	auto seq = Sequence::create(func, interval, next, nullptr);

	runAction(seq);
}

//---------------------------
FireWorks* FireWorks::create(const std::string& frameName)
{
	FireWorks* fire = new (std::nothrow) FireWorks;
	if (fire && fire->initWithSpriteFrameName(frameName))
	{
		fire->initBullet();
		fire->autorelease();
		fire->initBody();
		return fire;
	}
	else
	{
		CC_SAFE_DELETE(fire);
		return nullptr;
	}
}

void FireWorks::initBody()
{
	auto body = PhysicsBody::createBox(getContentSize());
	body->setGravityEnable(true);
	body->setLinearDamping(0.8f);
	setPhysicsBody(body);
	setBitmask(
		ENEMY_AMMO_CATEGORYBITMASK,
		ENEMY_AMMO_COLLISIONBITMASK,
		ENEMY_AMMO_CONTACTTESTBITMASK);
}

void FireWorks::onEnter()
{
	Bullet::onEnter();

	ActionInterval* delay = DelayTime::create(0.5f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(FireWorks::resetLinearDamping));
	ActionInterval* seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void FireWorks::resetLinearDamping()
{
	_physicsBody->setLinearDamping(0.0f);
}

//------------------------
EneRebounceBullet* EneRebounceBullet::create( const std::string& frameName )
{
	EneRebounceBullet* rb = new (std::nothrow) EneRebounceBullet;
	if (rb && rb->initWithSpriteFrameName(frameName))
	{
		rb->initBullet();
		rb->initBody();
		rb->autorelease();
		return rb;
	}
	else
	{
		CC_SAFE_DELETE(rb);
		return nullptr;
	}
}

void EneRebounceBullet::initBody()
{
	PhysicsMaterial pm(0.0,1.0,0.5);
	auto body = PhysicsBody::createCircle(getContentSize().width*0.5,pm);
	body->setGravityEnable(true);
	body->setLinearDamping(0.8f);
	setPhysicsBody(body);
	setBitmask(
		ENEMY_AMMO2_CATEGORYBITMASK,	
		ENEMY_AMMO2_COLLISIONBITMASK,	
		ENEMY_AMMO2_CONTACTTESTBITMASK);	
}

void EneRebounceBullet::OtherDealWhenContact( const BasePro& rs )
{
	// 閿熸枻鎷蜂竴閿熸枻鎷锋挒閿熸枻鎷烽敓闃惰鎷烽敓鏂ゆ嫹閿熸枻鎷风粺閿熸枻�?
	if ( rs.getMyId() == UnitId::eBottomWall )
	{
		setDeadType(DeadType::Dead_Force);
		m_isDead = true;
	}
}

//-----------------------------
BulletToTarget* BulletToTarget::create(const std::string& frameName, bool isSelf /*= true*/)
{
	BulletToTarget* rb = new (std::nothrow) BulletToTarget;
	if (rb && rb->initWithSpriteFrameName(frameName))
	{
		rb->initBullet();
		rb->m_isSelf = isSelf;
		rb->initBody();
		rb->autorelease();
		return rb;
	}
	else
	{
		CC_SAFE_DELETE(rb);
		return nullptr;
	}
}

void BulletToTarget::initBody()
{
	auto body = PhysicsBody::createBox(getContentSize());
	body->setGravityEnable(false);
	setPhysicsBody(body);
	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
	m_BasePro.setHurts(Ene_Blt_hurt*10);
}

void BulletToTarget::FixAngleOfSelf()
{
	auto PlVec = GameData::getInstance()->getEnemyPlaneVec();
	if (PlVec.empty())
	{
		return;
	}
	else
	{
		float wpRot = m_weapon->getRotation();
		if (wpRot < 0)
		{
			//瀵婚敓鏂ゆ嫹閿熸枻鎷峰彥閿熶茎鍖℃嫹�?
			FindAmiOfSelf(-1);
		}
		else
		{
			//瀵婚敓鏂ゆ嫹閿熸彮杈圭鎷风洰閿熸枻�?
			FindAmiOfSelf(1);
		}
	}
	
}

void BulletToTarget::onEnter()
{
	Bullet::onEnter();
	if (m_isSelf)
	{
		FixAngleOfSelf();
	}
	else
	{
		FixAngleToMainplane();
	}
}

void BulletToTarget::FindAmiOfSelf( int direct )
{
	bool isFound = false;
	Vec2 amiPos = Vec2(360 + direct * 360, 640);
	float tempdis = 100000;
	auto mypos = convertToWorldSpaceAR(Vec2::ZERO);
	auto PlVec = GameData::getInstance()->getEnemyPlaneVec();
	for (auto it = PlVec.begin(); it != PlVec.end(); it++)
	{
		PhysicsUnit* pu = (*it);
		auto pos = pu->convertToWorldSpaceAR(Vec2::ZERO);
		float dis = pos.distance(mypos);
		if (direct < 0)
		{
			if (pos.x > mypos.x)
			{
				continue;
			}
		}
		else
		{
			if (pos.x < mypos.x)
			{
				continue;
			}
		}
		if (dis < tempdis)
		{
			isFound = true;
			amiPos = pos;
			tempdis = dis;
		}
	}
	if (isFound)
	{
		auto myPos = convertToWorldSpaceAR(Vec2::ZERO);
        Vec2 newVec = amiPos - myPos;
		float angle = newVec.getAngle() * Pi2Angle;

		getPhysicsBody()->setRotation(90-angle);
		newVec.normalize();
		_physicsBody->setVelocity(newVec * m_FlySpeed);
	}
}

void BulletToTarget::FixAngleToMainplane()
{
	auto mainPl = GameData::getInstance()->getMainPlane();
	if (!mainPl)
	{
		return;
	}
	auto mainplPos = mainPl->convertToWorldSpaceAR(Vec2::ZERO);
	auto myPos = convertToWorldSpaceAR(Vec2::ZERO);

	Vec2 newVec = mainplPos - myPos;
	newVec.normalize();
	_physicsBody->setVelocity(newVec * m_FlySpeed);
}

//--------------------------------------------
BulletSurroundMainPl* BulletSurroundMainPl::create(int lvl)
{
	BulletSurroundMainPl* bullet = new (std::nothrow) BulletSurroundMainPl;
	if (bullet && bullet->init())
	{
		bullet->initBullet();
		bullet->autorelease();
		bullet->initBody();
		bullet->m_Level = lvl;
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void BulletSurroundMainPl::onEnter()
{
	Bullet::onEnter();

	ListeningStopWeapon();

	m_curBodyRadius = 10.0f;
	float DoublePI = 2 * PI;

	switch (m_Level)
	{
	case 1:
		{
			m_radius = 200;
			addSurroundBullet(0,false);
			addSurroundBullet(DoublePI * 1/3,false);
			addSurroundBullet(DoublePI * 2/3,false);
		}
		break;
	case 2:
		{
			m_radius = 250;
			addSurroundBullet(0,false);
			addSurroundBullet(DoublePI * 1/4,false);
			addSurroundBullet(DoublePI * 2/4,false);
			addSurroundBullet(DoublePI * 3/4,false);
		}
		break;
	case 3:
		{
			m_radius = 300;
			addSurroundBullet(0);
			addSurroundBullet(DoublePI * 1/5);
			addSurroundBullet(DoublePI * 2/5);
			addSurroundBullet(DoublePI * 3/5);
			addSurroundBullet(DoublePI * 4/5);
		}
		break;

	default:
		break;
	}

	ActionInterval* rotate = RotateBy::create(1.0f + rand_0_1()*0.2,600);
	ActionInterval* rep = RepeatForever::create(rotate);
	runAction(rep);

	//涓€閿熸枻鎷锋椂閿熸枻鎷烽敓缂存拝鎷烽敓?
	ActionInterval* delay = DelayTime::create(m_dur);
	CallFunc* func = CallFunc::create(this,callfunc_selector(PhysicsUnit::DeathForce));
	ActionInterval* seq = Sequence::create(delay, func,nullptr);
	runAction(seq);
}

void BulletSurroundMainPl::addSurroundBullet( float angle,bool isflyout /*= true*/ )
{
	//閿熸枻鎷烽敓鏂ゆ嫹閿熼樁顏庢嫹宓岀倲閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鎺ョ鎷烽敓鏂ゆ嫹浣嶉敓鏂ゆ�?
	Vec2 originVec = Vec2(m_radius,0);
	originVec.rotate(Vec2(0,0),angle);

	std::string file = String::createWithFormat("ammo/surround/surround_%d.png",m_Level)->getCString();

	SurroundBullet* bullet = SurroundBullet::create(file,m_dur,isflyout);
	bullet->setPosition(Vec2(0,0));
	bullet->setOffset(originVec);
	bullet->setScale(0.2);
	addChild(bullet);

	ActionInterval* mv = MoveTo::create(1.5f,originVec);
	bullet->runAction(mv);

	ActionInterval* scale = ScaleTo::create(1.0f,1.0);
	bullet->runAction(scale);
}

void BulletSurroundMainPl::setBody(float dt)
{
	if (m_curBodyRadius < m_radius*1.1)
	{
		m_curBodyRadius += 180*dt;
	}
	auto body = PhysicsBody::createCircle(m_curBodyRadius);
	body->setGravityEnable(false);
	body->setRotationEnable(false);
	setPhysicsBody(body);
	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
}

void BulletSurroundMainPl::initBody()
{
	setInvincible(true);
	setInvincibleAtEdge();
	setBody(0.0);
	schedule(schedule_selector(BulletSurroundMainPl::setBody), 0.1);
}

float BulletSurroundMainPl::getDur()
{
	return m_dur;
}

void BulletSurroundMainPl::removeMeO( Ref* obj )
{
	DeathForce();
}


//-----------------------------------------
ThreePetalsBullet* ThreePetalsBullet::create(const std::string& file)
{
	ThreePetalsBullet* bullet = new (std::nothrow) ThreePetalsBullet;
	if (bullet && bullet->initWithFile(file))
	{
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void ThreePetalsBullet::initBody()
{
	auto body = PhysicsBody::createCircle(getContentSize().width*0.7);
	body->setGravityEnable(false);
	setPhysicsBody(body);
	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
	setInvincible(true);
	setDelayRemoveTime(2.0f);
}

void ThreePetalsBullet::onEnter()
{
	Bullet::onEnter();

	auto myPos = convertToWorldSpaceAR(Vec2::ZERO);

	//閿熸枻鎷烽敓鏂ゆ嫹閿熺嫛璁规嫹閿熸枻鎷烽敓鏂ゆ嫹骞曢敓鏂ゆ嫹
	ActionInterval* mv = MoveTo::create(3.0f,Vec2(myPos.x, m_WinSize.height * 1.5));
	ActionInterval* ease = CCEaseSineIn::create(mv);
	runAction(ease);

	ActionInterval* rotate = RotateBy::create(3.0f, 1080);
	runAction(rotate);

	addParticle();
}

void ThreePetalsBullet::addParticle()
{
	auto pos = Vec2(getContentSize().width*0.5, getContentSize().height*0.5);
	EffectsManager::addParticle("particla/Petals/Lvl_1/flower_lvl_1_1.plist",this,pos,false);
	EffectsManager::addParticle("particla/Petals/Lvl_1/flower_lvl_1_2.plist",this,pos,false);
}

void ThreePetalsBullet::DoSomethingBeforeDead()
{
	//log("");
}


//--------------------------------------------
DartBulletRebounce* DartBulletRebounce::create( const std::string& frameName, int direct )
{
	DartBulletRebounce* bullet = new (std::nothrow) DartBulletRebounce;
	if (bullet && bullet->initWithSpriteFrameName(frameName))
	{
		bullet->initBullet();
		bullet->autorelease();
		bullet->initBody();
		bullet->m_direct = direct;
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void DartBulletRebounce::onEnter()
{
	Bullet::onEnter();
	setInvincible(true); 
	PlayAction();
	fixAngle();
}

void DartBulletRebounce::initBody()
{
	PhysicsBody* body = PhysicsBody::createCircle(getContentSize().width*0.5);
	body->setGravityEnable(false);
	body->setVelocity(Vec2(0,10));
	setPhysicsBody(body);

	setBitmask(
		HOODLE_CATEGORYBITMASK,
		HOODLE_COLLISIONBITMASK_P,
		HOODLE_CONTACTTESTBITMASK);
	m_BasePro.setHurts(-1000);
	setMaxChance(2);
}

void DartBulletRebounce::PlayAction()
{
	//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷疯�?
	ActionInterval* rotate = RotateBy::create(1.0f,-2000);
	ActionInterval* rep = RepeatForever::create(rotate);
	runAction(rep);
}

void DartBulletRebounce::fixAngle()
{
	auto curV = _physicsBody->getVelocity();

	//閿熸枻鎷疯浆涓€閿熸枻鎷烽敓瑙掕鎷?
	float r = (m_direct * 30 + 90) * Angle2pi;
	curV = curV.forAngle(r);
  	curV.normalize();
	
	_physicsBody->setVelocity(curV * m_FlySpeed);
}

void DartBulletRebounce::OtherDealWhenContact( const BasePro& rs )
{
	if (rs.getMyId() == UnitId::eLeftWall || rs.getMyId() == UnitId::eRightWall || rs.getMyId() == UnitId::eUpWall)
	{
		setBitmask(
			SELF_AMMO_CATEGORYBITMASK,
			SELF_AMMO_COLLISIONBITMASK_P,
			SELF_AMMO_CONTACTTESTBITMASK);
	}
}

//-------------------------------------------------------
DartBulletSmall* DartBulletSmall::create( const std::string& framename)
{
	DartBulletSmall* bullet = new (std::nothrow) DartBulletSmall;
	if (bullet && bullet->initWithSpriteFrameName(framename))
	{
		bullet->initBullet();
		bullet->autorelease();
		bullet->initBody();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void DartBulletSmall::initBody()
{
	PhysicsBody* body = PhysicsBody::createCircle(getContentSize().width*0.5);
	body->setGravityEnable(false);
	setPhysicsBody(body);

	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
	setInvincible(true);
}

void DartBulletSmall::onEnter()
{
	Bullet::onEnter();

	ActionInterval* rotate = RotateBy::create(0.5f,-1080);
	ActionInterval* rep = RepeatForever::create(rotate);
	runAction(rep);
}

//----------------------------
DartBulletTrace* DartBulletTrace::create( const std::string& framename )
{
	DartBulletTrace* bullet = new (std::nothrow) DartBulletTrace;
	if (bullet && bullet->initWithSpriteFrameName(framename))
	{
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void DartBulletTrace::initBody()
{
	auto s = getContentSize();
	PhysicsBody* body = PhysicsBody::createCircle(getContentSize().width*0.5 * getScale());
	body->setGravityEnable(false);
	setPhysicsBody(body);

	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
	setInvincible(true);
	setInvincibleAtEdge();
}

void DartBulletTrace::onEnter()
{
	Bullet::onEnter();
	setScale(3.0f);
	schedule(schedule_selector(DartBulletTrace::UpdatePos));
	schedule(schedule_selector(DartBulletTrace::Hurt),0.1);

	{
		ActionInterval* delay = DelayTime::create(5.0f);
		CallFunc* func = CallFunc::create(this,callfunc_selector(PhysicsUnit::DeathForce));
		ActionInterval* seq = Sequence::create(delay,FadeOut::create(0.3), func, nullptr);
		runAction(seq);
	}

	{
		ActionInterval* rotate = RotateBy::create(1.5f,-1080);
		ActionInterval* rep = RepeatForever::create(rotate);
		runAction(rep);
	}
	{
		auto delay = DelayTime::create(8.0f);
		auto fadeout = FadeOut::create(2.0f);
		auto seq = Sequence::create(delay, fadeout, nullptr);
		runAction(seq);
	}
	addParticle();
}

void DartBulletTrace::UpdatePos( float dt )
{
	auto enemyList = GameData::getInstance()->getEnemyPlaneVec();
	if (enemyList.empty())
	{
		//log("no energy");
	}
	else
	{
//		PhysicsUnit* pl = GameData::getInstance()->getEnemyPlaneByMaxHp();
		Vec2 myPos = this->getPhysicsBody()->getPosition();
		BasePlane* closePl = EnemyPlaneController::getInstance()->getTheClosePlane(myPos);
		Vec2 AimPos;
		if (closePl)
		{
			AimPos = closePl->getPhysicsBody()->getPosition();
		}
		else
		{
			AimPos = myPos;
		}

		AimPos.x = AimPos.x + rand_minus1_1() * 100;
		AimPos.y = AimPos.y + rand_minus1_1() * 100;

		Vec2 movePos = MoveStep(AimPos,myPos,8);
		this->setPosition(movePos);
	}

//	fixPos();
}

void DartBulletTrace::fixPos()
{
	Vec2 myPos = this->getPosition();

	if (myPos.x >= m_WinSize.width - 50)
	{
		myPos.x = m_WinSize.width - 50;
	}
	else if(myPos.x <= 50)
	{
		myPos.x = 50;
	}
	if (myPos.y >= m_WinSize.height - 100)
	{
		myPos.y = m_WinSize.height - 100;
	}
	else if(myPos.y <= 50)
	{
		myPos.y = 50;
	}
	this->setPosition(myPos);
}

void DartBulletTrace::Hurt( float dt )
{
	initBody();
}

void DartBulletTrace::addParticle()
{
	auto size = getContentSize();
	auto pos = Vec2(size.width * 0.5, size.height *0.5);
	EffectsManager::addParticle("particla/Huixuanbiao/Huixuanbiao.plist",this,pos,false);
	auto partc = EffectsManager::addParticle("particla/Huixuanbiao/Huixuanbiao.plist",this,pos,false);
	if (partc)
	{
		partc->setPositionType(ParticleSystem::PositionType::GROUPED);
	}
}

//-------------------------------------
SurroundBullet* SurroundBullet::create(const std::string& file, float dur, bool isflyout /*= false*/)
{
	SurroundBullet* bullet = new (std::nothrow) SurroundBullet;
	if (bullet && bullet->initWithFile(file))
	{
		bullet->setDur(dur);
		bullet->autorelease();
		bullet->m_isFlyOut = isflyout;
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void SurroundBullet::onEnter()
{
	Sprite::onEnter();
	//addParticle();
	FixRotate();

	if (!m_isFlyOut)
	{
		//閿熸枻鎷烽敓鏂ゆ�?
		ActionInterval* delay = DelayTime::create(m_dur - 2.0f);
		ActionInterval* fadeout = FadeOut::create(2.0f);
		CallFunc* func = CallFunc::create(this,callfunc_selector(SurroundBullet::removeMe));
		ActionInterval* seq = Sequence::create(delay, fadeout, func, nullptr);
		runAction(seq);
	}
	else
	{
		//閿熺即绛规嫹鍘婚敓鏂ゆ嫹閿熸枻鎷?
		auto delay = DelayTime::create(m_dur - 0.5);
		auto func = CallFunc::create(this, callfunc_selector(SurroundBullet::FlyOut));
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}
}

void SurroundBullet::onExit()
{
	Sprite::onExit();

	
}

void SurroundBullet::FlyOut()
{
	auto parent = getParent();
	auto parentPos = parent->convertToWorldSpaceAR(Vec2::ZERO);
	auto mypos = convertToWorldSpaceAR(Vec2::ZERO);

	Vec2 newVec = mypos - parentPos;
	newVec.rotate(Vec2(0,0),-90 * Angle2pi);
	newVec.normalize();
	newVec *= 1000;

	PhysicsUnit* circle = (PhysicsUnit*)parent;

	//閿熻姤鎹㈤敓鏂ゆ嫹鍘熼敓鏂ゆ嫹閿熸枻�?
	FlyOutBullet* bullet = FlyOutBullet::create();
	bullet->setPosition(mypos);
	bullet->resetHurt(circle->getBasePro().getHurts());
	bullet->setBelongTo(UnitId::eRedCard);
	bullet->getPhysicsBody()->setVelocity(newVec);
	UIController::getInstance()->getPhysicsLayer()->addChild(bullet);

	auto vec = newVec;
	auto angle = vec.getAngle() * Pi2Angle;
	bullet->setRotation(-angle + 180);

	removeMe();
}

void SurroundBullet::FixRotate()
{
	auto vec = m_offset;
	vec.rotate(Vec2(0,0),90);
	auto angle = vec.getAngle() * Pi2Angle;
	setRotation(-angle + 28);
}

void SurroundBullet::setOffset( const Vec2& pos )
{
	m_offset = pos;
}

void SurroundBullet::removeMe()
{
	removeFromParentAndCleanup(true);
}

void SurroundBullet::setDur( float t )
{
	m_dur = t;
}

//-------------------------------
FlyOutBullet* FlyOutBullet::create()
{
	FlyOutBullet* bullet = new (std::nothrow) FlyOutBullet;
	if (bullet && bullet->initWithFile("ammo/surround/surround_3_1.png"))
	{
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void FlyOutBullet::initBody()
{
	auto body = PhysicsBody::createBox(getContentSize());
	body->setGravityEnable(false);
	body->setRotationEnable(false);
	setPhysicsBody(body);
	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
	m_BasePro.setHurts(Ene_Blt_hurt*10);
	setInvincible(true);
}

//---------------------------------------------------------------------
AirazorBullet* AirazorBullet::create( const std::string& file, float r, bool isPierce )
{
	AirazorBullet* bullet = new (std::nothrow) AirazorBullet;
	if (bullet && bullet->initWithFile(file))
	{
		bullet->m_isPierce = isPierce;
		bullet->m_rotate = r;
		bullet->initBullet();
		bullet->autorelease();
		bullet->initBody();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void AirazorBullet::onEnter()
{
	Bullet::onEnter();
	appear();
}

void AirazorBullet::initBody()
{
	auto body = PhysicsBody::createCircle(getContentSize().width*0.5);
	body->setGravityEnable(false);
	body->setMass(0.00001);
	setPhysicsBody(body);
	m_BasePro.setHurts(Ene_Blt_hurt * 10);
	if (m_isPierce)
	{
		setInvincible(true);
		setDelayRemoveTime(1.0f);
		setBitmask(
			SELF_AMMO_CATEGORYBITMASK,
			SELF_AMMO_COLLISIONBITMASK_P,
			SELF_AMMO_CONTACTTESTBITMASK);
	}
	else
	{
		setBitmask(
			SELF_AMMO_CATEGORYBITMASK,
			SELF_AMMO_COLLISIONBITMASK,
			SELF_AMMO_CONTACTTESTBITMASK);
	}
}

void AirazorBullet::appear()
{
	float radius = 100;
	Vec2 originVec = Vec2(0, -radius);
	originVec.rotate(Vec2::ZERO, m_rotate * Angle2pi );

	setOpacity(0);
	ActionInterval* mv = MoveBy::create(0.5f, originVec);
	ActionInterval* fadein = FadeIn::create(0.5f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(AirazorBullet::FixAngle));
	ActionInterval* seq = Sequence::create(mv,func, nullptr);
	runAction(seq);
	runAction(fadein);
}

void AirazorBullet::FixAngle()
{
	Vec2 targetPos;
	auto mainPl = GameData::getInstance()->getMainPlane();
	auto mypos = Vec2(360,0);
	if(mainPl) {
		mypos = mainPl->getPhysicsBody()->getPosition();
	}

	if(GameData::getInstance()->getEnemyPlaneVec().empty())
	{
		targetPos = Vec2(m_WinSize.width*0.5, m_WinSize.height);
	}
	else
	{
		auto pl = EnemyPlaneController::getInstance()->getTheClosePlane(mypos);
		if (!pl)
		{
			targetPos = Vec2(m_WinSize.width*0.5, m_WinSize.height);
		}
		else
		{
			targetPos = pl->getPhysicsBody()->getPosition();
		}
		if (targetPos.y < 0 || targetPos.x < 0 || targetPos.x > m_WinSize.width)
		{
			targetPos = Vec2(m_WinSize.width*0.5, m_WinSize.height);
		}
	}

	mypos = this->getPhysicsBody()->getPosition();
	auto newVec = targetPos - mypos;
	m_flyDirect = newVec;
	float angle = newVec.getAngle() * Pi2Angle - 90;

	if (angle > 180)
	{
		angle = angle - 360;
	}
	ActionInterval* rt = RotateBy::create(0.5f,-angle);
	CallFunc* func = CallFunc::create(this,callfunc_selector(AirazorBullet::FlyOut));
	ActionInterval* seq = Sequence::create(rt, func, nullptr);
	runAction(seq);
}

void AirazorBullet::FlyOut()
{
	m_flyDirect.normalize();
	_physicsBody->setVelocity(m_flyDirect * 1500);
	if (m_isPierce)
	{
		AddTail();
	}
}

void AirazorBullet::AddTail()
{
	auto s = getContentSize();
	EffectsManager::addParticle("particla/SwordTail/Sword1.plist",this,Vec2(s.width*0.5,0),false);
	EffectsManager::addParticle("particla/SwordTail/Sword2.plist",this,Vec2(s.width*0.5,0),false);
}

void AirazorBullet::DoSomethingBeforeDead()
{
	
}


//---------------------------------------
DrawLighting::DrawLighting()
{

}

DrawLighting* DrawLighting::create( const Vec2& startPos,const Vec2& endPos )
{
	DrawLighting* lighting = new (std::nothrow) DrawLighting;
	if (lighting && lighting->init())
	{
		lighting->autorelease();
		lighting->m_startPos = startPos;
		lighting->m_endPos = endPos;
		return lighting;
	}
	else
	{
		CC_SAFE_DELETE(lighting);
		return nullptr;
	}
}

void DrawLighting::onEnter()
{
	Node::onEnter();
	setPosition(m_startPos);
	RefreshPicture();
}

void DrawLighting::RefreshPicture()
{
	int num1 = random(1,6);
	std::string file = CCString::createWithFormat("ammo/lighting/longlighting_%d.png",num1)->getCString();
	m_Sp = Sprite::create(file);
	m_Sp->setAnchorPoint(Vec2(0.5,0));
	addChild(m_Sp);

	auto newVec = m_endPos - m_startPos;
	float angle = newVec.getAngle() * Pi2Angle;
	setRotation(-angle + 90);

	//閿熸枻鎷疯彞閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽�?
	auto s = m_Sp->getContentSize();
	auto dis = m_startPos.distance(m_endPos);
	float ratio = dis/s.height;
	if (ratio <= 1.0)
	{
		Rect rect = Rect(0,0, s.width, s.height * ratio);
		m_Sp->setTextureRect(rect);
	}
	else
	{
		setScaleY(ratio);
	}
	

	ActionInterval* delay = DelayTime::create(1.0f/24);
	CallFunc* func = CallFunc::create(this,callfunc_selector(DrawLighting::removeMe));
	ActionInterval* seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void DrawLighting::removeMe()
{
	removeFromParentAndCleanup(true);
}


//-=------------------------
NewLightingLayer::NewLightingLayer()
{
	auto s = Director::getInstance()->getWinSize();

	m_EdgeL = 50.0f;
	m_EdgeR = s.width - 50.0f;
	m_EdgeD = 50.0f;
	m_EdgeU = s.height - 50.0f;
}

NewLightingLayer* NewLightingLayer::create( int num, int hurt )
{
	NewLightingLayer* layer = new (std::nothrow) NewLightingLayer;
	if (layer && layer->init())
	{
		layer->m_lightingNums = num;
		layer->setHurt(hurt);
		layer->autorelease();
		return layer;
	}
	else
	{
		CC_SAFE_DELETE(layer);
		return nullptr;
	}
}

void NewLightingLayer::onEnter()
{
	Layer::onEnter();
	NotificationCenter::getInstance()->addObserver(this,callfuncO_selector(NewLightingLayer::removeMeO),
		Msg_StopMainPlBullet,nullptr);
	StartLighting();
}

void NewLightingLayer::onExit()
{
	Layer::onExit();
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void NewLightingLayer::FindAllTargets()
{
	m_targetList.clear();
	m_posList.clear();

	//閿熼ズ杈炬嫹閿熸枻鎷烽敓鏂ゆ嫹閿熺即浼欐嫹閿熸枻鎷烽敓鏂ゆ嫹
	auto mainpl = GameData::getInstance()->getMainPlane();
	auto curpos = Vec2(0,0);

	if (mainpl)
	{
		curpos = mainpl->convertToWorldSpaceAR(Vec2::ZERO);
		m_posList.push_back(curpos);
	}
	else
	{
		//鍒犻敓鏂ゆ嫹
		removeFromParentAndCleanup(true);
		return;
	}

	auto plVec = GameData::getInstance()->getEnemyPlaneVec();

	//閿熸枻鎷烽敓鍙鎷烽敓瑙掑嚖鎷烽敓鏂ゆ嫹BOSS
	for (auto it = plVec.begin(); it != plVec.end(); it++)
	{
		auto pl = (*it);
		auto plidx = pl->getBasePro().getMyId();
		if (plidx == UnitId::ePlaneBoss )
		{
			curpos = pl->convertToWorldSpaceAR(Vec2::ZERO);
			bool isInView = isInScreen(curpos);//閿熻鍑ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻�?
			if (!isInView)
			{
				break;
			}
			curpos.y -= 60;
			m_posList.push_back(curpos);
			m_targetList.push_back(pl);
			break;
		}
	}

	//閿熸枻鎷烽敓鏂ゆ嫹閫氱洰閿熸枻鎷?
	int findTimes =  plVec.size() - m_posList.size() + 1;
	for (int i = 0; i < findTimes; i++)
	{
		if (m_posList.size() > m_lightingNums )
		{
			break;
		}
		bool isfind = FindTarget(curpos);	//娌￠敓鎻鎷风洰閿熺枼锛岄敓鏂ゆ嫹涔堥敓鏂ゆ嫹閿熻妭纭锋嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹瀵婚敓鏂ゆ嫹
		if (!isfind)
		{
			break;;
		}
	}

	//閿熸枻鎷烽敓鍙鎷蜂箣閿熸枻鎷烽敓鏂ゆ嫹閿熸枻�?
	DrawLight();
}

bool NewLightingLayer::FindTarget( Vec2& startPos )
{
	auto plVec = GameData::getInstance()->getEnemyPlaneVec();
	PhysicsUnit* TargetPl  = nullptr;
	Vec2 targetPos = Vec2(0,0);

	//閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽€氶敓鍙紮�?
	float dis = 100000.0f;
	bool isSuc = false;
	for (auto it = plVec.begin(); it != plVec.end(); it++)
	{
		auto pl = (*it);
		if (pl)
		{
			bool isfound = isFound(pl);		//閿熻鍑ゆ嫹閿熺獤鎾呮嫹閿熻鎲嬫嫹閿熸彮纰夋嫹閿熸枻鎷风洰閿熸枻鎷?
			if (isfound)
			{
				continue;
			}
			Vec2 pos = pl->convertToWorldSpaceAR(Vec2::ZERO);
			bool isInView = isInScreen(pos);//閿熻鍑ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻�?
			if (!isInView)
			{
				continue;
			}
			float TempDis = pos.distance(startPos);
			if (TempDis < dis)
			{
				isSuc = true;
				dis = TempDis;
				targetPos = pos;
				TargetPl = pl;
			}
		}
	}

	//閿熸枻鎷峰綍閿熸彮纰夋嫹閿熶茎鏁屼紮�?
	if(isSuc)
	{
		startPos = targetPos;
		m_posList.push_back(targetPos);
		m_targetList.push_back(TargetPl);
	}

	return isSuc;
}

bool NewLightingLayer::isFound( PhysicsUnit* target )
{
	bool b = false;
	for (int i = 0; i < m_targetList.size(); i++)
	{
		auto pu = m_targetList.at(i);
		if (pu == target)
		{
			b = true;
			break;
		}
	}

	return b;
}

void NewLightingLayer::DrawLight()
{
	if (m_posList.empty())
	{
		return;
	}
	for (int i = 0; i < m_posList.size() - 1; i++)
	{
		Vec2 startPos = m_posList.at(i);
		Vec2 endPos = m_posList.at(i+1);
		DrawLighting* light = DrawLighting::create(startPos, endPos);
		addChild(light);
	}
	for (int i = 1; i < m_posList.size(); i++)
	{
		Vec2 pos = m_posList.at(i);
		LightingAttackPoint* attack = LightingAttackPoint::create();
		attack->getBasePro().setMyId(UnitId::eLightAttackPoint);
		attack->setPosition(pos);
		attack->setBelongTo(UnitId::eYellowCard);	//閿熸枻鎷烽敓鏂ゆ嫹涓洪敓鏂ゆ嫹閿熻妭浼欐嫹閿熸枻�?
		attack->resetHurt(m_hurt);
		addChild(attack);
	}
}

void NewLightingLayer::StartLighting()
{
	{
		ActionInterval* interval = DelayTime::create(1.0f/24);
		CallFunc* func = CallFunc::create(this,callfunc_selector(NewLightingLayer::FindAllTargets));
		ActionInterval* seq = Sequence::create(func, interval, nullptr);
		ActionInterval* rep = RepeatForever::create(seq);
		runAction(rep);
	}
}

void NewLightingLayer::removeMeO(Ref* obj)
{
	removeFromParentAndCleanup(true);
}

bool NewLightingLayer::isInScreen( const Vec2& pos )
{
	bool b = false; 
	if (pos.x > m_EdgeL && pos.x < m_EdgeR &&
		pos.y > m_EdgeD && pos.y < m_EdgeU)
	{
		b = true;
	}

	return b;
}

void NewLightingLayer::setHurt( int hurt )
{
	m_hurt = hurt;
}

//-------------------------------
LightingAttackPoint* LightingAttackPoint::create()
{
	LightingAttackPoint* point = new (std::nothrow) LightingAttackPoint;
	if (point && point->init())
	{
		point->initBullet();
		point->initBody();
		point->autorelease();
		return point;
	}
	else
	{
		CC_SAFE_DELETE(point);
		return nullptr;
	}
}

void LightingAttackPoint::initBody()
{
	auto body = PhysicsBody::createCircle(20);
	body->setGravityEnable(false);
	setPhysicsBody(body);
	setUnitId(UnitId::eBullet_38);

	setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
	setInvincibleAtEdge();
}

void LightingAttackPoint::onEnter()
{
	Bullet::onEnter();

	ActionInterval* delay = DelayTime::create(0.5f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(PhysicsUnit::DeathForce));
	ActionInterval* seq = Sequence::create(delay,func,nullptr);
	runAction(seq);
}

//---------------------------------
ExplosionBullet* ExplosionBullet::create( float angle)
{
	ExplosionBullet* bullet = new (std::nothrow) ExplosionBullet;
	if (bullet && bullet->initWithFile("animation/ExplosionBullet/FlyState.png"))
	{
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();
		bullet->m_angle = angle;
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void ExplosionBullet::initBody()
{
	PhysicsBody* body = PhysicsBody::createBox(getContentSize());
	body->setLinearDamping(5.0);
	body->setGravityEnable(false);
	setPhysicsBody(body);

	setBitmask(ENEMY_AMMO_CATEGORYBITMASK,
			0,
			ENEMY_AMMO_CONTACTTESTBITMASK);
	setInvincible(true);
	setBelongTo(UnitId::ePlaneBoss);
}

void ExplosionBullet::onEnter()
{
	Bullet::onEnter();
	FixBulletAngle();
	ExplAction();
}

void ExplosionBullet::Explosion()
{
	setVisible(false);

	NewAudio::getInstance()->playEffect(Music_Boss1Shoot1);
	auto s = getContentSize();

	for (int i = 0; i < 8; i++)
	{
		Weapon* wp = Weapon::create(this);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setBltResTag(ResTag::Bullet_BossFireAmmo);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setShootTimes(3);
		wp->setBltHurts(Ene_Blt_hurt);
		wp->setBltSpeed(500);
		wp->setBltScaleX(0.65);
		wp->setBltScaleY(0.65);
		wp->setBltLinearDamping(0.2);
		wp->setRotation(i * 360.0f/8 );
		wp->setOffset(Vec2(s.width*0.5, s.height*0.5));
		wp->StartShoot();
		addChild(wp);
		wp->runAction(RotateBy::create(0.45,20));
	}

	//閿熸暀浼欐�?
	/*for (int i = 0; i < 8; i++)
	{
		Weapon* wp = Weapon::create(m_target);
		wp->setBullstType(BulletType::Enemy_BossDart);
		wp->ChangePro(Ene_Blt_hurt,10.f,0.3f);	
		wp->setBltSpeed(500);
		wp->setRotation(i * 360.0f/8);
		wp->setScale(0.1f);
		wp->setOffset(Vec2(s.width*0.5, s.height*0.5));
		wp->StartShoot();
		addChild(wp);
	}*/

	//N閿熸枻鎷烽敓缂存拝鎷烽敓鏂ゆ嫹绾﹂敓?
	ActionInterval* delay = DelayTime::create(10.5f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(PhysicsUnit::DeathForce));
	ActionInterval* seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void ExplosionBullet::FixBulletAngle()
{
	auto originVec = Vec2(0,-1);
	originVec.rotate(Vec2(0,0), -m_angle * DEG_RAD);
	_physicsBody->setVelocity(originVec * 2000);
	getPhysicsBody()->setRotation(180 + m_angle);
}

void ExplosionBullet::ExplAction()
{
	setScaleX(0.25);

	auto layer = UIController::getInstance()->getEffectsLayer();

	auto scale = ScaleTo::create(0.5f,1.0f);
	auto animat1 = layer->getFramesAnimation("ExplBult_1_", 11, 1.0/60);
	auto animat2 = layer->getFramesAnimation("ExplBult_2_", 6, 1.0/60);
	auto func = CallFunc::create(this, callfunc_selector(ExplosionBullet::Explosion));
	auto seq = Sequence::create(scale, animat1, animat2, func, nullptr);
	runAction(seq);
}

//---------------------------------
DartOfBoss3* DartOfBoss3::create()
{
	DartOfBoss3* dart = new (std::nothrow) DartOfBoss3;
	auto file = GameData::getInstance()->getBulletFactory()->getResByTag(ResTag::Bullet_BossBigDart);
	if (dart && dart->initWithSpriteFrameName(file))
	{
		dart->initBody();
		dart->autorelease();
		return dart;
	}
	else
	{
		CC_SAFE_DELETE(dart);
		return nullptr;
	}
}

void DartOfBoss3::initBody()
{
	setInvincible(true);

	PhysicsBody* body = PhysicsBody::createCircle(getContentSize().width * 0.5);
	body->setGravityEnable(false);
	body->setCategoryBitmask(ENEMY_AMMO_CATEGORYBITMASK);
	body->setCollisionBitmask(0);
	body->setContactTestBitmask(ENEMY_AMMO_CONTACTTESTBITMASK);
	setPhysicsBody(body);

	schedule(schedule_selector(DartOfBoss3::setBody),0.1);

}

void DartOfBoss3::onEnter()
{
	PhysicsUnit::onEnter();
	addParticle();
}

void DartOfBoss3::setBody(float dt)
{
	PhysicsBody* body = PhysicsBody::createCircle(getContentSize().width * 0.5);
	body->setGravityEnable(false);
	body->setCategoryBitmask(ENEMY_AMMO_CATEGORYBITMASK);
	body->setCollisionBitmask(0);
	body->setContactTestBitmask(ENEMY_AMMO_CONTACTTESTBITMASK);
	setPhysicsBody(body);
}

void DartOfBoss3::addParticle()
{
	auto s = getContentSize();
	auto partc = EffectsManager::addParticle("particla/Huixuanbiao/Huixuanbiao_1.plist",this,Vec2(s.width * 0.5, s.height*0.5),false);
	if (partc)
	{
		partc->setScale(2.5);
	}
}

//----------------------------------
Bullet1OfBoss4* Bullet1OfBoss4::create(ResTag res, float stayTime)
{
	Bullet1OfBoss4* bullet = new (std::nothrow) Bullet1OfBoss4;
	auto file = GameData::getInstance()->getBulletFactory()->getResByTag(res);
	if (bullet && bullet->initWithSpriteFrameName(file))
	{
		bullet->m_StayTime = stayTime;
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void Bullet1OfBoss4::FloatUpDown()
{
	setOpacity(0);
	auto fadein = FadeIn::create(0.5 + rand_0_1() * 0.2);
	auto mvUp = MoveBy::create(1.5f + rand_0_1() * 0.5, Vec2(0,50));
	auto ease = EaseInOut::create(mvUp, 0.8);
	auto mvDown = ease->reverse();
	auto seq = Sequence::create(fadein, ease, mvDown, nullptr);
	auto rep = RepeatForever::create(seq);
	runAction(rep);
}

void Bullet1OfBoss4::initBody()
{
	PhysicsBody* body = PhysicsBody::createCircle(getContentSize().width * 0.5);
	body->setGravityEnable(false);
	body->setLinearDamping(0.3);
	body->setMass(0.1);
	setPhysicsBody(body);
	setBitmask(ENEMY_AMMO_CATEGORYBITMASK, 0, ENEMY_AMMO_CONTACTTESTBITMASK);
	m_BasePro.setHurts(Ene_Blt_hurt);

}

void Bullet1OfBoss4::onEnter()
{
	Bullet::onEnter();
	FloatUpDown();

	BreathAction(this);

	ActionInterval* delay = DelayTime::create(m_StayTime /*+ rand_0_1()*/);
	CallFunc* func = CallFunc::create(this, callfunc_selector(Bullet1OfBoss4::Start));
	//CallFunc* func = CallFunc::create(this, callfunc_selector(Bullet1OfBoss4::MoveToPl));
	ActionInterval* seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void Bullet1OfBoss4::Helix( float dt )
{
	//閿熺煫纰夋嫹閿熸枻鎷峰墠閿熸枻鎷烽敓鏂ゆ嫹閿熷姭搴︼綇鎷烽敓鏂ゆ嫹寮忛敓鏂ゆ�?
	Vec2 curV = _physicsBody->getVelocity();

	//閿熸彮纰夋嫹閿熷姭璁规嫹閿熸枻鎷烽敓鏂ゆ嫹閿熶茎鍑ゆ嫹閿熺锝忔嫹閿熸枻鎷烽敓鑴氳鍑ゆ嫹閿熸枻鎷烽敓鎻紮鎷烽敓鏂ゆ嫹�?閿熸枻鎷烽敓鏂ゆ嫹涓€閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋�?
	Vec2 faxian = curV.getPerp();
	faxian.normalize();
	_physicsBody->applyImpulse(faxian * 5.0f,Vec2(0,0));
}

void Bullet1OfBoss4::StopHelix()
{
	unschedule(schedule_selector(Bullet1OfBoss4::Helix));
	MoveToPl();
}

void Bullet1OfBoss4::MoveToPl()
{
	stopAllActions();
	_physicsBody->setLinearDamping(0.0);
	auto pl = GameData::getInstance()->getMainPlane();
	Vec2 pos = Vec2(360,-10);
	if (pl)
	{
		pos = pl->convertToWorldSpaceAR(Vec2::ZERO);
	}
	
	auto mypos = convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 newVec = pos - mypos;
	newVec.normalize();
	_physicsBody->setVelocity(newVec * 800);
}

void Bullet1OfBoss4::Start()
{ 
	_physicsBody->setVelocity(Vec2(0,-100));

	auto mv = MoveBy::create(1.5f, Vec2(0, - 200));
	runAction(mv);

	schedule(schedule_selector(Bullet1OfBoss4::Helix));

	ActionInterval* delay = DelayTime::create(1.5f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(Bullet1OfBoss4::StopHelix));
	ActionInterval* seq = Sequence::create(delay,func, nullptr);
	runAction(seq);
}

void Bullet1OfBoss4::BreathAction( Node* node )
{
	float curScale = node->getScale();

	auto scaleX = ScaleTo::create(0.15,0.9 * curScale,curScale);
	auto scaleY = ScaleTo::create(0.15,curScale,curScale * 0.9);
	auto easeX = EaseSineInOut::create(scaleX);
	auto easeY = EaseSineInOut::create(scaleY);

	auto seq = Sequence::create(easeX, easeY, nullptr);
	auto rep = RepeatForever::create(seq);
	node->runAction(rep);

	auto rot = RotateBy::create(1.0f,45);
	auto rep1 = RepeatForever::create(rot);
	node->runAction(rep1);
}

//------------------------------------
Bullet2OfBoss4* Bullet2OfBoss4::create()
{
	Bullet2OfBoss4* bullet = new (std::nothrow) Bullet2OfBoss4;
	auto file = GameData::getInstance()->getBulletFactory()->getResByTag(ResTag::Bullet_BossShuiPaoSmall);
	if (bullet && bullet->initWithSpriteFrameName(file))
	{
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void Bullet2OfBoss4::initBody()
{
	PhysicsBody* body = PhysicsBody::createCircle(getContentSize().width * 0.5);
	body->setGravityEnable(false);
	body->setMass(0.1);
	setPhysicsBody(body);
	setBitmask(ENEMY_AMMO_CATEGORYBITMASK, 0, ENEMY_AMMO_CONTACTTESTBITMASK);
	m_BasePro.setHurts(Ene_Blt_hurt);

}

void Bullet2OfBoss4::onEnter()
{
	Bullet::onEnter();

	setScale(0.5f);
	runAction(ScaleTo::create(1.0f,1.0f));

	MoveBezier();
}

void Bullet2OfBoss4::MoveBezier()
{
	auto pos = convertToWorldSpaceAR(Vec2::ZERO);
	auto endPos = Vec2(m_WinSize.width*0.5, -100);
	auto mainPl = GameData::getInstance()->getMainPlane();
	if (mainPl)
	{
		endPos = mainPl->convertToWorldSpaceAR(Vec2::ZERO);
	}

	//閿熸枻鎷烽敓鍙柇纰夋嫹鍓嶅埛閿熸枻鎷烽敓鍔害鍑ゆ嫹閿熸枻鎷风‘閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽�?
	auto v = _physicsBody->getVelocity();
	_physicsBody->setVelocity(Vec2(0,1));

	ccBezierConfig config;
	if (v.x < 0)
	{
		config.controlPoint_1 = pos;
		config.controlPoint_2 = Vec2(pos.x - 400, pos.y + 250);
		config.endPosition = Vec2(endPos.x + 150, -100);
	}
	else
	{
		config.controlPoint_1 = pos;
		config.controlPoint_2 = Vec2(pos.x + 400, pos.y + 250);
		config.endPosition = Vec2(endPos.x - 150, -100);
	}
	
	auto mv = BezierTo::create(2.0f, config);
	auto func = CallFunc::create(this, callfunc_selector(Bullet2OfBoss4::StartFloat));
	auto seq = Sequence::create(mv, func, nullptr);
	runAction(mv);
}

void Bullet2OfBoss4::StartFloat()
{
	schedule(schedule_selector(Bullet2OfBoss4::newFalling),0.1);
}

void Bullet2OfBoss4::newFalling(float dt)
{
	_physicsBody->applyForce(Vec2(rand_minus1_1()*6.0, -rand_0_1()* 3),Vec2(0,0));
}

//------------------------------------
Bullet3OfBoss4* Bullet3OfBoss4::create()
{
	Bullet3OfBoss4* bullet = new (std::nothrow) Bullet3OfBoss4;
	auto file = GameData::getInstance()->getBulletFactory()->getResByTag(ResTag::Bullet_BossShuiPaoBig);
	if (bullet && bullet->initWithSpriteFrameName(file))
	{
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void Bullet3OfBoss4::initBody()
{
	PhysicsBody* body = PhysicsBody::createCircle(getContentSize().width * 0.5);
	body->setGravityEnable(false);
	body->setLinearDamping(0.3);
	body->setMass(0.1);
	body->setVelocity(Vec2(0,400));
	setPhysicsBody(body);
	setInvincible(true);
	setBitmask(ENEMY_AMMO_CATEGORYBITMASK, 0, ENEMY_AMMO_CONTACTTESTBITMASK);
}

void Bullet3OfBoss4::onEnter()
{
	Bullet::onEnter();
	
	setScale(0.8);
	Bullet1OfBoss4::BreathAction(this);

	setPosition(Vec2(m_WinSize.width*0.5 + rand_minus1_1() * 250, -200));

	auto delay = DelayTime::create(5.0f);
	auto func = CallFunc::create(this, callfunc_selector(Bullet3OfBoss4::expolsion));
	auto delay1 = DelayTime::create(1.5f);
	auto rm = CallFunc::create(this, callfunc_selector(PhysicsUnit::DeathForce));
	auto seq = Sequence::create(delay, func, delay1, rm, nullptr);
	runAction(seq);

	/*auto fadeout = FadeOut::create(7.0);
	runAction(fadeout);

	auto scale = ScaleTo::create(7.0, 2.0);
	runAction(scale);*/
}

void Bullet3OfBoss4::addWp(Node* node, int hurt)
{
	node->setVisible(false);
	auto s = node->getContentSize();
	float randScale = 0.2 * rand_minus1_1();
	for (int i = 0; i < 15; ++i)
	{
		Weapon* wp = Weapon::create(node);
		wp->setBullstType(BulletType::Enemy_default);
		wp->setBltResTag(ResTag::Bullet_BossShuiPaoSmall);
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setBltGravityAble(true);
		wp->setDelayShoot(rand_0_1() * 0.05);
		wp->setBltSpeed(100 + rand_0_1() * 100);
		randScale = 0.2 * rand_minus1_1();
		wp->setBltScaleX(0.8f + randScale);
		wp->setBltScaleY(0.8f + randScale);
		wp->setBltMass(0.01);
		wp->setPosition(Vec2(s.width*0.5, 80));
		wp->setShootTimes(1);
		wp->setBltHurts(hurt);
		wp->StartShoot();
		node->addChild(wp);
		wp->setRotation(360 * rand_0_1());
	}
}

void Bullet3OfBoss4::expolsion()
{
	int hurt = m_BasePro.getHurts();
	addWp(this, hurt);
}

//------------------------------------
Bullet4OfBoss5* Bullet4OfBoss5::create(float dur)
{
	Bullet4OfBoss5* bullet = new (std::nothrow) Bullet4OfBoss5;
	auto file = GameData::getInstance()->getBulletFactory()->getResByTag(ResTag::Bullet_BossShuiPaoBig);
	if (bullet && bullet->initWithSpriteFrameName(file))
	{
		bullet->m_dur = dur;
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void Bullet4OfBoss5::initBody()
{
	PhysicsBody* body = PhysicsBody::createCircle(getContentSize().width * 0.5);
	body->setGravityEnable(false);
	setPhysicsBody(body);
	setInvincible(true);
	setBitmask(ENEMY_AMMO_CATEGORYBITMASK, 0, ENEMY_AMMO_CONTACTTESTBITMASK);
	m_BasePro.setHurts(Ene_Blt_hurt);

}

void Bullet4OfBoss5::onEnter()
{
	Bullet::onEnter();
	setOpacity(0);
	auto fadein = FadeIn::create(1.5 + rand_0_1() * 0.2);
	runAction(fadein);

	setScale(0.8);
	Bullet1OfBoss4::BreathAction(this);

	float t = 5.0;

	auto delay = DelayTime::create(t - 2.0 + m_dur);
	auto func = CallFunc::create(this, callfunc_selector(Bullet3OfBoss4::expolsion));
	auto delay1 = DelayTime::create(1.5f);
	auto rm = CallFunc::create(this, callfunc_selector(PhysicsUnit::DeathForce));
	auto seq = Sequence::create(delay, func, delay1, rm, nullptr);
	runAction(seq);

	/*auto fadeout = FadeOut::create(t);
	runAction(fadeout);

	auto scale = ScaleTo::create(t, 2.0);
	runAction(scale);*/
}

void Bullet4OfBoss5::expolsion()
{
	int hurt = m_BasePro.getHurts();
	Bullet3OfBoss4::addWp(this, hurt);
}

//--------------------------------
BulletMeteor* BulletMeteor::create(const std::string& file, float v)
{
	BulletMeteor* bullet = new (std::nothrow) BulletMeteor;
	if (bullet && bullet->initWithFile(file))
	{
		bullet->m_velocity = v;
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void BulletMeteor::initBody()
{
	Vec2 v = Vec2(-1,-1);
	PhysicsBody* body = PhysicsBody::createCircle(20);
	body->setGravityEnable(false);
	body->setVelocity(v * m_velocity * random(0.9f,1.0f));
	setPhysicsBody(body);
	setInvincible(true);
	setBitmask(SELF_SCOPE_CATEGORYBITMASK, 0, SELF_SCOPE_CONTACTTESTBITMASK);

	float angle = v.getAngle() * RAD_DEG;
	getPhysicsBody()->setRotation(-angle);
}

void BulletMeteor::addParticle()
{
	EffectsManager::addParticle("particla/Meteor/tuowei.plist",this, Vec2(0,0), false);

	/*int randNum = random(0,1);
	if (randNum)
	{
		EffectsManager::addParticle("particla/Meteor/lizituowei.plist",this, Vec2(0,0), false);
	}
	else
	{
		EffectsManager::addParticle("particla/Meteor/tuowei.plist",this, Vec2(0,0), false);
	}*/
}

void BulletMeteor::onEnter()
{
	Bullet::onEnter();

	addParticle();
	//randRemove();
}

void BulletMeteor::randRemove()
{
	float randNum = rand_0_1();
	if (randNum < 0.5f)
	{
		auto delay = DelayTime::create( 0.3f + rand_0_1() * 0.3f);
		auto func = CallFunc::create(this, callfunc_selector(PhysicsUnit::DeathForce));
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}
}

void BulletMeteor::explosion()
{
	auto layer = UIController::getInstance()->getPhysicsLayer();
	auto pos = convertToWorldSpaceAR(Vec2::ZERO);
	EffectsManager::addParticle("particla/Meteor/baokaihuoxing.plist",layer,pos);
}

void BulletMeteor::DoSomethingBeforeDead()
{
	auto pos = convertToWorldSpaceAR(Vec2::ZERO);

	if (pos.x > 50 && pos.x < m_WinSize.width - 50 &&
		pos.y > 100)
	{
		explosion();
	}
}


//-------------------------------
CrossBullet* CrossBullet::create( const std::string& file, Weapon* weapon )
{
	CrossBullet* bullet = new (std::nothrow) CrossBullet;
	if (bullet && bullet->initWithSpriteFrameName(file))
	{
		bullet->m_weapon = weapon;
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();
		return bullet;
	}
	else
	{
		CC_SAFE_DELETE(bullet);
		return nullptr;
	}
}

void CrossBullet::initBody()
{
	PhysicsBody* body = PhysicsBody::createBox(getContentSize());
	body->setGravityEnable(false);
	setPhysicsBody(body);
	setInvincible(true);
	setBitmask(SELF_SCOPE_CATEGORYBITMASK, 0, SELF_SCOPE_CONTACTTESTBITMASK);
}

void CrossBullet::onEnter()
{
	Bullet::onEnter();
	setAnchorPoint(Vec2(0.5,0));
	{
		float angle = m_weapon->getRotation();
		auto rot = RotateTo::create(0.1, angle);
		auto back = RotateTo::create(0.1, -angle);
		auto seq = Sequence::create(rot, back, nullptr);
		auto rep = RepeatForever::create(seq);
		runAction(rep);
	}
	{
		float t = 0.1 + rand_0_1() * 0.2;
		auto tint = TintTo::create(t, ccc3(255,200,0));
		auto tintBack = TintTo::create(t, 255,255,255);
		auto seq = Sequence::create(tint, tintBack, nullptr);
		auto rep = RepeatForever::create(seq);
		runAction(rep);
	}

	auto s = getContentSize();

	schedule(schedule_selector(CrossBullet::cross));
}

void CrossBullet::cross(float dt)
{
	float rot = getRotation() - 90;
	Vec2 v = _physicsBody->getVelocity();
	v.normalize();
	v = v.forAngle(-rot * DEG_RAD);
	_physicsBody->setVelocity(v * m_FlySpeed);
}

//------------------------------------------
void BulletConvolution::onEnter()
{
	Bullet::onEnter();

	auto delay = DelayTime::create(m_ConvolutionDur);
	auto func = CallFunc::create(this, callfunc_selector(BulletConvolution::BulletReverse));
	auto seq = Sequence::create(delay, func, nullptr);
	auto rep = Repeat::create(seq,m_ConvolutionTimes);
	runAction(rep);
}

void BulletConvolution::BulletReverse()
{
	auto curV = _physicsBody->getVelocity();
	auto reverseV = -curV;
	_physicsBody->setVelocity(reverseV);

	float curRot = getRotation();
	getPhysicsBody()->setRotation(curRot + 180);
}


//-------------------------------
DirectBullet* DirectBullet::create( const std::string& file,float angleOffset, bool isLeft, bool isFlipX )
{
	DirectBullet* bullet = new (std::nothrow) DirectBullet;
	if (bullet && bullet->initWithSpriteFrameName(file))
	{
		bullet->m_AngleOffset = angleOffset;
		bullet->m_isFlipX = isFlipX;
		bullet->m_isLeft = isLeft;
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();

		return bullet;
	}
	CC_SAFE_DELETE(bullet);
	return nullptr;
}

void DirectBullet::initBody()
{
	PhysicsBody* body = PhysicsBody::createBox(getContentSize());
	body->setGravityEnable(false);
	setPhysicsBody(body);
	setInvincible(true);
	setInvincibleAtEdge(true);
	setBitmask(ENEMY_AMMO_CATEGORYBITMASK, 0, ENEMY_AMMO_CONTACTTESTBITMASK);
}

void DirectBullet::onEnter()
{
	Bullet::onEnter();
	NotificationCenter::getInstance()->removeObserver(this, Msg_BossDead);	//娑撯偓闁跨喐鏋婚幏鐤洣闁跨喓瀚涚粵瑙勫闁跨喐鏋婚幏鐑芥晸閺傘倖瀚圭拋顓炲珯闁跨喐鏋婚幏鐑芥�?
	NotificationCenter::getInstance()->removeObserver(this, Msg_ProtectRemove);	//娑撯偓闁跨喐鏋婚幏鐤洣闁跨喓瀚涚粵瑙勫闁跨喐鏋婚幏鐑芥晸閺傘倖瀚圭拋顓炲珯闁跨喐鏋婚幏鐑芥�?
	setBelongTo(UnitId::ePlaneBoss);

	m_OriginAngle = getRotation();
	_physicsBody->setVelocity(Vec2(0,0));
	m_prePos = convertToWorldSpaceAR(Vec2::ZERO);
	m_curPos = m_prePos;
	m_FixAngle = 0;
	m_curAngle = 0;

}

void DirectBullet::refreshAngle( float t )
{
	m_curPos = convertToWorldSpaceAR(Vec2::ZERO);
	if (m_curPos.distance(m_prePos) < 0.1)
	{
		return;
	}
	if (m_curAngle != m_FixAngle)
	{
		m_curAngle = m_FixAngle;
		return;
	}

	auto newVec = m_curPos - m_prePos;
	float angle = newVec.getAngle() * RAD_DEG;
	if (m_isFlipX)
	{
		getPhysicsBody()->setRotation(-angle - m_AngleOffset + m_FixAngle);
	}
	else
	{
		getPhysicsBody()->setRotation(-angle + 180 + m_AngleOffset + m_FixAngle);
	}

	m_prePos = m_curPos;
}

void DirectBullet::runBezier()
{
	auto mypos = convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 plPos = Vec2(m_WinSize.width * 0.5, m_WinSize.height * 0.1);
	auto pl = GameData::getInstance()->getMainPlane();
	if (pl)
	{
		plPos = pl->convertToWorldSpaceAR(Vec2::ZERO);
	}

	auto newVec = plPos - mypos;

	ccBezierConfig config;
	if (m_isLeft)
	{
		config.controlPoint_1 = Vec2(-200,20);
		config.controlPoint_2 = Vec2(newVec.x*0.4 - 100, newVec.y * 0.4);
		config.endPosition = newVec;
	}
	else
	{
		config.controlPoint_1 = Vec2(200, 20);
		config.controlPoint_2 = Vec2(newVec.x*0.4 + 100, newVec.y * 0.4);
		config.endPosition = newVec;
	}

	auto mv = BezierBy::create(m_FlyTime, config);
	auto func = CallFunc::create(this, callfunc_selector(DirectBullet::GoBack));
	auto wait = DelayTime::create(1.0f);
	auto back = mv->reverse();
	auto resm = CallFunc::create(this,callfunc_selector(DirectBullet::ResumeRotate));
	auto seq = Sequence::create(mv, func, back, resm, nullptr);
	//auto rep = Repeat::create(seq, 1);
	runAction(seq);

	schedule(schedule_selector(DirectBullet::refreshAngle));
}

void DirectBullet::GoBack()
{
	m_FixAngle = 180;

	return;

	if (m_FixAngle < 10)
	{
		m_FixAngle = 180;
	}
	else if(m_FixAngle > 170)
	{
		m_FixAngle = 0;
	}
}

void DirectBullet::Start()
{
	m_FixAngle = 0;	//閿熸枻鎷烽敓鐭绛规嫹鍘绘椂閿熸枻鎷烽敓鏂ゆ嫹杞敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熻剼鍑ゆ嫹杞敓鏂ゆ�?

	//閿熸枻鎷烽敓鏂ゆ嫹鐑?
	setOpacity(255);
	float r = getRotation();
	float angle = 30;
	if (r > 0 && r < 90)
	{
		angle = 30;
	}
	else if(r > 90 && r < 180)
	{
		angle = -30;
	}
	ShuaGuang();
	auto delay = DelayTime::create(0.8);
	auto mv = CallFunc::create(this, callfunc_selector(DirectBullet::runBezier));
	auto seq1 = Sequence::create(delay, mv, nullptr);
	runAction(seq1);
}

void DirectBullet::ResumeRotate()
{
	unschedule(schedule_selector(DirectBullet::refreshAngle));
	getPhysicsBody()->setRotation(m_OriginAngle);
}

void DirectBullet::DoSomethingBeforeDead()
{

}

void DirectBullet::ShuaGuang()
{
	Size s = getContentSize();

	auto sf = SpriteFrameCache::getInstance()->getSpriteFrameByName("nps1.png");
	auto stencil = Sprite::createWithSpriteFrame(sf);
	bool isFlipx = this->isFlippedX();
	stencil->setFlippedX(isFlipx);

	auto clipper = ClippingNode::create();
	clipper->setPosition(s.width * 0.5, s.height * 0.5);
	clipper->setStencil(stencil);		//閿熸枻鎷烽敓鐭纭锋嫹妯￠敓鏂ゆ嫹 
	clipper->setInverted(false);		//閿熸枻鎷烽敓鐭簳甯嫹鏉夐�?
	clipper->setContentSize(s);
	clipper->setAlphaThreshold(0.05);		//閿熸枻鎷烽敓鐭紮鎷烽敓鐙″簳甯嫹閿熺锤lpha鍊间�?
	addChild(clipper);			

	auto content = Sprite::create("FirstUIRes/Sptigeguang.png");//閿熸枻鎷烽敓鐭》鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
	content->setBlendFunc(BlendFunc::ADDITIVE);
	clipper->addChild(content);	
	content->setFlippedX(isFlipx);

	if (m_isLeft)
	{
		content->setPosition(s.width, s.height * 0.5);
		MoveTo* moveAction = MoveTo::create(0.2f, Vec2(-s.width, 30));
		MoveTo* moveBackAction = MoveTo::create(0.01f, Vec2(s.width,30));
		auto rep = Repeat::create(Sequence::create( moveAction, moveBackAction,NULL), 3);
		content->runAction(rep);
	}
	else
	{
		content->setPosition(0, s.height * 0.5);
		MoveTo* moveAction = MoveTo::create(0.2f, Vec2(s.width, 30));
		MoveTo* moveBackAction = MoveTo::create(0.01f, Vec2(-s.width,30));
		auto rep = Repeat::create(Sequence::create( moveAction, moveBackAction,NULL), 3);
		content->runAction(rep);
	}
	
}

//----------------------------------------------
FallBulletOfEyeBoss* FallBulletOfEyeBoss::create( const std::string& file )
{
	FallBulletOfEyeBoss* bullet = new (std::nothrow) FallBulletOfEyeBoss;
	if (bullet && bullet->initWithSpriteFrameName(file))
	{
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();
		return bullet;
	}
	CC_SAFE_DELETE(bullet);
	return nullptr;
}

void FallBulletOfEyeBoss::initBody()
{
	auto body = PhysicsBody::createCircle(getContentSize().width * 0.5);
	body->setGravityEnable(true);
	body->setLinearDamping(0);
	setPhysicsBody(body);

	setBitmask(ENEMY_AMMO_CATEGORYBITMASK,
		ENEMY_AMMO_COLLISIONBITMASK,
		ENEMY_AMMO_CONTACTTESTBITMASK);
}

void FallBulletOfEyeBoss::onEnter()
{
	Bullet::onEnter();

	{
//		auto rot = RotateBy::create(2.0f, 360);
//		auto rep = RepeatForever::create(rot);
//		runAction(rep);
	}
	{
//		auto scale = ScaleTo::create(2.0f, 2);
//		runAction(scale);
	}
	{
		auto delay = DelayTime::create(2.0f);
		auto func = CallFunc::create(this, callfunc_selector(FallBulletOfEyeBoss::FlyToTarget));
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}
}

void FallBulletOfEyeBoss::FlyToTarget()
{
	_physicsBody->setLinearDamping(0);
	_physicsBody->setGravityEnable(false);
	_physicsBody->setRotationEnable(true);

	auto myPos = this->getPhysicsBody()->getPosition();
	auto plPos = Vec2(m_WinSize.width *0.5, -100);
	auto mainPl = GameData::getInstance()->getMainPlane();
	if (mainPl)
	{
		plPos = mainPl->getPhysicsBody()->getPosition();
	}

	Vec2 newVec = plPos - myPos;
	newVec.normalize();

	float angle =  -(newVec.getAngle() * Pi2Angle) - 90;
	this->setRotation(angle);

	newVec *= 900;
	_physicsBody->setVelocity(newVec);
}

//---------------------------------------------------
BezierBulletOfEyeBoss* BezierBulletOfEyeBoss::create( const std::string& file )
{
	BezierBulletOfEyeBoss* bullet = new (std::nothrow) BezierBulletOfEyeBoss;
	if (bullet && bullet->initWithSpriteFrameName(file))
	{
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();
		return bullet;
	}
	CC_SAFE_DELETE(bullet);
	return nullptr;
}

void BezierBulletOfEyeBoss::initBody()
{
	auto body = PhysicsBody::createCircle(getContentSize().width * 0.5);
	body->setGravityEnable(false);
	setPhysicsBody(body);

	setBitmask(ENEMY_AMMO_CATEGORYBITMASK,
		ENEMY_AMMO_COLLISIONBITMASK,
		ENEMY_AMMO_CONTACTTESTBITMASK);

	setInvincibleAtEdge(true);
}

void BezierBulletOfEyeBoss::onEnter()
{
	Bullet::onEnter();
	runBezier();

	{
		auto scale = ScaleTo::create(1.0f, 1.5);
		runAction(scale);
	}
	{
		auto rot = RotateBy::create(1.0f, 360);
		auto rep = RepeatForever::create(rot);
		runAction(rep);
	}
}

void BezierBulletOfEyeBoss::runBezier()
{
	auto v = _physicsBody->getVelocity();
	_physicsBody->setVelocity(Vec2::ZERO);

	v.normalize();
	Vec2 controlPos1 = v * (150 + rand_0_1()*200);	//閿熸枻鎷烽敓鐙＄鎷?

	auto plPos = Vec2(m_WinSize.width*0.5, -100);
	auto myPos = convertToWorldSpaceAR(Vec2::ZERO);

	auto mainPl = GameData::getInstance()->getMainPlane();
	if (mainPl)
	{
		plPos = mainPl->convertToWorldSpaceAR(Vec2::ZERO);
	}

	auto newVec = plPos - myPos;
	Vec2 endPos = Vec2(newVec.x + rand_minus1_1() * 100,  (newVec * 5).y);
	Vec2 controlPos2 = newVec*0.5;
	/*if (controlPos1.x < 0)
	{
	controlPos2 = Vec2(newVec.x * 0.5 - 50 - rand_0_1() * 100, newVec.y * 0.5);
	}
	else
	{
	controlPos2 = Vec2(newVec.x * 0.5 + 50 + rand_0_1() * 100, newVec.y * 0.5);
	}*/
	
	ccBezierConfig config;
	config.controlPoint_1 = controlPos1;
	config.controlPoint_2 = controlPos2;
	config.endPosition = endPos;

	auto mv = BezierBy::create(3.0f, config);
	runAction(mv);
}

//--------------------------------
DanxiangLaserOfEyeBoss::DanxiangLaserOfEyeBoss()
{
	m_laserLength = -1;
	m_saosheAngle = 0;
	m_beginAngle = 0;
	m_saosheTime = -1;
	m_laserWidth = 2.0f;
	m_warningTime = -1.0;
	m_laser = nullptr;
}

DanxiangLaserOfEyeBoss* DanxiangLaserOfEyeBoss::create(float t)
{
	DanxiangLaserOfEyeBoss* node = new (std::nothrow) DanxiangLaserOfEyeBoss;
	if (node && node->init())
	{
		node->m_dur = t;
		node->initBullet();
		node->autorelease();
		node->initBody();
		return node;
	}
	CC_SAFE_DELETE(node);
	return nullptr;
}

void DanxiangLaserOfEyeBoss::initBody()
{
	PhysicsBody* body = PhysicsBody::createCircle(1);
	body->setDynamic(false);
	setPhysicsBody(body);
	setBitmask(ENEMY_AMMO_CATEGORYBITMASK,
		0,
		ENEMY_AMMO_CONTACTTESTBITMASK);

	setInvincible(true);
	resetHurt(-1);
}

void DanxiangLaserOfEyeBoss::onEnter()
{
	Bullet::onEnter();

	if (m_warningTime > 0)
	{
		auto warning = Sprite::create("ammo/EyeBoss/laser.png");
		warning->setBlendFunc(BlendFunc::ADDITIVE);
		warning->setScaleY(0.2);
		warning->setAnchorPoint(Vec2(0,0.5));
		addChild(warning,-1);
		warning->setColor(ccc3(255,0,0));
		warning->setScaleX(m_laserLength/warning->getContentSize().width);
		warning->setRotation(m_beginAngle);
		warning->setOpacity(0);
		warning->setName("warning");

		{
			auto fadein = FadeIn::create(0.1);
			auto fadeout = FadeOut::create(0.1);
			auto seq = Sequence::create(fadein, fadeout, nullptr);
			auto rep = Repeat::create(seq, m_warningTime/0.2);
			auto laser = CallFunc::create(this, callfunc_selector(DanxiangLaserOfEyeBoss::LaserBegin));
			auto seq1 = Sequence::create(rep, laser, nullptr);
			warning->runAction(seq1);
		}
	}
	else
	{
		LaserBegin();
	}
}

void DanxiangLaserOfEyeBoss::ShootPos()
{
	auto sp1 = Sprite::create("ammo/EyeBoss/shootPos_1.png");
	sp1->setBlendFunc(BlendFunc::ADDITIVE);
	addChild(sp1);
	auto sp2 = Sprite::create("ammo/EyeBoss/shootPos_2.png");
	sp2->setBlendFunc(BlendFunc::ADDITIVE);
	addChild(sp2);
	auto sp3 = Sprite::create("ammo/EyeBoss/shootPos_3.png");
	sp3->setBlendFunc(BlendFunc::ADDITIVE);
	addChild(sp3);

	sp1->setOpacity(0);
	sp2->setOpacity(0);
	sp3->setOpacity(0);

	sp1->setScale(0.1);
	sp2->setScale(3);
	sp3->setScale(0.1);

	{
		auto fadeout = FadeIn::create(0.3);
		auto scale = ScaleTo::create(0.3,1.2);
		auto spw = Spawn::create(fadeout, scale, nullptr);
		sp1->runAction(spw);
	}
	{
		auto fadeout = FadeIn::create(0.3f);
		auto scale = ScaleTo::create(0.3, 1.2);
		auto spw = Spawn::create(fadeout, scale, nullptr);
		sp2->runAction(spw);

		auto rot = RotateBy::create(0.5f,360);
		auto rep = RepeatForever::create(rot);
		sp2->runAction(rep);
	}
	{
		auto fadeout = FadeIn::create(0.1f);
		auto scale = ScaleTo::create(0.15f,3.0);
		auto fadein = FadeOut::create(0.2f);
		auto seq = Sequence::create(fadeout, scale, fadein, nullptr);
		sp3->runAction(seq);
	}
}

void DanxiangLaserOfEyeBoss::ShootLaser()
{
	m_LaserPoint = Sprite::create("ammo/EyeBoss/laser.png");
	m_LaserPoint->setAnchorPoint(Vec2(0,0.5));
	m_LaserPoint->setOpacity(255);
	m_LaserPoint->setScaleY(m_laserWidth);
	m_LaserPoint->setBlendFunc(BlendFunc::ADDITIVE);
	addChild(m_LaserPoint);

	auto s = m_LaserPoint->getContentSize();
	m_laser = Bullet::createWithPicture("ammo/EyeBoss/laser.png");
	m_laser->setPosition(s.width * 0.5, 0);
	m_laser->setOpacity(0);
	m_laser->setScaleY(m_laserWidth + 0.2);
	m_laser->setBitmask(ENEMY_AMMO_CATEGORYBITMASK,
		0,
		ENEMY_AMMO_CONTACTTESTBITMASK);
	m_laser->getPhysicsBody()->setGravityEnable(false);
	m_laser->setInvincible(true);
	m_laser->resetHurt(-1);
	m_laser->setInvincibleAtEdge(true);
	addChild(m_laser);
	NotificationCenter::getInstance()->removeAllObservers(m_laser);

	{
		auto fadein = FadeIn::create(0.1f);
		auto fadeout = FadeOut::create(0.1f);
		auto seq = Sequence::create(fadein, fadeout, nullptr);
		auto rep = RepeatForever::create(seq);
		m_laser->runAction(rep);
	}

	if (m_laserLength > 0)
	{
		float scale = m_laserLength / s.width;
		m_LaserPoint->setScaleX(scale);
		m_laser->setScaleX(scale);


		m_laser->setPosition(s.width * 0.5 * scale, 0);
	}

	if (m_saosheTime > 0)
	{
		m_LaserPoint->setRotation(m_beginAngle);
		auto delay = DelayTime::create(0.65);
		auto rot = RotateTo::create(m_saosheTime, m_beginAngle + m_saosheAngle);
		auto back = RotateTo::create(m_saosheTime, m_beginAngle - m_saosheAngle);
		auto seq = Sequence::create(rot, back, nullptr);
		auto rep = Repeat::create(seq,5);
		auto seq1 = Sequence::create(delay, rep, nullptr);
		m_LaserPoint->runAction(seq1);

		auto partc = EffectsManager::addParticle("particla/LaserPenkou/test1.plist",m_LaserPoint, Vec2(0, s.height * 0.5), false);
		if (partc)
		{
			partc->setRotation(-90);
			partc->setPositionType(ParticleSystem::PositionType::GROUPED);
			partc->setScaleX(0.2);
			partc->setScaleY(1.0);
		}
	}

	BlendFunc blend;
	blend.src = GL_SRC_COLOR;
	blend.dst = GL_ONE;
	m_laser->setBlendFunc(blend);

	{
		auto scale = ScaleBy::create(0.03, 0.85);
		auto back = scale->reverse();
		auto seq = Sequence::create(scale, back, nullptr);
		auto rep = RepeatForever::create(seq);
		//m_laser->runAction(rep);
	}
	{
		auto fadein = FadeIn::create(0.1f);
		auto fadeout = FadeOut::create(0.02f);
		auto seq = Sequence::create(fadein, fadeout, nullptr);
		auto rep = RepeatForever::create(seq);
		//m_laser->runAction(rep);
	}

	if(m_dur > 0)
	{
		auto delay = DelayTime::create(m_dur);
		auto func = CallFunc::create(this, callfunc_selector(DanxiangLaserOfEyeBoss::FadeInAndRemove));
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}
	schedule(schedule_selector(DanxiangLaserOfEyeBoss::Hurts),0.1);
	schedule(schedule_selector(DanxiangLaserOfEyeBoss::UpdateLaserRotate));
}

void DanxiangLaserOfEyeBoss::FadeInAndRemove()
{
	m_LaserPoint->stopAllActions();
	unschedule(schedule_selector(DanxiangLaserOfEyeBoss::Hurts));
	unschedule(schedule_selector(DanxiangLaserOfEyeBoss::UpdateLaserRotate));

	{
		m_laser->stopAllActions();
		auto fadein = FadeOut::create(0.3);
		auto rm = CallFunc::create(this, callfunc_selector(PhysicsUnit::DeathForce));
		auto seq = Sequence::create(fadein, rm, nullptr);
		m_laser->runAction(seq);

		m_LaserPoint->runAction(FadeOut::create(0.3));
		//m_LaserPoint->runAction(ScaleTo::create(0.15,1.0, 0.1));
		//m_laser->runAction(ScaleTo::create(0.15,1.0, 0.1));
	}
}

void DanxiangLaserOfEyeBoss::Hurts( float t )
{
	auto s = m_laser->getContentSize();
	PhysicsBody* body = PhysicsBody::createBox(Size(m_laserLength, s.height * 0.5));
	body->setGravityEnable(false);
	m_laser->setPhysicsBody(body);
	m_laser->setBitmask(ENEMY_AMMO_CATEGORYBITMASK,
		0,
		ENEMY_AMMO_CONTACTTESTBITMASK);
}

void DanxiangLaserOfEyeBoss::setLaserLength( float length )
{
	m_laserLength = length;
}

void DanxiangLaserOfEyeBoss::LaserSaoshe( float t, float beginAngle, float angle )
{
	m_beginAngle = beginAngle;
	m_saosheTime = t;
	m_saosheAngle = angle;
}

void DanxiangLaserOfEyeBoss::UpdateLaserRotate(float t)
{
	float angle = m_LaserPoint->getRotation();
	if(angle == 0) {
		angle = this->getRotation();
		PhysicsBody* body = this->m_laser->getPhysicsBody();
		if(body && body->getReferenceCount() > 0) {
			body->setRotation(angle);
		}
	} else {
		float x = m_LaserPoint->getContentSize().width * m_LaserPoint->getScaleX() * 0.5;
		Vec2 vec = Vec2(x,0);
		vec.rotate(Vec2(0,0),-angle * DEG_RAD);
		PhysicsBody* body = this->m_laser->getPhysicsBody();
		Vec2 bodyPos = this->convertToWorldSpaceAR(vec);
		if(body && body->getReferenceCount() > 0) {
			body->setRotation(angle);
			body->setPosition(bodyPos);
		}
	}
}

void DanxiangLaserOfEyeBoss::LaserBegin()
{
	bool b = GameData::getInstance()->getIsBossDead();
	if (b)
	{
		return;
	}

	auto node = getChildByName("warning");
	if (node)
	{
		node->removeFromParentAndCleanup(true);
	}

	auto func1 = CallFunc::create(this, callfunc_selector(DanxiangLaserOfEyeBoss::ShootPos));
	auto delay = DelayTime::create(0.5);
	auto func2 = CallFunc::create(this, callfunc_selector(DanxiangLaserOfEyeBoss::ShootLaser));
	auto seq = Sequence::create(func1, delay, func2, nullptr);
	runAction(seq);
}

void DanxiangLaserOfEyeBoss::AddWarning( float t )
{
	m_warningTime = t;
}

void DanxiangLaserOfEyeBoss::BossDead( Ref* obj )
{
	this->DeathForce();
}

void DanxiangLaserOfEyeBoss::onExit()
{
	Bullet::onExit();
}

//-------------------------------------------
TelescopicLaserOfEnyBoss* TelescopicLaserOfEnyBoss::create( const std::string& file )
{
	TelescopicLaserOfEnyBoss* bullet = new (std::nothrow) TelescopicLaserOfEnyBoss;
	if (bullet && bullet->init())
	{
		bullet->m_FileName = file;
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();
		return bullet;
	}
	CC_SAFE_DELETE(bullet);
	return nullptr;
}

void TelescopicLaserOfEnyBoss::initBody()
{
	PhysicsBody* body = PhysicsBody::createBox(Size(10,10));
	body->setGravityEnable(false);
	body->setRotationEnable(false);
	setPhysicsBody(body);
	setBitmask(ENEMY_AMMO_CATEGORYBITMASK, 0, ENEMY_AMMO_CONTACTTESTBITMASK);
	setInvincible(true);
	resetHurt(-1);
}

void TelescopicLaserOfEnyBoss::onEnter()
{
	Bullet::onEnter();
	getPhysicsBody()->setRotation(0);
	addEyes();
	MoveDown();
	MoveLR();
}

void TelescopicLaserOfEnyBoss::addEyes()
{
	auto s = getContentSize();
	{
		m_Eye1 = Sprite::createWithSpriteFrameName(m_FileName);
		m_Eye1->setPosition(50,0);
		addChild(m_Eye1);
	}
	{
		m_Eye2 = Sprite::createWithSpriteFrameName(m_FileName);
		m_Eye2->setPosition(-50,0);
		addChild(m_Eye2);
	}

	float dis = 25 + rand_0_1() * 70; 
	float t = 0.5 + rand_0_1() * 0.2;
	{
		auto mv = MoveBy::create(t, Vec2(dis,0));
		auto ease = EaseSineInOut::create(mv);
		auto back = ease->reverse();
		auto seq = Sequence::create(ease, back, nullptr);
		auto rep = RepeatForever::create(seq);
		m_Eye1->runAction(rep);
	}
	{
		auto mv = MoveBy::create(t, Vec2(-dis,0));
		auto ease = EaseSineInOut::create(mv);
		auto back = ease->reverse();
		auto seq = Sequence::create(ease, back, nullptr);
		auto rep = RepeatForever::create(seq);
		m_Eye2->runAction(rep);
	}

	{
		m_laser = Sprite::create("ammo/EyeBoss/laser2.png");
		m_laser->setPosition(s.width * 0.5, s.height * 0.5);
		m_laser->setScaleX(0.001f);
		m_laser->setBlendFunc(BlendFunc::ADDITIVE);
		addChild(m_laser,-1);

		TextureCache::getInstance()->addImage("ammo/EyeBoss/laser3.png");
	}

	schedule(schedule_selector(TelescopicLaserOfEnyBoss::updateEyeTelescopicLaser));
}

void TelescopicLaserOfEnyBoss::MoveDown()
{
	auto mvdown = MoveBy::create(10.0f, Vec2(0,-1350));
	runAction(mvdown);
}

void TelescopicLaserOfEnyBoss::updateEyeTelescopicLaser(float t)
{
	m_TextureId++;
	m_TextureId %= 4;
	if (m_TextureId == 0)
	{
		m_laser->setTexture("ammo/EyeBoss/laser2.png");
	}
	else if(m_TextureId == 2)
	{
		m_laser->setTexture("ammo/EyeBoss/laser3.png");
	}
	m_laser->setBlendFunc(BlendFunc::ADDITIVE);

	Vec2 eye1_pos = m_Eye1->convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 eye2_pos = m_Eye2->convertToWorldSpaceAR(Vec2::ZERO);

	float dis = eye1_pos.distance(eye2_pos);
	float bulletSize = m_laser->getContentSize().width;
	m_laser->setScaleX(dis / bulletSize);

	m_time += 1.0f;
	if (m_time > 10.0f)
	{
		m_time = 0.0f;
		//閿熸枻鎷烽敓鏂ゆ嫹閿熷壙鐚存�?
		PhysicsBody* body = PhysicsBody::createBox(Size(dis,10));
		body->setGravityEnable(false);
		body->setRotationEnable(false);
		setPhysicsBody(body);
		resetHurt(-1);
		setBitmask(ENEMY_AMMO_CATEGORYBITMASK, 0, ENEMY_AMMO_CONTACTTESTBITMASK);
	}
}

void TelescopicLaserOfEnyBoss::MoveLR()
{
	stopActionByTag(15081815);
	float t = 2.0 + rand_0_1() * 1.0;

	auto mypos = convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 mvDirect;
	if (mypos.x < m_WinSize.width * 0.5)
	{
		mvDirect = Vec2(m_WinSize.width * 0.85 - mypos.x, 0);
	}
	else
	{
		mvDirect = Vec2(m_WinSize.width * 0.15 - mypos.x, 0);
	}
	auto mv = MoveBy::create(t, mvDirect);
	auto next = CallFunc::create(this, callfunc_selector(TelescopicLaserOfEnyBoss::MoveLR));
	auto seq = Sequence::create(mv, next, nullptr);
	seq->setTag(15081815);
	runAction(seq);
}

//------------------------------
void DrawShapeOfEyeBoss::onEnter()
{
	Layer::onEnter();
	
	initEyesPos();
	addEyeOneByOne();

	{
		auto delay = DelayTime::create(20.0f);
		auto rm = RemoveSelf::create(true);
		auto seq = Sequence::create(delay, rm, nullptr);
		runAction(seq);
	}
	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(DrawShapeOfEyeBoss::RemoveMe),Msg_BossDead, nullptr);
}

void DrawShapeOfEyeBoss::onExit()
{
	Layer::onExit();
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void DrawShapeOfEyeBoss::initEyesPos()
{
	auto ws = Director::getInstance()->getWinSize();
	{
		//1
		Vec2 pos = Vec2(ws.width * 0.5, ws.height * 0.7);
		m_EyePosVec.push_back(pos);
	}
	{
		//2
		Vec2 pos = Vec2(ws.width * 0.1, ws.height * 0.4);
		m_EyePosVec.push_back(pos);
	}
	{
		//3
		Vec2 pos = Vec2(ws.width * 0.9, ws.height * 0.4);
		m_EyePosVec.push_back(pos);
	}
	{
		//4
		Vec2 pos = Vec2(ws.width * 0.15, ws.height * 0.05);
		m_EyePosVec.push_back(pos);
	}
	{
		//5
		Vec2 pos = Vec2(ws.width * 0.85, ws.height * 0.05);
		m_EyePosVec.push_back(pos);
	}

}

void DrawShapeOfEyeBoss::addEyeOneByOne()
{
	m_EpVec.push_back(new EyePro(0,1));
	m_EpVec.push_back(new EyePro(1,3));
	m_EpVec.push_back(new EyePro(3,4));
	m_EpVec.push_back(new EyePro(4,2));
	m_EpVec.push_back(new EyePro(2,0));
	m_EpVec.push_back(new EyePro(0,3));
	m_EpVec.push_back(new EyePro(3,2));
	m_EpVec.push_back(new EyePro(2,1));
	m_EpVec.push_back(new EyePro(1,4));
	m_EpVec.push_back(new EyePro(4,0));
	m_EpVecIt = m_EpVec.begin();

	auto delay = DelayTime::create(0.2);
	auto func = CallFunc::create(this, callfunc_selector(DrawShapeOfEyeBoss::addOneLaser));
	auto seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void DrawShapeOfEyeBoss::addOneLaser()
{
	if (m_EpVecIt == m_EpVec.end())
	{
		auto delay = DelayTime::create(6);		//閿熸枻鎷烽敓鎻紮鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓渚ョ鎷烽敓鏂ゆ嫹鏃堕敓鎴掑埌閿熸枻鎷烽敓鏂ゆ嫹鏄犻敓鎻紮鎷烽敓缂存拝鎷烽敓绲yer
		auto rm = RemoveSelf::create(true);
		auto seq = Sequence::create(delay, rm, nullptr);
		runAction(seq);
	}
	else
	{
		EyePro* ep = (*m_EpVecIt);
		auto sp = Sprite::createWithSpriteFrameName("siyan2.png");
		sp->setPosition(m_EyePosVec.at(ep->beginId));
		sp->setScale(2.0f);
		addChild(sp);

		void* data = (void*)(ep);
		auto delay = DelayTime::create(0.2);
		auto func = CCCallFuncND::create(this, callfuncND_selector(DrawShapeOfEyeBoss::addOneLaserCB), data);
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
		m_EpVecIt++;
	}
}

void DrawShapeOfEyeBoss::addOneLaserCB( Node* node, void* data )
{
	EyePro* ep = (EyePro*)data;

	auto beginPos = m_EyePosVec.at(ep->beginId);
	auto endPos = m_EyePosVec.at(ep->endId);
	auto vec = endPos - beginPos;
	auto angle = vec.getAngle() * RAD_DEG;
	auto dis = beginPos.distance(endPos);

	
	float interval = 0.2f;
	if (m_LaserCount > 4)
	{
		DanxiangLaserOfEyeBoss* eye = DanxiangLaserOfEyeBoss::create(10.0 - (m_LaserCount - 4));
		eye->setPosition(beginPos);
		eye->setLaserLength(dis);
		eye->setLaserWidth(1.0f);
		eye->setRotation(-angle);
		eye->AddWarning(0.8);
		addChild(eye);

		interval = 1.0f;
	}
	else
	{
		DanxiangLaserOfEyeBoss* eye = DanxiangLaserOfEyeBoss::create(10.0);
		eye->setPosition(beginPos);
		eye->setLaserLength(dis);
		eye->setLaserWidth(1.0f);
		eye->setRotation(-angle);
		addChild(eye);
	}

	{
		auto delay = DelayTime::create(interval);
		auto func = CallFunc::create(this, callfunc_selector(DrawShapeOfEyeBoss::addOneLaser));
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}
	m_LaserCount++;
}

void DrawShapeOfEyeBoss::RemoveMe( Ref* obj )
{
	removeFromParentAndCleanup(true);
}


//-------------------------------------------------
AutoExplosionOfYizhiBoss* AutoExplosionOfYizhiBoss::create( const std::string& name )
{
	AutoExplosionOfYizhiBoss* bullet = new (std::nothrow) AutoExplosionOfYizhiBoss;
	if (bullet && bullet->initWithSpriteFrameName(name))
	{
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();
		return bullet;
	}
	CC_SAFE_DELETE(bullet);
	return nullptr;
}

void AutoExplosionOfYizhiBoss::autoExplosion()
{
	if (!m_IsExpl)
	{
		m_IsExpl = true;
		auto s = getContentSize();

		auto mypos = convertToWorldSpaceAR(Vec2::ZERO);
		
		UIController::getInstance()->getEffectsLayer()->displayDeathExplosion(this->convertToWorldSpaceAR(Point::ZERO));

		float r = 360.0f/12;
		for (int i = 0; i < 12; i++)
		{
			Weapon* wp = Weapon::create(nullptr);
			wp->setBullstType(BulletType::Enemy_default);
			//wp->setBltResTag(ResTag::Bullet_EnemyNor3);
			wp->setBltFrameName("yizhiBoss_ammo.png");
			wp->setBltBelongTo(UnitId::ePlaneBoss);		
			wp->setBltSelfRotate(360);
			wp->setShootTimes(2);
			wp->setBltHurts(Ene_Blt_hurt);
			wp->setBltSpeed(500);
			wp->setBltLinearDamping(0.2);
			wp->setRotation(i * r );
			wp->setOffset(mypos);
			wp->StartShoot();
			UIController::getInstance()->getPhysicsLayer()->addChild(wp);
			wp->runAction(RotateBy::create(0.45,20));
		}

		{
			setVisible(false);
			_physicsBody->setVelocity(Vec2::ZERO);

			auto delay = DelayTime::create(0.5);
			auto rm = CallFunc::create(this, callfunc_selector(PhysicsUnit::DeathForce));
			auto seq = Sequence::create(delay, rm, nullptr);
			runAction(seq);
		}
	}
}

void AutoExplosionOfYizhiBoss::onEnter()
{
	Bullet::onEnter();
	
	auto delay = DelayTime::create(m_ExplTime);
	auto func = CallFunc::create(this, callfunc_selector(AutoExplosionOfYizhiBoss::autoExplosion));
	auto seq = Sequence::create(delay, func ,nullptr);
	runAction(seq);
}

void AutoExplosionOfYizhiBoss::setExplosionTime( float t )
{
	m_ExplTime = t;
}

void AutoExplosionOfYizhiBoss::initBody()
{
	auto body = PhysicsBody::createBox(getContentSize());
	body->setGravityEnable(false);
	setPhysicsBody(body);

	setBitmask(ENEMY_AMMO_CATEGORYBITMASK, ENEMY_AMMO_COLLISIONBITMASK, ENEMY_AMMO_CONTACTTESTBITMASK);
	resetHurt(-1);
	setInvincible(true);
}

void AutoExplosionOfYizhiBoss::OtherDealWhenContact(const BasePro& rs)
{
	autoExplosion();
}

//----------------------------------
SpiralMissile* SpiralMissile::create( const std::string& name )
{
	SpiralMissile* bullet = new (std::nothrow) SpiralMissile;
	if (bullet && bullet->initWithSpriteFrameName(name))
	{
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();
		return bullet;
	}
	CC_SAFE_DELETE(bullet);
	return nullptr;
}

void SpiralMissile::onEnter()
{
	Bullet::onEnter();

	setBitmask(ENEMY_AMMO_CATEGORYBITMASK, ENEMY_AMMO_COLLISIONBITMASK, ENEMY_AMMO_CONTACTTESTBITMASK);

	RandChangeTargetPos();
	schedule(schedule_selector(SpiralMissile::FlyToTarget));
	schedule(schedule_selector(SpiralMissile::LockTargetPos),0.5f, 100, 1.0 + rand_0_1() * 1.0f);

	{
		auto delay = DelayTime::create(0.5 + rand_0_1() * 0.5);
		auto func1 = CallFunc::create(this, callfunc_selector(SpiralMissile::RandChangeTargetPos));
		auto seq = Sequence::create(delay, func1, nullptr);
		auto rep = Repeat::create(seq, 2);

		runAction(rep);
	}
}

void SpiralMissile::FlyToTarget( float dt )
{
	Vec2 curVec = _physicsBody->getVelocity();
	Vec2 myPos = convertToWorldSpaceAR(Vec2::ZERO);
	
	Vec2 newVec = m_targetPos - myPos;

	float distance = myPos.distance(m_targetPos);
	curVec.smooth(newVec,100,1 + distance); 
	curVec.normalize();
	_physicsBody->setVelocity(curVec * m_FlySpeed);

	float bulletR = curVec.getAngle()* RAD_DEG;
	this->getPhysicsBody()->setRotation(90-bulletR);		

	if (m_FlySpeed < 500)
	{
		m_FlySpeed += 5;
	}
}

cocos2d::Vec2 SpiralMissile::getRandPos()
{
	auto pos = Vec2(m_WinSize.width * 0.5 + rand_minus1_1() * 200, m_WinSize.height * 0.6 - rand_0_1() * 100);
	return pos;
}

void SpiralMissile::RandChangeTargetPos()
{
	m_targetPos = getRandPos();
}

void SpiralMissile::LockTargetPos(float t)
{
	m_FlySpeed += 300;

	auto mypos = convertToWorldSpaceAR(Vec2::ZERO);
	auto mainPl = GameData::getInstance()->getMainPlane();
	if (mainPl)
	{
		m_targetPos = mainPl->convertToWorldSpaceAR(Vec2::ZERO);
	}
	else
	{
		m_targetPos = Vec2(m_WinSize.width * 0.5 + rand_minus1_1() * 200, - 200);
	}
}

void SpiralMissile::initBody()
{
	auto body = PhysicsBody::createBox(getContentSize());
	body->setGravityEnable(false);
	setPhysicsBody(body);

	setBitmask(ENEMY_AMMO_CATEGORYBITMASK, ENEMY_AMMO_COLLISIONBITMASK, ENEMY_AMMO_CONTACTTESTBITMASK);
	resetHurt(-1);
}

//-----------------------------------------------------
SpeedUpBulletOfYizhiBoss* SpeedUpBulletOfYizhiBoss::create( const std::string& name )
{
	SpeedUpBulletOfYizhiBoss* bullet = new (std::nothrow) SpeedUpBulletOfYizhiBoss;
	if (bullet && bullet->initWithSpriteFrameName(name))
	{
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();
		return bullet;
	}
	CC_SAFE_DELETE(bullet);
	return nullptr;
}

void SpeedUpBulletOfYizhiBoss::onEnter()
{
	Bullet::onEnter();
	{
		auto delay = DelayTime::create(1.5f);
		auto func = CallFunc::create(this, callfunc_selector(SpeedUpBulletOfYizhiBoss::start));
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}
}

void SpeedUpBulletOfYizhiBoss::SpeedUp( float t )
{
	auto curV = _physicsBody->getVelocity();
	_physicsBody->setVelocity(curV * 1.1);
}

void SpeedUpBulletOfYizhiBoss::start()
{
	schedule(schedule_selector(SpeedUpBulletOfYizhiBoss::SpeedUp));
}

void SpeedUpBulletOfYizhiBoss::initBody()
{
	auto body = PhysicsBody::createBox(getContentSize());
	body->setGravityEnable(false);
	setPhysicsBody(body);

	setBitmask(ENEMY_AMMO_CATEGORYBITMASK, ENEMY_AMMO_COLLISIONBITMASK, ENEMY_AMMO_CONTACTTESTBITMASK);
	resetHurt(-1);
}

//---------------------------------------------------------
ExplosionCyclotronOfYizhiBoss* ExplosionCyclotronOfYizhiBoss::create( const std::string& name )
{
	ExplosionCyclotronOfYizhiBoss* bullet = new (std::nothrow) ExplosionCyclotronOfYizhiBoss;
	if (bullet && bullet->initWithSpriteFrameName(name))
	{
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();
		return bullet;
	}
	CC_SAFE_DELETE(bullet);
	return nullptr;
}

void ExplosionCyclotronOfYizhiBoss::autoExplosion()
{
	UIController::getInstance()->getEffectsLayer()->displayDeathExplosion(this->convertToWorldSpaceAR(Point::ZERO));

	auto s = getContentSize();
	int randNum = random(0,1);
	for (int i = 0; i < 3; i++)
	{
		Weapon* wp = Weapon::create(this);
		wp->setBullstType(BulletType::Convolution);
		//wp->setBltResTag(ResTag::Bullet_EnemyNor3);
		wp->setBltFrameName("yizhiBoss_ammo.png");
		wp->setBltBelongTo(UnitId::ePlaneBoss);
		wp->setShootTimes(10);
		wp->setBltSelfRotate(360);
		wp->setBltHurts(Ene_Blt_hurt);
		wp->setBltCategorybitmask(ENEMY_AMMO_CATEGORYBITMASK);
		wp->setBltCollisionbitmask(ENEMY_AMMO_COLLISIONBITMASK);
		wp->setBltContacttestbitmask(ENEMY_AMMO_CONTACTTESTBITMASK);
		wp->setBltSpeed(500);
		//wp->setBltScaleY(1.0f);
		//wp->setBltScaleX(0.2f);
		wp->setBltLinearDamping(0.2);
		wp->setRotation(i * 120 );
		wp->setOffset(Vec2(s.width*0.5, s.height*0.5));
		wp->StartShoot();
		addChild(wp);
		wp->runAction(RotateBy::create(10.0f,1200 - 2400 * randNum));
	}

	//閿熸枻鎷风偢閿熸枻鎷烽敓鏂ゆ嫹閿熺殕鐚存嫹鍒犻敓鏂ゆ嫹閿熸枻鎷烽敓鎺ョ�?
	{
		setVisible(false);
		_physicsBody->setVelocity(Vec2::ZERO);

		auto delay = DelayTime::create(2.0f);
		auto rm = CallFunc::create(this, callfunc_selector(PhysicsUnit::DeathForce));
		auto seq = Sequence::create(delay, rm, nullptr);
		runAction(seq);
	}
}

void ExplosionCyclotronOfYizhiBoss::onEnter()
{
	Bullet::onEnter();
	setInvincible(true);
	auto delay = DelayTime::create(0.5f);
	auto func = CallFunc::create(this, callfunc_selector(ExplosionCyclotronOfYizhiBoss::autoExplosion));
	auto seq = Sequence::create(delay, func ,nullptr);
	runAction(seq);
}

void ExplosionCyclotronOfYizhiBoss::initBody()
{
	auto body = PhysicsBody::createBox(getContentSize());
	body->setGravityEnable(false);
	setPhysicsBody(body);

	setBitmask(ENEMY_AMMO_CATEGORYBITMASK, ENEMY_AMMO_COLLISIONBITMASK, ENEMY_AMMO_CONTACTTESTBITMASK);
	resetHurt(-1);
}

//----------------------------------------------
SpeedDownAndUniform* SpeedDownAndUniform::create( const std::string& name )
{
	SpeedDownAndUniform* bullet = new (std::nothrow) SpeedDownAndUniform;
	if (bullet && bullet->initWithSpriteFrameName(name))
	{
		bullet->initBullet();
		bullet->initBody();
		bullet->autorelease();
		return bullet;
	}
	CC_SAFE_DELETE(bullet);
	return nullptr;
}

void SpeedDownAndUniform::onEnter()
{
	Bullet::onEnter();
	{
		auto delay = DelayTime::create(0.5);
		auto func = CallFunc::create(this, callfunc_selector(SpeedDownAndUniform::SlowDown));
		auto seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}
}

void SpeedDownAndUniform::SlowDown()
{
	_physicsBody->setLinearDamping(0.0f);
}

void SpeedDownAndUniform::initBody()
{
	auto body = PhysicsBody::createCircle(getContentSize().width*0.5);
	body->setGravityEnable(false);
	setPhysicsBody(body);

	setBitmask(ENEMY_AMMO_CATEGORYBITMASK, ENEMY_AMMO_COLLISIONBITMASK, ENEMY_AMMO_CONTACTTESTBITMASK);
	resetHurt(-1);
}

//----------------------------
void FixDirect::setFixTarget( Sprite* sp )
{
	m_FixTarget = sp;
	m_CurPos = m_FixTarget->convertToWorldSpaceAR(Vec2::ZERO);
	m_PrePos = m_CurPos;
}

void FixDirect::BeginFix()
{
	m_CurPos = m_FixTarget->convertToWorldSpaceAR(Vec2::ZERO);
	if (m_CurPos.distance(m_PrePos) < 0.1)
	{
		m_PrePos = m_CurPos;
		return;
	}
	auto newVec = m_CurPos - m_PrePos;
	auto angle = newVec.getAngle() * RAD_DEG;
	m_FixTarget->setRotation(-angle);

	m_PrePos = m_CurPos;
}

//-----------------------------------
FireworkBullet* FireworkBullet::create(int hurt)
{
	FireworkBullet* bullet = new (std::nothrow) FireworkBullet;
	if (bullet && bullet->init())
	{
		bullet->m_hurt = hurt;
		bullet->initBullet();
		bullet->autorelease();
		bullet->initBody();
		return bullet;
	}
	CC_SAFE_DELETE(bullet);
	return nullptr;
}

void FireworkBullet::initBody()
{
	auto body = PhysicsBody::createCircle(20);
	body->setGravityEnable(false);
	setPhysicsBody(body);

	setBitmask(SELF_SCOPE_CATEGORYBITMASK, 0, SELF_SCOPE_CONTACTTESTBITMASK);
	setInvincible(1);

}

void FireworkBullet::onEnter()
{
	Bullet::onEnter();
	m_BasePro.setHurts(m_hurt);
	setDelayRemoveTime(0.5);
	auto layer = UIController::getInstance()->getPhysicsLayer();
	auto myPos = convertToWorldSpaceAR(Vec2::ZERO);

	m_particle1 = EffectsManager::addParticle("particla/firework/test1.plist",layer,myPos,false);
	m_particle1->setScale(2.0f);
	m_targetPos = Vec2(360 + rand_minus1_1() * 300,1500);
	m_FlySpeed = 2500;
	m_direct = 1;

	_physicsBody->setVelocity(Vec2(rand_minus1_1() * 30, -70 - rand_0_1() * 10));

	schedule(schedule_selector(FireworkBullet::FlyToTarget));

	{
		auto delay = DelayTime::create(0.1f + rand_minus1_1() * 0.1);
		auto func = CallFunc::create(this, callfunc_selector(FireworkBullet::StartBaidong));
		auto seq = Sequence::create(delay, func, nullptr);
		//runAction(seq);
	}
}

void FireworkBullet::FlyToTarget( float dt )
{
	m_targetPos = Vec2(360 + rand_minus1_1() * 300,1500);

	Vec2 curVec = _physicsBody->getVelocity();
	Vec2 myPos = convertToWorldSpaceAR(Vec2::ZERO);
	if (m_particle1)
	{
		Vec2 partcPos = m_particle1->convertToWorldSpaceAR(Vec2::ZERO);
		Vec2 lerpPos = partcPos.lerp(myPos,0.3);
		m_particle1->setPosition(lerpPos);
	}

	Vec2 newVec = m_targetPos - myPos;

	float distance = myPos.distance(m_targetPos);
	curVec.smooth(newVec,100,1 + distance); 
	curVec.normalize();
	_physicsBody->setVelocity(curVec * m_FlySpeed);

	if (m_FlySpeed < 2000)
	{
		m_FlySpeed += 50;
	}
}

void FireworkBullet::baidong( float t )
{
	Vec2 curVec = _physicsBody->getVelocity();
	
	if (m_direct == 1)
	{
		m_direct = -1;
	}
	else
	{
		m_direct = 1;
	}
	_physicsBody->setVelocity(Vec2(200 * m_direct, curVec.y * 1.5));
}

void FireworkBullet::StartBaidong()
{
	schedule(schedule_selector(FireworkBullet::baidong),0.1);	
}

void FireworkBullet::DoSomethingBeforeDead()
{
	auto delay = DelayTime::create(0.5f);
	auto func = CallFuncN::create(this,callfuncN_selector(FireworkBullet::Dying));
	auto seq = Sequence::create(delay, func, nullptr);
	m_particle1->runAction(seq);
}

void FireworkBullet::Dying(Node* node)
{
	if (node)
	{
		node->removeFromParentAndCleanup(true);
	}
}
