#ifndef __GAME_ROLE_SCENE_H__
#define __GAME_ROLE_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "../GameCommon/NewDataMgr.h"
#include "CommonWidget.h"

USING_NS_CC;
USING_NS_CC_EXT;

using namespace cocostudio::timeline;
using namespace cocos2d::ui;

#define LocalZOrde_1000		1000
#define LocalZOrde_500		500

class ShowGoodsLayer;
class CommonLayer;
/*******************************************
/* 界面管理
*******************************************/
class GameInterfaceMgr
{
public:
	enum InterfaceTag
	{

		InterfaceTag_None = 1888,			// 任务
		InterfaceTag_Task,					// 任务
		InterfaceTag_ShopBuy,				// 商店
		InterfaceTag_Upgrade,				// 魔法界面
		InterfaceTag_CardOption,			// 卡牌选择
		InterfaceTag_Checkpoint,			// 关卡选择
		InterfaceTag_SignIn,				// 签到
		InterfaceTag_LevelOption,			// 关卡
		InterfaceTag_VIPBuy,				// VIP
		InterfaceTag_VIPReward,				// VIP奖励
		InterfaceTag_ResoutSuc,				// 成功结算
		InterfaceTag_ResoutFail,			// 失败结算
		InterfaceTag_RewardCard,			// 奖励卡牌
	};
	
	static GameInterfaceMgr* getInstance(){
		static GameInterfaceMgr nGameInterfaceMgr;
		return &nGameInterfaceMgr;
	}
	// 获得/设置父节点
	CC_SYNTHESIZE(Node*, _node, ParentNode);

	// 添加下一个界面
	void addInterfaceToList( InterfaceTag tag , bool isShow = true, void* data = nullptr );
	// 显示下一个界面
	void showNextInterface( void );
	// 关才当前界面
	void closeCurInterface( void );
	// clean所有界面
	void closeAllInterface( void );
	// 获得当前界面数
	int getCurInterfaceNum(void);
	// 获得/设置当前界面Tag

	CC_SYNTHESIZE(InterfaceTag, mCurInterfaceTag, CurInterfaceTag);

private:

private:
	// 附加数据
	void*	_data;	

	std::list<InterfaceTag>	_interfaceVector;
};


/*******************************************
* 主界面
*******************************************/
class MainPlaneShowLayer;
class GameRoleScene : public Layer, public CommonLayer
{
public:
	enum ButtonTag
	{
		Tag_BtnRole1,				// 选择角色ID
		Tag_BtnRole2,				// 选择角色ID
		Tag_BtnRole3,				// 选择角色ID

		Tag_BtnLeftRoll,			// 左翻页
		Tag_BtnRightRoll,			// 右翻页

		Tag_BtnVIP,					// 新手礼包
		Tag_BtnNewPack,				// 新手礼包
		Tag_SignIn,					// 签到
		Tag_Play,					// 开始

	};

	GameRoleScene();
	~GameRoleScene();

	CREATE_FUNC(GameRoleScene);
	static Scene* createScene();


	virtual bool init();
	virtual void onEnter();
	virtual void onExit();
    virtual void onEnterTransitionDidFinish();


	static GameRoleScene* getGameRoleScene();
	void initInterface( void );

	// 升级动画
	void showPaySuccee(Ref* sender);
	void removeUpgradeAnim( Node* node );

	void playButtonPro( int roleid );

	// 购买成功特效
	void buySucceedAction( void );
	void callFuncbuySucceed( Node* node );
	void callFuncDeleyTouch(Node* node);


	void playUnlockEffect( void );


	// Even Callfunc
	void onMessageEvent(Ref* sender);
	virtual void setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback);
	virtual void callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType);
	void callFuncOptionCard(Ref* ref, Widget::TouchEventType touchType);
	void callFuncPageviewRole(Ref* ref, PageView::EventType eventType);
	void callFuncChangeRoleDemo(Node* node);

	// 退出游戏
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event); 
	void callFuncExitGame( void );

	void updateOptionRole(ButtonTag tag);
	void updateRoleAtk(void);
	void updateDemoRole(void);

	//播放解锁音效
	void playUnlockRoleSound();
	//播放选择音效
	void playOptionRoleSound();

	// 登录判断
	bool signInJudge(void);
	// vip判断
	bool vipRewardJudge(void);
		
private:
	Node* _myLayer;

	Size	_winSize;

	int _lastOptionRoleId;
	int	_curOptionRoleId;
	MainPlaneShowLayer* _showLayer;
};

/*******************************************
* 主界面顶部和底部框
*******************************************/
class MainSceneTaskBar : public Layer, public CommonLayer
{
public:
	enum ButtonTag
	{
		Tag_BtnUpgrade,			// 升级
		Tag_BtnCard,			// 卡牌
		Tag_BtnMain,			// 主界面
		Tag_BtnShop,			// 商城
		Tag_BtnVIP,				// VIP
		Tag_BgMusic,			// 背景音乐
		Tag_SoundEff,			// 音效

		Tag_BuyGole,
		Tag_None,

	};
	MainSceneTaskBar();
	~MainSceneTaskBar();



	static void showLayout(Node* node, bool parm = true);
	virtual void onEnter();
	virtual void onExit();

	CREATE_FUNC(MainSceneTaskBar);

private:
	virtual void onMessageEvent(Ref* sender);
	virtual void setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback);
	virtual void callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType);

	void callFuncDeleyTouch(Node* node);

	// 音乐音效
	void selectedMusicEvent(Ref* pSender, CheckBox::EventType type);
	void selectedSoundEvent(Ref* pSender, CheckBox::EventType type);

	void initInterface();
	void updataShowData(void);

	void checkTaskFinish(void);

	void MsgProShowButtomBar(Ref* sender);
	void MsgProHideButtomBar(Ref* sender);
	void MsgProChangeGemGold(Ref* sender);

private:
	Node*	_mainSceneTaskBar;

	bool _isShowButtomBar;

};

/*******************************************
* 魔法屋
*******************************************/
class UpgradeLayer :public Layer, public CommonLayer
{
public:
	enum ButtonTag
	{
		Tag_Back,			// 返回
		Tag_Upgrade,		// 升级

		Tag_ItemRole,
		Tag_ItemDaZhao,
		Tag_ItemGold,
		Tag_ItemRedBall,
		Tag_ItemBlueBall,
		Tag_itemYellowBall,

	};
	UpgradeLayer ()
		:_myLayer(nullptr)
		, _isGuideUpGrade(false)
	{

	}

	~UpgradeLayer ()
	{

	}

	static void showLayout( Node* node, int tag );
	virtual void onEnter();
	virtual void onExit();

	CREATE_FUNC(UpgradeLayer);

private:
	virtual void onMessageEvent(Ref* sender);
	virtual void setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback);
	virtual void callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType);
	void callFuncOptionItemEvent(Ref* ref, Widget::TouchEventType touchType);


	void initInterface();

	void upgradeItemPro(int tag);
	void updateInterface( void );
	void updateObjectInfo();

	void playUpgradeAnim(void);
	void removeUpgradeAnim(Node* node);
	void callFuncRemoveFlash(Node* node);

private:
	Node*	_myLayer;

	int _curOptionItemId;

	bool	_isGuideUpGrade;

};

/*******************************************
* 卡牌选择界面
*******************************************/
class CardOptionLayout : public Layer, public CommonLayer
{
public:
	enum ButtonTag
	{
		Tag_Back,			// 返回
		Tag_Card1,			// 卡牌1
		Tag_Card2,			// 卡牌2
		Tag_Card3,			// 卡牌3


		Tag_CloseOption,	// 关闭选择界面
		Tag_Confirm,		// 
	};

	CardOptionLayout()
		:OptionCardTouchListener(nullptr)
		,_touchLayer(nullptr)
		,_curSelectCardType(0)
		, _lastSelectCardType(0)
		, _isOption(false)
		, _isGuideUseCard(false)
	{

	}

	~CardOptionLayout()
	{

	}

	static void showLayout( Node* node, int tag );

	virtual void onEnter();
	virtual void onExit();
	virtual bool onTouchBegan(Touch *touch, Event *unused_event);

	CREATE_FUNC(CardOptionLayout);


private:

	void initInterface();


	// Even Callfunc
	virtual void onMessageEvent(Ref* sender);
	virtual void setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback);
	virtual void callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType);
	void callFuncCardTypeOptionEvent(Ref* ref, Widget::TouchEventType touchType);
	void callFuncCardOptionEvent(Ref* ref, Widget::TouchEventType touchType);

	void runButtonAction( Node* node );


	void showConfirmOption( void );

	void optionCardTypePro(int type);

	void updateOptionCard(void);
	void updateCardList(void);

	void callFuncChangeOptionCard(bool isUse);
	void callFuncRemoveCardFlash(Node* node);

	bool checkCardIsUnlock(void);
	void setOptionState(Node* node, int tag);
private:

	Layer*	_touchLayer;

	Node* _myLayer;

	Button* _cardBtn[3];

	// 确认卡牌选择界面触摸
	EventListenerTouchOneByOne *OptionCardTouchListener;

	int		_lastSelectCardType;	//	1-3
	int		_curSelectCardType;		//	1-3
	int		_curSelectCardID;		//	1-9	

	bool	_isOption;
	bool	_isGuideUseCard;
};

/*******************************************
*	卡牌信息界面
*******************************************/
class CardInformatLayer :public Layer, public CommonLayer
{
public:

	typedef enum ButtonTag
	{
		Tag_Close,			// 关闭

		Tag_Confirm,		// 
	};
	CardInformatLayer()
		:_myLayer(nullptr)
		, _curSelectCardID(1)
		, _confirmCallFunc(nullptr)
		, mGuideState(false)
	{

	}

	~CardInformatLayer()
	{

	}
	typedef std::function<void(bool)> ccConfirmCallBack;

	static CardInformatLayer* getCardInformatLayer(void);
	virtual void onEnter();
	virtual void onExit();

	CREATE_FUNC(CardInformatLayer);

	void setCardIndex(int index);
	void addConfirmCallFunc(const ccConfirmCallBack& callfunc);

	CC_SYNTHESIZE(bool, mGuideState, GuideState);
private:
	virtual void onMessageEvent(Ref* sender);
	virtual void setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback);
	virtual void callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType);

	void initInterface();

	void updateSingleCardInfo(void);
	void setCardRolePos();

	void playCardUnlockAnim(void);
private:
	Node*	_myLayer;

	int _curSelectCardID;

	ccConfirmCallBack	_confirmCallFunc;
};

/*******************************************
*	任务
*******************************************/
class TaskListView : public cocos2d::Layer, public cocos2d::extension::TableViewDataSource, public cocos2d::extension::TableViewDelegate, public CommonLayer
{
public:
	enum ListType
	{
		ListType_DayTask,
		ListType_AchiTask,
	};
	virtual bool init(void);

	CC_SYNTHESIZE(ListType, mlistType, MyListType);

	static TaskListView* create(ListType type);
	CREATE_FUNC(TaskListView);

	void initTaskList(void);

	virtual void onMessageEvent(Ref* sender);
	virtual void setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback){}
	virtual void callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType){}

	virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view) {};
	virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view);
	virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell);
	virtual cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView *table, ssize_t idx);
	virtual cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx);
	virtual ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table);

	void callFuncGetRewardEvent(Ref* ref, Widget::TouchEventType touchType);
	std::string getGoodsResWithID(int id);

	void updateTableItem(TableViewCell *cell, int taskid);

private:
	std::map<int, int>		_taskMap;

};


class GameTaskList :public Layer,public CommonLayer
{
public:
	
	typedef enum ButtonTag
	{
		Tag_Back,			// 返回
		Tag_Close,			// 关闭

		Tag_LabelDayTask,	// 日任务
		Tag_LabelAchTask,	// 成就任务 
	};
	GameTaskList ()
		:_mylayer(nullptr)
	{

	}

	~GameTaskList ()
	{

	}

	static void showLayout( Node* node, int tag );
	virtual void onEnter();
	virtual void onExit();


	CREATE_FUNC( GameTaskList );

private:
	virtual void onMessageEvent(Ref* sender);
	virtual void setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback);
	virtual void callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType);
	void callFuncLabelEvent(Ref* ref, Widget::TouchEventType touchType);
	void callFuncGetRewardEvent(Ref* ref, Widget::TouchEventType touchType);

	void initInterface();
	void addTaskItemToDayList(void);
	void addTaskItemToAchiList(void);

	void checkTaskFinish(void);

	std::string getGoodsResWithID(int id);

	void changeTaskLable(int id);
private:
	Node*	_mylayer;

	int _curGetTaskID;
};

/*******************************************
*	商店
*******************************************/
class ShopBuyLayer :public Layer, public CommonLayer
{
public:

	enum ButtonTag
	{
		Tag_Back,			// 返回

		Tag_OverflowPack,	// 超值
		Tag_TyrantPack,		// 土豪
		Tag_BuyGold,
		Tag_BuyShield,
		Tag_BuyDaZhao,
		Tag_BuyPropPack,

	};
	ShopBuyLayer()
		:_widgetLayer(nullptr)
		, _showGoodsInfoLayer(nullptr)
	{

	}

	~ShopBuyLayer()
	{

	}

	static void showLayout(Node* node, int tag);
	virtual void onEnter();
	virtual void onExit();

	CREATE_FUNC(ShopBuyLayer);

	CC_SYNTHESIZE(int, _optionLabel, optionLabel);			//标签页面

private:
	virtual void onMessageEvent(Ref* sender);
	virtual void setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback);
	virtual void callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType);

	void initInterface();
	void showGoodsInfo(int tag);

private:
	Node*	_widgetLayer;

	PackShowLayer* _showGoodsInfoLayer;

};


/*******************************************
*	签到任务
*******************************************/
class SignInLayer :public Layer, public CommonLayer
{
public:

	typedef enum ButtonTag
	{
		Tag_Close,			// 关闭
		Tag_GetReward,		// 领取


	};
	SignInLayer()
		:_myLayer(nullptr)
	{

	}

	~SignInLayer()
	{

	}

	static void showLayout(Node* node, int tag);
	virtual void onEnter();
	virtual void onExit();

	CREATE_FUNC(SignInLayer);



private:
	virtual void onMessageEvent(Ref* sender);
	virtual void setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback);
	virtual void callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType);

	void initInterface();

	void addToRewardTips(void);
private:
	Node*	_myLayer;


};

/*******************************************
*	关卡选择
*******************************************/
class LevelOptionLayer :public Layer, public CommonLayer
{
public:

	typedef enum ButtonTag
	{
		Tag_Back,			// 关闭

		Tag_Start,			// 开始
		Tag_NewPack,		// 新手礼包
		Tag_ThPack,			// 土豪礼包

		Tag_LeftPage,		// 左翻页
		Tag_RightPage,		// 右翻页
	};
	LevelOptionLayer()
		:_myLayer(nullptr)
	{

	}

	~LevelOptionLayer()
	{

	}

	static void showLayout(Node* node, int tag);
	virtual void onEnter();
	virtual void onExit();

	CREATE_FUNC(LevelOptionLayer);



private:
	virtual void onMessageEvent(Ref* sender);
	virtual void setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback);
	virtual void callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType);
	virtual void callFuncLevelButtonEvent(Ref* ref, Widget::TouchEventType touchType);


	void callFuncPageviewRole(Ref* ref, PageView::EventType eventType);

	void showPackInfoWithTag(int tag);
	void updateScrollPageButton(void);
	void setOptionLevelBut(int level);
	void updateAnimWithLevel(int level);
	void callFuncScrollTo(Node* node);
	void initInterface();
	std::string getLevelStateRes(int lv);

private:
	Node*	_myLayer;


};

/*******************************************
*	VIP购买
*******************************************/
class VipLayer :public Layer, public CommonLayer
{
public:

	typedef enum ButtonTag
	{
		Tag_Close,			// 关闭

		Tag_Buy,			// 购买
	};
	VipLayer()
		:_myLayer(nullptr)
	{

	}

	~VipLayer()
	{

	}

	static void showLayout(Node* node, int tag);
	virtual void onEnter();
	virtual void onExit();

	CREATE_FUNC(VipLayer);

private:
	virtual void onMessageEvent(Ref* sender);
	virtual void setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback);
	virtual void callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType);

	void initInterface();

private:
	Node*	_myLayer;


};

/*******************************************
*	VIP每日赠送
*******************************************/
class VipPresentingLayer :public Layer, public CommonLayer
{
public:

	typedef enum ButtonTag
	{
		Tag_Close,			// 关闭

		Tag_Get,			// 领取
	};
	VipPresentingLayer()
		:_myLayer(nullptr)
	{

	}

	~VipPresentingLayer()
	{

	}

	static void showLayout(Node* node, int tag);
	virtual void onEnter();
	virtual void onExit();

	CREATE_FUNC(VipPresentingLayer);

private:
	virtual void onMessageEvent(Ref* sender);
	virtual void setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback);
	virtual void callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType);

	void initInterface();

private:
	Node*	_myLayer;

};

/*******************************************
*	奖励卡牌赠送
*******************************************/
class RewardCardLayer :public Layer
{
public:

	typedef enum ButtonTag
	{

	};
	RewardCardLayer()
		:_myLayer(nullptr)
	{

	}

	~RewardCardLayer()
	{

	}

	static void showLayout(Node* node, int tag);
	virtual void onEnter();
	virtual void onExit();

	CREATE_FUNC(RewardCardLayer);

private:

	void initInterface();
	void callFuncFront(Node* node);
	void callFuncReverse(Node* node);

private:
	Node*	_myLayer;

};
#endif
