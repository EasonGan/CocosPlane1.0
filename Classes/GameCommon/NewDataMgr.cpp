#include "NewDataMgr.h"
#include "NewAudio.h"
#include "../JniHelp/CppCallJava.h"
#include "GameDefine.h"

//#ifdef WIN32
//#pragma comment(lib, "E:\\Cocos_Project\\cocos2d-x-3.3\\project\\PlaneGame2\\proj.win32\\sqlite3.lib")
//#endif

//SINGLETONDATA_DEFINE(NewDataMgr);

#define File_DataBase_Name		"weme.db"

using namespace std;

NewDataMgr::NewDataMgr()
	:_sqlitedb(nullptr)
	, mGameTaskSystem(nullptr)
	, mGameBuySystem(nullptr)
	, mGameRoleSystem(nullptr)
	, mGameCardSystem(nullptr)
	, mGameGradeMsg(nullptr)
	, mGuideNewUser(false)
	, mGuideHunDun(false)
	, mGuideCard(false)
	, mGuideDaZhao(false)
	, mIsGuideIng(false)
	, mIsFirstEntry(true)
{
	mRoleConfig.roleIndex	= 1;

	mUserData.Gold = 0;
}

NewDataMgr::~NewDataMgr()
{
	//sqlite3_close( _sqlitedb );
}

void NewDataMgr::initGameData( void )
{
	// 数据库
	std::string path= FileUtils::getInstance()->getWritablePath() + File_DataBase_Name;
	std::string sql;
	int result;

	if ( !_sqlitedb )
	{
		result = sqlite3_open(path.c_str(),&_sqlitedb);
#if 0
		char pwd[100] = "weme8888123456";
		char *zErrorMsg = 0;
		sqlite3_key(_sqlitedb, pwd, strlen(pwd));
		if (SQLITE_OK != sqlite3_exec(_sqlitedb, "select gold from userData_Table", NULL, NULL, &zErrorMsg))
		{
			sqlite3_rekey(_sqlitedb, pwd, strlen(pwd));
		}
#endif
		log("open sqlite Resoult[%d]", result);
	}
	if ( !mGameTaskSystem )
	{
		mGameTaskSystem = new GameTaskSystem();
		mGameTaskSystem->initGameTask( _sqlitedb );
	}

	if ( !mGameBuySystem )
	{
		mGameBuySystem = new GameBuySystem();
		mGameBuySystem->initBuyData( _sqlitedb );
	}

	if (!mGameRoleSystem)
	{
		mGameRoleSystem = new GameRoleSystem(_sqlitedb);
	}

	if (!mGameGradeMsg)
	{
		mGameGradeMsg = new GameGradeMsg(_sqlitedb);
	}

	if (!mGameCardSystem)
	{
		mGameCardSystem = new GameCardSystem(_sqlitedb);
	}
	initUserData( _sqlitedb );
	initSignInInfo();
	initLevelStarToMap();

	if ( !getNewUserState() )
	{
		setNewUserState(false);
		setGuideNewUser(false);
		setGuideDaZhao(false);
		setGuideCard(true);
	}


	// 音乐
#ifdef WIN32
	NewAudio::getInstance()->setMusicState( false );
	NewAudio::getInstance()->setSoundEffState( false );
#else
	NewAudio::getInstance()->setMusicState( true );
	NewAudio::getInstance()->setSoundEffState( true );
#endif
}

void NewDataMgr::initUserData( sqlite3* db )
{
	std::string sql;
	int result;
	char *zErrorMsg = 0;

	// 查询表字段
	sql = "select gold from userData_Table";
	result=sqlite3_exec(db,sql.c_str(),NULL,NULL,&zErrorMsg);

	// 表不存在
	if ( result != SQLITE_OK )
	{
		//	CCASSERT( result == SQLITE_OK, "roleinfo table not existes");
		sql="create table if not exists userData_Table("\
			"newUser integer"\
			",life integer"\
			",gold integer"\
			",gem integer"\
			",dazhao integer"\
			",hudun integer"\
			",optionRoleId integer"\
			")";

		result=sqlite3_exec(db,sql.c_str(),NULL,NULL,&zErrorMsg);

		if(result == SQLITE_OK)   
		{
			// 插入数据 用户基础数据
			sql = "insert into userData_Table values ( 0, 3, 20000, 0, 5, 5 ,1)";

			result=sqlite3_exec(db,sql.c_str(),NULL,NULL,NULL);

			if(result!=SQLITE_OK)
				CCLOG("insert err");
			
		}
	}

	// 查询语句
	sql = " select newUser ,life ,gold ,gem , dazhao, hudun, optionRoleId from userData_Table";
	sqlite3_stmt *statement;
	if ( sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr) == SQLITE_OK )
	{
		// 初始化角色列表
		while (sqlite3_step(statement) == SQLITE_ROW) 
		{
			mUserData.IsNewUser		= sqlite3_column_int(statement, 0);
			mUserData.Life			= sqlite3_column_int(statement, 1);
			mUserData.Gold			= sqlite3_column_int(statement, 2);
			mUserData.Gem			= sqlite3_column_int(statement, 3);
			mUserData.DaZhao		= sqlite3_column_int(statement, 4);
			mUserData.HuDun			= sqlite3_column_int(statement, 5);
			mRoleConfig.roleIndex	= sqlite3_column_int(statement, 6);
		}
	}

	sqlite3_finalize(statement);
}

std::string NewDataMgr::getXmlStringWithKey( const char* key )
{
#if 0
	CCLog("------NewRoleDataMgr::getXmlStringWithKey :%s", key);
	if ( _xmlString.empty() )
	{
		_xmlString = FileUtils::getInstance()->getValueMapFromFile("strings.xml");
	}
	CCLog("------NewRoleDataMgr::getXmlStringWithKeyEnd :%s", key);

	return _xmlString[key].asString();
#else

	auto xmlString = FileUtils::getInstance()->getValueMapFromFile("strings.xml");

	return xmlString[key].asString();

#endif
}


int NewDataMgr::getGoodsNumWithID(GoodsID id)
{
	switch (id)
	{
	case GoodsID_Gold:
		return mUserData.Gold;
		break;
	case GoodsID_Life:
		return mUserData.Life;
		break;
	case GoodsID_DaZhao:
		return mUserData.DaZhao;
		break;
	case GoodsID_HuDun:
		return mUserData.HuDun;
		break;
	default:
		break;
	}
	return 0;
}
 
void NewDataMgr::addGoodsNumWithID(GoodsID id, int num, bool isUpdateTab /*= true*/)
{
	switch (id)
	{
	case GoodsID_Gold:
		mUserData.Gold += num;
		break;
	case GoodsID_Life:
		mUserData.Life += num;
		break;
	case GoodsID_DaZhao:
		mUserData.DaZhao += num;
		break;
	case GoodsID_HuDun:
		mUserData.HuDun += num;
		break;
	default:
		break;
	}

	if ( isUpdateTab )
	{
		std::string sql;
		sql.append("update userData_Table set  ");
		sql.append(StringUtils::format("life = %d, gold = %d, dazhao = %d , hudun = %d", mUserData.Life, mUserData.Gold, mUserData.DaZhao, mUserData.HuDun));

		if (_sqlitedb)
		{
			int result;
			char* zErrorMsg = 0;

			result = sqlite3_exec(_sqlitedb, sql.c_str(), NULL, NULL, &zErrorMsg);
			if (result != SQLITE_OK)
				CCLOG("insert err");
		}
	}
}

void NewDataMgr::setOptionRoleId(int id)
{
	mRoleConfig.roleIndex = id;

	std::string sql;
	sql.append("update userData_Table set  ");
	sql.append(StringUtils::format("optionRoleId = %d", id));

	if (_sqlitedb)
	{
		int result;
		char* zErrorMsg = 0;

		result = sqlite3_exec(_sqlitedb, sql.c_str(), NULL, NULL, &zErrorMsg);
		if (result != SQLITE_OK)
			CCLOG("insert err");
	}
}

void NewDataMgr::setNewUserState(bool state)
{
	UserDefault::getInstance()->setBoolForKey("NewUserState", state);
}

bool NewDataMgr::getNewUserState(void)
{
	bool state = UserDefault::getInstance()->getBoolForKey("NewUserState", true);

	return state;
}

bool NewDataMgr::getCardUseState()
{
	bool state = UserDefault::getInstance()->getBoolForKey("CardUserState", false);

	return state;
}

void NewDataMgr::setCardUseState(bool state)
{
	UserDefault::getInstance()->setBoolForKey("CardUserState", state);
}

bool NewDataMgr::getFirstEntryUpgradeState()
{
	bool state = UserDefault::getInstance()->getBoolForKey("FirstEntryUpgradeState", false);

	return state;
}

void NewDataMgr::setFirstEntryUpgradeState(bool state)
{
	UserDefault::getInstance()->setBoolForKey("FirstEntryUpgradeState", state);
}

bool NewDataMgr::getFirstEntryCardState()
{
	bool state = UserDefault::getInstance()->getBoolForKey("FirstEntryCardState", false);

	return state;
}

void NewDataMgr::setFirstEntryCardState(bool state)
{
	UserDefault::getInstance()->setBoolForKey("FirstEntryCardState", state);

}

int NewDataMgr::getCurMaxLevel(void)
{
	int level = UserDefault::getInstance()->getIntegerForKey("getCurLevel", 1);

	return level;
}

void NewDataMgr::setCurMaxLevel(int level)
{
	level = (16 <= level) ? 16 : level;
	UserDefault::getInstance()->setIntegerForKey("getCurLevel", level);
}

int NewDataMgr::getVipGetDate(void)
{
	int date = UserDefault::getInstance()->getIntegerForKey("VipGetDate", 0);

	return date;
}

void NewDataMgr::setVipGetDate(int date)
{
	UserDefault::getInstance()->setIntegerForKey("VipGetDate", date);
}

int NewDataMgr::getVipDay(void)
{
	int day = UserDefault::getInstance()->getIntegerForKey("VipDay", 0);

	return day;
}

void NewDataMgr::setVipDay(int day)
{
	UserDefault::getInstance()->setIntegerForKey("VipDay", day);
}

int NewDataMgr::getSignInDay(void)
{
	int day = UserDefault::getInstance()->getIntegerForKey("SignInDay", 0);

	return day;
}

void NewDataMgr::setSignInDay(int day)
{
	UserDefault::getInstance()->setIntegerForKey("SignInDay", day);
}

double NewDataMgr::getHistoryLifeTime( void )
{
	double time = UserDefault::getInstance()->getDoubleForKey("getHistoryLifeTime", 0);

	return time;
}

void NewDataMgr::setHistoryLifeTime( double time )
{
	UserDefault::getInstance()->setDoubleForKey( "getHistoryLifeTime", time );
}

double NewDataMgr::getScrollUpdateTime( void )
{
	double time = UserDefault::getInstance()->getDoubleForKey("getScrollUpdateTime", 0);

	return time;
}

void NewDataMgr::setScrollUpdateTime( double time )
{
	UserDefault::getInstance()->setDoubleForKey( "getScrollUpdateTime", time );
}

int NewDataMgr::getHistoryHighResult( void )
{
	int data = UserDefault::getInstance()->getIntegerForKey("getHistoryHighResult", 0);

	return data;
}

void NewDataMgr::setHistoryHighResult( int result )
{
	UserDefault::getInstance()->setDoubleForKey( "getHistoryHighResult", result );

}

bool NewDataMgr::getTyrantPackBuyState(void)
{
	// 真为已购买
	bool state = UserDefault::getInstance()->getBoolForKey("TyrantPackBuyState", false);
	return state;
}

void NewDataMgr::setTyrantPackBuyState(bool state)
{
	UserDefault::getInstance()->setBoolForKey("TyrantPackBuyState", state);
}

void NewDataMgr::initLevelStarToMap(void)
{
	int curMaxLevel = getCurMaxLevel();

	for (int i = 1; i <= curMaxLevel; i++)
	{
		int grade = getLevelStarGrade(i);
		_levelStarMap.insert(pair<int, int>(i, grade));
	}
}

int NewDataMgr::getLevelStarGrade(int lv)
{
	std::string levelGrade = StringUtils::format("level_%d", lv);
	return UserDefault::getInstance()->getIntegerForKey(levelGrade.c_str(), 0);
}

int NewDataMgr::getLevelStarGradeForMap(int lv)
{
	map<int, int>::iterator	it = _levelStarMap.find(lv);
	if (it == _levelStarMap.end()){ return 0; }
	return it->second;
}

int NewDataMgr::getCountStarNum(void)
{
	int curMaxLevel = getCurMaxLevel();
	int starNum = 0;

	for (int i = 1; i <= 16; i++)
	{
		map<int, int>::iterator	it = _levelStarMap.find(i);
		if (it != _levelStarMap.end())
		{
			starNum += it->second;
		}
	}
	return starNum;
}

void NewDataMgr::setLevelStarGrade(int lv, int grade)
{
	std::string levelGrade = StringUtils::format("level_%d", lv);

	map<int, int>::iterator	it = _levelStarMap.find(lv);
	if (it == _levelStarMap.end())
	{
		_levelStarMap.insert(pair<int, int>(lv, grade));
		UserDefault::getInstance()->setIntegerForKey(levelGrade.c_str(), grade);
	}
	else
	{
		if ( it->second < grade )
		{
			it->second = grade;
			UserDefault::getInstance()->setIntegerForKey(levelGrade.c_str(), grade);
		}
	}

}

void NewDataMgr::setLevelFinishState(int lv, bool state)
{
	std::string levelStr = StringUtils::format("levelFinish_%d", lv);
	UserDefault::getInstance()->setBoolForKey(levelStr.c_str(), state);
}

bool NewDataMgr::getLevelFinishState(int lv)
{
	std::string levelStr = StringUtils::format("levelFinish_%d", lv);
	bool levelFinishState = UserDefault::getInstance()->getBoolForKey(levelStr.c_str(), false);

	return levelFinishState;
}

double NewDataMgr::convertTimeStrToNumber( std::string time, SystemTime key )
{
	char* token = (char*)"||";
	int nend=0;
	int nbegin=0;
	int  day, mon, year, hour, min, sec; 

	// 年
	nend = time.find(token, nbegin);
	if ( -1 != nend)
	{
		std::string temp = time.substr( nbegin, nend - nbegin);
		year = atoi( temp.c_str() );
		nbegin = nend + strlen(token);

		if ( SystemTime_Year == key ) return year;
	}

	// 月
	nend = time.find(token, nbegin);
	if ( -1 != nend)
	{
		mon = atoi( time.substr( nbegin, nend - nbegin).c_str() );
		nbegin = nend + strlen(token);

		if ( SystemTime_Mon == key ) return mon;
	}

	// 日
	nend = time.find(token, nbegin);
	if ( -1 != nend)
	{
		day = atoi( time.substr( nbegin, nend - nbegin).c_str() );
		nbegin = nend + strlen(token);

		if ( SystemTime_Day == key ) return day;
	}

	// 时
	nend = time.find(token, nbegin);
	if ( -1 != nend)
	{
		hour = atoi( time.substr( nbegin, nend - nbegin).c_str() );
		nbegin = nend + strlen(token);

		if ( SystemTime_Hour == key ) return hour;
	}

	// 分
	nend = time.find(token, nbegin);
	if ( -1 != nend)
	{
		min = atoi( time.substr( nbegin, nend - nbegin).c_str() );
		nbegin = nend + strlen(token);

		if ( SystemTime_Min == key ) return min;
	}

	// 秒
	nend = time.find(token, nbegin);
	if ( -1 != nend)
	{
		sec = atoi( time.substr( nbegin, nend - nbegin).c_str() );

		if ( SystemTime_Sec == key ) return sec;
	}

	CCLOG("convertTimeStrToNumber day:%d hour:%d min:%d sec:%d",day, hour, min, sec);
	return day*24  + hour*60 + min*60 + sec;
}

csvEnemyPlanetype_s* NewDataMgr::getEnemyPlaneWithKey( int key )
{
	return CSV_MGR()->GetCsvEnemyPlanetype()->GetByID( key );

}

csvBulletConfig_s* NewDataMgr::getBulletConfigWithId(int id)
{
	return CSV_MGR()->GetCsvBulletConfig()->GetByID(id);
}

csvBossProConfig_s* NewDataMgr::getBossProConfigWithId(int id)
{
	return CSV_MGR()->GetCsvBossProConfig()->GetByID(id);
}

void NewDataMgr::initSignInInfo(void)
{
	_signInInfoMap.insert(pair<int, SignInRewardInfo >(1, SignInRewardInfo(500, 0, 0, 0, false)));
	_signInInfoMap.insert(pair<int, SignInRewardInfo >(2, SignInRewardInfo(0, 0, 1, 1, false)));
	_signInInfoMap.insert(pair<int, SignInRewardInfo >(3, SignInRewardInfo(800, 0, 0, 0, false)));
	_signInInfoMap.insert(pair<int, SignInRewardInfo >(4, SignInRewardInfo(0, 1, 1, 1, false)));
	_signInInfoMap.insert(pair<int, SignInRewardInfo >(5, SignInRewardInfo(1000, 0, 0, 0, false)));
	_signInInfoMap.insert(pair<int, SignInRewardInfo >(6, SignInRewardInfo(2000, 0, 0, 0, false)));

	// 每六天后宝箱出品概率
	SignInRewardInfo other(0,0,0,0,false);
	{
		int probability = random(1, 100);

		if (2 >= probability)
		{
			int goods = random(1, 3);

			if (1 == goods)
			{
				other.dazhao = 2;
			}
			else if (2 == goods)
			{
				other.hudun = 2;
			}
			else if (3 == goods)
			{
				other.life = 2;
			}
		}
		else if (5 >= probability)
		{
			other.gold = 2000;
		}
		else if (8 >= probability)
		{
			int goods = random(1, 2);

			if (1 == goods)
			{
				other.gold = 1000;
			}
			else if (2 == goods)
			{
				other.life = 1;
			}
		}
		else if (12 >= probability)
		{
			other.gold = 800;
		}
		else if (18 >= probability)
		{
			int goods = random(1, 2);

			if (1 == goods)
			{
				other.dazhao = 1;
			}
			else if (2 == goods)
			{
				other.hudun = 1;
			}
		}
		else
		{
			other.gold = 500;
		}
	}

	_signInInfoMap.insert(pair<int, SignInRewardInfo >(7, other));

}

SignInRewardInfo* NewDataMgr::getSignInRewardInfo(int day)
{
	day = (7 <= day) ? 7 : day;
	map<int, SignInRewardInfo>::iterator	it = _signInInfoMap.find(day);
	if (it == _signInInfoMap.end()){ return nullptr; }
	return &it->second;
}

void NewDataMgr::getSignInReward(int day)
{
	auto goodsInfo = getSignInRewardInfo(day + 1);

	if (goodsInfo->gold)	addGoodsNumWithID(GoodsID_Gold, goodsInfo->gold);
	if (goodsInfo->dazhao)	addGoodsNumWithID(GoodsID_DaZhao, goodsInfo->dazhao);
	if (goodsInfo->hudun)	addGoodsNumWithID(GoodsID_HuDun, goodsInfo->hudun);
	if (goodsInfo->life)	addGoodsNumWithID(GoodsID_Life, goodsInfo->life);

	CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_UpdateGold);
	setSignInDay(day + 1);
}

void NewDataMgr::conversionLevelGrade(int lv, int time)
{
	if (0 == lv)
	{
		NewDataMgr::getInstance()->setLevelStarGrade(0, 3);
	}
	else
	{
		int grade[3] =
		{
			NewDataMgr::getInstance()->getBossProConfigWithId(lv)->grade1,
			NewDataMgr::getInstance()->getBossProConfigWithId(lv)->grade2,
			NewDataMgr::getInstance()->getBossProConfigWithId(lv)->grade3
		};

		int lvgrade = 0;
		if (0 < time && time < grade[1])
		{
			lvgrade = 3;
		}
		else if (grade[1] <= time && time < grade[0])
		{
			lvgrade = 2;
		}
		else
		{
			lvgrade = 1;
		}

		NewDataMgr::getInstance()->setLevelStarGrade(lv, lvgrade);
	}
	
}
