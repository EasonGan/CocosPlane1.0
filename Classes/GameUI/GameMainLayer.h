#ifndef __GAME_MAIN_LAYER_H__
#define __GAME_MAIN_LAYER_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "../GameUI/CommonWidget.h"
#include "../GameCommon/NewDataMgr.h"

USING_NS_CC;
USING_NS_CC_EXT;

using namespace cocostudio::timeline;
using namespace cocos2d::ui;

/************************************************************************/
/* 游戏战斗主界面                                                        */
/************************************************************************/

class GameMainLayer : public Layer//, public CommonLayer
{
public:
	GameMainLayer()
		:_mainLayerRootNode(nullptr)
//		,_loadingBarEnergy(nullptr)
		,_countTime(0)
		,_countAddLifeNum(0)
		,_maxLifeBar(0)
		,_curMaxLifeBar(0)
		,_decayInterval(0)
		,_decayCountT(0)
		,m_fighting_shield_num(0)
		,m_fighting_skill_num(0)
		,m_fighting_shield_used(0)
		,m_fighting_skill_used(0)
		,m_HpLimit(100.0f)
		,_decayIng(false)	// 默认非衰减中
		,m_isDead(false)
		,m_isCanBeHurt(true)
	{
		int curRole = NewDataMgr::getInstance()->mRoleConfig.roleIndex;
		m_HpLimit = 0;// NewDataMgr::getInstance()->getRoleHpWithId(curRole);
		m_CurHp = m_HpLimit;
	}

	~GameMainLayer()
	{
	}

	enum ButtonTag
	{
		Tag_DaZhao,
		Tag_HuDun,
		Tag_Pause,			// 关闭
	};

	CREATE_FUNC(GameMainLayer);

	void onMessageEvent(Ref* sender);


	void updateGoodsNum(void);

	// 消耗1个生命
	void consumeLife(void);
	// 消耗1个护盾
	void consumeHudun(void);
	// 消耗1个大招
	void consumeDazhao(void);

    void changeLifeNum(int lifeNum);
    void changeHudunNUm(int hudunNum);
    void changeDazhaoNum(int dazhaoNum);

	// 更新金币
	void updateUIGold( int goldnum );
	// 更新成绩
	void updateUIScore( int score );
	// 更新Km
	void updataGamedistance( int km );


	//血量持续减少
	void AddHp(float num);

	//在受到伤害之后的一段时间内，不会受到伤害
	void WudiSometime(float dur = 1.0f);
	void WudiOver(Node* node);

	void GameFailed();
	bool isRealDead(){return m_isDead;}
	void SetRealDead(bool b);

	void showBuyFuHuoWidget( void );
	void showSettingWidget( void );

	static Node * seekNodeByName(Node* node, const std::string &name );
	static void DelayPuse(Node* node);
	void GamePause();


	// 播放远征成功
	void playPassSucceed( void );
	void callFuncGuang1( Node* node );
	void callFuncNodeMove( Node* node );
	void setDaZhaoState( bool state );
	void startDaZhaoSD( void );

	// 开始技能
	void startDaZhao();
	//释放护盾
	void startHuDun();
	//获得护盾时在没有保护时释放
	void addHuDun();

	void helpFeqCard(void);
	void helpFeqHuDun(void);
	void helpFeqDaZhao(void);
	void showAddShieldEffect();
	void showAddSkillEffect();

	bool shieldIsEnough();
	bool skillIsEnough();
	void setUserItemNumWhenGameEnd(bool isWin);
private:
	void showAddEffect(Widget* node);

	virtual void onEnter();
	virtual bool init();
	virtual void onExit();

	// 初始界面数据
	void initInterface( void );

	Node* createEnergyFullAnim( Vec2 ps );
	void removeEnergyAnim( Node* node );
	void callFuncRandAnim( float t );

	void playLifeBarWarning( void );

	void buyFuhuoBtnEvent(CommonWidget::EventKey key);
	void buyFuhuoCancel( void );
	void BuyFuHuoConfirm( void );

	void callbackPause( Ref* ref );
	// 大招
	void callbackButtonHudun(Ref* ref);
	void callbackButtonPack(Ref* ref);
	void callbackDazhao( Ref* ref );
	void playDaZhaoCD( void );
	void callFuncDaZhaoCD( Node* node );
	void callFuncWidgetBuyDz( void );
	void callFuncWidgetBuyDzExit( void );
	void callWidgetGoldTips( void );

private:

//	LoadingBar* _loadingBarEnergy;


	Node*	_mainLayerRootNode;
	float	_countAddLifeNum;
	int		_countTime;
	float	_curMaxLifeBar;
	float	_maxLifeBar;
	
	float	_decayInterval;			// 衰减延时
	float	_decayCountT;			// 衰减总时长
	bool	_decayIng;				// 衰减状态 ture衰减中

	float	m_HpLimit;				//血上限
	float	m_CurHp;				//当前血量值
	bool	m_isDead;				//是否打死
	bool	m_isCanBeHurt;
	int m_fighting_shield_num;
	int m_fighting_skill_num;

	int m_fighting_shield_used;
	int m_fighting_skill_used;
};



// 复活
class BuyFuHuo : public Layer
{
public:

	enum ButtonTag
	{
		Tag_BuyFuhuo,		// 购买
		Tag_Close,		// 购买

	};

	BuyFuHuo()
		:_fuHuoLayout(nullptr)
	{

	}

	~BuyFuHuo()
	{

	}

	static void showLayout( Node* node );

	virtual void onEnter();
	virtual void onExit();

	CREATE_FUNC( BuyFuHuo );


private:
	void onMessageEvent(Ref* sender);

	void initInterface();

	// Even Callfunc
	void setEventForNode( Button* node, ButtonTag tag );
	void callFuncButtonEvent( Ref* ref );

	void buyFuhuoCancel( void );
	void BuyFuHuoConfirm( void );

	void callFuncAction1( Node* node );
	void callFuncEge(Node* node);
private:

	Node* _fuHuoLayout;


};



// 设置
class SettingLayer : public Layer ,public CommonLayer
{
public:

	enum ButtonTag
	{
		Tag_ExitGame,		// 退出游戏
		Tag_ContinueGame,	// 继续游戏

	};

	SettingLayer()
		:_settingayout(nullptr)
	{

	}

	~SettingLayer()
	{

	}

	static void showLayout( Node* node );

	virtual void onEnter();

	void onMessageEvent(Ref* sender){};
	virtual void setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback);
	virtual void callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType);
	CREATE_FUNC( SettingLayer );


private:

	void initInterface();

	// 音乐音效
	void selectedMusicEvent(Ref* pSender,CheckBox::EventType type);
	void selectedSoundEvent(Ref* pSender,CheckBox::EventType type);
	
private:

	Node* _settingayout;


};

#endif
