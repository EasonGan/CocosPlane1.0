#ifndef __GAME_SYSTEM_H__
#define __GAME_SYSTEM_H__



#include "cocos2d.h"
#include "../CsvMgr/CsvMgr.h"
#include "sqlite3/include/sqlite3.h"



USING_NS_CC;
/************************************************************************/
/* 游戏相关系统管理                                                  */
/************************************************************************/
	
// 消息ID
#define MsgEventName_ToTaskBarLayer			"to taskbar"
#define MsgEventName_ToRoleLayer			"to Role"
#define MsgEventName_ToCardLayer			"to card"
#define MsgEventName_ToCardInfoLayer		"to cardInfo"
#define MsgEventName_ToUpgradeLayer			"to upgrader"
#define MsgEventName_ToShopLayer			"to shop"
#define MsgEventName_ToGoodsInfoLayer		"to goodsInfo"
#define MsgEventName_ToTaskLayer			"to task"
#define MsgEventName_ToSignInLayer			"to SignIn"
#define MsgEventName_ToLevelOptionLayer		"to LevelOption"
#define MsgEventName_ToVIPLayer				"to VIP"
#define MsgEventName_ToVIPPresentingLayer	"to VIPPresenting"
#define MsgEventName_ToSucResult			"to SucResult"
#define MsgEventName_ToFailResult			"to FailResult"
#define MsgEventName_ToGameMainLayer		"to MainLayer"
#define MsgEventName_ToGameFuHuo			"to FuHuo"
#define MsgEventName_ToRolePack				"to RolePack"


enum EProtocolID
{
	// 任务栏菜单
	EProtocolID_ShowButtomBar = 1,
	EProtocolID_HideButtomBar,
	EProtocolID_UpdateGold,
	EProtocolID_UpdateTaskNum,



	//主界面
	EProtocolID_BuySuccessEffect,
	EProtocolID_UnlockRole,
	EProtocolID_BuyRolePackFinish,
	EProtocolID_BuyRolePackCancel,
	EProtocolID_VipBuyCallFunc,
	EProtocolID_UpdateRoleAtk,
	EProtocolID_HelpFeqPlay,
	EProtocolID_HelpFeqCard,


	// Vip获取
	EProtocolID_VipRewardGet,


	// 魔法屋
	EProtocolID_UpgradeObject,

	// 卡牌
	EProtocolID_UnlockCard,
	EProtocolID_HelpFeqStep2,


	// 商城
	EProtocolID_BuyGoodsSucc,

	// 物品信息
	EProtocolID_RemoveGoodsInfo,

	// 任务奖励领取
	EProtocolID_GetTaskReward,

	// 土豪礼包购买成功
	EProtocolID_TyrantPackBuy,
	EProtocolID_DaZhaoBuy,
	EProtocolID_DaHudun,
	EProtocolID_FuHuoBuy,
	EProtocolID_PropPackBuy,

};

struct MakeMsgData
{
	EProtocolID id;
	std::string msg;

	MakeMsgData(std::string pmsg, EProtocolID pid)
	{
		msg = pmsg;
		id = pid;
	}
};

class CProtocolManager
{
public:
	static CProtocolManager* getInstance()
	{
		static CProtocolManager inst;
		return &inst;
	}

public: 
	void sendNotificationMsg(const std::string msg, EProtocolID id);

private:

};

/*******************************************
* 任务系统
*******************************************/

// 常用统计属性
enum GamePropertyEnum
{
	GamePrKey_killMonNum,			// 击杀敌机数
	GamePrKey_BGradeNum,			// 达成等级数
	GamePrKey_AGradeNum,			// 达成等级数
	GamePrKey_SGradeNum,			// 达成等级数
	GamePrKey_SSGradeNum,			// 达成等级数
	GamePrKey_getRedBallNum,		// 接红球数
	GamePrKey_getBlueBallNum,		// 接蓝球数
	GamePrKey_getYellowBallNum,		// 接绿球数
	GamePrKey_RedKillerNum,			// 红球击杀数
	GamePrKey_BlueKillerNum,		// 蓝球击杀数
	GamePrKey_YellowKillerNum,		// 红球击杀数
	GamePrKey_totalKm,				// 达成Km
	GamePrKey_minKillBossTime,		// 最小击杀boss时间
	GamePrKey_RedScore,				// 红球成绩
	GamePrKey_BlueScore,			// 蓝球成绩
	GamePrKey_YellowScore,			// 绿球成绩
	GamePrKey_DeadNum,				// 死亡次数

	GamePrKey_FinishGameNum,		// 闯关次数
	GamePrKey_BuyGoldTime,			// 够买金币次数
	GamePrKey_UseDaZhao,			// 使用大招次数
	GamePrKey_UseHuDun,				// 使用护盾次数
	GamePrKey_UpgradeObject,		// 升级了魔法
	GamePrKey_GetStarNum,			// 获得星数

	GamePrKey_Max,
};
#define TaskSystem_DayTaskNum		5		// 每日任务数量











class GameTaskSystem
{
public:

	// 此枚举与GameTaskConfig.csv一一对应
	enum DayTaskTag
	{
		/*************** 每日任务 ********/
		DayTaskTag_Task1 = 101,		//购买1次金币补给
		DayTaskTag_Task2,			//使用1次陨石流星雨必杀。
		DayTaskTag_Task3,			//消耗1次护盾。
		DayTaskTag_Task4,			//对任意魔法项目进行1次升级。
		DayTaskTag_Task5,			//在战斗中接到30个蓝色精灵球。
		DayTaskTag_Task6,			//在战斗中接到30个红色精灵球。
		DayTaskTag_Task7,			//在战斗中接到30个黄色精灵球。
		DayTaskTag_Task8,			//完成3次闯关。
		DayTaskTag_Task9,			//击杀50个敌人。

		// 以上添加每日任务
		

		/*************** 成就任务********/
		DayTaskTag_Task10	= 201,		//通过第2关。
		DayTaskTag_Task11,				//通过第4关。
		DayTaskTag_Task12,				//通过第6关。
		DayTaskTag_Task13,				//通过第8关。
		DayTaskTag_Task14,				//通过第10关。
		DayTaskTag_Task15,				//通过第12关。
		DayTaskTag_Task16,				//通过第14关。
		DayTaskTag_Task17,				//通过第16关。
		DayTaskTag_Task18,				//死亡30次。
		DayTaskTag_Task19,				//死亡50次。
		DayTaskTag_Task20,				//死亡100次。
		DayTaskTag_Task21,				//解锁全部主角。
		DayTaskTag_Task22,				//累积击杀500个敌人。
		DayTaskTag_Task23,				//累积击杀1000个敌人。
		DayTaskTag_Task24,				//累积击杀2000个敌人。
		DayTaskTag_Task25,				//升级女神达到满级。
		DayTaskTag_Task26,				//升级陨石流星雨达到满级。
		DayTaskTag_Task27,				//升级金币加成达到满级。
		DayTaskTag_Task28,				//升级红色精灵球达到满级。
		DayTaskTag_Task29,				//升级蓝色精灵球达到满级。
		DayTaskTag_Task30,				//升级黄色精灵球达到满级。
		DayTaskTag_Task31,				//获得9个星星
		DayTaskTag_Task32,				//获得22个星星
		DayTaskTag_Task33,				//获得35个星星
		DayTaskTag_Task34,				//获得48个星星

		// 以上添加成就任务
		DayTaskTag_AchiTaskEnd
	};

	struct DayTaskStr
	{
		int taskId;				// 任务ID
		bool isFinish;			// 任务是否完成
	};

	GameTaskSystem();

	~GameTaskSystem();

	void initGameProperty( void );

	// 获取/设置任务日期
	int getTaskDate( void );
	void setTaskDate( int date );

	int getSignInDate(void);
	void setSignInDate(int date);


	void initGameTask( sqlite3* db );
	void initDayTaskTable( void );
	void initTaskState(void);
	void createDayTaskList();
	void createAchiTaskList();

	void saveGameAboutData( void );
	void saveGamePropertyData( void );
	void saveDayTaskData( void );
	void saveAchiTaskData(void);

	void getTaskRewardWithTaskId(int id);
	void setTaskStateToTable(int taskid);
	void delTaskStateToTable(int taskid);


	std::vector<int> getDayTaskList( void );
	std::vector<int> getAchiTaskList(void);
	int getDayTaskFinishNum( void );
	int getAchiTaskFinishNum(void);


	// 设置任务相关属性数值
	void setGameTaskProperty(int *data);
	void addTaskPropertyWithKey(GamePropertyEnum key, int num);

	csvGameTaskConfig_s* getGameTaskListWithID( int id );

	int					_GamePropertyArr[GamePrKey_Max];

private:
	void updateDayTaskData(void);
	void updateAchiTaskData(void);

	void cleanTaskProperty( void );

private:
	sqlite3*			_sqlitedb;

	// 每日任务列表
	std::vector<int>	_dayTaskList;
	// 成就任务列表
	std::vector<int>	_achiTaskList;


};



/*******************************************
* 角色系统
*******************************************/
class GameRoleSystem
{
public:
	enum TableKey
	{
		TableKey_LockState,
		TableKey_Grade,
	};

	GameRoleSystem(sqlite3* db);

	~GameRoleSystem(){}

	// 初始化角色列表
	void initRoleInfo( sqlite3* db );

	// 获得角色相关信息
	csvRoleUpgrade_s* getRoleConfigWithID(int id);

	// 获得角色等级
	int getRoleGradeWithID(int id);
	
	// 获得角色价格
	int getRolePriceWithID(int id);

	// 设置角色解锁状态
	void setRoleLockState(int id, bool state);
	
	// 更新表数据
	void updateTable(TableKey key,int id, int data);
private:

	sqlite3* _sqlite;
};


/*******************************************
* 卡牌系统
*******************************************/
class GameCardSystem
{
public:
	GameCardSystem(sqlite3* db);
	~GameCardSystem()
	{

	}

	void initCardInfo(sqlite3* db);
	// 初始化上次选择卡牌
	void initCardOption(sqlite3* db);

	int getCardPriceWithID(int id);
	bool buyCardWithID(int id);
	
	void updateCardLock(void);
	void setCardLockWithId(int cardid);
	void setOptionCard(int cardType, int cardid);
	void unLockCard2();
	void firstBuyUnlockCard(void);
	csvCardAtkPConfig_s* getCardConfigWithId(int id);

	CC_SYNTHESIZE(int, mNewUnlockCard, NewUnlockCard);
private:
	sqlite3* _sqlite;

	int _curOptionCardId;

};



/*******************************************
* 物品购物系统
*******************************************/

enum BuyType
{
	BuyType_Gold = 1,		// 金币
	BuyType_RMB = 2,		// 人民币
	BuyType_Free = 3		// 免费
};

enum BuyKey
{
	BuyKey_NULL					= 0,
	BuyKey_Role2				= 101, //希尔梅丽雅
	BuyKey_Role3				= 102, //蕾娜斯

	BuyKey_BlueCard				= 201, //蓝色品质卡牌
	BuyKey_VioletCard			= 202, //紫色品质卡牌
	
	BuyKey_UpgradeRole			= 301,	// 升级女神
	BuyKey_UpgradeDaZhao		= 401,	// 升级大招
	BuyKey_UpgradeGold			= 501,	// 升级金币
	BuyKey_UpgradeRedBall		= 601,	// 升级红球
	BuyKey_UpgradeBlueBall		= 701,	// 升级蓝球
	BuyKey_UpgradeYellowBall	= 801,	// 升级黄球

	BuyKey_NewPack				= 901,	// 购买新手礼包
	BuyKey_OverflowPack			= 1001,	// 购买超值礼包
	BuyKey_TyrantPack			= 1101,	// 购买土豪礼包
	BuyKey_BuyGold				= 1201,	// 购买金币
	BuyKey_BuyShield			= 1301,	// 购买护盾
	BuyKey_BuyDaZhao			= 1401,	// 购买大招
	BuyKey_BuyLife				= 1501,	// 购买生命
	BuyKey_BuyVIP				= 1601,	// 购买VIP首送

	BuyKey_BuyVIPPre			= 1602,	// 购买VIP每日赠送
	BuyKey_BuyFuHuo				= 1701,	// 购买复活

	BuyKey_BuyPropPack			= 1801, // 道具礼包
	BuyKey_BuyRolePack			= 1901, // 女神礼包

};


// vip有效天数
#define VIPValid_Day		30

typedef struct PackGoodsInfo
{
	BuyKey key;
	int gold;
	int life;
	int dazhao;
	int hudun;

	PackGoodsInfo(int goldnum, int lifenum, int dazhaonum, int hudunnum)
	{
		gold = goldnum;
		life = lifenum;
		dazhao = dazhaonum;
		hudun = hudunnum;
	}
}PackGoodsInfo;



class GameBuySystem :public Node
{
public:
	GameBuySystem();
	~GameBuySystem();

	void initBuyData( sqlite3* db );

	void initGoodsData( sqlite3* db );
	void addGoodsNumWithKey( int key, int num );
	void updateGoodsNumConfig(int key);
	void checkFirstBuyPackState(void);


	// 获得物品购买
	csvBuyList_s* getBuyGoodsDataWithID(int id);

	bool buyGoodsWithKey( int key , int num = 1, bool isFree = false);
	bool buyGoodsWithPrice(int goodsprice);
	void firstBuyPackPrc(void);
	std::string getGoodsPayCodeWithKey(int key);

	PackGoodsInfo* getPackGoodsInfoWithKey(int key);
	void sendBuySuccessMsg(void);
	 
	CC_SYNTHESIZE(bool, m_NotionState, NotionState);
	CC_SYNTHESIZE(bool, mIsFree, FreeState);
private:
	// 处理来自第三方通知
	void updateNotion(float t);

	void showBuyTips( BuyType type, int needGold );

	void callFuncBuyWidget(void);



private:
	sqlite3*			_sqlitedb;

	std::map<int, PackGoodsInfo >		_packInfoMap;

	int _curBuykey;

};


/*******************************************
* 等级管理 
*******************************************/
struct gradeStr
{
	int curGrade;
	int maxGrade;
};
class GameGradeMsg
{
public:
	enum ObjectType
	{
		ObjectType_Role = 1,
		ObjectType_DaZhao,
		ObjectType_Gold,
		ObjectType_RedBall,
		ObjectType_BlueBall,
		ObjectType_YellowBall
	};

	GameGradeMsg(sqlite3* db)
	{
		_sqlite = db;
		initGradeInfo();
	}
	~GameGradeMsg()
	{
	}

	void initGradeInfo(void);
	
	//获得对象等级
	gradeStr* getObjectGradeWithType(int type);

	// 设置对象等级
	void setObjectGradeWithType(int type, int grade);

	void buyObjectGradeWithType(int type, bool isFree = false);
	void upgradeobjectWithType(int type);
	// 获得对象升级购买数据
	csvBuyList_s getObjectUpgradePriceWithType(int type);

	int getRewardGoldWithLv(float goldNum);
	float  getObjectAtkWithCurLv( int type );

private:
	int _curUpgradeType;
	sqlite3* _sqlite;

	std::map<int, gradeStr* >		_objectGradeMap;

};



#endif