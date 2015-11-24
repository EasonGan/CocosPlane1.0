#include "BasePlane.h"
#include "../Weapon/weapon.h"
#include "../../GameCommon/GameData.h"
#include "../Layer/UIController.h"
#include "../../GameCommon/EffectsManager.h"
#include "../AI.h"
#include "../../GameUI/GameMainLayer.h"
#include "../../JniHelp/CppCallJava.h"
#include "../../GameCommon/GameDefine.h"
#include "../Layer/PhysicsLayer.h"
#include "../../GameCommon/GameConfig.h"
#include "../BossTimeOut.h"
#include "../SpecialBody.h"
#include "../Layer/RollingBg.h"
#include "../DropGoods.h"
#include "../EnemyPlaneController.h"
#include "../Weapon/EditorWeapon.h"
#include "BossHpBar.h"
#include "../ContactManager.h"
#include "../DropGoods.h"
#include "../../GameUI/GameSceneMgr.h"
#include "../PlaneExplosion.h"

using namespace experimental;

#define BossBarTag "BossHpBar"
#define WudiActionTag	150928

BasePlane::BasePlane()
	:m_ShootMusicAble(true)
	,m_HpBar(nullptr)
	,m_isNeedFlyToMainPl(false)
	,m_IsShootAble(true)
	,m_FeignDeath(false)
	,func(nullptr)
//	,m_ConfigDanmuId(-1)
//	,m_ConfigFIreInterval(-1)
{
	m_Musicfile = "";
}

void BasePlane::setBitmask(const int& cate,const int& collision,const int& test )
{
	_physicsBody->setCategoryBitmask(cate);
	_physicsBody->setCollisionBitmask(collision);
	_physicsBody->setContactTestBitmask(test);
}

void BasePlane::initBody()
{
	auto body = PhysicsBody::createBox(Size(80,80));
	body->setGravityEnable(false);
	body->setRotationEnable(false);
	body->setMass(1000000);
	this->setPhysicsBody(body);
}

void BasePlane::FuncAfterEnter()
{
	
}

void BasePlane::AnimationWhenDead()
{
	//UIController::getInstance()->getEffectsLayer()->displayDeathExplosion(this->convertToWorldSpaceAR(Point::ZERO));
}

void BasePlane::setShootAble( bool b )
{
	m_IsShootAble = b;
	setShootMusicAble(b);

	for (int i = 0; i < m_NorWpVec.size(); i++)
	{
		Weapon* wp = m_NorWpVec.at(i);
		if (wp)
		{
			wp->setShootAble(b);
		}
	}

	for (int i = 0; i < m_TempWpVec.size(); ++i)
	{
		Weapon* wp = m_TempWpVec.at(i);
		if (wp)
		{
			wp->setShootAble(b);
		}
	}

	for (int i = 0; i < m_SpecialWpVec.size(); ++i)
	{
		Weapon* wp = m_SpecialWpVec.at(i);
		if (wp)
		{
			wp->setShootAble(b);
		}
	}
}

void BasePlane::OtherDealWhenContact( const BasePro& rs )
{
	if (rs.getMyId() == UnitId::eLightAttackPoint)
	{
		UIController::getInstance()->getEffectsLayer()->displayLightingEff(this);
	}
}

void BasePlane::removePartical( Node* node )
{
	node->removeFromParentAndCleanup(true);
}

void BasePlane::WudiOver(float dt)
{
	setInvincible(0);
	unschedule(schedule_selector(BasePlane::WudiOver));
}

void BasePlane::setBltHurts( int hurts )
{
	m_BasePro.setHurts(hurts);
}

void BasePlane::setDefense( int defense )
{
	m_BasePro.setDefense(defense);
}

void BasePlane::PlayMusicWhenDead()
{
	if (this->m_Musicfile.empty()) {
		int randNum = random(1, 5);
		switch (randNum) {
			case 1:
				this->m_Musicfile = Music_PlDead1;
				break;
			case 2:
				this->m_Musicfile = Music_PlDead2;
				break;
			case 3:
				this->m_Musicfile = Music_PlDead3;
				break;
			case 4:
				this->m_Musicfile = Music_PlDead4;
				break;
			case 5:
				this->m_Musicfile = Music_PlDead5;
				break;

			default:
				break;
		}
	}
	NewAudio::getInstance()->playEffect(this->m_Musicfile);
}

void BasePlane::DoSomethingBeforeDead()
{
	if(this->func != nullptr)
	{
		this->func(this);
	}
}

void BasePlane::FlyToMainPlaneWhenSpeedUp()
{
	//schedule(schedule_selector(BasePlane::CheckIsSpeedUp));
}

void BasePlane::FlyToMainPlane(float dt)
{
	/*bool isSpeedUp = GameData::getInstance()->getisSpeedUp();
	if (!isSpeedUp)
	{
		unschedule(schedule_selector(BasePlane::FlyToMainPlane));

		ActionInterval* mv = MoveTo::create(2.0f, Vec2(360,-100));
		runAction(mv);
		return;
	}
	stopAllActions();
	
	Point myPos = this->convertToWorldSpaceAR(Point::ZERO);
	myPos.y -= 30;

	setPosition(myPos);*/
}

void BasePlane::setDeadMusic( const std::string& file )
{
	m_Musicfile = file;
}

void BasePlane::setShootMusicAble( bool b )
{
	m_ShootMusicAble = b;
}

cocos2d::Vec2 BasePlane::getArmatPosOffset()
{
	return Vec2(0,0);
}

Size BasePlane::getSkinSize()
{
	return m_SkinSize;
}

void BasePlane::ShowHpBar()
{
	m_HpBar = HpBar::create(this);
	m_HpBar->setPosition(0, m_SkinSize.height * 0.5 + 5);
	addChild(m_HpBar);
}

void BasePlane::updateHpBarHeight(float height) {
	Vec2 oldPos = this->m_HpBar->getPosition();
	this->m_HpBar->setPosition(Vec2(oldPos.x,oldPos.y + height));
}

void BasePlane::FuncWhenHpChanged()
{
	if (!m_HpBar)
	{
		return;
	}
	float percent = m_BasePro.getCurHp()* 1.0 / m_BasePro.getMaxHp()* 100;
	m_HpBar->RefreshHpBar(percent);
}

void BasePlane::DeathImpactOfDrops()
{
	GameData::getInstance()->addScore(m_PlanePro.getScore());
	GameData::getInstance()->addGoldNum(m_PlanePro.getGold());
}

void BasePlane::AddOneWp( Weapon* weapon,WeaponType wt )
{
	weapon->setWeaponType(wt);

	switch (wt)
	{
	case WeaponType::Normal:
		m_NorWpVec.push_back(weapon);
		break;
	case WeaponType::Replace:
		if (!m_NorWpVec.empty())
		{
			for (auto it = m_NorWpVec.begin(); it < m_NorWpVec.end(); it++)
			{
				Weapon* wp = (*it);
				wp->setShootAble(false);
			}
		}
		m_TempWpVec.push_back(weapon);
		break;
	case WeaponType::Addition:
		m_SpecialWpVec.push_back(weapon);
		break;
	default:
		break;
	}
	weapon->StartShoot();
	addChild(weapon);
}

void BasePlane::AddWpVec( std::vector<Weapon*> wpVec,WeaponType wt )
{
	switch (wt)
	{
	case WeaponType::Normal:
		for (auto it = wpVec.begin(); it < wpVec.end(); it++)
		{
			Weapon* wp = (*it);
			wp->setWeaponType(wt);
			wp->StartShoot();
			addChild(wp);
			m_NorWpVec.push_back(wp);
		}
		break;
	case WeaponType::Replace:
		if (!m_NorWpVec.empty())
		{
			for (auto it = m_NorWpVec.begin(); it < m_NorWpVec.end(); it++)
			{
				Weapon* wp = (*it);
				wp->setShootAble(false);
			}
		}
		for (auto it = wpVec.begin(); it < wpVec.end(); it++)
		{
			Weapon* wp = (*it);
			wp->setWeaponType(wt);
			wp->StartShoot();
			addChild(wp);
			m_TempWpVec.push_back(wp);
		}
		break;
	case WeaponType::Addition:
		for (auto it = wpVec.begin(); it < wpVec.end(); it++)
		{
			Weapon* wp = (*it);
			wp->setWeaponType(wt);
			wp->StartShoot();
			addChild(wp);
			m_SpecialWpVec.push_back(wp);
		}
		break;
	default:
		break;
	}

}

void BasePlane::RemoveOneWeapon( Weapon* weapon )
{
	auto wt = weapon->getWeaponType();
	switch (wt)
	{
	case WeaponType::Normal:
		for (auto it = m_NorWpVec.begin(); it < m_NorWpVec.end(); it++)
		{
			Weapon* wp = (*it);
			if (wp == weapon)
			{
				m_NorWpVec.erase(it);
				break;
			}
		}
		break;
	case WeaponType::Replace:
		for (auto it = m_TempWpVec.begin(); it < m_TempWpVec.end(); it++)
		{
			Weapon* wp = (*it);
			if (wp == weapon)
			{
				m_TempWpVec.erase(it);
				break;
			}
		}
		break;
	case WeaponType::Addition:
		for (auto it = m_SpecialWpVec.begin(); it < m_SpecialWpVec.end(); it++)
		{
			Weapon* wp = (*it);
			if (wp == weapon)
			{
				m_SpecialWpVec.erase(it);
				break;
			}
		}
		break;
	default:
		break;
	}
	if (weapon)
	{
		weapon->removeFromParentAndCleanup(true);
	}
}

void BasePlane::resumeNorWp()
{
	for (auto it = m_TempWpVec.begin(); it != m_TempWpVec.end(); ++it)
	{
		Weapon* wp = (*it);
		if (wp)
		{
			wp->removeFromParentAndCleanup(true);
		}
	}
	m_TempWpVec.clear();

	for (auto it = m_NorWpVec.begin(); it < m_NorWpVec.end(); it++)
	{
		auto wp = (*it);
		wp->setShootAble(true);
	}

}

void BasePlane::clearWeapon()
{
	for (auto it = m_SpecialWpVec.begin(); it != m_SpecialWpVec.end(); ++it)
	{
		Weapon* weapon = (*it);
		weapon->removeFromParentAndCleanup(true);
	}
	for (auto it = m_NorWpVec.begin(); it != m_NorWpVec.end(); ++it)
	{
		Weapon* weapon = (*it);
		weapon->removeFromParentAndCleanup(true);
	}
	for (auto it = m_TempWpVec.begin(); it != m_TempWpVec.end(); ++it)
	{
		Weapon* weapon = (*it);
		weapon->removeFromParentAndCleanup(true);
	}
	m_SpecialWpVec.clear();
	m_TempWpVec.clear();
	m_NorWpVec.clear();
}

void BasePlane::onExit()
{
	PhysicsUnit::onExit(); 
	clearWeapon();
}

void BasePlane::addRewards( float HpPct, int wardNum, DropGoodsType dgt )
{
	auto drop = DropReward(HpPct, wardNum, dgt);
	m_PlanePro.DropVec.push_back(drop);
}

void BasePlane::setNeedFlyToMainPl( bool b )
{
	m_isNeedFlyToMainPl = b;
}

void BasePlane::StartShoot()
{
	for (int i = 0; i < m_NorWpVec.size(); i++)
	{
		Weapon* wp = m_NorWpVec.at(i);
		if (wp)
		{
			wp->StartShoot();
		}
	}

	for (int i = 0; i < m_TempWpVec.size(); ++i)
	{
		Weapon* wp = m_TempWpVec.at(i);
		if (wp)
		{
			wp->StartShoot();
		}
	}

	for (int i = 0; i < m_SpecialWpVec.size(); ++i)
	{
		Weapon* wp = m_SpecialWpVec.at(i);
		if (wp)
		{
			wp->StartShoot();
		}
	}
}

void BasePlane::DeathImpactOfDropReward()
{
	std::string dropArray = this->getDropArray();
	if(dropArray != "") {
		this->initDropVecFromDropArr(dropArray);
	}
	auto vec = getDropVec(m_PlanePro.DropVec);
	m_PlanePro.DropVec = vec;

	if(m_DeadType == DeadType::Dead_Kill)
	{
		NewDataMgr::getInstance()->mGameTaskSystem->_GamePropertyArr[GamePrKey_killMonNum] += 1;
	}
}


void BasePlane::initDropVecFromDropArr(std::string dropArray)
{

    std::string str = dropArray;
    std::stringstream ss(str);
    std::string sub_str;
    if(str.find("|") != -1) {
        int index = 0;
        while (getline(ss, sub_str, '|')) {
            if(sub_str == "") {
				index++;
                continue;
            }

            if((index == 1 && EnemyPlaneController::getInstance()->isFirst) || (index == 0 && !EnemyPlaneController::getInstance()->isFirst)) {
				index++;
                continue;
            }

            std::string str2 = sub_str;
            std::stringstream ss2(str2);
            std::string sub_str2;
            if(str2.find("#") != -1) {
                while (getline(ss2, sub_str2, '#')) {
                    parseOneItem(sub_str2);
                }
            } else {
                parseOneItem(str2);
            }

            index++;
        }
    }
}

void BasePlane::parseOneItem(std::string dropArray)
{
    float dropNum = 0;
    int itemId = 0;
    int num = 0;
    std::string str = dropArray;
    std::stringstream ss(str);
    std::string sub_str;
    if(str.find(",") != -1) {
        int index = 0;
        while (getline(ss, sub_str, ',')) {
            switch (index) {
                case 0:
                    itemId = atoi(sub_str.c_str());
                    break;
                case 1:
                    num = atoi(sub_str.c_str());
                    break;
                case 2:
                    dropNum = atof(sub_str.c_str());
                    break;
            }
            index++;
        }
    }

//    for(int i = 0 ; i < num ; i++) {
        if(dropNum == 100 || random(1,100) <= dropNum) {
            this->addDropItemToVec(itemId,num);
        }
//    }
}

void BasePlane::setDestroyCallFun(const std::function<void(BasePlane*)> &func)
{
	this->func = func;
}

void BasePlane::addDropItemToVec(int itemId,int num)
{
    DropReward reward;
    reward.DropTp = GameData::getInstance()->getDropsFactory()->getGoodsTypeFromItemId(itemId);
    reward.nums = num;
    m_PlanePro.DropVec.push_back(reward);
}


//---------------------------------
MainPlane::MainPlane()
{
	m_SpeedUpNode = nullptr;
	m_SpeedUpTail = nullptr;
	m_NorAnimationNode = nullptr;
	m_Protect = nullptr;
	m_IsHasMagnet = false;
	m_isMoveAniPlayingL = false;
	m_isMoveAniPlayingR = false;
	m_isFinished = true;
	m_isTouchEnabled = true;
	m_isNeedTrackParticle = false;
	m_shootMusic = "";
	m_TempPosX = 0.0f;
	m_WpLvl = 1;
	m_bitMaskCate = SELF_PLANE_CATEGORYBITMASK;
	m_bitMaskColl = 0;
	m_bitMaskCont = SELF_PLANE_CONTACTTESTBITMASK;
}

MainPlane* MainPlane::create( PlaneType pt )
{
	MainPlane* plane = new (std::nothrow) MainPlane;
	if(plane && plane->initWithFile("weapon.png"))
	{
		plane->setPlaneType(pt);
		plane->initBody();
		plane->InitSkin();
		plane->ShowHpBar();
		plane->autorelease();
		return plane;
	}
	CC_SAFE_DELETE(plane);
	return nullptr;
}

void MainPlane::onEnter() {
	Sprite::onEnter();
	//PhysicsUnitManager::getInstance()->AddUnit(this);
	if(this->m_isNeedTrackParticle) {
		this->initMoveInParticle();
	}
	setInvincible(true);
	FuncAfterEnter();
	initMoveEdge();

	if (GameSceneType::Scene_Type_PlayInterface == GameSceneMgr::getInstance()->getCurScen())
	{
		auto touchListener = EventListenerTouchOneByOne::create();
		touchListener->onTouchBegan = CC_CALLBACK_2(MainPlane::onTouchBegan, this);
		touchListener->onTouchMoved = CC_CALLBACK_2(MainPlane::onTouchMoved, this);
		touchListener->onTouchEnded = CC_CALLBACK_2(MainPlane::onTouchEnded, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	}

	schedule(schedule_selector(MainPlane::setBody),0.1);

	Revive();
	receiveRewards();
	ShootMusic();

}

void MainPlane::DoSomethingBeforeDead()
{
	CppCallJava::getInstance()->vibrate(400);
	GameData::getInstance()->setMainPlane(nullptr);
    UIController::getInstance()->getMainUILayer()->consumeLife();
	NewDataMgr::getInstance()->mGameTaskSystem->_GamePropertyArr[GamePrKey_DeadNum] += 1;

	CppCallJava::getInstance()->vibrate(350);
	int lifeNum = NewDataMgr::getInstance()->getGoodsNumWithID(GoodsID::GoodsID_Life);
	if (lifeNum < 1)
	{
		GameData::getInstance()->setAllEnemyPlaneInvincible(true);
		EnemyPlaneController::getInstance()->mainPlaneDeathPause();
		UIController::getInstance()->getMainUILayer()->showBuyFuHuoWidget();
		auto pos = convertToWorldSpaceAR(Vec2::ZERO);
		pos = Vec2(pos.x + rand_minus1_1() * 100, pos.y + rand_minus1_1() * 100);

		for (int i = 0; i < 20; ++i)
		{
			auto drop = GameData::getInstance()->getDropsFactory()->createDropItem(pos,5,DropMod::Drop_Mod,false,false);
			drop->ListenMainplFuhuo();
			drop->setMagnetDis(1600);
			drop->setInvincibleAtEdge();
			drop->setDelayRemoveTime(0.01);
			auto body = drop->getPhysicsBody();
			body->setCategoryBitmask(REWARD_CATEGORYBITMASK);
			body->setCollisionBitmask(REWARD_COLLISIONBITMASK_WALL);
			body->setContactTestBitmask(REWARD_CONTACTTESTBITMASK_WALL);
			body->setGravityEnable(false);
			body->setVelocity(Vec2(rand_minus1_1() * 360, rand_minus1_1() * 640));
			body->applyForce(Vec2(0,1500));
		}
		for (int i = 0; i < 20; ++i)
		{
			auto drop = GameData::getInstance()->getDropsFactory()->createDropItem(pos,6,DropMod::Drop_Mod,false,false);
			drop->ListenMainplFuhuo();
			drop->setMagnetDis(1600);
			drop->setInvincibleAtEdge();
			drop->setDelayRemoveTime(0.01);
			auto body = drop->getPhysicsBody();
			body->setCategoryBitmask(REWARD_CATEGORYBITMASK);
			body->setCollisionBitmask(REWARD_COLLISIONBITMASK_WALL);
			body->setContactTestBitmask(REWARD_CONTACTTESTBITMASK_WALL);
			body->setGravityEnable(false);
			body->setVelocity(Vec2(rand_minus1_1() * 360, rand_minus1_1() * 640));
			body->applyForce(Vec2(0,1500));
		}
		for (int i = 0; i < 20; ++i)
		{
			auto drop = GameData::getInstance()->getDropsFactory()->createDropItem(pos,7,DropMod::Drop_Mod,false,false);
			drop->ListenMainplFuhuo();
			drop->setMagnetDis(1600);
			drop->setInvincibleAtEdge();
			drop->setDelayRemoveTime(0.01);
			auto body = drop->getPhysicsBody();
			body->setCategoryBitmask(REWARD_CATEGORYBITMASK);
			body->setCollisionBitmask(REWARD_COLLISIONBITMASK_WALL);
			body->setContactTestBitmask(REWARD_CONTACTTESTBITMASK_WALL);
			body->setGravityEnable(false);
			body->setVelocity(Vec2(rand_minus1_1() * 360, rand_minus1_1() * 640));
			body->applyForce(Vec2(0,1500));
		}
		auto drop = GameData::getInstance()->getDropsFactory()->createDropItem(pos,8,DropMod::Drop_Mod,false,false);
		drop->ListenMainplFuhuo();
		drop->setMagnetDis(1600);
		drop->setInvincibleAtEdge();
		drop->setDelayRemoveTime(0.01);
		auto body = drop->getPhysicsBody();
		body->setCategoryBitmask(REWARD_CATEGORYBITMASK);
		body->setCollisionBitmask(REWARD_COLLISIONBITMASK_WALL);
		body->setContactTestBitmask(REWARD_CONTACTTESTBITMASK_WALL);
		body->setGravityEnable(false);
		body->setVelocity(Vec2(rand_minus1_1() * 360, rand_minus1_1() * 640));
		body->applyForce(Vec2(0,1500));
	}
	else
	{
		if(UIController::getInstance()->getMainUILayer()->shieldIsEnough())
		{
			UIController::getInstance()->getPhysicsLayer()->addPlaneWithProtect();
		}
		else
		{
			UIController::getInstance()->getPhysicsLayer()->addPlane();
		}
	}
}

void MainPlane::AnimationWhenDead()
{
	UIController::getInstance()->getEffectsLayer()->displayDeathExplosion(this->convertToWorldSpaceAR(Point::ZERO));
}

void MainPlane::initBody()
{
	auto body = PhysicsBody::createBox(Size(30,50));
	body->setPositionOffset(Vec2(0,0));
	body->setGravityEnable(false);
	body->setRotationEnable(false);
	body->setMass(100000);
	setPhysicsBody(body);
	setBitmask(m_bitMaskCate,m_bitMaskColl,m_bitMaskCont);
}

void MainPlane::delayAddParticle(float time,std::string particlePath,Node* node)
{
	this->runAction(Sequence::create(DelayTime::create(time),CallFunc::create(CC_CALLBACK_0(MainPlane::toAddParticle,this,particlePath,node)), nullptr));
}

void MainPlane::toAddParticle(std::string particlePath,Node* node)
{
	Size s = node->getContentSize();
	EffectsManager::addParticle(particlePath, node, Vec2(s.width*0.5, s.height*0.5), false);
}

void MainPlane::delayAddNode(float time,float endTime)
{
	if (m_SpeedUpNode)
	{
		m_SpeedUpNode->runAction(Sequence::create(DelayTime::create(time),CallFunc::create(CC_CALLBACK_0(MainPlane::addTheNode,this,endTime)), nullptr));
	}
}

void MainPlane::addTheNode(float endTime)
{
	if(this->getPhysicsBody()->getPosition().y < 980) {
		this->runAction(MoveBy::create(0.2,Vec2(0,150)));
		GameData::getInstance()->GameSpeedUp();
	}
	this->setTouchAble(true);
	this->m_RechargeNode->removeFromParent();
	SpeedUpProtect* sp = SpeedUpProtect::create();
	sp->setPosition(0,-getContentSize().height*0.3);
	m_SpeedUpNode->addChild(sp);

	m_SpeedUpTail = Node::create();
	m_SpeedUpTail->setPosition(35,-250);
	addChild(m_SpeedUpTail,-1);
	sp->addTail(m_SpeedUpTail);
	m_SpeedUpTail->setScale(1.2f);
	{
		ActionInterval* scale = ScaleTo::create(1.0f,1.2f);
		runAction(scale);
	}

	if (endTime > 0)
	{
		ActionInterval* delay = DelayTime::create(endTime);
		CallFunc* func = CallFunc::create(this,callfunc_selector(MainPlane::removeSpeedUpEffect));
		ActionInterval* seq = Sequence::create(delay, func ,nullptr);
		m_SpeedUpNode->runAction(seq);
	}
}

void MainPlane::setNeedTrackParticle(bool isNeed)
{
	this->m_isNeedTrackParticle = isNeed;
}

void MainPlane::runReadyAction(float dur)
{
//	this->setTouchAble(false);
	//this->delayAddNode(2,dur);
	if(this->getPhysicsBody()->getPosition().y > 240) {
		this->runAction(MoveBy::create(1.5,Vec2(0,-150)));
	}
}

void MainPlane::addSpeedUpEffect(float dur)
{
	setTouchAble(false);
	setShootAble(false);

	return;

	if (!m_SpeedUpNode)
	{
		m_SpeedUpNode = Node::create();
		m_SpeedUpNode->setPosition(getContentSize().width*0.5,getContentSize().height + 30);
		addChild(m_SpeedUpNode,1);

		this->m_RechargeNode = Node::create();
		m_RechargeNode->setPosition(getContentSize().width*0.5,getContentSize().height + 30);
		this->addChild(this->m_RechargeNode,2);
		this->delayAddParticle(0,"particla/recharge/test1.plist",this->m_RechargeNode);
		this->delayAddParticle(0.5,"particla/recharge/test2.plist",this->m_RechargeNode);
		this->delayAddParticle(0.9,"particla/recharge/test3.plist",this->m_RechargeNode);

		if(!this->m_Protect) {
			this->runAction(Sequence::create(DelayTime::create(0.9), CallFunc::create(CC_CALLBACK_0(MainPlane::AddProtect,this)),DelayTime::create(0.8),CallFunc::create(CC_CALLBACK_0(MainPlane::runReadyAction,this,dur)), nullptr));
		} else {
			this->runAction(Sequence::create(DelayTime::create(0.9), CallFunc::create(CC_CALLBACK_0(MainPlane::runReadyAction,this,dur)), nullptr));
		}

		setTouchAble(false);
		setShootAble(false);
	}
}

void MainPlane::removeSpeedUpEffect()
{
	if (m_SpeedUpNode)
	{
//		ExplisonScreen* exp = ExplisonScreen::create();
//		exp->setPosition(360,640);
//		UIController::getInstance()->getEffectsLayer()->addChild(exp);
//
//		GameData::getInstance()->setUpdateSpeed(1.0f);
//		setShootAble(true);
//		setTouchAble(true);

		m_SpeedUpNode->removeFromParentAndCleanup(true);
		m_SpeedUpNode = nullptr;
		m_SpeedUpTail->removeFromParentAndCleanup(true);
		m_SpeedUpTail = nullptr;
//		m_RechargeNode->removeFromParent();
		m_RechargeNode = nullptr;

		{
//			ActionInterval* scale = ScaleTo::create(1.0f,1.0f);
//			runAction(scale);
		}
	}
}


void MainPlane::FlyUp()
{
	setTouchAble(false);
	setInvincible(true);

	ActionInterval* moveUp = MoveBy::create(1.0,ccp(0,1700));
	CallFunc* func = CallFunc::create(this,callfunc_selector(MainPlane::FlyUpCallback));
	ActionInterval* seq = Sequence::create(moveUp,func,nullptr);
	runAction(seq);
}

void MainPlane::FlyUpCallback()
{
	GameData::getInstance()->setUpdateSpeed(1.0f);
	UIController::getInstance()->getMainUILayer()->setUserItemNumWhenGameEnd(true);
	UIController::getInstance()->ShowResultLayer(true);
}

void MainPlane::Revive()
{
	stopAllActionsByTag(WudiActionTag);
	setInvincible(1);

	ActionInterval* delay = DelayTime::create(2.0);
	CallFuncN* funcN = CallFuncN::create(this,callfuncN_selector(MainPlane::WudiOver));
	ActionInterval* seq = Sequence::create(delay, funcN, nullptr);
	seq->setTag(WudiActionTag);
	runAction(seq);
}

void MainPlane::WudiOver( Node* node )
{
	setInvincible(false);
}

void MainPlane::OtherDealWhenContact( const BasePro& rs )
{
	
}

void MainPlane::AddProtect()
{
	if (m_Protect)
	{
		return;
	}
	
	m_Protect = PlaneProtect::create();
	m_Protect->setPosition(5,20);
	addChild(m_Protect,1,20150416);

	NewDataMgr::getInstance()->mGameTaskSystem->_GamePropertyArr[GamePrKey_UseHuDun] += 1;
}

void MainPlane::startProtect()
{
	if(m_Protect)
	{
		Revive();
		m_Protect->removeMe();
		m_Protect = nullptr;
	}

    if(!UIController::getInstance()->getMainUILayer()->shieldIsEnough())
	{
        return;
    }
	
    AddProtect();
}

void MainPlane::ShootMusic()
{
	CallFunc* shoot = CallFunc::create(this,callfunc_selector(MainPlane::PlayShootMusic));
	ActionInterval* delay = DelayTime::create(0.1 + rand_0_1()*0.1);
	CallFuncN* funcN = CallFuncN::create(this,callfuncN_selector(MainPlane::ShootMusicCB));
	ActionInterval* seq = Sequence::create(shoot,delay,funcN,nullptr);

	Node* node = Node::create();
	addChild(node);
	node->runAction(seq);
}

void MainPlane::ShootMusicCB(Node* node)
{
	if (node)
	{
		node->removeFromParentAndCleanup(true);
	}
	ShootMusic();
}

void MainPlane::PlayShootMusic()
{
	if (m_ShootMusicAble)
	{
		if (!m_shootMusic.empty())
		{
			NewAudio::getInstance()->playEffect(m_shootMusic);
		}
	}
}

void MainPlane::setShootMusic( const std::string& musicfile )
{
	m_shootMusic = musicfile;
}

void MainPlane::TintWhenHurt( float dur /*= 1.0f*/ )
{
	ActionInterval* tintTo = TintTo::create(0.1,255,255,255);
	ActionInterval* tintRed = TintTo::create(0.1,150,0,0);
	ActionInterval* seq = Sequence::create(tintRed,tintTo,nullptr);
	ActionInterval* repeat = Repeat::create(seq,(int)dur/0.2);
	CallFunc* funcN = CallFunc::create(this,callfunc_selector(MainPlane::resumeColor));
	ActionInterval* seq1 = Sequence::create(repeat,funcN,nullptr);
	runAction(seq1);
}

void MainPlane::resumeColor()
{

}

void MainPlane::addMagnet()
{
}

void MainPlane::removeMagnet()
{
	m_IsHasMagnet = false;
	auto node = getChildByTag(6151936);
	if (node)
	{
		node->removeFromParentAndCleanup(true);
	}
}

bool MainPlane::onTouchBegan( Touch *touch, Event *unused_event )
{
	return true;
}

void MainPlane::onTouchMoved( Touch *touch, Event *unused_event )
{
	bool isPause = GameData::getInstance()->isGamePause();
	if (isPause)
	{
		return;
	}
	if (!m_isTouchEnabled)
	{
		return;
	}
	Vec2 prePos = touch->getPreviousLocation();
	Vec2 pos = touch->getLocation();

	m_PrePos = prePos;
	m_CurPos = pos;

	Vec2 newPos= pos - prePos;
	newPos *= 1.1;
	Vec2 myPos = convertToWorldSpaceAR(Vec2::ZERO);
	newPos = myPos + newPos;

	if (newPos.y > m_MoveEdgeUp)
	{
		newPos.y = m_MoveEdgeUp;
	}
	else if (newPos.y < m_MoveEdgeDown)
	{
		newPos.y = m_MoveEdgeDown;
	}
	if (newPos.x > m_MoveEdgeRight)
	{
		newPos.x = m_MoveEdgeRight;
	}
	else if (newPos.x < m_MoveEdgeLeft)
	{
		newPos.x = m_MoveEdgeLeft;
	}

	setPosition(newPos);
/*
	float dis = pos.x - prePos.x;
	if (dis > 5)
	{
		if (!m_isMoveAniPlayingR)
		{
			playMoveAnimationR();
		}
	}
	else if(dis < -5)
	{
		if (!m_isMoveAniPlayingL)
		{
			playMoveAnimationL();
		}
	}
	*/

}

void MainPlane::onTouchEnded( Touch *touch, Event *unused_event )
{
	//playNorAnimation();
}

void MainPlane::initMoveInParticle()
{
	EffectsManager::addParticle("particla/mainPlaneRestart/test1.plist",this,Vec2(getContentSize().width * 0.5,-220), false,0,-10);
	EffectsManager::addParticle("particla/mainPlaneRestart/test2.plist",this,Vec2(getContentSize().width * 0.5,getContentSize().height * 0.5 - 30), false,0,-10);
}

void MainPlane::InitSkin()
{

	m_NorAnimationNode = Sprite::create("weapon.png");
	m_NorAnimationNode->setPosition(getContentSize().width*0.5, getContentSize().height*0.5);
	addChild(m_NorAnimationNode);
	m_NorAnimationNode->runAction(getNorAnimation());

	/*{
		m_MoveAnimationNodeL = Sprite::create("weapon.png");
		m_MoveAnimationNodeL->setPosition(getContentSize().width*0.5, getContentSize().height*0.5);
		addChild(m_MoveAnimationNodeL);
		m_MoveAnimationNodeL->setVisible(false);
		auto moveAnimat = getMoveAnimationL();
		m_MoveAnimationNodeL->runAction(moveAnimat);
	}
	{
		m_MoveAnimationNodeR = Sprite::create("weapon.png");
		m_MoveAnimationNodeR->setPosition(getContentSize().width*0.5, getContentSize().height*0.5);
		addChild(m_MoveAnimationNodeR);
		m_MoveAnimationNodeR->setVisible(false);
		auto moveAnimat = getMoveAnimationR();
		m_MoveAnimationNodeR->runAction(moveAnimat);
	}*/

	setSkinSize(Size(100,100));
}

void MainPlane::playNorAnimation()
{
	m_isMoveAniPlayingL = false;
	m_isMoveAniPlayingR = false;
	m_isFinished = true;

	m_NorAnimationNode->stopAllActions();
	m_NorAnimationNode->runAction(getNorAnimation());

	m_NorAnimationNode->setVisible(true);
}

ActionInterval* MainPlane::getMoveAnimationL(bool isHead)
{
	std::string qianzhui;
	switch (m_PlaneType)
	{
	case PlaneType::PlaneType_Wind:
		qianzhui = "PlAnimationL_";
		break;
	case PlaneType::PlaneType_Fire:
		qianzhui = "Pl2MoveL_";
		break;
	case PlaneType::PlaneType_Electric:
		qianzhui = "Pl3MoveL_";
		break;		

	default:
		break;
	}
	Animate* moveAnimat = nullptr;
	if (isHead)
	{
		moveAnimat = EffectsManager::getFramesAnimation(qianzhui, 8, 1.0f/50, 0, 1);
	}
	else
	{
		moveAnimat = EffectsManager::getFramesAnimation(qianzhui, 16, 1.0f/50, 8, 1);
	}
	return moveAnimat;
}

ActionInterval* MainPlane::getMoveAnimationR(bool isHead)
{
	std::string qianzhui;
	switch (m_PlaneType)
	{
	case PlaneType::PlaneType_Wind:
		qianzhui = "PlAnimationR_";
		break;
	case PlaneType::PlaneType_Fire:
		qianzhui = "Pl2MoveR_";
		break;
	case PlaneType::PlaneType_Electric:
		qianzhui = "Pl3MoveR_";
		break;
	default:
		break;
	}
	Animate* moveAnimat = nullptr;
	if (isHead)
	{
		moveAnimat = EffectsManager::getFramesAnimation(qianzhui, 8, 1.0f/50, 0, 1);
	}
	else
	{
		moveAnimat = EffectsManager::getFramesAnimation(qianzhui, 16, 1.0f/50, 8, 1);
	}
	return moveAnimat;
}

ActionInterval* MainPlane::getNorAnimation()
{
	std::string qianzhui;
	switch (m_PlaneType)
	{
	case PlaneType::PlaneType_Wind:
		qianzhui = "Lead_wind_";
		break;
	case PlaneType::PlaneType_Fire:
		qianzhui = "MainPl2_";
		break;
	case PlaneType::PlaneType_Electric:
		qianzhui = "MainPl3_";
		break;
	default:
		break;
	}

	Animate* animate = EffectsManager::getFramesAnimation(qianzhui, 8, 0.1f, 0, REPEAT_FORVER);
	return animate;
}

void MainPlane::checkIsMoveing( float dt )
{
	
}

void MainPlane::AnimatFinished()
{
	m_isFinished = true;
}

void MainPlane::WeaponUpgrade()
{
	if (!m_IsShootAble)
	{
		return;
	}
	m_WpLvl++;
	if (m_WpLvl > 5)
	{
		m_WpLvl = 5;
	}
	clearWeapon();
	GameData::getInstance()->getWeaponFactory()->AddMainPlaneWp((int)m_PlaneType, m_WpLvl, this);
}

void MainPlane::WeaponDowngrade()
{
	if (!m_IsShootAble)
	{
		return;
	}
	m_WpLvl--;
	if (m_WpLvl < 1)
	{
		m_WpLvl = 1;
	}
	clearWeapon();
	GameData::getInstance()->getWeaponFactory()->AddMainPlaneWp((int)m_PlaneType, m_WpLvl, this);
}

void MainPlane::ImpactOfHp( const BasePro& pro )
{
	if (m_isDead)
	{
		return;
	}
	//log("******************* mainpl Hp = %d,   Hurts = %d ****************", m_SelfPro.curHP, rs.hurts);
	bool isBossDead = GameData::getInstance()->getIsBossDead();
	if(isBossDead)
	{
		return;
	}
	if (m_BasePro.getIsInvincible())
	{
		return;
	}

	if(pro.getHurts() < 0)
	{
		if (m_Protect)
		{
			startProtect();
			return;
		}
		//UIController::getInstance()->getMainUILayer()->AddHp(rs.getHurts());

		if (pro.getMyId() == UnitId::ePlaneBoss || pro.getBelongTo() == UnitId::ePlaneBoss)
		{
			GameData::getInstance()->setIsHurtByBoss(true);
		}
		FuncHpEmpty(pro);
		DeathImpactOfDropReward();
		DeathImpactOfDrops();
		PlayMusicWhenDead();			//
		AnimationWhenDead();			//
		m_isDead = true;
	}
}

void MainPlane::setSkinOffset( const Vec2& offset )
{
	Vec2 pos = m_NorAnimationNode->getPosition();
	pos += offset;
	m_NorAnimationNode->setPosition(pos);
}

void MainPlane::receiveRewards()
{
	auto delay = DelayTime::create(0.8f);
	auto func = CallFunc::create(this,callfunc_selector(MainPlane::receiveRewardsCB));
	auto seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void MainPlane::receiveRewardsCB()
{
	NotificationCenter::getInstance()->postNotification(Msg_MainPlFuhuo, nullptr);
}

void MainPlane::setBody( float t )
{
	initBody();
}

void MainPlane::gameEnd()
{
	EnemyPlaneController::getInstance()->pauseSelfAndChild(this);
}

void MainPlane::initMoveEdge()
{
	m_MoveEdgeLeft = 30.0f;
	m_MoveEdgeRight = m_WinSize.width - 30;
	m_MoveEdgeUp = m_WinSize.height * 0.95;
	m_MoveEdgeDown = m_WinSize.height * 0.1;
}

void MainPlane::PlayDaZhao()
{
	//GameData::getInstance()->setIsRewardValid(false);
	stopAllActionsByTag(WudiActionTag);
	setInvincible(1);
	//setTouchAble(false);
	//setShootAble(false);
	ClearScreenDazhao();

	/*{
	auto mv2Mid = MoveTo::create(0.5f, Vec2(m_WinSize.width * 0.5, 250));
	auto func = CallFunc::create(this, callfunc_selector(MainPlane::ClearScreenDazhao));
	auto seq = Sequence::create(mv2Mid, func, nullptr);
	runAction(seq);
	}*/
}

void MainPlane::addDaZhao()
{
	SpecialLaser* missile = SpecialLaser::create();
	missile->setPlane(this);
	missile->setScale(1.5);
	missile->setPosition(Vec2(0,-50));
	addChild(missile,1000);
}

void MainPlane::addFengHuang()
{
	FengHuangSkill* dazhao = FengHuangSkill::create();
	dazhao->setMainPlane(this);
	//dazhao->setPosition(m_WinSize.width * 0.5,m_WinSize.height * 0.5);
	UIController::getInstance()->getPhysicsLayer()->addChild(dazhao);
}

void MainPlane::onExit()
{
	BasePlane::onExit();
}

void MainPlane::PlaneFadeOut( Node* node )
{
	node->runAction(FadeOut::create(0.5f));

	auto vec = node->getChildren();
	for (auto it = vec.begin(); it != vec.end(); it++)
	{
		Node* child = (*it);
		PlaneFadeOut(child);
	}
}

void MainPlane::PlaneFadeIn( Node* node )
{
	node->runAction(FadeIn::create(0.2f));

	auto vec = node->getChildren();
	for (auto it = vec.begin(); it != vec.end(); it++)
	{
		Node* child = (*it);
		PlaneFadeIn(child);
	}
}

void MainPlane::ResumePlaneState()
{
	GameData::getInstance()->setIsRewardValid(true);
	setPosition(m_WinSize.width * 0.5, m_WinSize.height - 200);
	PlaneFadeIn(this);
	setScale(3.0f);
	
	auto scale = ScaleTo::create(0.5f, 1.0f);
	auto ease = EaseExponentialOut::create(scale);
	runAction(ease);

	auto mv = MoveTo::create(0.5f,Vec2(m_WinSize.width * 0.5, m_WinSize.height * 0.15));
	auto funcN = CallFunc::create(this,callfunc_selector(MainPlane::CanbeTouch));
	auto seq = Sequence::create(mv, funcN, nullptr);
	runAction(seq);
}

void MainPlane::CanbeTouch()
{
	setInvincible(false);
	setShootAble(true);
	setTouchAble(true);
}

void MainPlane::ClearScreenDazhao()
{
	ExplisonScreen* exp = ExplisonScreen::create();
	exp->setPosition(360,640);
	UIController::getInstance()->getPhysicsLayer()->addChild(exp);
}



//-----------------------------
void BaseBoss::FuncAfterEnter()
{
	UIController::getInstance()->getPhysicsLayer()->BossIn();
	setFeignDeath(true);

	ActionInterval* mvMid = MoveTo::create(2.0f, getMidPos());
	CallFunc* func = CallFunc::create(this,callfunc_selector(BaseBoss::BossDelayIn));
	ActionInterval* seq = Sequence::create(mvMid, func, nullptr);
	runAction(seq);
}

void BaseBoss::BossDelayIn()
{
//	auto mainPl = GameData::getInstance()->getMainPlane();
//	if (mainPl)
//	{
//		mainPl->removeSpeedUpEffect();
//	}
	
	addAi();
	addRating();
}

void BaseBoss::BossExplosion()
{
	Size s = getContentSize();

	//Vec2 pos = Vec2( s.width * ( rand_0_1()*0.8 + 0.2 ),s.height*(rand_0_1() * 0.8));
	Vec2 pos = Vec2( s.width * 0.5,s.height*0.6);
	
	ExplosionSprite* es = ExplosionSprite::create();
	es->setPosition(pos);
	es->setScale(4.0f);
	addChild(es);
}

void BaseBoss::displayBossExplosion()
{
	BaseExplosion* expl = BossExplosion::create();
	expl->setPlane(this);
	addChild(expl);
	expl->StartExplosion();
}

void BaseBoss::OtherDealWhenContact( const BasePro& rs )
{
	auto wudi = m_BasePro.getIsInvincible();
	if (wudi)
	{
		return;
	}
	
	ActionInterval* delay = DelayTime::create(0.1f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(BaseBoss::resumeColor));
	ActionInterval* seq = Sequence::create(delay, func ,nullptr);
	runAction(seq);

	if (m_BasePro.getCurHp() > 0)
	{
		float randNum = rand_0_1();
		if (randNum > 0.5)
		{
			PlaneProtect::ContactEff(this, 100.0f, rs.getContactPos(),BeHitEffRadius(), BeHitEffOffset());
		}
	}

	auto node = UIController::getInstance()->getPhysicsLayer()->getChildByName(BossBarTag);
	if (node)
	{
		BossHpBar* bar = (BossHpBar*) node;
		bar->UpdateHp(rs.getHurts());
	}
}

void BaseBoss::resumeColor()
{
	m_Armat->setColor(ccc3(255,255,255));
	m_Armat->setOpacity(255);
}

void BaseBoss::shake()
{
	UIController::getInstance()->getEffectsLayer()->NodeShake(this,1.0f);
}

void BaseBoss::RealDead()
{
	DeathForce();
}

void BaseBoss::DoSomethingBeforeDead()
{
	BasePlane::DoSomethingBeforeDead();
}

void BaseBoss::addRating()
{
	m_ratingNode = CalculateRating::create();
	m_ratingNode->startRating();
	addChild(m_ratingNode);
}

void BaseBoss::FuncHpEmpty(const BasePro& pro)
{
	GameData::getInstance()->DelEnemyPlane(this);
	m_Armat->setColor(ccc3(255,rand_0_1()*255,0));
	m_Armat->setOpacity(200);

	setBitmask(ENEMY_PLANE_CATEGORYBITMASK,0,0);
	stopAllActions();
	RemoveTimeOut();
	setShootAble(false);
	
	m_Ai->stopAllActions();
	m_Ai->FuncWhenTargetHpEmpry();
	m_Ai->DeathAction();
	m_PlanePro.setKiller(pro.getMyId());
	m_ratingNode->showRating();
	
	NotificationCenter::getInstance()->postNotification(Msg_BossDead, nullptr);
	//NotificationCenter::getInstance()->postNotification(Msg_StopMainPlBullet,nullptr);

	GameData::getInstance()->setIsBossDead(true);
	GameData::getInstance()->setIsHurtByBoss(false);

	displayBossExplosion();
}

float BaseBoss::BeHitEffRadius()
{
	return 0.7;
}

cocos2d::Vec2 BaseBoss::BeHitEffOffset()
{
	return Vec2(0, 0);
}

void BaseBoss::SwitchBgWhenDead( int BgId )
{
//	m_BgId = BgId;
}

void BaseBoss::InitSkinWithArmat( const std::string& name )
{
	auto s = getContentSize();
	m_Armat = Armature::create(name);
	m_Armat->setPosition(Vec2(s.width * 0.5, s.height * 0.5) + getArmatPosOffset());
	addChild(m_Armat);
	m_Armat->getAnimation()->playWithIndex(0);

	setSkinSize(m_Armat->getContentSize());
}

void BaseBoss::FuncWhenHpChanged()
{
	BasePlane::FuncWhenHpChanged();
}

void BaseBoss::ShowBossHpBar( int HpNum, int maxHp )
{
	BossHpBar* bossBar = BossHpBar::create(this);
	bossBar->setHpNumbers(HpNum);
	bossBar->setMaxHp(maxHp);
	bossBar->setPosition(m_WinSize.width * 0.5, m_WinSize.height - 50);
	UIController::getInstance()->getPhysicsLayer()->addChild(bossBar);
	bossBar->setName(BossBarTag);
}

BossHpBar* BaseBoss::getBossHpBar()
{
	Layer* layer = UIController::getInstance()->getPhysicsLayer();
	Node* node = layer->getChildByName(BossBarTag);
	BossHpBar* hpBar = (BossHpBar*)node;
	return hpBar;
}

void BaseBoss::Angry()
{
	Sprite* sp = Sprite::create("weapon.png");
	//sp->setPosition(getSkinSize().width * 0.5, getSkinSize().height * 0.5);
	sp->setScale(5.0);
	m_Armat->addChild(sp,1000);
	m_Armat->getAnimation()->playWithIndex(0);
	m_Ai->stopAllActions();
	stopAllActions();
	_physicsBody->setCollisionBitmask(0);
	_physicsBody->setContactTestBitmask(0);

	auto mainpl = GameData::getInstance()->getMainPlane();
	if (mainpl)
	{
		mainpl->setShootAble(false);
	}

	{
		auto scale = ScaleTo::create(1.0f, 1.3f);
		auto delay = DelayTime::create(2.0f);
		auto scaleback = ScaleTo::create(1.0f, 1.0f);
		auto func = CallFunc::create(this, callfunc_selector(BaseBoss::AngryCB));
		auto seq = Sequence::create(scale, delay, scaleback, func, nullptr);
		runAction(seq);
	}

	{
		auto rot = RotateTo::create(0.01, 3);
		auto back = RotateTo::create(0.01, -3);
		auto seq = Sequence::create(rot, back, nullptr);
		auto rep = Repeat::create(seq, 100);
		runAction(rep);
	}
	{
		auto mv = MoveBy::create(0.5f, Vec2(0,-100));
		auto back = mv->reverse();
		auto delay = DelayTime::create(3.5);
		auto seq = Sequence::create(mv, delay, back, nullptr);
		runAction(seq);
	}
	auto animat = EffectsManager::getFramesAnimation("BossAngry_",1.0/24, REPEAT_FORVER);
	sp->runAction(animat);
}

void BaseBoss::AngryCB()
{
	EnemyPlaneController::getInstance()->resumeSendGiftPlane();
	NotificationCenter::getInstance()->postNotification(Msg_BossAngryEnd,nullptr);

	_physicsBody->setCollisionBitmask(ENEMY_PLANE_COLLISIONBITMASK);
	_physicsBody->setContactTestBitmask(ENEMY_PLANE_CONTACTTESTBITMASK);

	stopAllActions();
	m_Ai->FixPositionBeforeNext();

	auto mainpl = GameData::getInstance()->getMainPlane();
	if (mainpl)
	{
		mainpl->setShootAble(true);
	}
}

void BaseBoss::addTimeOut(float maxTime)
{
	m_BossTimeOut = BossTimeOut::create();
	m_BossTimeOut->setMaxTime(maxTime);
	m_BossTimeOut->setInterval(3.0f);
	addChild(m_BossTimeOut);
}

void BaseBoss::RemoveTimeOut()
{
	if (m_BossTimeOut)
	{
		m_BossTimeOut->removeFromParentAndCleanup(true);
		m_BossTimeOut = nullptr;
	}
}

void BaseBoss::ImpactOfHp( const BasePro& pro )
{
	if (m_isDead)
	{
		return;
	}
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
		int hurt = 0;
		if (pro.getHurts() > 0)
		{
			hurt = pro.getHurts();
		}
		else
		{
			hurt = pro.getHurts() + m_BasePro.getDefense();
		}
		m_BasePro.setCurHp(m_BasePro.getCurHp() + hurt);
		FuncWhenHpChanged();
		if (m_BasePro.getCurHp() <= 0)
		{
			FuncHpEmpty(pro);
			DeathImpactOfDrops();
			PlayMusicWhenDead();
			AnimationWhenDead();
			setDeadType(DeadType::Dead_Kill);
			if (m_FeignDeath)
			{
				setInvincible(1);
				m_BasePro.setCurHp(1);
			}
			else
			{
				m_isDead = true;
			}
		}
	}
}

cocos2d::Vec2 BaseBoss::getMidPos()
{
	return Vec2(m_WinSize.width * 0.5, m_WinSize.height * 0.75);
}





//-----------------------------
IronManBoss* IronManBoss::create( const std::string& artFile, int lvl )
{
	IronManBoss* Iman = new (std::nothrow) IronManBoss;
	
	if (Iman && Iman->initWithFile("weapon.png"))
	{
		Iman->InitSkinWithArmat(artFile);
		Iman->initBody();
		Iman->ShowHpBar();
		Iman->autorelease();
		Iman->setBossLvl(lvl);
		Iman->setDeadMusic(Music_BossKuiJiaDead);
		return Iman;
	}
	else
	{
		CC_SAFE_DELETE(Iman);
		return nullptr;
	}
}


void IronManBoss::addAi()
{
	PlaneAI* ai = IronManBossAI::create(this);
	addChild(ai);
	m_Ai = ai;

	ai->StartAI(m_BossLvl);

	setBitmask(ENEMY_PLANE_CATEGORYBITMASK,
		ENEMY_PLANE_COLLISIONBITMASK,
		ENEMY_PLANE_CONTACTTESTBITMASK);

	getHpBar()->setHpBarOffset(Vec2(0,-140));
}

void IronManBoss::initBody()
{
	auto body = PhysicsBody::createBox(Size(300,200));
	body->setPositionOffset(Vec2(0,100));
	body->setGravityEnable(false);
	body->setRotationEnable(false);
	body->setMass(20000);
	this->setPhysicsBody(body);
	setBitmask(ENEMY_PLANE_CATEGORYBITMASK,0,0);
	setInvincibleAtEdge(true);
}

void IronManBoss::OtherDealWhenContact( const BasePro& rs )
{
	BaseBoss::OtherDealWhenContact(rs);
	
}

cocos2d::Vec2 IronManBoss::getArmatPosOffset()
{
	return Vec2(0,0);
}


//--------------------------
CalculateRating* CalculateRating::create()
{
	CalculateRating* cr = new (std::nothrow) CalculateRating;
	if (cr && cr->init())
	{
		cr->autorelease();
		return cr;
	}
	else
	{
		CC_SAFE_DELETE(cr);
		return nullptr;
	}
}

void CalculateRating::updateTime( float dt )
{
	m_SurvivlaTime += 1.0f;
}

BossRating CalculateRating::getRating()
{
	bool isHurtByBoss = GameData::getInstance()->getIsHurtByBoss();
	BossRating br;
	if (m_SurvivlaTime <= 10.0f)
	{
		if (isHurtByBoss)
		{
			br = BossRating::e_A;
		}
		else
		{
			br = BossRating::e_SS;
		}
	}
	else if(m_SurvivlaTime < 20.0f)
	{
		if (isHurtByBoss)
		{
			br = BossRating::e_A;
		}
		else
		{
			br = BossRating::e_S;
		}
	}
	else if(m_SurvivlaTime < 30.0f)
	{
		br = BossRating::e_A;
	}
	else
	{
		br = BossRating::e_B;
	}
	return br;
}

void CalculateRating::showRating()
{
	if (m_isShow)
	{
		return;
	}
	m_isShow = true;
	BossRating br = getRating();
	SurvivalTimeRecord();

	unschedule(schedule_selector(CalculateRating::updateTime));
	GameData::getInstance()->addBossRecord(m_BossType,br,m_SurvivlaTime);

	std::string BgFile;
	std::string gradeFile;
	std::string discribFile;

	int score = 0;
	switch (br)
	{
	case BossRating::e_SS:
		gradeFile = "Rating/rating_doubles.png";
		discribFile = "Rating/perfect.png";
		score = 20000;
		break;
	case BossRating::e_S:
		gradeFile = "Rating/rating_s.png";
		discribFile = "Rating/cool.png";
		score = 15000;
		break;
	case BossRating::e_A:
		gradeFile = "Rating/rating_a.png";
		discribFile = "Rating/great.png";
		score = 10000;
		break;
	case BossRating::e_B:
		gradeFile = "Rating/rating_b.png";
		discribFile = "Rating/good.png";
		score = 7000;
		break;
	default:
		break;
	}

	GameData::getInstance()->addScore(score);

	auto s = Director::getInstance()->getWinSize();

	auto Bg = Sprite::create("Rating/ratingBg.png");
	Bg->setPosition(-s.width * 0.5, s.height * 0.65);
	UIController::getInstance()->getPhysicsLayer()->addChild(Bg);

	auto bgSize = Bg->getContentSize();

	auto grade = Sprite::create(gradeFile);
	grade->setPosition(bgSize.width*0.75, bgSize.height*0.65);
	Bg->addChild(grade);
	grade->setOpacity(0);

	auto discrib = Sprite::create(discribFile);
	discrib->setPosition(bgSize.width*0.3, bgSize.height*0.35);
	Bg->addChild(discrib);

	{
		auto delay = DelayTime::create(1.5);
		auto fadein = FadeIn::create(0.2f);
		auto scale = ScaleBy::create(0.2f,2.0);
		auto scaleEase = EaseSineIn::create(scale);
		auto scaleBack = scaleEase->reverse();

		auto seq1 = Sequence::create(delay, scaleEase, scaleBack, nullptr);
		auto seq2 = Sequence::create(delay, fadein, nullptr);

		grade->runAction(seq1);
		grade->runAction(seq2);
	}

	{
		auto mvIn = MoveTo::create(1.0,Vec2(s.width * 0.5, s.height * 0.65));
		auto easeIn = EaseElasticIn::create(mvIn);
		auto stay = DelayTime::create(3.0f);
		auto mvOut = MoveTo::create(0.2,Vec2(s.width * 1.5, s.height * 0.65));
		auto rm = RemoveSelf::create(true);
		auto seq = Sequence::create(easeIn, stay, mvOut, rm, nullptr);
		Bg->runAction(seq);
	}
}

void CalculateRating::startRating()
{
	schedule(schedule_selector(CalculateRating::updateTime),1.0f);
}

void CalculateRating::removeRatingSp( Node* node )
{
	node->removeFromParentAndCleanup(true);
}

void CalculateRating::setBossType( int tp )
{
	m_BossType = tp;
}

void CalculateRating::SurvivalTimeRecord()
{
	int* data = NewDataMgr::getInstance()->mGameTaskSystem->_GamePropertyArr;
	if (m_SurvivlaTime < data[GamePrKey_minKillBossTime] || 0 == data[GamePrKey_minKillBossTime] )
	{
		data[GamePrKey_minKillBossTime] = m_SurvivlaTime;
	}
}


//-----------------------------
BoneWarriorBoss* BoneWarriorBoss::create( const std::string& artFile, int lvl )
{
	BoneWarriorBoss* Warrior = new (std::nothrow) BoneWarriorBoss;

	if (Warrior && Warrior->initWithFile("weapon.png"))
	{
		Warrior->InitSkinWithArmat(artFile);
		Warrior->initBody();
		Warrior->setBossLvl(lvl);
		Warrior->ShowHpBar();
		Warrior->autorelease();
		Warrior->setDeadMusic(Music_BossKuLouDead);
		return Warrior;
	}
	else
	{
		CC_SAFE_DELETE(Warrior);
		return nullptr;
	}
}

void BoneWarriorBoss::addAi()
{
	PlaneAI* ai = BoneWarriorAI::create(this);
	addChild(ai);
	m_Ai = ai;
	ai->StartAI(m_BossLvl);

	setBitmask(ENEMY_PLANE_CATEGORYBITMASK,
		ENEMY_PLANE_COLLISIONBITMASK,
		ENEMY_PLANE_CONTACTTESTBITMASK);

	getHpBar()->setHpBarOffset(Vec2(0,-350));
	getHpBar()->setHpBarLengthScale(0.7);
}

void BoneWarriorBoss::initBody()
{
	auto body = PhysicsBody::createBox(Size(300,400),PHYSICSBODY_MATERIAL_DEFAULT,Vec2(0,50));
	body->setGravityEnable(false);
	body->setRotationEnable(false);
	body->setMass(20000);
	this->setPhysicsBody(body);
	setBitmask(ENEMY_PLANE_CATEGORYBITMASK,0,0);
	setInvincibleAtEdge(true);
}

void BoneWarriorBoss::onEnter()
{
	BaseBoss::onEnter();
}

void BoneWarriorBoss::OtherDealWhenContact( const BasePro& rs )
{
	BaseBoss::OtherDealWhenContact(rs);
	
}

//-------------------------------
RenzheBoss* RenzheBoss::create( const std::string& artFile, int lvl )
{
	RenzheBoss* renzhe = new (std::nothrow) RenzheBoss;

	if (renzhe && renzhe->initWithFile("weapon.png"))
	{
		renzhe->InitSkinWithArmat(artFile);
		renzhe->initBody();
		renzhe->setBossLvl(lvl);
		renzhe->ShowHpBar();
		renzhe->autorelease();
		renzhe->setDeadMusic(Music_BossRenZheDead);
		return renzhe;
	}
	else
	{
		CC_SAFE_DELETE(renzhe);
		return nullptr;
	}
}

void RenzheBoss::addAi()
{
	PlaneAI* ai = RenzheBossAI::create(this);
	addChild(ai);
	m_Ai = ai;

	ai->StartAI(m_BossLvl);

	setBitmask(ENEMY_PLANE_CATEGORYBITMASK,
		ENEMY_PLANE_COLLISIONBITMASK,
		ENEMY_PLANE_CONTACTTESTBITMASK);

	getHpBar()->setHpBarOffset(Vec2(0,-120));
}

void RenzheBoss::initBody()
{
	auto body = PhysicsBody::createBox(Size(300,300),PHYSICSBODY_MATERIAL_DEFAULT,Vec2(-30,50));
	body->setGravityEnable(false);
	body->setRotationEnable(false);
	body->setMass(20000);
	this->setPhysicsBody(body);
	setBitmask(ENEMY_PLANE_CATEGORYBITMASK,0,0);
	setInvincibleAtEdge(true);
}

//-------------------------------------------
PoseidonBoss* PoseidonBoss::create( const std::string& artFile, int lvl )
{
	PoseidonBoss* poseidon = new (std::nothrow) PoseidonBoss;

	if (poseidon && poseidon->initWithFile("weapon.png"))
	{
		poseidon->InitSkinWithArmat(artFile);
		poseidon->initBody();
		poseidon->setBossLvl(lvl);		
		poseidon->ShowHpBar();
		poseidon->autorelease();
		poseidon->setDeadMusic(Music_BossHaiShenDead);
		return poseidon;
	}
	else
	{
		CC_SAFE_DELETE(poseidon);
		return nullptr;
	}
}

void PoseidonBoss::initBody()
{
	auto body = PhysicsBody::createBox(Size(150,350),PHYSICSBODY_MATERIAL_DEFAULT,Vec2(-30,100));
	body->setGravityEnable(false);
	body->setRotationEnable(false);
	body->setMass(20000);
	this->setPhysicsBody(body);
	setBitmask(ENEMY_PLANE_CATEGORYBITMASK,0,0);
	setInvincibleAtEdge(true);
}

void PoseidonBoss::addAi()
{
	PlaneAI* ai = PoseidonBossAI::create(this);
	addChild(ai);
	m_Ai = ai;

	ai->StartAI(m_BossLvl);

	setBitmask(ENEMY_PLANE_CATEGORYBITMASK,
		ENEMY_PLANE_COLLISIONBITMASK,
		ENEMY_PLANE_CONTACTTESTBITMASK);

	getHpBar()->setHpBarOffset(Vec2(0,-120));
}

cocos2d::Vec2 PoseidonBoss::BeHitEffOffset()
{
	return Vec2(0, 30);
}

//--------------------------------------
HpBar::HpBar()
{
	m_HpBar = nullptr;
	m_Plane = nullptr;
}

HpBar* HpBar::create(BasePlane* plane)
{
	HpBar* bar = new (std::nothrow)HpBar;
	if (bar && bar->initWithFile("Image/HpBarBg1.png"))
	{
		bar->m_Plane = plane;
		bar->autorelease();
		return bar;
	}
	else
	{
		CC_SAFE_DELETE(bar);
		return nullptr;
	}
}

void HpBar::onEnter()
{
	Sprite::onEnter();
	ShowHpBar();
}

void HpBar::ShowHpBar()
{
	auto skinSize = m_Plane->getSkinSize();
	auto s = getContentSize();
	setScaleX(skinSize.width/s.width * 0.6);
	setBlendFunc(BlendFunc::DISABLE);
	setColor(Color3B::BLACK);

	Sprite* sp = Sprite::create("Image/HpBarBg1.png");
	sp->setColor(Color3B::YELLOW);
	m_HpBar = ProgressTimer::create(sp);
	m_HpBar->setType(kCCProgressTimerTypeBar);  
	m_HpBar->setBarChangeRate(Vec2(1,0));
	m_HpBar->setPercentage(100.0f);
	m_HpBar->setPosition(ccp(s.width*0.5,s.height*0.5));
	m_HpBar->setMidpoint(Vec2(0,0.5));
	addChild(m_HpBar);
}

void HpBar::setHpBarOffset( const Vec2& offset )
{
	auto pos = getPosition();
	this->setPosition(pos + offset);
}

void HpBar::RefreshHpBar(float percent)
{
	if (!m_HpBar)
	{
		return;
	}
	m_HpBar->setPercentage(percent);
}

void HpBar::SetHpBarVisible( bool visible )
{
	this->setVisible(visible);
}

void HpBar::setHpBarLengthScale( float scale )
{
	float curScale = getScaleX();
	setScaleX(scale * curScale);
}

//---------------------------------------
EnemyPlane* EnemyPlane::createWithArmature( const std::string& file,const std::string& TrackId )
{
	EnemyPlane* plane = new (std::nothrow) EnemyPlane;
	if (plane && plane->init())
	{
		plane->InitSkinWithArmature(file);
		plane->initBody();
		plane->ShowHpBar();
		plane->initTrack(TrackId);
		plane->autorelease();
		return plane;
	}
	else
	{
		CC_SAFE_DELETE(plane);
		return nullptr;
	}
}

EnemyPlane* EnemyPlane::createWithPicture( const std::string& file, const std::string& TrackId )
{
	EnemyPlane* plane = new (std::nothrow) EnemyPlane;
	if (plane && plane->init())
	{
		plane->InitSkinWithPicture(file);
		plane->initBody();
		plane->ShowHpBar();
		plane->initTrack(TrackId);
		plane->autorelease();
		return plane;
	}
	else
	{
		CC_SAFE_DELETE(plane);
		return nullptr;
	}
}

void EnemyPlane::InitSkinWithPicture( const std::string& file )
{
	m_Picture = Sprite::createWithSpriteFrameName(file);
	addChild(m_Picture);
	setSkinSize(m_Picture->getContentSize());
}

void EnemyPlane::InitSkinWithArmature( const std::string& name )
{
	auto s = getContentSize();
	m_Armat = PlaneArmature::create(name, this);
	m_Armat->setPosition(Vec2(s.width * 0.5, s.height * 0.5) + getArmatPosOffset());
	addChild(m_Armat);
	m_Armat->playRestAnimation();

	setSkinSize(m_Armat->getContentSize());
}

void EnemyPlane::addWeapon()
{
	GameData::getInstance()->getWeaponFactory()->AddEnemyWp(m_WeaponPro, this);
}

void EnemyPlane::setFlipXonEnter()
{
	m_isNeedFlipX = true;
}

EnemyPlane::~EnemyPlane()
{
	m_trackMgr->purge();
}

void EnemyPlane::onEnter()
{
	BasePlane::onEnter();

	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(EnemyPlane::ListenBossAngryCB), Msg_BossAngry, nullptr);
	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(EnemyPlane::ListenBossAngryEndCB), Msg_BossAngryEnd, nullptr);

	if (m_isNeedFlipX)
	{
		auto pos = convertToWorldSpaceAR(Vec2::ZERO);
		if (pos.x < m_WinSize.width * 0.5)
		{
			m_Picture->setFlippedX(true);
		}
	}

	if (m_Armat)
	{
		std::string ArmatName = m_Armat->getArmatureData()->name;
		if (ArmatName == "plane_5" || ArmatName == "plane_10" )
		{
			auto delay = DelayTime::create(2.0f);
			auto func = CallFunc::create(this, callfunc_selector(EnemyPlane::StartFire));
			auto seq = Sequence::create(delay, func, nullptr);
			runAction(seq);
		}
	}
}

void EnemyPlane::OtherDealWhenContact( const BasePro& rs )
{
	if (m_BasePro.getIsInvincible())
	{
		return;
	}
	int randNum = random(1,10);
	if (randNum > 4)
	{
		return;
	}
	auto s = getSkinSize();
	auto pos = Vec2(0, -s.height*0.25);
	auto partc1 = EffectsManager::addParticle("particla/NorPlContact/NorPlContact1.plist",this,pos);
	auto partc2 = EffectsManager::addParticle("particla/NorPlContact/NorPlContact2.plist",this,pos);
	if (partc1)
	{
		partc1->setPositionType(ParticleSystem::PositionType::GROUPED);
		partc1->setName("partc1");
	}
	if (partc2)
	{
		partc2->setPositionType(ParticleSystem::PositionType::GROUPED);
		partc2->setName("partc2");
	}

	{
		auto mv = MoveBy::create(0.05f, Vec2(0,5));
		auto back = mv->reverse();
		auto seq = Sequence::create(mv, back, nullptr);
		runAction(seq);
	}
	{
		auto scale = ScaleTo::create(0.05f, 1.0f, 0.95f);
		auto back = ScaleTo::create(0.05f, 1.0f, 1.0f);
		auto seq = Sequence::create(scale, back, nullptr);
		runAction(seq);
	}
	{
		auto tint = TintTo::create(0.05, 200, 0, 0);
		auto back = TintTo::create(0.05, 255, 255, 255);
		auto seq = Sequence::create(tint, back, nullptr);
		if (m_Picture)
		{
			m_Picture->runAction(seq);
		}
		else
		{
			m_Armat->runAction(seq);
		}
	}
}

void EnemyPlane::initTrack(const std::string& TrackId)
{
	m_trackMgr = TrackManager::create(this, TrackId);
}

void EnemyPlane::startTrack()
{
	if (m_trackMgr)
	{
		m_trackMgr->startAction();
	}
}

void EnemyPlane::StartFire()
{
	this->m_haveCallBack = false;
	if (m_Armat)
	{
		std::string ArmatName = m_Armat->getArmatureData()->name;
		if (ArmatName == "plane_5" || ArmatName == "plane_10" )
		{
			addConfigWeapon(true);
			return;
		}
		m_Armat->playShootAnimation();
	}
	else
	{
		addConfigWeapon(true);
	}
}

void EnemyPlane::addConfigWeapon(bool isOneTime)
{
	EditorWeaponFactory::createAndAddWeapon(this,this->getBarrageArray(),isOneTime);
}

void EnemyPlane::ImpactOfHp( const BasePro& pro )
{
	if (isScreenEdge(pro))
	{
		if (getIsMoveIn())
		{
			return;
		}
		else
		{
			if (m_DeadType == DeadType::Dead_default)
			{
				setDeadType(DeadType::Dead_Nature);
			}
			m_isDead = true;
			return;
		}
	}
	else
	{
		if (m_BasePro.getIsInvincible())
		{
			return;
		}
		int hurt = 0;
		if (pro.getHurts() > 0)
		{
			hurt = pro.getHurts();
		}
		else
		{
			hurt = pro.getHurts() + m_BasePro.getDefense();
		}
		m_BasePro.setCurHp(m_BasePro.getCurHp() + hurt);
		FuncWhenHpChanged();
		if (m_BasePro.getCurHp() <= 0)
		{
			m_isMoveIn = false;
			FuncHpEmpty(pro);
			DeathImpactOfDropReward();
			DeathImpactOfDrops();
			PlayMusicWhenDead();
			AnimationWhenDead();
			setDeadType(DeadType::Dead_Kill);
			if (m_FeignDeath)
			{
				setInvincible(1);
				m_BasePro.setCurHp(1);
			}
			else
			{
				m_isDead = true;
			}
		}
	}
}

void EnemyPlane::ListenBossAngryCB( Ref* sender )
{
	_actionManager->pauseTarget(this);
}

void EnemyPlane::ListenBossAngryEndCB( Ref* sender )
{
	_actionManager->resumeTarget(this);
}

void EnemyPlane::FuncHpEmpty( const BasePro& pro )
{
	GameData::getInstance()->DelEnemyPlane(this);
	
	stopAllActions();
	schedule(schedule_selector(EnemyPlane::CheckToRemove));

	if (m_Armat)
	{
		m_Armat->pause();
	}

	if (m_Picture)
	{
		auto tint = TintTo::create(0.1, ccc3(255,0,0));
		auto rep = RepeatForever::create(tint);
		m_Picture->runAction(rep);
	}

	if (m_Armat)
	{
		auto tint = TintTo::create(0.1, ccc3(255,0,0));
		auto rep = RepeatForever::create(tint);
		m_Armat->runAction(rep);

		std::string ArmatName = m_Armat->getArmatureData()->name;
		if (ArmatName == "plane_21" || ArmatName == "plane_22" || ArmatName == "plane_23" ||
			ArmatName == "plane_24" || ArmatName == "plane_25" || ArmatName == "plane_26")
		{
			if (random(1,10) > 7)
			{
				BaseExplosion* expl = SpecialEnemyPlaneExpl::create();
				expl->setPlane(this);
				addChild(expl);
				expl->StartExplosion();
				return;			
			}
		}
	}

	{
		BaseExplosion* expl = NorEnemyPlaneExpl::create();
		expl->setPlane(this);
		addChild(expl);
		expl->StartExplosion();
	}
}

void EnemyPlane::DoSomethingBeforeDead()
{
	GameData::getInstance()->DelEnemyPlane(this);
	BasePlane::DoSomethingBeforeDead();
}

void EnemyPlane::CheckToRemove(float t)
{
	auto myPos = convertToWorldSpaceAR(Vec2::ZERO);
	if (myPos.x < m_WinSize.width && myPos.x > 0 &&
		myPos.y > 0 && myPos.y < m_WinSize.height)
	{
		
	}
	else
	{
		DeathForce();
	}
}

//---------------------------------
FemaleGunBoss* FemaleGunBoss::create( const std::string& artFile, int lvl )
{
	FemaleGunBoss* boss = new (std::nothrow) FemaleGunBoss;

	if (boss && boss->initWithFile("weapon.png"))
	{
		boss->InitSkinWithArmat(artFile);
		boss->initBody();
		boss->setBossLvl(lvl);		
		boss->ShowHpBar();
		boss->autorelease();
		boss->setDeadMusic(Music_BossNvQiangDead);
		return boss;
	}
	CC_SAFE_DELETE(boss);
	return nullptr;
}

void FemaleGunBoss::initBody()
{
	auto body = PhysicsBody::createBox(Size(150,350),PHYSICSBODY_MATERIAL_DEFAULT,Vec2(-30,100));
	body->setGravityEnable(false);
	body->setRotationEnable(false);
	body->setMass(20000);
	this->setPhysicsBody(body);
	setBitmask(ENEMY_PLANE_CATEGORYBITMASK,0,0);
	setInvincibleAtEdge(true);
}

void FemaleGunBoss::addAi()
{
	PlaneAI* ai = FemaleGunBossAi::create(this);
	addChild(ai);
	m_Ai = ai;

	ai->StartAI(m_BossLvl);

	setBitmask(ENEMY_PLANE_CATEGORYBITMASK,
		ENEMY_PLANE_COLLISIONBITMASK,
		ENEMY_PLANE_CONTACTTESTBITMASK);

	getHpBar()->setHpBarOffset(Vec2(0,-120));
}

//-------------------------------------------
EyeBoss* EyeBoss::create( const std::string& artName, int lvl )
{
	EyeBoss* boss = new (std::nothrow) EyeBoss;

	if (boss && boss->initWithFile("weapon.png"))
	{
		boss->InitSkinWithArmat(artName);
		boss->initBody();
		boss->setBossLvl(lvl);		
		boss->ShowHpBar();
		boss->autorelease();
		boss->setDeadMusic(Music_BossYanJingDead);
		return boss;
	}
	CC_SAFE_DELETE(boss);
	return nullptr;
}

void EyeBoss::initBody()
{
	auto body = PhysicsBody::createBox(Size(150,350),PHYSICSBODY_MATERIAL_DEFAULT,Vec2(-30,100));
	body->setGravityEnable(false);
	body->setRotationEnable(false);
	body->setMass(20000);
	this->setPhysicsBody(body);
	setBitmask(ENEMY_PLANE_CATEGORYBITMASK,0,0);
	setInvincibleAtEdge(true);
}

void EyeBoss::addAi()
{
	PlaneAI* ai = EyeBossAi::create(this);
	addChild(ai);
	m_Ai = ai;

	ai->StartAI(m_BossLvl);

	setBitmask(ENEMY_PLANE_CATEGORYBITMASK,
		ENEMY_PLANE_COLLISIONBITMASK,
		ENEMY_PLANE_CONTACTTESTBITMASK);

	getHpBar()->setHpBarOffset(Vec2(0,-120));
}

//--------------------------
YizhiBoss* YizhiBoss::create( const std::string& artName, int lvl )
{
	YizhiBoss* boss = new (std::nothrow) YizhiBoss;

	if (boss && boss->initWithFile("weapon.png"))
	{
		boss->InitSkinWithArmat(artName);
		boss->initBody();
		boss->setBossLvl(lvl);		
		boss->ShowHpBar();
		boss->autorelease();
		boss->setDeadMusic(Music_BossYiZhiDead);
		return boss;
	}
	CC_SAFE_DELETE(boss);
	return nullptr;
}

void YizhiBoss::initBody()
{
	auto body = PhysicsBody::createBox(Size(150,350),PHYSICSBODY_MATERIAL_DEFAULT,Vec2(-30,100));
	body->setGravityEnable(false);
	body->setRotationEnable(false);
	body->setMass(20000);
	this->setPhysicsBody(body);
	setBitmask(ENEMY_PLANE_CATEGORYBITMASK,0,0);
	setInvincibleAtEdge(true);
}

void YizhiBoss::addAi()
{
	PlaneAI* ai = YizhiBossAI::create(this);
	addChild(ai);
	m_Ai = ai;

	ai->StartAI(m_BossLvl);

	setBitmask(ENEMY_PLANE_CATEGORYBITMASK,
		ENEMY_PLANE_COLLISIONBITMASK,
		ENEMY_PLANE_CONTACTTESTBITMASK);

	getHpBar()->setHpBarOffset(Vec2(0,-120));
}

//----------------------------------
DoubleGunBoss* DoubleGunBoss::create( const std::string& artName, int lvl )
{
	DoubleGunBoss* boss = new (std::nothrow) DoubleGunBoss;

	if (boss && boss->initWithFile("weapon.png"))
	{
		boss->InitSkinWithArmat(artName);
		boss->initBody();
		boss->setBossLvl(lvl);		
		boss->ShowHpBar();
		boss->autorelease();
		boss->setDeadMusic(Music_BossShuangQiangDead);
		return boss;
	}
	CC_SAFE_DELETE(boss);
	return nullptr;
}

void DoubleGunBoss::initBody()
{
	auto body = PhysicsBody::createBox(Size(150,350),PHYSICSBODY_MATERIAL_DEFAULT,Vec2(-30,100));
	body->setGravityEnable(false);
	body->setRotationEnable(false);
	body->setMass(20000);
	this->setPhysicsBody(body);
	setBitmask(ENEMY_PLANE_CATEGORYBITMASK,0,0);
	setInvincibleAtEdge(true);
}

void DoubleGunBoss::addAi()
{
	PlaneAI* ai = DoubleGunBossAI::create(this);
	addChild(ai);
	m_Ai = ai;

	ai->StartAI(m_BossLvl);

	setBitmask(ENEMY_PLANE_CATEGORYBITMASK,
		ENEMY_PLANE_COLLISIONBITMASK,
		ENEMY_PLANE_CONTACTTESTBITMASK);

	getHpBar()->setHpBarOffset(Vec2(0,-120));
}
