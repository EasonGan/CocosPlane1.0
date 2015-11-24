#include "BulletFactory.h"
#include "Bullet.h"
#include "../Layer/UIController.h"
#include "../../GameCommon/GameData.h"
#include "../../GameCommon/EffectsManager.h"

//----------------------------------
BulletFactory* BulletFactory::create()
{
	BulletFactory* fact = new (std::nothrow) BulletFactory;
	if (fact && fact->init())
	{
		fact->retain();
		fact->autorelease();
		return fact;
	}
	else
	{
		CC_SAFE_DELETE(fact);
		return nullptr;
	}
}

bool BulletFactory::init()
{
	do 
	{
	} while (0);

	return true;
}

Bullet* BulletFactory::SelfBullet_Default(Weapon* weapon, ResTag res)
{
	Bullet* bullet = Bullet::create(getResByTag(res));
	bullet->setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK,
		SELF_AMMO_CONTACTTESTBITMASK);
	bullet->setUnitId(UnitId::eBullet_1);
	bullet->setZOrder(-100);
	return bullet;
}

Bullet* BulletFactory::EnemyBullet_Default( Weapon* weapon, ResTag res )
{
	Bullet* bullet = Bullet::create(getResByTag(res));
	bullet->setBitmask(
		ENEMY_AMMO_CATEGORYBITMASK,
		ENEMY_AMMO_COLLISIONBITMASK,
		ENEMY_AMMO_CONTACTTESTBITMASK);
	bullet->setUnitId(UnitId::eBullet_37);
	return bullet;
}

//-----------------
Bullet* BulletFactory::EnemyBullet_1( Weapon* weapon, ResTag res )
{
	Bullet* bullet = Bullet::create(getResByTag(res));
	bullet->setBitmask(
		ENEMY_AMMO_CATEGORYBITMASK,
		ENEMY_AMMO_COLLISIONBITMASK,
		ENEMY_AMMO_CONTACTTESTBITMASK);
	bullet->setUnitId(UnitId::eBullet_2);

	return bullet;
}

Bullet* BulletFactory::EnemyBullet_2( Weapon* weapon, ResTag res )
{
	Bullet* bullet = Bullet::create(getResByTag(res));
	bullet->setBitmask(
		ENEMY_AMMO_CATEGORYBITMASK,
		ENEMY_AMMO_COLLISIONBITMASK,
		ENEMY_AMMO_CONTACTTESTBITMASK);
	bullet->setUnitId(UnitId::eBullet_3);

	return bullet;
}


//--------------------------------------------------
//导弹
Bullet* BulletFactory::SelfMissileForDis( Weapon* weapon )
{
	//Bullet* bullet = MissileForDis::create(getBatchNode(ResTag::defaultType)->getTexture());
	Bullet* bullet = MissileForDis::create();
	bullet->setBelongTo(UnitId::eBlueCard);	//设置为属于篮球

	return bullet;
}

Bullet* BulletFactory::SelfCrossBullet( Weapon* weapon, ResTag res )
{
	Bullet* bullet = CrossBullet::create(getResByTag(res), weapon);
	bullet->setBelongTo(UnitId::eBlueCard);	//设置为属于篮球

	return bullet;
}

void BulletFactory::purge()
{
	this->release();
}

Bullet* BulletFactory::createBullet(Weapon* weapon,const BulletType& tp,const ResTag& res)
{
	Bullet* bullet = nullptr;
	switch (tp)
	{
	case BulletType::Self_Default:
		bullet = SelfBullet_Default(weapon,res);	//通用
		break;
	case BulletType::Self_DartBullet:
		bullet = CreateDartBullet();
		break;
	case BulletType::Self_ToTarget:
		bullet = Bullet2Target(weapon,res);
		break;
	case BulletType::Self_SmallDartBullet:
		bullet = CreateSmallDartBullet();
		break;
	case BulletType::Self_LaserBullet:
		bullet = CreateLaserBullet();
		break;
	case BulletType::Self_MissleForDis:
		bullet = SelfMissileForDis(weapon);
		break;
	case BulletType::Self_Cross:
		bullet = SelfCrossBullet(weapon, res);
		break;
	case BulletType::Convolution:
		bullet = BulletConvolution::create(weapon);
		break;
	case BulletType::Enemy_BossDart:
		bullet = BossBulletDart(weapon);
		break;
	case BulletType::Enemy_Fireworks:
		bullet = EnemyFireworks(weapon);
		break;
	case BulletType::Enemy_Boss4Bezier:
		bullet = BezierBulletOfBoss4(weapon);
		break;
	case BulletType::Boss1_GravityBullet:
		bullet = BossBulletGravity(weapon);
		break;
	case BulletType::Enemy_BossRebound:
		bullet = BossBulletRebound(weapon);
		break;

	case BulletType::Enemy_default:
		bullet = EnemyBullet_Default(weapon,res);//通用
		break;
	case BulletType::Enemy_Bullet1:
		bullet = EnemyBullet_1(weapon,res);
		break;
	case BulletType::Enemy_Gravity:
		bullet = EnemyBulletGravity(weapon,res);
		break;
	case BulletType::Enemy_FallIng:
		bullet = EnemyBulletFalling(weapon, res);
		break;
	case BulletType::Enemy_Bezier:
		bullet = EnemyBulletBezier(weapon, res);
		break;
	case BulletType::Enemy_BezierLeft:
		bullet = EnemyBulletBezierLeft(weapon, res);
		break;
	case BulletType::Enemy_BezierRight:
		bullet = EnemyBulletBezierRight(weapon, res);
		break;
	case BulletType::Enemy_Grenade:
		bullet = EnemyGrenade( weapon, res );
		break;
	case BulletType::Enemy_HelixBullet:
		bullet = createHelixBullet(weapon, res);
		break;
	case BulletType::Enemy_HelixExBullet:
		bullet = createHelixExBullet(weapon, res);
		break;
	case BulletType::Enemy_FallingExBullet:
		bullet = EnemyBulletFallingEx(weapon, res);
		break;
	case BulletType::Enemy_Rebounce:
		bullet = EnemyRebounce(weapon, res);
		break;
	case BulletType::BezierBullet_EyeBoss:
		{
			bullet = BezierBulletOfEyeBoss::create(BulletFactory::getResByTag(res));
			break;
		}
	case BulletType::FallBullet_EyeBoss:
		{
			bullet = FallBulletOfEyeBoss::create(BulletFactory::getResByTag(res));
			break;
		}
	case BulletType::TelescopicLaser_EyeBoss:
		{
			bullet = TelescopicLaserOfEnyBoss::create(BulletFactory::getResByTag(res));
			break;
		}
	case BulletType::YizhiBoss_AutoExpl:
		{
			bullet = AutoExplosionOfYizhiBoss::create(BulletFactory::getResByTag(res));
			break;
		}
	case BulletType::YizhiBoss_SpiralMissile:
		{
			bullet = SpiralMissile::create(BulletFactory::getResByTag(res));
			break;
		}
	case BulletType::YizhiBoss_SpeedUpBullet:
		{
			bullet = SpeedUpBulletOfYizhiBoss::create(BulletFactory::getResByTag(res));
			break;
		}
	case BulletType::YizhiBoss_ExplosionCyclotron:
		{
			bullet = ExplosionCyclotronOfYizhiBoss::create(BulletFactory::getResByTag(res));
			break;
		}
	case BulletType::YizhiBoss_SpeedDownAndUniform:
		{
			bullet = SpeedDownAndUniform::create(BulletFactory::getResByTag(res));
			break;
		}
	}
	return bullet;
}

Bullet* BulletFactory::EnemyBulletGravity( Weapon* weapon, ResTag res )
{
	Bullet* bullet = Bullet::create(getResByTag(res));
	bullet->getPhysicsBody()->setGravityEnable(true);
	bullet->getPhysicsBody()->setMass(0.001);
	bullet->setBitmask(
		ENEMY_AMMO_CATEGORYBITMASK,
		ENEMY_AMMO_COLLISIONBITMASK,
		ENEMY_AMMO_CONTACTTESTBITMASK);

	bullet->setBltFlySpeed(250);
	bullet->setUnitId(UnitId::eBullet_4);

	return bullet;
}

Bullet* BulletFactory::createSelfMissileForMaxHpTarget(const float& AngleOffet )
{
	Bullet* bullet = MissileForBloodlimit::create(getResByTag(ResTag::defaultType),AngleOffet);
	bullet->setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
	bullet->setUnitId(UnitId::eBullet_13);

	return bullet;
}

Bullet* BulletFactory::createStrayBullet()
{
	Bullet* bullet = Bullet::create(getResByTag(ResTag::defaultType));
	bullet->setBitmask(
		SELF_AMMO_CATEGORYBITMASK,
		SELF_AMMO_COLLISIONBITMASK_P,
		SELF_AMMO_CONTACTTESTBITMASK);
	bullet->setUnitId(UnitId::eBullet_15);

	bullet->setBltFlySpeed(1000 + rand_minus1_1() * 100);
	bullet->resetHurt(-150000);
	bullet->setScale(1 + rand_minus1_1() * 0.2);

	//播放火鸟动画
	UIController::getInstance()->getEffectsLayer()->playFireBirdAnimation(bullet);
	return bullet;
}

Bullet* BulletFactory::EnemyBulletFalling(Weapon* node, ResTag res)
{
	Bullet* bullet = FallingBullet::create(getResByTag(res));
	bullet->setScale(0.7);
	bullet->setUnitId(UnitId::eBullet_8);
	return bullet;
}

Bullet* BulletFactory::EnemyBulletBezier( Weapon* weapon, ResTag res )
{
	Bullet* bullet = CurveBullet::create(getResByTag(res));
	bullet->setUnitId(UnitId::eBullet_9);
	return bullet;
}

Bullet* BulletFactory::EnemyBulletBezierLeft( Weapon* weapon, ResTag res )
{
	Bullet* bullet = CurveBullet2::create(getResByTag(res), CurveBullet2::Dirct_Left);
	bullet->setUnitId(UnitId::eBullet_10);
	return bullet;
}

Bullet* BulletFactory::EnemyBulletBezierRight( Weapon* weapon, ResTag res )
{
	Bullet* bullet = CurveBullet2::create(getResByTag(res), CurveBullet2::Dirct_Right);
	bullet->setUnitId(UnitId::eBullet_11);
	return bullet;
}

Bullet* BulletFactory::BossBulletGravity( Weapon* weapon )
{
	Bullet* bullet = Bullet::create(getResByTag(ResTag::defaultType));
	bullet->getPhysicsBody()->setGravityEnable(true);
	bullet->getPhysicsBody()->setMass(0.001);
	bullet->setBitmask(
		ENEMY_AMMO_CATEGORYBITMASK,
		ENEMY_AMMO_COLLISIONBITMASK,
		ENEMY_AMMO_CONTACTTESTBITMASK);

	bullet->setBltFlySpeed(300);
	bullet->setUnitId(UnitId::eBullet_5);

	return bullet;
}

Bullet* BulletFactory::BossBulletRebound( Weapon* weapon )
{
	Bullet* bullet = ReboundBullect::create( getResByTag(ResTag::Bullet_BossKulou) );
	bullet->setUnitId(UnitId::eBullet_6);
	bullet->setBelongTo(UnitId::ePlaneBoss);

	return bullet;

}

Bullet* BulletFactory::EnemyGrenade( Weapon* weapon, ResTag res )
{
	Bullet* bullet = GrenadeBullect::create(getResByTag(res));
	bullet->getPhysicsBody()->setMass(0.1);
	bullet->setBitmask(
		ENEMY_AMMO_CATEGORYBITMASK,
		ENEMY_AMMO_COLLISIONBITMASK,
		ENEMY_AMMO_CONTACTTESTBITMASK);
	bullet->setUnitId(UnitId::eBullet_7);

	return bullet;
}

Bullet* BulletFactory::createHelixBullet(Weapon* weapon, ResTag res)
{
	HelixBullet* bullet = HelixBullet::create(getResByTag(res));
	bullet->setUnitId(UnitId::eBullet_16);
	return bullet;
}

Bullet* BulletFactory::createHelixExBullet(Weapon* weapon, ResTag res)
{
	HelixBulletEx* bullet = HelixBulletEx::create(getResByTag(res));
	bullet->setUnitId(UnitId::eBullet_17);
	return bullet;
}

Bullet* BulletFactory::EnemyBulletFallingEx(Weapon* weapon, ResTag res)
{
	Bullet* bullet = FallingBulletEX::create(getResByTag(res));
	bullet->setUnitId(UnitId::eBullet_18);
	return bullet;
}

Bullet* BulletFactory::CreateDartBullet()
{
	DartBullet* bullet = DartBullet::create(getResByTag(ResTag::Bullet_Card_BigDart));
	bullet->setScale(2.0f);
	bullet->setUnitId(UnitId::eBullet_21);
	return bullet;
}

Bullet* BulletFactory::CreateSmallDartBullet()
{
	DartBulletSmall* bullet = DartBulletSmall::create(getResByTag(ResTag::Bullet_Card_SmallDart));
	//bullet->setScale(0.5f);
	bullet->setBelongTo(UnitId::eRedCard);	//设置为属于红球
	return bullet;
}

Bullet* BulletFactory::CreateLaserBullet()
{
	Bullet* bullet = LaserBullet::create();
	bullet->setBelongTo(UnitId::eYellowCard);	//设置为属于黄球

	return bullet;
}

Bullet* BulletFactory::BossBulletDart( Weapon* weapon )
{
	Bullet* bullet = BossDart::create(getResByTag(ResTag::Bullet_BossFireAmmo));
	bullet->setBelongTo(UnitId::ePlaneBoss);

	return bullet;
}

Bullet* BulletFactory::EnemyFireworks( Weapon* weapon )
{
	Bullet* bullet = FireWorks::create(getResByTag(ResTag::Bullet_EnemyNor1));
	bullet->setUnitId(UnitId::eBullet_19);

	return bullet;
}

Bullet* BulletFactory::EnemyRebounce( Weapon* weapon, ResTag res )
{
	Bullet* bullet = EneRebounceBullet::create(getResByTag(res));
	bullet->setUnitId(UnitId::eBullet_20);

	return bullet;
}

Bullet* BulletFactory::Bullet2Target( Weapon* weapon, ResTag res )
{
	Bullet* bullet = BulletToTarget::create(getResByTag(res));
	bullet->setUnitId(UnitId::eBullet_25);
	return bullet;
}

Bullet* BulletFactory::BezierBulletOfBoss4( Weapon* weapon )
{
	Bullet2OfBoss4* bullet = Bullet2OfBoss4::create();

	return bullet;
}

std::string BulletFactory::getResByTag( ResTag tag )
{
	std::string res = "";
	switch (tag)
	{
	case ResTag::defaultType:
		res = "bullet_31.png";
		break;

	//敌方普通敌机子弹
	case ResTag::Bullet_EnemyNor1:
		res = "bullet_31.png";
		break;
	case ResTag::Bullet_EnemyNor2:
		res = "bullet_32.png";
		break;
	case ResTag::Bullet_EnemyNor3:
		res = "bullet_33.png";
		break;
	case ResTag::Bullet_EnemyNor4:
		res = "bullet_34.png";
		break;

	//敌方BOSS子弹
	case ResTag::Bullet_BossFireAmmo:
		res = "boss1_dart.png";
		break;
	case ResTag::Bullet_BossShuiPaoBig:
		res = "boss4_shuipaoBig.png";
		break;
	case ResTag::Bullet_BossShuiPaoSmall:
		res = "boss4_shuipaoSmall.png";
		break;
	case ResTag::Bullet_BossKulou:
		res = "boss2_kulou.png";
		break;
	case ResTag::Bullet_BossBigDart:
		res = "boss3_bigDart.png";
		break;
	case ResTag::Bullet_BossSmallDart:
		res = "boss3_smallDart.png";
		break;
	case ResTag::Bullet_BossEye1:
		res = "new_bullet_eye.png";
		break;
	case ResTag::Bullet_BossEye2:
		res = "new_bullet_eye.png";
		break;
	case ResTag::Bullet_BossEye3:
		res = "new_bullet_eye.png";
		break;
	case ResTag::Bullet_BossEye4:
		res = "new_bullet_eye.png";
		break;
	case ResTag::Bullet_BossEye5:
		res = "new_bullet_eye.png";
		break;
		
	//卡牌子弹
	case ResTag::Bullet_Card_BigDart:
		res = "card_smallDart.png";
		break;
	case ResTag::Bullet_Card_SmallDart:
		res = "carr_bigDart.png";
		break;

	default:
		break;
	}

	return res;
}

