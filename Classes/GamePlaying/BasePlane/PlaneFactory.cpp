#include "PlaneFactory.h"
#include "BasePlane.h"
#include "../PhysicsUnit.h"
#include "../Weapon/WeaponFactory.h"
#include "../Weapon/weapon.h"
#include "../Layer/UIController.h"
#include "../../GameCommon/GameData.h"
#include "../../GameCommon/GameConfig.h"
#include "../../GameCommon/EffectsManager.h"
#include "../../GameCommon/GameDefine.h"
#include "../Layer/RollingBg.h"
#include "../Layer/PhysicsLayer.h"
#include "../../GameCommon/NewDataMgr.h"
#include "../../GameCommon/GameSystem.h"

//------------------------
PlaneFactory* PlaneFactory::create()
{
	PlaneFactory* pf = new (std::nothrow) PlaneFactory;
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

void PlaneFactory::purge()
{
	this->release();
}

bool PlaneFactory::init()
{
	do 
	{

	} while (0);

	return true;
}

MainPlane* PlaneFactory::CreateMainPlane(const PlaneType& tp,bool isMoveUp,bool playMusic)
{
	auto curPlane = GameData::getInstance()->getMainPlane();
	if (curPlane)
	{
		//curPlane->removeFromParentAndCleanup(true);
		//GameData::getInstance()->setMainPlane(nullptr);
	}

	CCSize size = CCDirector::sharedDirector()->getWinSize();

	MainPlane* plane = MainPlane::create(tp);
	plane->setUnitId(UnitId::eMainPlane);
	plane->setTouchAble(true);
	plane->setInvincibleAtEdge();
	plane->setPosition(Vec2(size.width*0.5, -80));
	plane->getHpBar()->SetHpBarVisible(false);

	switch (tp)
	{
	case PlaneType::PlaneType_Wind:
		if (playMusic)
		{
			plane->setShootMusic(Music_shootAmmo1);
		}
		plane->setSkinOffset(Vec2(8,0));
		break;
	case PlaneType::PlaneType_Fire:
		if (playMusic)
		{
			plane->setShootMusic(Music_shootAmmo2);
		}
		break;
	case PlaneType::PlaneType_Electric:
		if (playMusic)
		{
			plane->setShootMusic(Music_shootAmmo3);
		}
		break;
	default:
		break;
	}

	if (isMoveUp)
	{
		ActionInterval* moveTo = MoveTo::create(1,Vec2(size.width * 0.5,size.height * 0.3));
		EaseSineOut* in = EaseSineOut::create(moveTo);
		plane->runAction(in);
	}

	int hp = 1;// NewDataMgr::getInstance()->getRoleHpWithId((int)tp);
	plane->setMaxHp(hp);

	int allHurts = -1;// -NewDataMgr::getInstance()->getRoleAtkWithId((int)tp);
	plane->setBltHurts(allHurts);

	int defense = 0;// NewDataMgr::getInstance()->getRoleDefenseWithId((int)tp);
	plane->setDefense(defense);

	{
		int planeLvl = NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(GameGradeMsg::ObjectType_Role)->curGrade;
		planeLvl += 1;
		//planeLvl = (planeLvl-1)/12 + 1;
		auto weaponFact = GameData::getInstance()->getWeaponFactory();
		weaponFact->AddMainPlaneWp((int)tp, planeLvl, plane);
	}
	//plane->setShootAble(false);
	GameData::getInstance()->setMainPlane(plane);

	return plane;
}

BasePlane* PlaneFactory::createEnemyPlane( int id,float userdata, int gamelvl )
{
    int curMapId = UIController::getInstance()->getGameBgMgr()->getCurBgId();

    float val1 = NewDataMgr::getInstance()->getEnemyPlaneWithKey()->HpCoe1;
    float val2 = NewDataMgr::getInstance()->getEnemyPlaneWithKey()->HpCoe2;
    int PlHp = pow((userdata * curMapId * gamelvl), val1) + val2 * userdata;

    val1 = NewDataMgr::getInstance()->getEnemyPlaneWithKey()->ScoreCoe1;
    val2 = NewDataMgr::getInstance()->getEnemyPlaneWithKey()->ScoreCoe2;
    int score = pow((userdata * curMapId * gamelvl), val1) + val2 * userdata;

    int hurt = -1000; //GameData::getInstance()->getBulletHurtsOfEnemyPl(userdata);

	LevelPlaneConfigData configData;
	configData.set_health(PlHp);
	configData.set_attack(hurt);
	configData.set_score(score);
	configData.set_name(id);

    BasePlane* pl = createEnemyPlane(&configData);
	return pl;
}


BasePlane* PlaneFactory::createEnemyPlane(LevelPlaneConfigData* configData)
{
    int id = configData->name();
    int PlHp = configData->health();
    int score = configData->score();
    int hurt = -configData->attack();
	BasePlane* pl = nullptr;

	//PlHp *= 1000;
	//id = 1011;
	if(id < 1000)
	{
		/*if(	id != 2 && 
			id != 3 &&
			id != 4 && 
			id != 5 &&
			id != 8 &&
			id != 9 &&
			id != 10 &&
			id != 15 &&
			id != 20 &&
			id != 21 &&
			id != 22 &&
			id != 23 &&
			id != 24 &&
			id != 25 &&
			id != 26	)*/
		{
			std::string file = String::createWithFormat("plane_%d.png", id)->getCString();
			pl = EnemyPlane::createWithPicture(file,configData->trajectoryID());
			if (id == 16)
			{
				EnemyPlane* enePl = (EnemyPlane*)pl;
				enePl->setFlipXonEnter();
			}
		}
		/*else 
		{
			std::string name = String::createWithFormat("plane_%d", id)->getCString();
			pl = EnemyPlane::createWithArmature(name,configData->trajectoryID());
			if(2 == id|| 8 == id|| 23 == id || 25 == id || 26 == id) {
				pl->setScale(1.1,1.1);
			}

			if(10 == id) {
				pl->updateHpBarHeight(35);
			} else if(15 == id) {
				pl->updateHpBarHeight(63);
			} else if(21 == id) {
				pl->updateHpBarHeight(70);
			}
		}*/

        EnemyPlane* plane = dynamic_cast<EnemyPlane*>(pl);
        if(plane)
            plane->startTrack();
		pl->setBltHurts(hurt);
		pl->setMaxHp(PlHp);
		pl->setUnitId(UnitId(id + (int)UnitId::ePlane_1));
		pl->setBitmask(
			ENEMY_PLANE_CATEGORYBITMASK,
			0,
			ENEMY_PLANE_CONTACTTESTBITMASK);
		pl->getPlanePro().setScore(score);
		pl->setNeedFlyToMainPl(true);
        pl->setBarrageArray(configData->barrageArr());
        pl->setPlaneId(configData->name());
	}
	else
	{
		switch (id)
		{
		case 1001:
			{
				pl = IronManBoss::create("kaijia",1);
				pl->setHpEff(HpEff(1.0f,100.0f));		//???????
//				pl->addRewards(0.0f, 10, DropGoodsType::DropGoods_Gold1);
//				pl->addRewards(0.0f, 3, DropGoodsType::DropGoods_Gold2);
//				pl->addRewards(0.0f, 1, DropGoodsType::DropGoods_Gold3);
				BaseBoss* boss = (BaseBoss*)pl;
				boss->ShowBossHpBar(2,PlHp);

				pl->getHpBar()->SetHpBarVisible(false);
			}
			break;
		case 1002:
			{
				//????BOSS
				pl = IronManBoss::create("kaijia",2);
				pl->setHpEff(HpEff(1.0f,100.0f));		//???????
//				pl->addRewards(0.0f, 13, DropGoodsType::DropGoods_Gold1);
//				pl->addRewards(0.0f, 5, DropGoodsType::DropGoods_Gold2);
//				pl->addRewards(0.0f, 2, DropGoodsType::DropGoods_Gold3);
				BaseBoss* boss = (BaseBoss*)pl;
				boss->ShowBossHpBar(2,PlHp);

				pl->getHpBar()->SetHpBarVisible(false);
			}
			break;
		case 1003:
			{
				pl = BoneWarriorBoss::create("BoneWarrior",1);
				pl->setHpEff(HpEff(1.0f,100.0f));		//???????
//				pl->addRewards(0.0f, 19, DropGoodsType::DropGoods_Gold1);
//				pl->addRewards(0.0f, 9, DropGoodsType::DropGoods_Gold2);
//				pl->addRewards(0.0f, 4, DropGoodsType::DropGoods_Gold3);
				BaseBoss* boss = (BaseBoss*)pl;
				boss->ShowBossHpBar(2,PlHp);

				pl->getHpBar()->SetHpBarVisible(false);
			}
			break;
		case 1004:
			{
				pl = BoneWarriorBoss::create("BoneWarrior",2);
				pl->setHpEff(HpEff(1.0f,100.0f));		//???????
//				pl->addRewards(0.0f, 22, DropGoodsType::DropGoods_Gold1);
//				pl->addRewards(0.0f, 11, DropGoodsType::DropGoods_Gold2);
//				pl->addRewards(0.0f, 5, DropGoodsType::DropGoods_Gold3);
				BaseBoss* boss = (BaseBoss*)pl;
				boss->ShowBossHpBar(2,PlHp);

				pl->getHpBar()->SetHpBarVisible(false);
			}
			break;
		case 1005:
			{
				pl = RenzheBoss::create("renzheboss-2",1);
				pl->setHpEff(HpEff(1.0f,100.0f));		//???????
//				pl->addRewards(0.0f, 28, DropGoodsType::DropGoods_Gold1);
//				pl->addRewards(0.0f, 15, DropGoodsType::DropGoods_Gold2);
//				pl->addRewards(0.0f, 7, DropGoodsType::DropGoods_Gold3);
				BaseBoss* boss = (BaseBoss*)pl;
				boss->ShowBossHpBar(2,PlHp);

				pl->getHpBar()->SetHpBarVisible(false);
			}
			break;
		case 1006:
			{
				pl = RenzheBoss::create("renzheboss-2",2);
				pl->setHpEff(HpEff(1.0f,100.0f));		//???????
//				pl->addRewards(0.0f, 31, DropGoodsType::DropGoods_Gold1);
//				pl->addRewards(0.0f, 17, DropGoodsType::DropGoods_Gold2);
//				pl->addRewards(0.0f, 8, DropGoodsType::DropGoods_Gold3);
				BaseBoss* boss = (BaseBoss*)pl;
				boss->ShowBossHpBar(2,PlHp);

				pl->getHpBar()->SetHpBarVisible(false);
			}
			break;
		case 1007:
			{
				pl = PoseidonBoss::create("boss-meinv",1);
				pl->setHpEff(HpEff(1.0f,100.0f));		//???????
				BaseBoss* boss = (BaseBoss*)pl;
				boss->SwitchBgWhenDead(2);
				boss->ShowBossHpBar(2,PlHp);

//				pl->addRewards(0.0f, 37, DropGoodsType::DropGoods_Gold1);
//				pl->addRewards(0.0f, 21, DropGoodsType::DropGoods_Gold2);
//				pl->addRewards(0.0f, 10, DropGoodsType::DropGoods_Gold3);
				pl->getHpBar()->SetHpBarVisible(false);
			}
			break;
		case 1008:
			{
				pl = PoseidonBoss::create("boss-meinv",2);
				pl->setHpEff(HpEff(1.0f,100.0f));		//???????
				BaseBoss* boss = (BaseBoss*)pl;
				boss->SwitchBgWhenDead(3);
				boss->ShowBossHpBar(2,PlHp);

				pl->getHpBar()->SetHpBarVisible(false);
//				pl->addRewards(0.0f, 40, DropGoodsType::DropGoods_Gold1);
//				pl->addRewards(0.0f, 23, DropGoodsType::DropGoods_Gold2);
//				pl->addRewards(0.0f, 11, DropGoodsType::DropGoods_Gold3);
			}
			break;
		case 1009:
			{
				pl = FemaleGunBoss::create("boss-nvpaoshou",1);
				pl->setHpEff(HpEff(1.0f,100.0f));		//???????
				BaseBoss* boss = (BaseBoss*)pl;
				boss->ShowBossHpBar(1,PlHp);

				pl->getHpBar()->SetHpBarVisible(false);
//				pl->addRewards(0.0f, 40, DropGoodsType::DropGoods_Gold1);
//				pl->addRewards(0.0f, 23, DropGoodsType::DropGoods_Gold2);
//				pl->addRewards(0.0f, 11, DropGoodsType::DropGoods_Gold3);
			}
			break;
		case 1010:
			{
				pl = FemaleGunBoss::create("boss-nvpaoshou",2);
				pl->setHpEff(HpEff(1.0f,100.0f));		//???????
				BaseBoss* boss = (BaseBoss*)pl;
				boss->ShowBossHpBar(1,PlHp);

				pl->getHpBar()->SetHpBarVisible(false);
//				pl->addRewards(0.0f, 40, DropGoodsType::DropGoods_Gold1);
//				pl->addRewards(0.0f, 23, DropGoodsType::DropGoods_Gold2);
//				pl->addRewards(0.0f, 11, DropGoodsType::DropGoods_Gold3);
			}
			break;
		case 1011:
			{
				pl = EyeBoss::create("boss-siyan",1);
				pl->setHpEff(HpEff(1.0f,100.0f));		//???????
				BaseBoss* boss = (BaseBoss*)pl;
				boss->ShowBossHpBar(1,PlHp);

				pl->getHpBar()->SetHpBarVisible(false);
//				pl->addRewards(0.0f, 40, DropGoodsType::DropGoods_Gold1);
//				pl->addRewards(0.0f, 23, DropGoodsType::DropGoods_Gold2);
//				pl->addRewards(0.0f, 11, DropGoodsType::DropGoods_Gold3);
			}
			break;
		case 1012:
			{
				pl = EyeBoss::create("boss-siyan",2);
				pl->setHpEff(HpEff(1.0f,100.0f));		//???????
				BaseBoss* boss = (BaseBoss*)pl;
				boss->ShowBossHpBar(1,PlHp);

				pl->getHpBar()->SetHpBarVisible(false);
//				pl->addRewards(0.0f, 40, DropGoodsType::DropGoods_Gold1);
//				pl->addRewards(0.0f, 23, DropGoodsType::DropGoods_Gold2);
//				pl->addRewards(0.0f, 11, DropGoodsType::DropGoods_Gold3);
			}
			break;
		case 1013:
			{
				pl = YizhiBoss::create("boss-yizhi",1);
				pl->setHpEff(HpEff(1.0f,100.0f));
				BaseBoss* boss = (BaseBoss*)pl;
				boss->ShowBossHpBar(1,PlHp);

				pl->getHpBar()->SetHpBarVisible(false);
//				pl->addRewards(0.0f, 40, DropGoodsType::DropGoods_Gold1);
//				pl->addRewards(0.0f, 23, DropGoodsType::DropGoods_Gold2);
//				pl->addRewards(0.0f, 11, DropGoodsType::DropGoods_Gold3);
			}
			break;
		case 1014:
			{
				pl = YizhiBoss::create("boss-yizhi",2);
				pl->setHpEff(HpEff(1.0f,100.0f));
				BaseBoss* boss = (BaseBoss*)pl;
				boss->ShowBossHpBar(1,PlHp);

				pl->getHpBar()->SetHpBarVisible(false);
//				pl->addRewards(0.0f, 40, DropGoodsType::DropGoods_Gold1);
//				pl->addRewards(0.0f, 23, DropGoodsType::DropGoods_Gold2);
//				pl->addRewards(0.0f, 11, DropGoodsType::DropGoods_Gold3);
			}
			break;
		case 1015:
			{
				pl = DoubleGunBoss::create("boss-haidao",1);
                pl->setHpEff(HpEff(1.0f,100.0f));
				BaseBoss* boss = (BaseBoss*)pl;
				boss->ShowBossHpBar(1,PlHp);

				pl->getHpBar()->SetHpBarVisible(false);
//				pl->addRewards(0.0f, 40, DropGoodsType::DropGoods_Gold1);
//				pl->addRewards(0.0f, 23, DropGoodsType::DropGoods_Gold2);
//				pl->addRewards(0.0f, 11, DropGoodsType::DropGoods_Gold3);
			}
			break;
		case 1016:
			{
				pl = DoubleGunBoss::create("boss-haidao",2);
				pl->setHpEff(HpEff(1.0f,100.0f));		//???????
				BaseBoss* boss = (BaseBoss*)pl;
				boss->ShowBossHpBar(1,PlHp);

				pl->getHpBar()->SetHpBarVisible(false);
//				pl->addRewards(0.0f, 40, DropGoodsType::DropGoods_Gold1);
//				pl->addRewards(0.0f, 23, DropGoodsType::DropGoods_Gold2);
//				pl->addRewards(0.0f, 11, DropGoodsType::DropGoods_Gold3);
			}
			break;
		default:
			break;
		}
		pl->setMaxHp(PlHp);
		pl->setUnitId(UnitId::ePlaneBoss);
		pl->setBltHurts(hurt);
		pl->getPlanePro().setScore(score);
		BaseBoss* boss = (BaseBoss*)pl;

		int gameLvl = NewDataMgr::getInstance()->getCurLevel();
		boss->addTimeOut(getBossTimeOut(gameLvl));
	}
	if (!pl)
	{
		return nullptr;
	} else {
		pl->setDropArray(configData->dropArr());
	}
	GameData::getInstance()->AddEnemyPlane(pl);
	return pl;
}

float PlaneFactory::getBossTimeOut( int gameLvl )
{
	if (gameLvl < 1)
	{
		gameLvl = 1;
	}
	float timeOut = NewDataMgr::getInstance()->getBossProConfigWithId(gameLvl)->timeOut;
	return timeOut;
}
