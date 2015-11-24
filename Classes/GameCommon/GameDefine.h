/********************************************************************
创建于:		2014/11/17  16:08	
文件名: 	E:\Cocos3.3_Project\MyGame\Classes\BaseClass.h 
作  者:		alex(alex)
修改于：	2014/11/17  16:08	

目  的:		
*********************************************************************/
#ifndef _GAME_DEFINE_H_
#define _GAME_DEFINE_H_

#include "cocos2d.h"

USING_NS_CC;


/************************************************************************/
/* 
//碰撞筛选
*/
/************************************************************************/

#define SELF_AMMO_CATEGORYBITMASK		0x00000010		//己方弹药的分类掩码CategoryBitmask
//#define SELF_AMMO_COLLISIONBITMASK		0x00010001		//己方弹药的碰撞掩码collisionBitmask
#define SELF_AMMO_COLLISIONBITMASK		0x00000000		//己方弹药的碰撞掩码collisionBitmask
#define SELF_AMMO_COLLISIONBITMASK_P	0x00000000		//穿透
#define SELF_AMMO_CONTACTTESTBITMASK	0x00010001		//己方弹药的碰撞测试掩码contacttestbitmask

#define SELF_PLANE_CATEGORYBITMASK		0x00000100		//己方飞机的分类掩码CategoryBitmask
#define SELF_PLANE_COLLISIONBITMASK		0x00101021		//己方飞机的碰撞掩码collisionBitmask
#define SELF_PLANE_CONTACTTESTBITMASK	0x00111021		//己方飞机的碰撞测试掩码contacttestbitmask

#define ENEMY_AMMO_CATEGORYBITMASK		0x00001000		//敌方弹药的分类掩码CategoryBitmask
#define ENEMY_AMMO_COLLISIONBITMASK		0x10000101		//敌方弹药的碰撞掩码collisionBitmask
#define ENEMY_AMMO_CONTACTTESTBITMASK	0x10000101		//敌方弹药的碰撞测试掩码contacttestbitmask

#define ENEMY_PLANE_CATEGORYBITMASK		0x00010000		//敌方飞机的分类掩码CategoryBitmask
#define ENEMY_PLANE_COLLISIONBITMASK	0x10000010		//敌方飞机的碰撞掩码collisionBitmask
#define ENEMY_PLANE_COLLISIONBITMASK_P	0x00000000		//敌方飞机的碰撞掩码collisionBitmask
#define ENEMY_PLANE_CONTACTTESTBITMASK	0x10000110		//敌方飞机的碰撞测试掩码contacttestbitmask

#define REWARD_CATEGORYBITMASK			0x00100000		//奖励的分类掩码CategoryBitmask
#define REWARD_COLLISIONBITMASK			0x00000100		//奖励的碰撞掩码collisionBitmask(只与己方飞机碰撞)
#define REWARD_COLLISIONBITMASK_WALL	0x01000000		//奖励的碰撞掩码collisionBitmask（与己方飞机和围墙碰撞）
#define REWARD_CONTACTTESTBITMASK		0x00000100		//奖励的碰撞测试掩码contacttestbitmask
#define REWARD_CONTACTTESTBITMASK_WALL	0x01000000		//奖励的碰撞测试掩码contacttestbitmask

//己方大招刚体筛选（既可与敌方飞机碰撞，也能与敌方子弹碰撞）
#define SELF_SCOPE_CATEGORYBITMASK		0x00000110		//
#define SELF_SCOPE_COLLISIONBITMASK		0x00011021		//
#define SELF_SCOPE_CONTACTTESTBITMASK	0x00011021		//

//己方爆炸刚体筛选，只与敌机发生碰撞，且穿越
#define SELF_EXPLOSION_CATEGORYBITMASK		0x00000010		//
#define SELF_EXPLOSION_COLLISIONBITMASK		0x00000000		//
#define SELF_EXPLOSION_CONTACTTESTBITMASK	0x00010001		//

//刚体围墙，与弹珠\掉落物发生实质碰撞
#define WALL_CATEGORYBITMASK			0x01000000		
#define WALL_COLLISIONBITMASK			0x10100020		
#define WALL_CONTACTTESTBITMASK			0x10100020	

//弹珠刚体，与敌机\敌方子弹和围墙发生实质碰撞
#define HOODLE_CATEGORYBITMASK			0x10000000		
#define HOODLE_COLLISIONBITMASK			0x11011002		
#define HOODLE_COLLISIONBITMASK_P		0x11000002		//与敌机发生穿透碰撞
#define HOODLE_CONTACTTESTBITMASK		0x11011002		

//挡板，只与弹珠发生碰撞
#define PADDLE_CATEGORYBITMASK			0x00000002		
#define PADDLE_COLLISIONBITMASK			0x10000000		
#define PADDLE_CONTACTTESTBITMASK		0x10000000		

// boss反弹弹 ：与围墙碰0x00000100  与敌机碰0x00000002
#define ENEMY_AMMO2_CATEGORYBITMASK		0x00000020		//敌方反弹弹药的分类掩码CategoryBitmask
#define ENEMY_AMMO2_COLLISIONBITMASK	0x01000100		//敌方反弹弹药的碰撞掩码collisionBitmask
#define ENEMY_AMMO2_COLLISIONBITMASK_P	0x01000000		//敌方反弹弹药的碰撞掩码collisionBitmask
#define ENEMY_AMMO2_CONTACTTESTBITMASK	0x01000100		//敌方反弹弹药的碰撞测试掩码contacttestbitmask

#define ScreenEdge_CATEGORYBITMASK		0x11111111
#define ScreenEdge_COLLISIONBITMASK_P	0x00000000
#define ScreenEdge_CONTACTTESTBITMASK	0x11111111

#define PixelToMeter	5			//像素与米的转换率（如：10px = 1米）

#define Pi2Angle 180/M_PI
#define Angle2pi M_PI/180


//各类带刚体精灵的类型ID
enum class UnitId
{
	DefaultIdx = -1,

	eBullet_1,
	eBullet_2,
	eBullet_3,
	eBullet_4,
	eBullet_5,
	eBullet_6,
	eBullet_7,
	eBullet_8,
	eBullet_9,
	eBullet_10,
	eBullet_11,
	eBullet_12,
	eBullet_13,
	eBullet_14,
	eBullet_15,
	eBullet_16,
	eBullet_17,
	eBullet_18,
	eBullet_19,
	eBullet_20,
	eBullet_21,
	eBullet_22,
	eBullet_23,
	eBullet_24,
	eBullet_25,
	eBullet_26,
	eBullet_27,
	eBullet_28,
	eBullet_29,
	eBullet_30,
	eBullet_31,
	eBullet_32,
	eBullet_33,
	eBullet_34,
	eBullet_35,
	eBullet_36,
	eBullet_37,
	eBullet_38,
	eBullet_max,

	ePlaneBoss,

	eBottomWall,		//4面墙
	eLeftWall,			
	eRightWall,			
	eUpWall,			
	eScreenExplosion,	//全屏爆炸
	eFanghudun,			//防护盾
	eScopeExplosion,	//范围爆炸

	eRedCard,			//3种卡牌
	eBlueCard,
	eYellowCard,
    eSuperCard,

	eRewardGold,
	eRewardRedDiamond,
	eRewardGreenDiamond,

	eLightAttackPoint,

	eScreenEdgeSmall,
	eScreenEdgeBig,

	ePlane_1,
	ePlane_2,
	ePlane_3,
	ePlane_4,
	ePlane_5,
	ePlane_6,
	ePlane_7,
	ePlane_8,
	ePlane_9,
	ePlane_10,
	ePlane_11,
	ePlane_12,
	ePlane_13,
	ePlane_14,
	ePlane_15,
	ePlane_16,
	ePlane_17,
	ePlane_18,
	ePlane_19,
	ePlane_20,
	ePlane_21,
	ePlane_22,
	ePlane_max,

	eMainPlane,

};

//BUFF的ID
enum class BuffId
{
	Buff_default = 0,		//默认无效
	Buff_Sandan,			//散弹BUFF
	Buff_Burning,			//燃烧BUFF
	Buff_TempNorWp1,		//主角普通子弹BUFF1
	Buff_TempNorWp2,		//主角普通子弹BUFF2
	Buff_TempNorWp3,		//主角普通子弹BUFF3
	Buff_LightingLvl1,		//闪电buff一级
	Buff_LightingLvl2,		//闪电buff二级
	Buff_LightingLvl3,		//闪电buff三级
	Buff_Magnet,			//磁铁
};


//子弹资源类型
enum class ResTag
{
	defaultType,
	
	//小怪子弹
	Bullet_EnemyNor1,
	Bullet_EnemyNor2,
	Bullet_EnemyNor3,
	Bullet_EnemyNor4,

	//BOSS子弹
	Bullet_BossFireAmmo,
	Bullet_BossShuiPaoBig,
	Bullet_BossShuiPaoSmall,
	Bullet_BossKulou,
	Bullet_BossBigDart,
	Bullet_BossSmallDart,
	Bullet_BossEye1,
	Bullet_BossEye2,
	Bullet_BossEye3,
	Bullet_BossEye4,
	Bullet_BossEye5,

	//卡牌子弹
	Bullet_Card_BigDart,
	Bullet_Card_SmallDart,

	//掉落物
	Drops_Golds1,
	Drops_Golds2,
	Drops_Golds3,
	Drops_WpBuff1,

	Drops_RedWp,
	Drops_YellowWp,
	Drops_BlueWp,
    Drops_SuperWp,

	Drops_Magnet,
	Drops_AddHp100,
	Drops_AddHp500,
    Drops_Shield,
    Drops_Skill,

};


//子弹类型
enum class BulletType
{
	Self_Default,

	Self_NorLinear,			//普通线性子弹
	Self_ToTarget,			//普通朝向目标的子弹

	Self_DartBullet,		//大飞镖
	Self_SmallDartBullet,	//小飞镖
	Self_LaserBullet,
	Self_MissleForDis,		//导弹
	Self_Cross,
	
	Enemy_default,
	Enemy_Bullet1,
	Enemy_Gravity,
	Enemy_FallIng,
	Enemy_Bezier,
	Enemy_BezierLeft,		// 可设置方向的曲线
	Enemy_BezierRight,		// 可设置方向的曲线
	Enemy_BossRebound,			// 反弹子弹
	Enemy_Grenade,			// 榴弹
	Enemy_HelixBullet,		// 旋转子子弹
	Enemy_HelixExBullet,	// 旋转子子弹WX
	Enemy_FallingExBullet,	// 飘落子弹
	Enemy_Fireworks,		//烟花
	Enemy_Rebounce,			//反弹
	Enemy_BossDart,			//BOSS的忍者镖
	Enemy_Boss4Bezier,		//BOSS4的贝塞尔子弹

	Boss1_GravityBullet,

	Convolution,			//回旋
	BezierBullet_EyeBoss,
	FallBullet_EyeBoss,
	TelescopicLaser_EyeBoss,

	YizhiBoss_AutoExpl,
	YizhiBoss_SpiralMissile,
	YizhiBoss_SpeedUpBullet,
	YizhiBoss_ExplosionCyclotron,
	YizhiBoss_SpeedDownAndUniform,
};

//掉落物-类型
enum class DropGoodsType
{
	DropGoods_Default = 0,
	DropGoods_RedWp,
	DropGoods_BlueWp,
	DropGoods_YellowWp,

	DropGoods_Gold1,
	DropGoods_Gold2,
	DropGoods_Gold3,
    DropGoods_Shield,
    DropGoods_SKill

};

enum class PlaneType
{
	PlaneType_Wind = 1,
	PlaneType_Fire,
	PlaneType_Electric,
};


//死亡统计结构体
typedef struct EnemyStatistics
{
	UnitId		EnePlaneId;		//死亡敌机类型
	UnitId		KillerId;		//击杀该敌机的源头类型
	int			Score;			//击杀得分

	EnemyStatistics(const UnitId& pId, const UnitId& kId, int score)
	{
		EnePlaneId = pId;
		KillerId = kId;
		Score = score;
	}
}ES;

//敌机死亡增加主飞机血量
typedef struct HpEff
{
	float		rate;		//概率
	float		hp;			//增加HP的值
	HpEff()
	{
		rate = -1.0f;		//默认小于0，表示无效
		hp = 0.0f;
	}

	HpEff(float r, float h)
	{
		rate = r;		
		hp = h;
	}

}HpEff;

//BOSS评级等级
enum class BossRating
{
	e_SS,
	e_S,
	e_A,
	e_B,
};

//BOSS类型及评级等级
typedef struct BossGrade
{
	int			bossType;
	float		SurvivalTime;
	BossRating	rateing;
}BossGrade;


//监听消息
#define Msg_Shake				"screen shake"				//屏幕抖动
#define Msg_BounceWallTimeout	"bounceWall time out"		//挡板时间到了
#define Msg_RecyclePadTimeout	"recyclePad time out"		//回收挡板时间到了
#define Msg_BossDead			"current boss is dead"		//BOSS死亡
#define Msg_StopMainPlBullet	"stop all bullet"			//停止所有子弹
#define Msg_OutScreen			"physivsunit is out screen"	//出了屏幕
#define Msg_ProtectRemove		"Protect Remove"			//护盾破裂
#define Msg_MainPlFuhuo			"MainPl	Fuhuo"				//主飞机复活
#define Msg_BossAngry			"Boss Angry"				//BOSS暴怒
#define Msg_BossAngryEnd		"Boss Angry End"			//BOSS暴怒结束





#endif