#include "weapon.h"
#include "../BasePlane/BasePlane.h"
#include "../../GameCommon/GameDefine.h"
#include "../../GameCommon/GameData.h"
#include "../Bullet/Bullet.h"
#include "../Bullet/BulletFactory.h"
#include "../Layer/UIController.h"
#include "../Layer/RollingBg.h"
#include "../BasePlane/PlaneManager.h"
#include "spine/extension.h"
#include "../../GameCommon/EffectsManager.h"
#include "../Layer/PhysicsLayer.h"


Weapon::Weapon()
	:m_isShootAble(true)
	,m_ShootTimes(-1)
	,m_curShootTimes(0)
	,m_WeaponParent(nullptr)
	,m_shootSpeed(0.1f)
	,m_delayShoot(0.0f)
	,m_isSwing(false)
{
	m_winSize = Director::getInstance()->getWinSize();
}

Weapon* Weapon::create( Node* pl )
{
	Weapon* wp = new (std::nothrow) Weapon;
	if (wp && wp->init())
	{
		wp->m_CreateType = CreateType::Native;
		wp->autorelease();
		wp->setWeaponParent(pl);
		return wp;
	}
	else
	{
		CC_SAFE_DELETE(wp);
		return nullptr;
	}
}

Weapon* Weapon::createByConfig( Node* pl, WeaponDelegate config )
{
	Weapon* wp = new (std::nothrow) Weapon;
	if (wp && wp->init())
	{
		wp->m_CreateType = CreateType::Config;
		wp->autorelease();
		wp->initWeaponWithConfig(config);
		wp->setWeaponParent(pl);
		return wp;
	}
	else
	{
		CC_SAFE_DELETE(wp);
		return nullptr;
	}
}

void Weapon::onEnter()
{
	Node::onEnter();
	if (m_ConfigIsAmmi)
	{
		//朝向主飞机
		auto mainPl = GameData::getInstance()->getMainPlane();
		if (mainPl)
		{
			auto plPos = mainPl->convertToWorldSpaceAR(Vec2::ZERO);
			auto myPos = convertToWorldSpaceAR(Vec2::ZERO);

			auto vec = plPos - myPos;
			float angle = vec.getAngle() * RAD_DEG;
			setRotation(-angle);
		}
	}
}

void Weapon::setBullstType(const BulletType& tp)
{
	m_bulletType = tp;
}

void Weapon::shoot(float dt)
{
	ShootOneBullet();
}

void Weapon::setShootSpeed(float newSpeed )
{
	m_shootSpeed = newSpeed;
}

void Weapon::initWeaponWithConfig(WeaponDelegate config)
{
	m_BltScaleX = config.getBltScaleX();
	m_BltScaleY = config.getBltScaleY();
	m_ConfigWeaponAngle = config.getConfigWeaponAngle();
	m_BltSpeed = config.getBltSpeed();
	m_ConfigShootInterval = config.getConfigShootInterval();
	m_ConfigWeaponDuration = config.getConfigWeaponDuration();
	m_ConfigWeaponOffsetX = config.getConfigWeaponOffsetX();
	m_ConfigWeaponOffsetY = config.getConfigWeaponOffsetY();
	m_ConfigWeaponRotationEachShoot = config.getConfigWeaponRotationEachShoot();
	m_ConfigWeaponMaxRotattion = config.getConfigWeaponMaxRotattion();

	m_ConfigWpNum = config.getConfigWpNum();
	m_ConfigExDis = config.getConfigExDis();
	m_ConfigZhenFu = config.getConfigZhenFu();
}

void Weapon::setDelayShoot(float dt)
{
	m_delayShoot = dt;
}

void Weapon::setShootTimes(int shootTimes)
{
	m_ShootTimes = shootTimes;
}

void Weapon::removeWeapon()
{
	//如果武器加载的对象是飞机，那么由飞机删除，否则，自己删除
	BasePlane* plane = dynamic_cast<BasePlane*>(m_WeaponParent);
	if (plane)
	{
		plane->RemoveOneWeapon(this);
	}
	else
	{
		removeFromParentAndCleanup(true);
	}
}

void Weapon::setOffset( const Vec2& offset )
{
	Vec2 pos = getPosition();
	setPosition(pos + offset);
}

void Weapon::StartShoot()
{
	switch (m_CreateType)
	{
	case CreateType::Native:
		if (!m_isSwing)
		{
			schedule(schedule_selector(Weapon::shoot), m_shootSpeed, CC_REPEAT_FOREVER, m_delayShoot);
		}
		break;
	case CreateType::Config:
		{
			if (m_ConfigWpNum <= 1)
			{
				setPosition(m_ConfigWeaponOffsetX, m_ConfigWeaponOffsetY);
			}

			//删除
			if (m_ConfigWeaponDuration > 0.0f)
			{
				auto dur = DelayTime::create(m_ConfigWeaponDuration);
				auto rm = CallFunc::create(this, callfunc_selector(Weapon::removeWeapon));
				auto seq = Sequence::create(dur, rm, nullptr);
				runAction(seq);
			}

			//发射时的角度
			this->setRotation(m_ConfigWeaponAngle);

			//扫射
			if (m_ConfigWeaponRotationEachShoot > 0.0f)
			{
				float t = fabs(m_ConfigWeaponMaxRotattion * m_ConfigShootInterval/m_ConfigWeaponRotationEachShoot);
				int direct = 1;
				if (m_ConfigWeaponRotationEachShoot < 0)
				{
					direct = -1;
				}
				auto rot = RotateBy::create(t, m_ConfigWeaponMaxRotattion * direct);
				auto back = rot->reverse();
				auto seq = Sequence::create(rot, back, nullptr);
				auto rep = RepeatForever::create(seq);
				runAction(rep);
			}
			if (!m_isSwing)
			{
				schedule(schedule_selector(Weapon::shoot), m_ConfigShootInterval, CC_REPEAT_FOREVER, m_delayShoot);
			}
		}

		break;
	default:
		break;
	}
}

void Weapon::PauseShoot( float pauseTime )
{
	m_isShootAble = false;

	auto pauseDur = DelayTime::create(pauseTime);
	auto func = CallFunc::create(this,callfunc_selector(Weapon::ResumeShoot));
	auto seq = Sequence::create(pauseDur, func, nullptr);
	auto rep = RepeatForever::create(seq);
	runAction(rep);
}

void Weapon::ResumeShoot()
{
	m_isShootAble = true;
}

void Weapon::ShootCrisscross( float interval, float offsetX )
{
	m_isSwing = true;

	auto shootBullet = CallFunc::create(this,callfunc_selector(Weapon::ShootOneBullet));
	auto mv = MoveBy::create(interval, Vec2(offsetX, 0));
	auto back = mv->reverse();
	auto seq = Sequence::create(mv, shootBullet, back, shootBullet, nullptr);
	auto rep = RepeatForever::create(seq);
	runAction(rep);
}

void Weapon::ShootOneBullet()
{
	if (!m_isShootAble)
	{
		return;
	}

	if (m_ShootTimes > 0)
	{
		if (m_curShootTimes >= m_ShootTimes )
		{
			removeWeapon();
			return;
		}
	}

	Bullet* bullet = GameData::getInstance()->getBulletFactory()->createBullet(this, m_bulletType, m_BltResTag);
	if (!m_BltFrameName.empty())
	{
		SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(m_BltFrameName);
		bullet->setSpriteFrame(frame);
		int bitMaskCate = bullet->getPhysicsBody()->getCategoryBitmask();
		int bitMaskColli = bullet->getPhysicsBody()->getCollisionBitmask();
		int bitMaskCont = bullet->getPhysicsBody()->getContactTestBitmask();
		bullet->initBody();
		bullet->setBitmask(bitMaskCate, bitMaskColli, bitMaskCont);
	}
	if (m_BltTailName != "null")
	{
		EffectsManager::addParticle(m_BltTailName,bullet,Vec2(bullet->getContentSize().width*0.5,5),false,-1.0f, m_BltZorder);
	}
	bullet->setWeaponDelegate(this);
	//子弹创建后根据武器的当前朝向，旋转相应的角度

	bullet->FixAngleWithWeapon(this);

	if(fabs(m_BltSelfRotate) > 0)
	{
		auto rotate = RotateBy::create(1.0f, m_BltSelfRotate);
		auto rep = RepeatForever::create(rotate);
		bullet->runAction(rep);
	}

	if (m_BltIsToPlane)
	{
		Size plSize = m_WeaponParent->getContentSize();
		m_WeaponParent->addChild(bullet);
		bullet->setZOrder(m_BltZorder);
		bullet->setPosition(plSize.width*0.5,-30);
	}
	else
	{
		Point pos = this->convertToWorldSpaceAR(Point::ZERO);
		bullet->setPosition(pos);
		bullet->setZOrder(m_BltZorder);
		if (!m_BulletLayer)
		{
			m_BulletLayer = UIController::getInstance()->getEffectsLayer();
		}
		m_BulletLayer->addChild(bullet);
	}
	m_curShootTimes++;
}



//---------------------------
AutoWeapon::AutoWeapon()
{
	m_isLeft = false;
	m_winSize = Director::getInstance()->getWinSize();
}

AutoWeapon* AutoWeapon::create( bool isEnemy /*= false*/ )
{
	AutoWeapon* wp = new (std::nothrow) AutoWeapon;
	if (wp && wp->init())
	{
		wp->autorelease();
		return wp;
	}
	else
	{
		CC_SAFE_DELETE(wp);
		return nullptr;
	}
}

void AutoWeapon::onEnter()
{
	Weapon::onEnter();
	schedule(schedule_selector(AutoWeapon::SeachTarget));
}

void AutoWeapon::SeachTarget( float dt )
{
	//先按距离排序
	GameData::getInstance()->SortByDis();

	std::list<PhysicsUnit*> Planelist = GameData::getInstance()->getEnemyPlaneVec();
	std::list<PhysicsUnit*>::iterator it = Planelist.begin();
	Point targetPos = Vec2(0,0);
	if (Planelist.empty())
	{
		this->setRotation(0);
		setBullstType(BulletType::Self_Default);
		return;
	}

	bool isFound = false;
	if (m_isLeft)
	{
		for (it; it != Planelist.end(); it++)
		{
			targetPos = (*it)->getPosition();
			if (targetPos.x <= m_winSize.width*0.5 && targetPos.y > m_winSize.height*0.15 + 100)
			{
				isFound = true;
				break;
			}
		}
	}
	else
	{
		for (it; it != Planelist.end(); it++)
		{
			targetPos = (*it)->getPosition();
			if (targetPos.x >= m_winSize.width*0.5 && targetPos.y > m_winSize.height*0.15 + 100)
			{
				isFound = true;
				break;
			}
		}
	}

	//根据目标位置和自己的位置，计算需要旋转的角度
	Point myPos = this->convertToWorldSpaceAR(Point::ZERO);

	Vec2 newVec = targetPos - myPos;
	float r = newVec.getAngle() * Pi2Angle;
	r = 90 - r;

	if (r > 60)
	{
		r = 60;
	}
	else if(r < -60)
	{
		r = -60;
	}
	if (!isFound)
	{
		this->setRotation(0);
	}
	else
	{
		this->setRotation(r);
	}
	setBullstType(BulletType::Self_Default);
}
