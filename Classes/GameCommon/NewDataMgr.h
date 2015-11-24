#ifndef __NEW_ROLE_DATA_MGR__
#define __NEW_ROLE_DATA_MGR__

#include "cocos2d.h"
#include "../SQLite/sqlite3.h"

#include "../CsvMgr/CsvMgr.h"
#include "GameSystem.h"



USING_NS_CC;





// 单件声明模板
 #define SINGLETONDATA_DECLEAR( className )	public:\
 	static className	*getInstance(){ if( inst == NULL ) { inst = new (std::nothrow) className(); }	return inst; }\
 	static void			purgeInstance(){ if( inst != NULL ) { CC_SAFE_DELETE(inst);	inst = NULL; } }\
 										private:\
 										static className	*inst;


// 单件定义模板。在cpp开始调用											
#define SINGLETONDATA_DEFINE( className )	className	*className::inst = NULL;

#define GameData_CardNum	9

/************************************************************************/
/* 角色升级相关规则                                                     */
/************************************************************************/
#define  RoleGrade_Max		60
//攻击公式
#define RoleAtkFormula( grade, coe1, coe2, coe3, coe4 )						( coe1 * grade + coe2 * coe3 * coe4)
//武器数公式
#define RoleWeaponNumFormula( grade, bass, coe )							( bass + grade/10 )
//防御公式
#define RoleDefenseFormula( grade, bass, coe )								( coe * grade + bass)
//幸运公式
#define RoleLuckFormula( grade, coe )										( coe * grade )
//经验公式
#define RoleExpFormula( grade, bass, coe1, coe2, coe3 )						( coe1 * grade * grade + coe3 * grade)
//HP公式
#define RoleHpFormula( grade, bass, coe1, coe2, coe3, coe4 )				( coe1 * grade + bass + pow(coe2 , coe3) * coe4 )

/************************************************************************/
/* 抽奖相关*/
/************************************************************************/
typedef enum RewardGoodsKey 
{
	RewardKey_1 = 1,			// 随机4星卡牌
	RewardKey_2,				// 随机3星卡牌
	RewardKey_3,				// 大经验水晶
	RewardKey_4,				// 金币
	RewardKey_5,				// 金币
	RewardKey_6,				// 翅膀
	// 以上为游戏外

	// 以下为游戏内
	RewardKey_7,				// 随机1星卡牌
	RewardKey_8,				// 随机2星卡牌
	RewardKey_9,				// 小经验水晶
	RewardKey_10,				// 中经验水晶
	RewardKey_11,				// 冲刺道具
	RewardKey_12,				// 护盾道具
	RewardKey_13,				// 大招道具

	// 任务奖励
	RewardKey_14,				// 金币
	RewardKey_15,				// 随机1~2星卡牌
	RewardKey_16,				// 中经验水晶
	RewardKey_17,				// 中经验水晶
	RewardKey_18,				// 中经验水晶
	RewardKey_19,				// 随机2星卡牌
	RewardKey_20,				// 随机1~2星卡牌
	RewardKey_21,				// 金币
	RewardKey_22,				// 金币
	RewardKey_23,				// 金币
	RewardKey_24,				// 大经验水晶

};
	

typedef struct GameCard
{
	// use this data
	int cardID;			// 卡牌ID 1-9
	int cardStarLv;		// 星级 1-5

}GameCard_S;

typedef struct  EntryGameConfig
{

	int		roleIndex;				// 角色索引

	GameCard_S	cardArr[3];			// 选择带入游戏的三类卡片

}EntryGameConfig_S;

typedef enum
{
	CardDateKey_isLock,
}CardDateKey;

typedef enum
{
	RoleDateKey_isLock,
	RoleDateKey_CurExp,
	RoleDateKey_Grade,
	RoleDateKey_isOption

}RoleDateKey;

// 时间
enum SystemTime
{
	SystemTime_Normal,		// 年
	SystemTime_Year,		// 年
	SystemTime_Mon,			// 月
	SystemTime_Day,			// 日
	SystemTime_Hour,		// 时
	SystemTime_Min,			// 分
	SystemTime_Sec			// 秒
};

// 物品ID
enum GoodsID
{
	GoodsID_Gold	= 1,
	GoodsID_Life	= 4,
	GoodsID_DaZhao	= 2,
	GoodsID_HuDun	= 3,
	GoodsID_Box		= 5,// 宝箱

	GoodsID_Role	= 6,
	GoodsID_Card	= 7,

};

// 用户数据
typedef struct UserData
{
	int IsNewUser;
	int Life;
	int Gold;
	int Gem;
	int DaZhao;
	int HuDun;
}UserData;

typedef struct SignInRewardInfo 
{
	int gold;
	int life;
	int dazhao;
	int hudun;
	bool isBox;	// 是否宝箱

	SignInRewardInfo(int goldNum, int lifeNum, int dazhaoNum, int hudunNum, bool isbox)
	{
		gold = goldNum;
		life = lifeNum;
		dazhao = dazhaoNum;
		hudun = hudunNum;
		isBox = isbox;
	}

	std::string goodsDes;
};


class NewDataMgr : public Ref
{
public:

	//SINGLETONDATA_DECLEAR(NewDataMgr);
	static NewDataMgr* getInstance()
	{
		static NewDataMgr inst;
		return &inst;
	}

	NewDataMgr();

	~NewDataMgr();


public:
	// 初始化读取数据库
	void initGameData( void );
	// 初始化用户数据
	void initUserData( sqlite3* db );
	
	// 获取/设置用户物品
	void addGoodsNumWithID(GoodsID id, int num, bool isUpdateTab = true);
	int getGoodsNumWithID(GoodsID id);
	
	void setOptionRoleId(int id);

	bool getNewUserState(void);
	void setNewUserState(bool state);

	// 初始签到信息
	void initSignInInfo( void );
	void getSignInReward(int day);
	SignInRewardInfo* getSignInRewardInfo(int day);

	// 获得小怪表
	csvEnemyPlanetype_s* getEnemyPlaneWithKey( int key = 1);
	// 获得子弹配置表
	csvBulletConfig_s* getBulletConfigWithId(int id);
	// 获得boss属性表
	csvBossProConfig_s* getBossProConfigWithId(int id);


	// 是否使用过卡牌
	bool getCardUseState();
	void setCardUseState(bool state);

	// 设置第一次进入升级界面
	bool getFirstEntryUpgradeState();
	void setFirstEntryUpgradeState(bool state);

	// 设置第一次进入card界面
	bool getFirstEntryCardState();
	void setFirstEntryCardState(bool state);

	// 关卡存储
	int  getCurMaxLevel(void);
	void setCurMaxLevel(int level);

	// 生命回复时间存储
	double  getHistoryLifeTime( void );
	void setHistoryLifeTime( double time );

	// 卷轴刷新时间存储
	double getScrollUpdateTime( void );
	void setScrollUpdateTime( double time );

	// 历史新高成绩存储
	int getHistoryHighResult( void );
	void setHistoryHighResult( int result );

	// vip领取日期
	int getVipGetDate(void);
	void setVipGetDate(int date);

	// vip剩余天
	int getVipDay(void);
	void setVipDay(int day);

	// 签到
	int getSignInDay(void);
	void setSignInDay(int day);

	// 土豪礼拜首充状态
	bool getTyrantPackBuyState(void);
	void setTyrantPackBuyState(bool state);

	// 关卡星级
	void initLevelStarToMap(void);
	int getLevelStarGrade(int lv);
	int getLevelStarGradeForMap(int lv);
	void setLevelStarGrade(int lv,int grade);
	void conversionLevelGrade(int lv, int time);
	int getCountStarNum(void);

	void setLevelFinishState(int lv,bool state);
	bool getLevelFinishState(int lv);

	// 转换字符串时间到数字
	double  convertTimeStrToNumber( std::string time ,SystemTime key = SystemTime_Normal);

	// 获得XML字符串
	std::string getXmlStringWithKey( const char* key );

	CC_SYNTHESIZE(int, m_curLevel, CurLevel);
	CC_SYNTHESIZE(bool, mGuideNewUser, GuideNewUser);
	CC_SYNTHESIZE(bool, mGuideHunDun, GuideHunDun);
	CC_SYNTHESIZE(bool, mGuideCard, GuideCard);
	CC_SYNTHESIZE(bool, mGuideDaZhao, GuideDaZhao);
	CC_SYNTHESIZE(bool, mIsGuideIng, GuideState);
	CC_SYNTHESIZE(bool, mIsFirstEntry, FirstEntryState);


public:

	GameTaskSystem*		mGameTaskSystem;
	GameBuySystem*		mGameBuySystem;
	GameRoleSystem*		mGameRoleSystem;
	GameCardSystem*		mGameCardSystem;
	GameGradeMsg*		mGameGradeMsg;

	EntryGameConfig_S mRoleConfig;

	UserData	mUserData;
private:
	ValueMap _xmlString;

	std::map<int, SignInRewardInfo >		_signInInfoMap;
	std::map<int, int >						_levelStarMap;

	sqlite3*	_sqlitedb;


};









#endif
