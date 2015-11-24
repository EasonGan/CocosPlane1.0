#include "DropGoods.h"
#include "Bullet/Bullet.h"
#include "Layer/PhysicsLayer.h"
#include "Layer/UIController.h"
#include "Weapon/WeaponFactory.h"
#include "Weapon/weapon.h"
#include "../GameCommon/GameData.h"
#include "../GameCommon/GameConfig.h"
#include "../GameUI/GameMainLayer.h"
#include "../GameCommon/NewDataMgr.h"
#include "../GameCommon/EffectsManager.h"
#include "../GameCommon/GameDefine.h"
#include "Bullet/BulletFactory.h"
#include "PhysicsUnit.h"
#include "EnemyPlaneController.h"

void DropGold::onEnter()
{
	PhysicsUnit::onEnter();
    this->initBody();
	//OrbitCamera* orbit = OrbitCamera::create(rand_0_1()*2, 100, 0, 0, 720, 0, 360);
	//runAction( RepeatForever::create( orbit ) );
	
	initMagnetDis();
	InitSpeed();

    float time = 0.3;
    if(this->getDropPro().getGold() == 10) {
        time = 0.5;
    } else if(this->getDropPro().getGold() == 5){
        time = 0.4;
    }
	if (m_isFlyToPlane)
	{
        this->waitTime = random(0.1, 0.5);
		this->runAction(Sequence::create(DelayTime::create(time),CallFunc::create(CC_CALLBACK_0(DropGold::startCheck,this)), nullptr));
	}
}

void DropGold::startCheck()
{
    schedule(schedule_selector(DropGold::MoveToMainPl));
}

void DropGold::startCheckO( Ref* obj )
{
	_physicsBody->setCategoryBitmask(REWARD_CATEGORYBITMASK);
	_physicsBody->setCollisionBitmask(REWARD_COLLISIONBITMASK);
	_physicsBody->setContactTestBitmask(REWARD_CONTACTTESTBITMASK);
	schedule(schedule_selector(DropGold::MoveToMainPl));
}

void DropGold::initBody()
{
	auto body = PhysicsBody::createCircle(1);
	body->setGravityEnable(false);
	if (m_DropMod == DropMod::Drop_Mod)
	{
		body->setGravityEnable(true);
	}
	body->setRotationEnable(true);
	body->setMass(1);
	body->setCategoryBitmask(REWARD_CATEGORYBITMASK);
	body->setCollisionBitmask(REWARD_COLLISIONBITMASK);
	body->setContactTestBitmask(REWARD_CONTACTTESTBITMASK);
	setPhysicsBody(body);

}

void DropGold::DoSomethingBeforeDead()
{
	if (m_DeadType == DeadType::Dead_Nature)
	{
		return;
	}

	if (m_DeadType == DeadType::Dead_Force)
	{
		return;
	}

	StatictisNum();
	
	//BOSS死亡的话那么不触发
	bool isBossDead = GameData::getInstance()->getIsBossDead();
	bool isValid = GameData::getInstance()->getIsRewardValid(); 
	if (!isBossDead && isValid)
	{
		ImpactOfBuff();
		Rewards();
	}
}

void DropGold::InitSpeed()
{
	switch (m_DropMod)
	{
	case Place_Mod:
		{
			setPosition(m_StartPos);
            float speed = SPEED_MOD_PLACE;
			_physicsBody->setVelocity(Vec2(0,speed));
		}
		break;
	case Drop_Mod:
		{
			float offsetX = (random(0,8) - 3.5) * 50;
			float offsetY = 900;
			auto myPos = getPosition();
			if (myPos.x > m_WinSize.width * 0.7)
			{
				offsetX = -200;
			}
			else if(myPos.x < m_WinSize.width * 0.3)
			{
				offsetX = 200;
			}
			auto disY = m_WinSize.height - myPos.y;
			if (disY < 300)
			{
				offsetY = disY;
			}
			Vec2 vec = Vec2(offsetX, offsetY);
			_physicsBody->setVelocity(vec);
			_physicsBody->applyForce(Vec2(0,-1500));
		}
		break;
	default:
		break;
	}
}

void DropGold::updateRewards(const int& rewardtype )
{
	
}

void DropGold::ActionCallback( Node* node )
{
	node->removeFromParentAndCleanup(true);
}

void DropGold::PlayMusicWhenDead()
{
	if (random(1,4) == 1)
	{
		NewAudio::getInstance()->playEffect(Music_getGolds);
	}
}

void DropGold::StatictisNum()
{
	if (m_BasePro.getMyId() == UnitId::eRedCard)
	{
		NewDataMgr::getInstance()->mGameTaskSystem->_GamePropertyArr[GamePrKey_getRedBallNum] += 1;
	}
	else if(m_BasePro.getMyId() == UnitId::eBlueCard)
	{
		NewDataMgr::getInstance()->mGameTaskSystem->_GamePropertyArr[GamePrKey_getBlueBallNum] += 1;
	}
	else if(m_BasePro.getMyId() == UnitId::eYellowCard)
	{
		NewDataMgr::getInstance()->mGameTaskSystem->_GamePropertyArr[GamePrKey_getYellowBallNum] += 1;
	}

	bool isHas = false;
	auto mainpl = GameData::getInstance()->getMainPlane();
	if (mainpl)
	{
		isHas = mainpl->getIsHasMagnet();
	}

	if(isHas)
	{
		//m_MagnetDis = 400;
		//unschedule(schedule_selector(DropGold::IsMainPlHasMagnet));
	}
}

void DropGold::MoveToMainPl( float dt )
{
    if(this->waitTime > 0) {
        this->waitTime -= dt;
        return;
    }

	auto mainPl = GameData::getInstance()->getMainPlane();
	if (!mainPl)
	{
		return;
	}
//	Vec2 myPos = convertToWorldSpaceAR(Vec2::ZERO);
//    Vec2 plPos = mainPl->convertToWorldSpaceAR(Vec2::ZERO);

    Vec2 myPos = this->getPhysicsBody()->getPosition();
    Vec2 plPos = mainPl->getPhysicsBody()->getPosition();
	float dis = myPos.distance(plPos);

	if (dis < m_MagnetDis)
	{
        if(this->getPhysicsBody()->isGravityEnabled()) {
            this->getPhysicsBody()->setGravityEnable(false);
        }
        Vec2 vec = plPos - myPos;
        vec.normalize();
        float speed = 1800;
        if(dis > speed * dt) {
            this->getPhysicsBody()->setVelocity(vec * speed);
        } else {
//            this->RemoveMyself();
            this->getPhysicsBody()->setVelocity(vec * 200);
        }
//		auto newPos = MoveStep(plPos, myPos, 35);
//        this->getPhysicsBody()->setPosition(newPos);
//		setPosition(newPos);
	}
}

void DropGold::setUpSpeed( float speed )
{
	m_UpSpeed = speed;
}

void DropGold::setRewardsInfo( int gold /*= 0*/, int score /*= 0*/ )
{
	m_DropPro.setGold(gold);
	m_DropPro.setScore(score);
}

void DropGold::DeathImpactOfDrops()
{
	GameData::getInstance()->addScore(m_DropPro.getScore());
	GameData::getInstance()->addGoldNum(m_DropPro.getGold());
}

void DropGold::ImpactOfBuff()
{
	auto mainpl = GameData::getInstance()->getMainPlane();
	if (mainpl)
	{
		if (m_DropPro.getBuff().buffId <= BuffId::Buff_default)
		{
			return;
		}
		bool isActive = mainpl->isBuffActive(m_DropPro.getBuff().buffId);
		if (isActive)
		{
			mainpl->resetBuff(m_DropPro.getBuff().buffId);
			return;
		}
		auto newBuff = BaseBuff::create(mainpl);
		newBuff->ActiveBuff(m_DropPro.getBuff().buffId,m_DropPro.getBuff().durarion);
		mainpl->addChild(newBuff);
	}
}

void DropGold::OtherDealWhenContact( const BasePro& rs )
{
}

void DropGold::ListenMainplFuhuo()
{
    this->waitTime = random(0.1, 0.5);
	NotificationCenter::getInstance()->addObserver(this,callfuncO_selector(DropGold::startCheckO), Msg_MainPlFuhuo, nullptr);
}

void DropGold::initMagnetDis()
{
	if(m_DropMod == DropMod::Place_Mod) 
	{
		m_MagnetDis = SUCTION_DIS_MOD_PLACE;
		//setMaxChance(2);
	}
	else 
	{
		m_MagnetDis = SUCTION_DIS_MOD_DROP;
	}
}

void DropGold::playerEffectGetShield()
{
	//this->playEffectGetUserItem(1);
	UIController::getInstance()->getMainUILayer()->showAddShieldEffect();
}

void DropGold::playerEffectGetSkill()
{
	UIController::getInstance()->getMainUILayer()->showAddSkillEffect();
	//this->playEffectGetUserItem(2);
}

void DropGold::playEffectGetUserItem(int userItemType)
{

	
}

void DropGold::effectCallBack2(Node* node,std::string str)
{
	if(!node)
	{
		return;
	}
	ParticleSystemQuad* particle4 = EffectsManager::addParticle(str,node,Vec2(0,140),false);
	particle4->setPositionType(ParticleSystem::PositionType::GROUPED);
}

void DropGold::effectCallBack(Node* node)
{
	if(!node)
	{
		return;
	}
	node->setOpacity(255);
	node->setScale(0.1);
	node->runAction(Sequence::create(ScaleTo::create(0.1,1),ScaleTo::create(0.1,0.7),ScaleTo::create(0.1,1),FadeOut::create(0.5), nullptr));
	node->getParent()->runAction(Sequence::create(DelayTime::create(0.9),RemoveSelf::create(true), nullptr));
}

DropItem* DropItem::create(int type, int addNum, int score, GameResource* resource, std::string name) {
    DropItem* sf = new (std::nothrow) DropItem;
    sf->drop_item_type = type;
    sf->drop_item_num = addNum;
    sf->drop_item_score = score;
    sf->drop_item_resource = resource;
    sf->drop_item_name = name;
    sf->autorelease();
    return sf;
}

DropsFactory* DropsFactory::create()
{
	DropsFactory* sf = new (std::nothrow) DropsFactory;
	if (sf && sf->init())
	{
		sf->retain();
		sf->autorelease();
		return sf;
	}
	else
	{
		CC_SAFE_DELETE(sf);
		return nullptr;
	}

}

DropGold* DropsFactory::getDropGoldFromRecoveryVector()
{
    DropGold* item = nullptr;
    if(this->recovery_vector.size() > 0) {
        item = this->recovery_vector.front();
        this->recovery_vector.eraseObject(item);
    }
    return item;
}

DropGoodsType DropsFactory::getGoodsTypeFromItemId(int itemId)
{
    switch (itemId) {
        case 5:
            return DropGoodsType::DropGoods_Gold1;
        case 6:
            return DropGoodsType::DropGoods_Gold2;
        case 7:
            return DropGoodsType::DropGoods_Gold3;
        case 8:
            return DropGoodsType::DropGoods_Shield;
        case 9:
            return DropGoodsType::DropGoods_SKill;
        default:
            return DropGoodsType::DropGoods_Gold1;
    }
}

bool DropsFactory::init()
{
    this->drop_item_config.insert(5,DropItem::create(DropItem::TYPE_GOLD, 1, 10, GameResource::create(GameResource::TYPE_FRAME, "gold_1", 8),  ""));
    this->drop_item_config.insert(6,DropItem::create(DropItem::TYPE_GOLD, 5, 10, GameResource::create(GameResource::TYPE_FRAME, "gold_2", 8), ""));
    this->drop_item_config.insert(7,DropItem::create(DropItem::TYPE_GOLD, 10, 10, GameResource::create(GameResource::TYPE_FRAME, "gold_3", 8), ""));
    this->drop_item_config.insert(8,DropItem::create(DropItem::TYPE_SHIELD, 1, 10, GameResource::create(GameResource::TYPE_PNG, "drop_item_shield.png"), ""));
    this->drop_item_config.insert(9,DropItem::create(DropItem::TYPE_SKILL, 1, 10, GameResource::create(GameResource::TYPE_PNG, "drop_item_skill.png"), ""));

    return true;
}

void DropsFactory::purge()
{
    this->drop_item_config.clear();
	this->release();
}

void DropsFactory::addDropItemRandom(const Point& pos, int dropItemId, unsigned int num)
{
    for(unsigned int i = 0 ; i < num ; i ++) 
	{
        auto p = getRandPos(pos);
        this->createDropItem(p, dropItemId, DropMod::Drop_Mod);
    }
}

DropGold* DropsFactory::createDropItem(const Point& pos, int dropItemId,DropMod dropMod,bool isChange,bool isFlyToPl)
{
	auto it = drop_item_config.find(dropItemId);
    if(it != drop_item_config.end())
	{
        DropItem* itemConfigData = this->drop_item_config.at(dropItemId);
        DropGold* item = nullptr;
//        DropGold* item = this->getDropGoldFromRecoveryVector();
        if(itemConfigData->drop_item_type == DropItem::TYPE_GOLD)
		{
            if(item == nullptr) {
                item = DropGold::create();
				item->getBasePro().setMyId(UnitId::eRewardGold);
            }
            if(itemConfigData->drop_item_num == 10) {
				item->getBasePro().setMyId(UnitId::eRewardGreenDiamond);
				item->setScale(1.6);
            } else if(itemConfigData->drop_item_num == 5) {
				item->getBasePro().setMyId(UnitId::eRewardRedDiamond);
                item->setScale(1.3);
            }
            item->m_DropPro.setGold(itemConfigData->drop_item_num);
            item->m_DropPro.setScore(itemConfigData->drop_item_score);
            item->setIsFlyToPlane(isFlyToPl);
            item->isChange = isChange;
            item->m_StartPos = pos;
            item->getDropPro().setGold(itemConfigData->drop_item_num);
        }
		else if(itemConfigData->drop_item_type == DropItem::TYPE_SHIELD)
		{
            if(item == nullptr) {
                item = ShieldItem::create();
                item->setScale(1.3);
            }
        }
		else if(itemConfigData->drop_item_type == DropItem::TYPE_SKILL)
		{
            if(item == nullptr) {
                item = SkillItem::create();
                item->setScale(1.3);
            }
        }
		else
		{
            log("can not find the dropItem type %s","");
        }
        if(item != nullptr)
		{
            DrawSpriteUtil::setSpriteWithGameResource(item,itemConfigData->drop_item_resource);
            item->setDropMod(dropMod);
            this->addDropItemsToLayer(pos, item);
        }
//        log("create num is %d =+=+=+=+=+=+",createNum);
//        log("all num is %d +=+=+=+=+=",allNUm);
        return item;
    }
	else
	{
        return nullptr;
    }
}

void DropsFactory::addDropItemsToLayer(const Point& pos,DropGold* dropGold)
{
    UIController::getInstance()->addScore(dropGold,pos);
}

void DropsFactory::AddDrops(const Point &pos, DropGoodsType tp, int nums) 
{
    switch (tp) {
        case DropGoodsType::DropGoods_Gold1:
            this->addDropItemRandom(pos, 5, nums);
            break;
        case DropGoodsType::DropGoods_Gold2:
            this->addDropItemRandom(pos, 6, nums);
            break;
        case DropGoodsType::DropGoods_Gold3:
            this->addDropItemRandom(pos, 7, nums);
            break;
        case DropGoodsType::DropGoods_Shield:
            this->addDropItemRandom(pos, 8, nums);
            break;
        case DropGoodsType::DropGoods_SKill:
            this->addDropItemRandom(pos, 9, nums);
            break;
    }

    for (int i = 0; i < nums; ++i) {
        switch (tp) {
            case DropGoodsType::DropGoods_RedWp:
                DropWithRedWp(pos);
                break;
            case DropGoodsType::DropGoods_YellowWp:
                DropWithYellowWp(pos);
                break;
            case DropGoodsType::DropGoods_BlueWp:
                DropWithBlueWp(pos);
                break;
            default:
                break;
        }
    }
}

void DropsFactory::DropWithRedWp( const Point& pos )
{
	auto p = getRandPos(pos);

	auto drop = DropRedWp::create(getResByTag(ResTag::Drops_RedWp));
	drop->setUnitId(UnitId::eRedCard);
	UIController::getInstance()->addScore(drop,p);
}

void DropsFactory::DropWithBlueWp( const Point& pos )
{
	auto p = getRandPos(pos);

	auto drop = DropBlueWp::create(getResByTag(ResTag::Drops_BlueWp));
	drop->setUnitId(UnitId::eBlueCard);
	UIController::getInstance()->addScore(drop,p);
}

void DropsFactory::DropWithYellowWp( const Point& pos )
{
	auto p = getRandPos(pos);

	auto drop = DropYellowWp::create(getResByTag(ResTag::Drops_YellowWp));
	drop->setUnitId(UnitId::eYellowCard);
	UIController::getInstance()->addScore(drop,p);
}

std::string DropsFactory::getResByTag( ResTag tag )
{
	std::string res = "";
	switch (tag)
	{
	case ResTag::Drops_WpBuff1:
		res = "WpBuff1.png";
		break;
	case ResTag::Drops_RedWp:
		res = "redWp.png";
		break;
	case ResTag::Drops_YellowWp:
		res = "yellowWp.png";
		break;
	case ResTag::Drops_BlueWp:
		res = "blueWp.png";
		break;
    case ResTag::Drops_SuperWp:
        res = "drop_item_super.png";
        break;
	case ResTag::Drops_Golds1:
		res = "DropGold1.png";
		break;
	case ResTag::Drops_Golds2:
		res = "DropGold2.png";
		break;
	case ResTag::Drops_Golds3:
		res = "DropGold3.png";
		break;
	case ResTag::Drops_AddHp100:
		res = "DropHp100.png";
		break;
	case ResTag::Drops_AddHp500:
		res = "DropHp500.png";
		break;
	case ResTag::Drops_Magnet:
		res = "DropMagnet.png";
		break;
    case ResTag::Drops_Shield:
        res = "drop_item_shield.png";
        break;
    case ResTag::Drops_Skill:
        res = "drop_item_skill.png";
        break;
	default:
		break;
	}
	return res;
}

cocos2d::Vec2 DropsFactory::getRandPos(const Vec2& centerPos)
{
	float randNumX = rand_0_1() - 0.5;
	float randNumY = rand_0_1() - 0.5;
	return Vec2(centerPos.x + 100 * randNumX,centerPos.y + 150 * randNumY);
}


void DropRedWp::OtherDealWhenContact(const BasePro& pro )
{
	UIController::getInstance()->getEffectsLayer()->playEffGetRewards(1);
}

void DropRedWp::delayGive()
{
	bool b = UIController::getInstance()->getMainUILayer()->isRealDead();
	if (b)
	{
		return;
	}
	bool isValid = GameData::getInstance()->getIsRewardValid(); 
	if (!isValid)
	{
		return;
	}

	if(!this->couldAddWp()) {
		return;
	}

	auto cardType = NewDataMgr::getInstance()->mRoleConfig.cardArr[0].cardID;
	auto cardLvl = NewDataMgr::getInstance()->mRoleConfig.cardArr[0].cardStarLv;
	cardType = cardType%3;

	int hurt = DataConfig::getCardHurts(DropGoodsType::DropGoods_RedWp, cardType, cardLvl);
	switch (cardType)
	{
	case 2:
		{
			//花
		cardLvl = 5;
		addFlower(cardLvl, hurt);
		}
		break;
	case 0:
		{
			//飞镖
		cardLvl = 5;
		addFeibiao(cardLvl, hurt);
		}
		break;
	case 1:
		{
			if (cardLvl <= 2)
			{
				cardLvl = 1;
			}
			else if(cardLvl <= 4)
			{
				cardLvl = 2;
			}
			else
			{
				cardLvl = 3;
			}
			//火圈
			cardLvl = 2;
			hurt = DataConfig::getCardHurts(DropGoodsType::DropGoods_RedWp, cardType, 1);
			addFireCircle(cardLvl, hurt);
		}
		break;
	default:
		break;
	}
}

void DropRedWp::onEnter()
{
	WpDrops::onEnter();

}

void DropYellowWp::delayGive()
{
	bool b = UIController::getInstance()->getMainUILayer()->isRealDead();
	if (b)
	{
		return;
	}
	bool isValid = GameData::getInstance()->getIsRewardValid(); 
	if (!isValid)
	{
		return;
	}
	if(!this->couldAddWp()) {
		return;
	}

	auto cardType = NewDataMgr::getInstance()->mRoleConfig.cardArr[2].cardID;
	auto cardLvl = NewDataMgr::getInstance()->mRoleConfig.cardArr[2].cardStarLv;
	cardType %= 3;
	//log("------ cardType	= %d", cardType);
	//log("++++++ cardlvl		= %d", cardLvl);

	int hurt = DataConfig::getCardHurts(DropGoodsType::DropGoods_YellowWp, cardType, cardLvl);

	switch (cardType)
	{
	case 0:
		{
			//龙
			cardLvl = 5;
			addDragon(cardLvl, hurt);
		}
		break;
	case 2:
		{
			//激光
			cardLvl = 3;
			addLaser(cardLvl, hurt);
		}
		break;
// 	case 0:
// 		{
// 			//闪电
// 			//addLighting(cardLvl); //??????×????ó????
// 		}
		break;

	default:
		break;
	}
}

void DropYellowWp::OtherDealWhenContact( const BasePro& pro )
{
	UIController::getInstance()->getEffectsLayer()->playEffGetRewards(2);
}

void DropYellowWp::onEnter()
{
	WpDrops::onEnter();
	//?ù???¨???à??????????????????????????????
	auto cardType = NewDataMgr::getInstance()->mRoleConfig.cardArr[2].cardID;
	auto cardLvl = NewDataMgr::getInstance()->mRoleConfig.cardArr[2].cardStarLv;
	cardType = cardType%3;

	int hurt = DataConfig::getCardHurts(DropGoodsType::DropGoods_YellowWp,cardType, cardLvl);

	switch (cardType)
	{
	case 1:
		{
		//闪电
		if(!this->couldAddWp()) {
			return;
		}
		addLighting(cardLvl, hurt);
		}
		break;

	default:
		break;
	}
}

void DropBlueWp::delayGive()
{
	bool b = UIController::getInstance()->getMainUILayer()->isRealDead();
	if (b)
	{
		return;
	}
	bool isValid = GameData::getInstance()->getIsRewardValid(); 
	if (!isValid)
	{
		return;
	}
	if(!this->couldAddWp()) {
		return;
	}

	//?ù???¨???à??????????????????????????????
	auto cardType = NewDataMgr::getInstance()->mRoleConfig.cardArr[1].cardID;
	//log("------ cardType	= %d", cardType);
	auto cardLvl = NewDataMgr::getInstance()->mRoleConfig.cardArr[1].cardStarLv;
	cardType = cardType%3;
	//log("------ cardType	= %d", cardType);
	//log("++++++ cardlvl		= %d", cardLvl);

	int hurt = DataConfig::getCardHurts(DropGoodsType::DropGoods_BlueWp,cardType, cardLvl);
	switch (cardType)
	{
	case 1:
		{
			//水泡
		cardLvl = 1;
		addMissile(cardLvl, hurt);
		}
		break;
	case 0:
		{
			//猫
		cardLvl = 5;
		addCat(cardLvl, hurt);
		}
		break;
	case 2:
		{
			//冰剑
		cardLvl = 5;
		addIcePiton(cardLvl, hurt);
		}
		break;

	default:
		break;
	}
}

void ShieldItem::OtherDealWhenContact(const BasePro& pro)
{

}

void SkillItem::OtherDealWhenContact(const BasePro& pro)
{

}

void DropBlueWp::OtherDealWhenContact( const BasePro& pro )
{
	UIController::getInstance()->getEffectsLayer()->playEffGetRewards(3);
}

void DropBlueWp::onEnter()
{
	WpDrops::onEnter();
}

void WpDrops::onEnter()
{
	DropGold::onEnter();
}

void WpDrops::Rewards()
{
	auto pl = GameData::getInstance()->getMainPlane();
	if (!pl)
	{
		return;
	}
	ActionInterval* delay = DelayTime::create(1.0f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(WpDrops::delayGive));
	ActionInterval* seq = Sequence::create(delay,func, nullptr);
	pl->runAction(seq);
}

void WpDrops::delayGive()
{

}

void WpDrops::PlayMusicWhenDead()
{
	//???????§
	NewAudio::getInstance()->playEffect(Music_getDrops);
}

void WpDrops::addFeibiao( int lvl, int hurts )
{
	auto wpfactory = GameData::getInstance()->getWeaponFactory();
	auto pl = GameData::getInstance()->getMainPlane();
	if (!pl)
	{
		return;
	}

	switch (lvl)
	{
	case 1:
	case 2:
		for (int i = 0; i < 5; i++)
		{
			int Hurt = hurts/3;
			auto wp = wpfactory->SpecialWp_SmallDart_S(pl,Hurt,10.0,0.5);
			wp->setRotation((i-2) * 10);
			ActionInterval* rotate = RotateBy::create(0.2f,6);
			ActionInterval* rep = RepeatForever::create(rotate);
			wp->runAction(rep);
			wp->setBltSpeed(500);
		}
		break;
	case 3:
	case 4:
		{
			//3??????
			auto pos = pl->convertToWorldSpaceAR(Vec2::ZERO);

			//????????
			auto bulletfactory = GameData::getInstance()->getBulletFactory();
			{
				DartBullet* bullet = DartBullet::create(bulletfactory->getResByTag(ResTag::Bullet_Card_BigDart));
				bullet->setPosition(pos);
				bullet->setScale(2.0f);
				bullet->resetHurt(hurts * 0.6);
				bullet->setBelongTo(UnitId::eRedCard);	//?è?????????ì?ò
				UIController::getInstance()->getEffectsLayer()->addChild(bullet);
			}
			for (int i = 0; i < 2; i++)
			{
				int direct = (i-0.5) * 2.0f;
				DartBulletRebounce* bullet = DartBulletRebounce::create(bulletfactory->getResByTag(ResTag::Bullet_Card_SmallDart),direct);
				bullet->setPosition(pos);
				bullet->setScale(1.0f);
				bullet->resetHurt(hurts * 0.2);
				bullet->setBelongTo(UnitId::eRedCard);	//?è?????????ì?ò
				UIController::getInstance()->getEffectsLayer()->addChild(bullet);
			}
		}
		break;
	case 5:
		{
			int Hurt = hurts/10;

			auto bulletFac = GameData::getInstance()->getBulletFactory();
			auto pos = pl->convertToWorldSpaceAR(Vec2::ZERO);
			auto bullet = DartBulletTrace::create(bulletFac->getResByTag(ResTag::Bullet_Card_BigDart));
			bullet->setPosition(pos);
			bullet->resetHurt(Hurt);
			bullet->setBelongTo(UnitId::eRedCard);	//?è?????????ì?ò
			UIController::getInstance()->getEffectsLayer()->addChild(bullet);
		}
		break;

	default:
		break;
	}
}

void WpDrops::addCat( int lvl,int hurts )
{
	auto pl = GameData::getInstance()->getMainPlane();
	if (!pl)
	{
		return;
	}
	switch (lvl)
	{
	case 1:
	case 2:
		{
			auto pos = pl->convertToWorldSpaceAR(Vec2::ZERO);
			pos.y += 100;
			//?????¨
			{
				auto bullet = CatBullet::create(true);
				bullet->setPosition(pos);
				bullet->resetHurt(hurts * 0.5);
				bullet->setBelongTo(UnitId::eBlueCard);	//?è???????????ò
				UIController::getInstance()->getEffectsLayer()->addChild(bullet);
			}
			{
				auto bullet = CatBullet::create(false);
				bullet->setPosition(pos);
				bullet->resetHurt(hurts * 0.5);
				bullet->setBelongTo(UnitId::eBlueCard);	//?è???????????ò
				UIController::getInstance()->getEffectsLayer()->addChild(bullet);
			}
		}
		break;
	case 3:
	case 4:
		{
			auto pos = pl->convertToWorldSpaceAR(Vec2::ZERO);
			pos.y += 100;
			//?????¨
			{
				auto bullet = CatBullet::create(true, CatBullet::wolf);
				bullet->setPosition(pos);
				bullet->resetHurt(hurts * 0.5);
				bullet->setBelongTo(UnitId::eBlueCard);	//?è???????????ò
				UIController::getInstance()->getEffectsLayer()->addChild(bullet);
			}
			{
				auto bullet = CatBullet::create(false,CatBullet::wolf);
				bullet->setPosition(pos);
				bullet->resetHurt(hurts * 0.5);
				bullet->setBelongTo(UnitId::eBlueCard);	//?è???????????ò
				UIController::getInstance()->getEffectsLayer()->addChild(bullet);
			}
		}
		break;
	case 5:
		{
			auto pos = pl->convertToWorldSpaceAR(Vec2::ZERO);
			pos.y += 100;
			//?????¨
			{
				auto bullet = CatBullet::create(true, CatBullet::tiger);
				bullet->setPosition(pos);
				bullet->resetHurt(hurts * 0.5);
				bullet->setBelongTo(UnitId::eBlueCard);	//?è???????????ò
				UIController::getInstance()->getEffectsLayer()->addChild(bullet);
			}
			{
				auto bullet = CatBullet::create(false,CatBullet::tiger);
				bullet->setPosition(pos);
				bullet->resetHurt(hurts * 0.5);
				bullet->setBelongTo(UnitId::eBlueCard);	//?è???????????ò
				UIController::getInstance()->getEffectsLayer()->addChild(bullet);
			}
		}
		break;

	default:
		break;

	}
	NewAudio::getInstance()->playEffect(Music_Tiger);
}

void WpDrops::addLaser( int lvl,int hurts )
{
	auto pl = GameData::getInstance()->getMainPlane();
	if (!pl)
	{
		return;
	}
	int Hurt = hurts/10;

	switch (lvl)
	{
	case 1:
	case 2:
		{
			auto mainLaser = GameData::getInstance()->getWeaponFactory()->
				SpecialWP_Laser_S(pl,Hurt,5.0f,1.0f);
		}
		break;
	case 3:
	case 4:
		{
			auto skinSize = pl->getContentSize();

			auto mainLaser = LaserBullet::create(2);
			mainLaser->setBelongTo(UnitId::eYellowCard);	//?è???????????ò
			mainLaser->setPosition(Vec2(skinSize.width*0.5, skinSize.height*0.5));
			mainLaser->resetHurt(Hurt * 0.6);
			pl->addChild(mainLaser,-1);

			auto outside3 = OutsideLaser::create(Vec2(-80,0),-1);
			outside3->setPosition(Vec2(skinSize.width*0.5 - 80, skinSize.height*0.5 - 50));
			outside3->resetHurt(Hurt * 0.2);
			outside3->setBelongTo(UnitId::eYellowCard);	//?è???????????ò
			pl->addChild(outside3);

			auto outside6 = OutsideLaser::create(Vec2(80,0),1);
			outside6->setPosition(Vec2(skinSize.width*0.5 + 80, skinSize.height*0.5 - 50));
			outside6->resetHurt(Hurt * 0.2);
			outside6->setBelongTo(UnitId::eYellowCard);	//?è???????????ò
			pl->addChild(outside6);
		}
		break;
	case 5:
		{
			auto skinSize = pl->getContentSize();

			auto mainLaser = LaserBullet::create(3);
			mainLaser->setBelongTo(UnitId::eYellowCard);	//?è???????????ò
			mainLaser->setPosition(Vec2(skinSize.width*0.5, skinSize.height*0.5));
			mainLaser->setScale(1.3);
			mainLaser->resetHurt(Hurt * 0.6);
			pl->addChild(mainLaser,-1);

			auto outside3 = OutsideLaser::create(Vec2(-120,0),-1);
			outside3->setPosition(Vec2(skinSize.width*0.5 - 120, skinSize.height*0.5 - 50));
			outside3->resetHurt(Hurt * 0.2);
			outside3->setBelongTo(UnitId::eYellowCard);	//?è???????????ò
			pl->addChild(outside3);

			auto outside6 = OutsideLaser::create(Vec2(120,0),1);
			outside6->setPosition(Vec2(skinSize.width*0.5 + 120, skinSize.height*0.5 - 50));
			outside6->resetHurt(Hurt * 0.2);
			outside6->setBelongTo(UnitId::eYellowCard);	//?è???????????ò
			pl->addChild(outside6);
		}
		break;
	default:
		break;
	}

}

void WpDrops::addFlower( int lvl,int hurts )
{
	auto pl = GameData::getInstance()->getMainPlane();
	if (!pl)
	{
		return;
	}
	switch (lvl)
	{
	case 1:
	case 2:
		{
			auto pos = pl->convertToWorldSpaceAR(Vec2::ZERO);
			auto bullet = ThreePetalsBullet::create("particla/Petals/Lvl_1/flower_lvl1.png");
			bullet->setPosition(pos);
			bullet->setBelongTo(UnitId::eRedCard);	//?è?????????ì?ò
			bullet->resetHurt(hurts);
			bullet->setScale(1.5f);
			UIController::getInstance()->getEffectsLayer()->addChild(bullet);
		}
		break;
	case 3:
	case 4:
		{
			auto pos = pl->convertToWorldSpaceAR(Vec2::ZERO);
			auto bullet = ThreePetalsBullet::create("particla/Petals/flower.png");
			bullet->setPosition(pos);
			bullet->setScale(2.5f);
			bullet->setBelongTo(UnitId::eRedCard);	//?è?????????ì?ò
			bullet->resetHurt(hurts);
			UIController::getInstance()->getEffectsLayer()->addChild(bullet);
		}
		break;
	case 5:
		{
			int Hurt = hurts/10;

			auto pos = pl->convertToWorldSpaceAR(Vec2::ZERO);
			FlowerBullet* bullet = FlowerBullet::create();
			bullet->setPosition(pos);
			bullet->resetHurt(Hurt);
			bullet->setBelongTo(UnitId::eRedCard);	//?è?????????ì?ò
			UIController::getInstance()->getEffectsLayer()->addChild(bullet,-1);
		}
		break;

	default:
		break;
	}
}

void WpDrops::addMissile( int lvl,int hurts )
{
	auto pl = GameData::getInstance()->getMainPlane();
	if (!pl)
	{
		return;
	}
	switch (lvl)
	{
	case 1:
	case 2:
		{
			{
				auto wp = GameData::getInstance()->getWeaponFactory()->SpecialWP_IceMisile_S(pl,hurts*0.5,10.0f,0.5f);
				wp->setOffset(Vec2(-30,20));
				wp->setRotation(-70);
			}
			{
				auto wp = GameData::getInstance()->getWeaponFactory()->SpecialWP_IceMisile_S(pl,hurts*0.5,10.0f,0.5f);
				wp->setOffset(Vec2(30,20));
				wp->setRotation(70);
			}
		}
		break;
	case 3:
	case 4:
		{
			//2????????????±?
			{
				auto wp = GameData::getInstance()->getWeaponFactory()->SpecialWP_IceMisile_S(pl,hurts*0.25,10.0f,0.5f);
				wp->setOffset(Vec2(-30,20));
				wp->setRotation(-70);
			}
			{
				auto wp = GameData::getInstance()->getWeaponFactory()->SpecialWP_IceMisile_S(pl,hurts*0.25,10.0f,0.5f);
				wp->setOffset(Vec2(30,20));
				wp->setRotation(70);
			}
			//1???ó??????????
			{
				auto wp = GameData::getInstance()->getWeaponFactory()->SpecialWP_IceMisile_S(pl,hurts*0.5,10.0f,0.5f);
				wp->setOffset(Vec2(0,50));
				wp->setBltScaleX(3.0);
				wp->setBltScaleY(3.0);
			}
		}
		break;
	case 5:
		{
			{
				auto wp = GameData::getInstance()->getWeaponFactory()->SpecialWP_IceMisile_S(pl,hurts*0.25,10.0f,1.0f);
				wp->setOffset(Vec2(-20,-40));
				wp->setDelayShoot(0.3f);
				wp->setRotation(-90);
			}
			{
				auto wp = GameData::getInstance()->getWeaponFactory()->SpecialWP_IceMisile_S(pl,hurts*0.25,10.0f,1.0f);
				wp->setOffset(Vec2(20,-40));
				wp->setDelayShoot(0.3f);
				wp->setRotation(90);
			}
			{
				auto wp = GameData::getInstance()->getWeaponFactory()->SpecialWP_IceMisile_S(pl,hurts*0.25,10.0f,1.0f);
				wp->setOffset(Vec2(-30,20));
				wp->setRotation(-50);
				wp->setBltScaleX(1.5);
				wp->setBltScaleY(1.5);
			}
			{
				auto wp = GameData::getInstance()->getWeaponFactory()->SpecialWP_IceMisile_S(pl,hurts*0.25,10.0f,1.0f);
				wp->setOffset(Vec2(30,20));
				wp->setRotation(50);
				wp->setBltScaleX(1.5);
				wp->setBltScaleY(1.5);
			}
		}
		break;

	default:
		break;
	}



}

void WpDrops::addDragon( int lvl,int hurts )
{
	auto pl = GameData::getInstance()->getMainPlane();
	if (!pl)
	{
		return;
	}
	hurts /= 5;
	switch (lvl)
	{
	case 1:
	case 2:
		{
			auto pos = pl->convertToWorldSpaceAR(Vec2::ZERO);
			FireDragon* bullet = FireDragon::create(Flaming::eSmall);
			bullet->setPosition(pos);
			bullet->resetHurt(hurts);
			bullet->setBelongTo(UnitId::eYellowCard);	//?è???????????ò
			UIController::getInstance()->getEffectsLayer()->addChild(bullet,-1);
		}
		break;
	case 3:
	case 4:
		{
			auto pos = pl->convertToWorldSpaceAR(Vec2::ZERO);
			FireDragon* bullet = FireDragon::create(Flaming::eMiddle);
			bullet->setPosition(pos);
			bullet->resetHurt(hurts);
			bullet->setBelongTo(UnitId::eYellowCard);	//?è???????????ò
			UIController::getInstance()->getEffectsLayer()->addChild(bullet,-1);
		}
		break;
	case 5:
		{
			auto pos = pl->convertToWorldSpaceAR(Vec2::ZERO);
			FireDragon* bullet = FireDragon::create(Flaming::eBig);
			bullet->setPosition(pos);
			bullet->resetHurt(hurts);
			bullet->setBelongTo(UnitId::eYellowCard);	//?è???????????ò
			UIController::getInstance()->getEffectsLayer()->addChild(bullet,-1);
		}
		break;

	default:
		break;
	}
}

void WpDrops::addFireCircle( int lvl,int hurts )
{
	auto mainPl = GameData::getInstance()->getMainPlane();
	if (!mainPl)
	{
		return;
	}

	auto pos = mainPl->convertToWorldSpaceAR(Vec2::ZERO);
	auto s = mainPl->getContentSize();
	BulletSurroundMainPl* bullet = BulletSurroundMainPl::create(lvl);
	bullet->setPosition(s.width*0.5,s.height*0.5);
	bullet->resetHurt(hurts);
	bullet->setBelongTo(UnitId::eRedCard);
	mainPl->addChild(bullet);
}

void WpDrops::addIcePiton( int lvl,int hurts )
{
	auto mainPl = GameData::getInstance()->getMainPlane();
	if (!mainPl)
	{
		return;
	}
	auto pos = mainPl->convertToWorldSpaceAR(Vec2::ZERO);

	switch (lvl)
	{
	case 1:
	case 2:
		for (int i = 0; i < 2; ++i)
		{
			AirazorBullet* bullet = AirazorBullet::create("ammo/sword/sword1.png",(i - 0.5)*60.0f,false);
			bullet->setPosition(pos);
			bullet->setBelongTo(UnitId::eBlueCard);	//?è???????????ò
			bullet->resetHurt(hurts/2);
			UIController::getInstance()->getEffectsLayer()->addChild(bullet);
		}
		break;
	case 3:
	case 4:
		for (int i = 0; i < 3; ++i)
		{
			AirazorBullet* bullet = AirazorBullet::create("ammo/sword/sword2.png", (i - 1)*40.0f,false);
			bullet->setPosition(pos);
			bullet->setBelongTo(UnitId::eBlueCard);	//?è???????????ò
			bullet->resetHurt(hurts/3);
			UIController::getInstance()->getEffectsLayer()->addChild(bullet);
		}
		break;
	case 5:
		for (int i = 0; i < 2; ++i)
		{
			AirazorBullet* bullet = AirazorBullet::create("ammo/sword/sword3.png", (i - 0.5)*40.0f,true);
			bullet->setPosition(pos);
			bullet->setBelongTo(UnitId::eBlueCard);	//?è???????????ò
			bullet->resetHurt(hurts/4);
			UIController::getInstance()->getEffectsLayer()->addChild(bullet);
		}
		for (int i = 0; i < 2; ++i)
		{
			AirazorBullet* bullet = AirazorBullet::create("ammo/sword/sword3.png", (i - 0.5)*120.0f,true);
			bullet->setPosition(pos);
			bullet->setBelongTo(UnitId::eBlueCard);	//?è???????????ò
			bullet->resetHurt(hurts/4);
			UIController::getInstance()->getEffectsLayer()->addChild(bullet);
		}
		break;

	default:
		break;
	}
}

void WpDrops::addLighting( int lvl,int hurts )
{
	switch (lvl)
	{
	case 1:
	case 2:
		{
			BuffStruct buff(BuffId::Buff_LightingLvl1,5.0f);
			m_DropPro.setBuff(buff);
		}
		break;
	case 3:
	case 4:
		{
			BuffStruct buff(BuffId::Buff_LightingLvl2,8.0f);
			m_DropPro.setBuff(buff);
		}
		break;
	case 5:
		{
			BuffStruct buff(BuffId::Buff_LightingLvl3,10.0f);
			m_DropPro.setBuff(buff);
		}
		break;

	default:
		break;
	}
}

bool WpDrops::couldAddWp()
{
	return !GameData::getInstance()->getIsBossDead();
}

void WpDrops::initMagnetDis()
{
	m_MagnetDis = 150;
}

void ShieldItem::DoSomethingBeforeDead()
{
	this->playerEffectGetShield();
    UIController::getInstance()->getMainUILayer()->changeHudunNUm(1);
    auto mainpl = GameData::getInstance()->getMainPlane();
    if(mainpl)
    {
        mainpl->AddProtect();
    }
}


void SkillItem::DoSomethingBeforeDead()
{
	this->playerEffectGetSkill();
    UIController::getInstance()->getMainUILayer()->changeDazhaoNum(1);
}
