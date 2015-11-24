/********************************************************************
创建于:		2015/03/11  14:33	
文件名: 	E:\Cocos3.3_Project\PlaneGame2\Classes\GamePlaying\Bullet\BulletFactory.h 
作  者:		alex()
修改于：	2015/03/11  14:33	

目  的:		
*********************************************************************/
#ifndef __BulletFactory_H__
#define __BulletFactory_H__


#include "cocos2d.h"
#include "cocos-ext.h"
#include "../../GameCommon/GameDefine.h"

USING_NS_CC;
USING_NS_CC_EXT;


//子弹创建工厂
class Bullet;
class Weapon;
class BulletFactory : public Ref
{
public:
	static BulletFactory* create();
	void purge();

	//根据ID取得子弹
	Bullet* createBullet(Weapon* weapon,const BulletType& tp, const ResTag& res = ResTag::defaultType);

	//普通敌机的子弹
	Bullet* CreateBulletOfNorPlane();

	//己方通用子弹
	Bullet* SelfBullet_Default(Weapon* weapon, ResTag res);

	//敌方通用子弹
	Bullet* EnemyBullet_Default(Weapon* weapon, ResTag res);

	//敌方子弹
	Bullet* EnemyBullet_1(Weapon* weapon, ResTag res);
	Bullet* EnemyBullet_2(Weapon* weapon, ResTag res);

	//自由落体子弹
	Bullet* EnemyBulletGravity(Weapon* weapon, ResTag res);

	//Boss自由落体子弹
	Bullet* BossBulletGravity(Weapon* weapon);
	// Bossr反弹子弹
	Bullet* BossBulletRebound(Weapon* weapon);
	// 榴弹
	Bullet* EnemyGrenade( Weapon* weapon, ResTag res );

	//飘落子弹
	Bullet* EnemyBulletFalling(Weapon* weapon, ResTag res);

	//贝塞尔曲线子弹
	Bullet* EnemyBulletBezier(Weapon* weapon, ResTag res);
	//贝塞尔曲线子弹可设置方向Left
	Bullet* EnemyBulletBezierLeft(Weapon* weapon, ResTag res);
	//贝塞尔曲线子弹可设置方向Right
	Bullet* EnemyBulletBezierRight(Weapon* weapon, ResTag res);
	//己方导弹(近距离导弹)
	Bullet* SelfMissileForDis(Weapon* weapon);

	Bullet* SelfCrossBullet(Weapon* weapon, ResTag res);

	//己方导弹，目标为血上限最大
	Bullet* createSelfMissileForMaxHpTarget(const float& AngleOffet );

	//己方流弹
	Bullet* createStrayBullet();

	//螺旋子弹
	Bullet* createHelixBullet(Weapon* weapon, ResTag res);

	//螺旋子弹EX
	Bullet* createHelixExBullet(Weapon* weapon, ResTag res);

	//飘落型子弹加强版
	Bullet* EnemyBulletFallingEx(Weapon* weapon, ResTag res);

	//烟花
	Bullet* EnemyFireworks( Weapon* weapon );

	//反弹
	Bullet* EnemyRebounce(Weapon* weapon, ResTag res);

	//飞镖
	Bullet* CreateDartBullet();		//(大)
	Bullet* CreateSmallDartBullet();//(小)

	//激光子弹
	Bullet* CreateLaserBullet();

	//忍者BOSS的飞镖
	Bullet* BossBulletDart(Weapon* weapon);

	//射出去瞬间，朝向距离最近目标
	Bullet* Bullet2Target(Weapon* weapon, ResTag res);

	//Boss4的贝赛尔曲线
	Bullet* BezierBulletOfBoss4(Weapon* weapon);

	static std::string getResByTag(ResTag tag);

private:
	bool init();
};



#endif // __BulletFactory_H__