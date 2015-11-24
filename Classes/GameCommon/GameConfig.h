/********************************************************************
创建于:		2015/02/15  10:11	
文件名: 	E:\Cocos3.3_Project\PlaneGame\Classes\Common\GameConfig.h 
作  者:		alex(alex)
修改于：	2015/02/15  10:11	

目  的:		游戏数据配置
*********************************************************************/
#ifndef __GameConfig_H__
#define __GameConfig_H__


#include "cocos2d.h"
#include "cocos-ext.h"
#include "NewDataMgr.h"

USING_NS_CC;
USING_NS_CC_EXT;

//血量
#define Ene_Plane_Hp_A		120	
#define Ene_Plane_Hp_B		230	
#define Ene_Plane_Hp_C		500	
#define Ene_Plane_Hp_D		1200	
#define Ene_Plane_Hp_E		800	
#define Ene_Plane_Hp_6		250	
#define Ene_Plane_Hp_7		1300	
#define Ene_Plane_Hp_8		1000	
#define Ene_Plane_Hp_9		2000	
#define Ene_Plane_Hp_10		1500	
#define Ene_Plane_Hp_max	2000

#define Boss1_MaxBlood		25000
#define Boss2_MaxBlood		28000
#define Boss3_MaxBlood		30000
#define Boss4_MaxBlood		35000

#define Protect_Hp			10000

//伤害
#define Ene_Blt_hurt		-30
#define Ene_Pl_hurt			-30
#define FallSton_hurt		-1000
#define Bow_hurt			-10000


//主角1基础攻击
#define SelfMainPl_1_NorBlt_hurt	-120							//普通基准值
#define SelfMainPl_1_AddBlt_hurt	SelfMainPl_1_NorBlt_hurt * 1.8	//加强版值

//主角2基础攻击
#define SelfMainPl_2_NorBlt_hurt	SelfMainPl_1_NorBlt_hurt * 1.6				
#define SelfMainPl_2_AddBlt_hurt	SelfMainPl_1_AddBlt_hurt * 1.6

//主角3基础攻击
#define SelfMainPl_3_NorBlt_hurt	SelfMainPl_2_NorBlt_hurt * 1.8				
#define SelfMainPl_3_AddBlt_hurt	SelfMainPl_2_AddBlt_hurt * 1.8

//击杀敌机得分
#define Score_Pl1			100
#define Score_Pl2			300
#define Score_Pl3			500
#define Score_Pl4			700
#define Score_Pl5			1200

//主角血量
#define MainPl_1_Hp			100
#define MainPl_2_Hp			100 * 1.38
#define MainPl_3_Hp			100 * 1.38 * 1.38

#define MainPlBltSpeed		1500

#define	EnemyBulletZorder	-100
#define	SelfBulletZorder	-200

#define MainPl_1_BaseHurt	-500
#define MainPl_2_BaseHurt	-1000
#define MainPl_3_BaseHurt	-2000

class DataConfig
{
public:
	//当前主角的普通伤害
	static int getCurMainPlHurt()
	{
		int curPlIdx = NewDataMgr::getInstance()->mRoleConfig.roleIndex;
		int lvl = 1;// NewDataMgr::getInstance()->getRoleGradeWithId(curPlIdx);
		int hurt = 0;// NewDataMgr::getInstance()->getRoleAtkWithId(curPlIdx, lvl);

		return hurt;
	}

	static int getCardHurts(DropGoodsType CardColor, int cardTp, int lvl)
	{
		int baseHurt = getMainPlBaseHurt();

		//卡牌基础加成
		float baseAddition = 0;
		switch (CardColor)
		{
		case DropGoodsType::DropGoods_RedWp:
			if (cardTp == 0)
			{
				//飞镖
				baseAddition = 3.09;
			}
			else if(cardTp == 1)
			{
				//火圈
				baseAddition = 0.32;
			}
			else if(cardTp == 2)
			{
				//花
				baseAddition = 4.15;
			}
			break;
		case DropGoodsType::DropGoods_BlueWp:
			if (cardTp == 0)
			{
				//猫
				baseAddition = 3.06;
			}
			else if(cardTp == 1)
			{
				//导弹
				baseAddition = 8.88;
			}
			else if(cardTp == 2)
			{
				//冰剑
				baseAddition = 6.09;
			}
			break;
		case DropGoodsType::DropGoods_YellowWp:
			if (cardTp == 0)
			{
				//龙
				baseAddition = 5.06;
			}
			else if(cardTp == 1)
			{
				//闪电
				baseAddition = 2.16;
			}
			else if(cardTp == 2)
			{
				//激光
				baseAddition = 3.8f;
			}
			break;
	
		default:
			break;
		}

		//技能加成
		float skillAddition = NewDataMgr::getInstance()->mGameGradeMsg->getObjectAtkWithCurLv(GameGradeMsg::ObjectType::ObjectType_BlueBall);
		
		//最终公式
		int hurt = baseAddition * (1.0f + skillAddition/5) * baseHurt;
		return hurt;
	}

	//主角1的普通子弹伤害
	static int getMainPl1NorHurt(float rate, int WeaponNum)
	{
		return SelfMainPl_1_NorBlt_hurt * rate/WeaponNum;
	}
	
	//主角1的加强子弹伤害
	static int getMainPl1AddHurt(float rate, int WeaponNum)
	{
		return SelfMainPl_1_AddBlt_hurt * rate/WeaponNum;
	}

	//主角2的普通子弹伤害
	static int getMainPl2NorHurt(float rate, int WeaponNum)
	{
		return SelfMainPl_2_NorBlt_hurt * rate/WeaponNum;
	}
	
	//主角2的加强子弹伤害
	static int getMainPl2AddHurt(float rate, int WeaponNum)
	{
		return SelfMainPl_2_NorBlt_hurt * rate/WeaponNum;
	}

	//主角3的普通子弹伤害
	static int getMainPl3NorHurt(float rate, int WeaponNum)
	{
		return SelfMainPl_3_NorBlt_hurt * rate/WeaponNum;
	}

	//主角3的加强子弹伤害
	static int getMainPl3AddHurt(float rate, int WeaponNum)
	{
		return SelfMainPl_3_NorBlt_hurt * rate/WeaponNum;
	}

	static int getMainPlBaseHurt()
	{
		int curRole = NewDataMgr::getInstance()->mRoleConfig.roleIndex;
		int baseHurt = 0;
		switch (curRole)
		{
		case 1:
			baseHurt = MainPl_1_BaseHurt;
			break;
		case 2:
			baseHurt = MainPl_2_BaseHurt;
			break;
		case 3:
			baseHurt = MainPl_3_BaseHurt;
			break;
		default:
			break;
		}
		return baseHurt;
	}

	static int getDazhaoHurtOfBullet()
	{
		float skillAddition = NewDataMgr::getInstance()->mGameGradeMsg->getObjectAtkWithCurLv(GameGradeMsg::ObjectType::ObjectType_DaZhao);
		return (getMainPlBaseHurt() * 5 + 500000/getMainPlBaseHurt()) * (1 + skillAddition);
	}

	static int getDazhaoHurtOfLaser()
	{
		float skillAddition = NewDataMgr::getInstance()->mGameGradeMsg->getObjectAtkWithCurLv(GameGradeMsg::ObjectType::ObjectType_DaZhao);
		return (getMainPlBaseHurt() * 30 + 3000000/getMainPlBaseHurt()) * (1 + skillAddition);
	}
};

#endif // __GameConfig_H__