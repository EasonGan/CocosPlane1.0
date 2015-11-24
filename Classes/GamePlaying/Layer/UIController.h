/********************************************************************
创建于:		2014/11/13  19:51	
文件名: 	E:\Cocos3.3_Project\MyGame\Classes\UIController.h 
作  者:		alex(alex)
修改于：	2014/11/13  19:51	

目  的:		
*********************************************************************/
#ifndef _UI_CONTROLLER_H_
#define _UI_CONTROLLER_H_


#include "cocos2d.h"

USING_NS_CC;

class MenuLayer;
class PhysicsLayer;
class Bullet;
class EffectsManager;
class DropGold;
class SpecialSkillReward;
class GameMainLayer;
class GameResultSuccess;
class RollingBg;
class CloudLayer;

class UIController : public Ref
{
public:
	static UIController* getInstance()
	{
		static UIController instance;
		return &instance;
	}

	void setPhysicsLayer(PhysicsLayer* layer);
	PhysicsLayer* getPhysicsLayer();

	void setMenuLayer(MenuLayer* layer);
	MenuLayer* getMenuLayer();

	void setMainUILayer( GameMainLayer* layer );
	GameMainLayer* getMainUILayer( void );

	void setEffectsLayer(EffectsManager* effectsLayer);
	EffectsManager* getEffectsLayer();

	void setGameBgMgr(RollingBg* layer);
	RollingBg* getGameBgMgr();

	void addBulletsForTarget(Sprite* weapon,Bullet* bullet,const Point& pos, const Vec2& vecDirec, bool isleft);
	void addMissile(Bullet* bullet,const Point& pos);

	//分数奖励
	void addScore(DropGold* sc,const Vec2& pos);

	//新的方式
	void addBullets(Bullet* bullet, const Point& pos, const int& Zorder);

	void addBulletStreakEff(Bullet* bullet, const Point& pos, const int& Zorder = -1);


	//显示结果界面
	void ShowResultLayer(bool isWin = true);

	//帮助界面
	void addEnergyFullTips();		//能量满提示

	void addCollectModeTips();		//收集模式提示

	void addKillModeTips();			//击杀模式提示

private:
	UIController();
	~UIController();

	Size				m_winSize;

	PhysicsLayer*		m_physicsLayer;
	MenuLayer*			m_menuLayer;
	EffectsManager*		m_EffectsLayer;
	GameMainLayer*		m_GameMainUiLayer;
	RollingBg*			m_GameBgMgr;
	CC_SYNTHESIZE(CloudLayer*, m_CloudLayer,CloudLayer);
};

#endif
