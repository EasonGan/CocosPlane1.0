#include "GameSystem.h"
#include "../CsvMgr/CsvMgr.h"
#include "../JniHelp/CppCallJava.h"
#include "NewDataMgr.h"
#include "../GameUI/CommonWidget.h"
#include "GameDefine.h"
#include "../GameUI/GameRoleScene.h"
#include "../GameUI/ItemRewardTips.h"
#include "../GameUtil/StatisticsHelp.h"

using namespace std;

const char* goodsPayCode[][2] =
{
	{ "30000923795301", "VIP " },	// 购买VIP
	{ "30000923795302", "超值礼包 " },	// 购买土豪礼包
	{ "30000923795303", "土豪礼包 " },	// 购买土豪礼包
	{ "30000923795304", "金币 " },	// 购买金币
	{ "30000923795305", "护盾 " },	// 购买护盾
	{ "30000923795306", "必杀 " },	// 购买必杀
	{ "30000923795307", "复活 " },	// 购买复活
};

void CProtocolManager::sendNotificationMsg(const std::string msg, EProtocolID id)
{
	NotificationCenter::getInstance()->postNotification(msg, (Ref*)id);
}

GameTaskSystem::GameTaskSystem()
	:_sqlitedb(nullptr)
{

}

GameTaskSystem::~GameTaskSystem()
{
	saveGamePropertyData();
}

void GameTaskSystem::initGameProperty( void )
{
	std::string sql;
	int result;
	char *zErrorMsg = 0;

	CCASSERT(_sqlitedb, "sqlitedb==null");

	// 查询表字段
	sql = "select data0 from gamePro_Table";
	result=sqlite3_exec(_sqlitedb,sql.c_str(),NULL,NULL,&zErrorMsg);

	// 表不存在
	if ( result != SQLITE_OK )
	{
		//	CCASSERT( result == SQLITE_OK, "roleinfo table not existes");
		sql.clear();
		sql="create table if not exists gamePro_Table(";

		for ( int i=GamePrKey_killMonNum; i<GamePrKey_Max; i++)
		{
			sql.append( "data" );
			sql.append(  __String::createWithFormat("%d", i)->getCString() );
			sql.append( " integer");

			if ( i != GamePrKey_Max-1)
			{
				sql.append( ",");
			}

		}
		sql.append( ")");

		result=sqlite3_exec(_sqlitedb,sql.c_str(),NULL,NULL,&zErrorMsg);

		if(result == SQLITE_OK)   
		{
			// 插入数据 
			sql.clear();
			sql = sql.append( "insert into gamePro_Table values (" );
			for ( int i=GamePrKey_killMonNum; i<GamePrKey_Max; i++)
			{
				sql.append( "0" );

				if ( i != GamePrKey_Max-1)
				{
					sql.append( ", ");
				}

			}
			sql.append( ")");				

			result=sqlite3_exec(_sqlitedb,sql.c_str(),NULL,NULL,&zErrorMsg);

			if(result!=SQLITE_OK)
				CCLOG("insert err");

		}
	}

	// 插入数据 
	sql.clear();
	sql = sql.append( "select " );
	for ( int i=GamePrKey_killMonNum; i<GamePrKey_Max; i++)
	{
		sql.append( "data" );
		sql.append(  __String::createWithFormat("%d", i)->getCString() );

		if ( i != GamePrKey_Max-1)
		{
			sql.append( ",");
		}

	}
	sql.append( " from gamePro_Table");		

	sqlite3_stmt *statement;
	if ( sqlite3_prepare_v2(_sqlitedb, sql.c_str(), -1, &statement, nullptr) == SQLITE_OK )
	{
		// 初始化列表
		while (sqlite3_step(statement) == SQLITE_ROW) 
		{		
			for ( int i=GamePrKey_killMonNum; i<GamePrKey_Max; i++)
			{
				_GamePropertyArr[i]			= sqlite3_column_int(statement, i);
			}
		}
	}
	sqlite3_finalize(statement);

}


void GameTaskSystem::initGameTask( sqlite3* db )
{
	_sqlitedb = db;
	
	// 存放进度相关
	initGameProperty();
	// 存放日任务表
	initDayTaskTable();
	// 存放任务状态
	initTaskState();
	

	int today = NewDataMgr::getInstance()->convertTimeStrToNumber(CppCallJava::getInstance()->getSystemTime(), SystemTime_Day);
	int history = getTaskDate();

	log("initGameTask	today[%d] history[%d]", today, history);
	// 当前天日期与历史日期对比
 	if ( today != history || _dayTaskList.empty())
	{
		setTaskDate( today );

		// 重新生成任务表
		cleanTaskProperty();
		createDayTaskList();
		saveDayTaskData();
	}

	createAchiTaskList();
}

void GameTaskSystem::initDayTaskTable( void )
{
	std::string sql;
	int result;
	char *zErrorMsg = 0;

	CCASSERT(_sqlitedb, "sqlitedb==null");
	
	// 查询表字段
	sql = "select taskid from dayTask_Table";
	result=sqlite3_exec(_sqlitedb,sql.c_str(),NULL,NULL,&zErrorMsg);

	// 表不存在
	if ( result != SQLITE_OK )
	{
		//	CCASSERT( result == SQLITE_OK, "roleinfo table not existes");
		sql.clear();
		sql="create table if not exists dayTask_Table("\
			"taskid integer"\
			",state integer"\
			")";


		result=sqlite3_exec(_sqlitedb,sql.c_str(),NULL,NULL,&zErrorMsg);

		if(result == SQLITE_OK)   
		{
						
		}
	}

	// 查询 
	sql.clear();
	sql = sql.append( "select taskid ,state from dayTask_Table");		

	sqlite3_stmt *statement;
	if ( sqlite3_prepare_v2(_sqlitedb, sql.c_str(), -1, &statement, nullptr) == SQLITE_OK )
	{
		// 初始化列表
		while (sqlite3_step(statement) == SQLITE_ROW) 
		{		
			int taskid = sqlite3_column_int(statement, 0);
			int taskState = sqlite3_column_int(statement, 1);
			_dayTaskList.push_back( taskid );

			auto taskItem = NewDataMgr::getInstance()->mGameTaskSystem->getGameTaskListWithID(taskid);
			taskItem->TaskState = taskState;
		}
	}
	sqlite3_finalize(statement);

}

void GameTaskSystem::initTaskState(void)
{
	std::string sql;
	int result;
	char *zErrorMsg = 0;

	CCASSERT(_sqlitedb, "sqlitedb==null");

	// 查询表字段
	sql = "select data0 from TaskState_Table";
	result = sqlite3_exec(_sqlitedb, sql.c_str(), NULL, NULL, &zErrorMsg);

	// 表不存在
	if (result != SQLITE_OK)
	{
		sql.clear();
		sql = "create table if not exists TaskState_Table("\
			"taskid integer"\
			",state integer"\
			")";


		result = sqlite3_exec(_sqlitedb, sql.c_str(), NULL, NULL, &zErrorMsg);

		if (result == SQLITE_OK)
		{
			// 插入数据 

// 			for (int i = DayTaskTag_Task10; i < DayTaskTag_AchiTaskEnd; i++)
// 			{
// 				sql.clear();
// 				sql.append(StringUtils::format("insert into TaskState_Table values (%d,%d)", i, getGameTaskListWithID(i)->TaskState));
// 
// 				result = sqlite3_exec(_sqlitedb, sql.c_str(), NULL, NULL, &zErrorMsg);
// 				if (result != SQLITE_OK)
// 					CCLOG("insert err");
// 			}
		}
	}

	// 查询 
	sql.clear();
	sql = sql.append("select  taskid, state from TaskState_Table");

	sqlite3_stmt *statement;
	if (sqlite3_prepare_v2(_sqlitedb, sql.c_str(), -1, &statement, nullptr) == SQLITE_OK)
	{
		// 初始化列表
		while (sqlite3_step(statement) == SQLITE_ROW)
		{
			int taskid = sqlite3_column_int(statement, 0);
			int taskState = sqlite3_column_int(statement, 1);

			auto taskItem = NewDataMgr::getInstance()->mGameTaskSystem->getGameTaskListWithID(taskid);
			taskItem->TaskState = taskState;

		}
	}
	sqlite3_finalize(statement);
}

void GameTaskSystem::setTaskStateToTable(int taskid)
{
	std::string sql;
	sql.append(StringUtils::format("insert into TaskState_Table values (%d,%d)", taskid, getGameTaskListWithID(taskid)->TaskState));

	if (_sqlitedb)
	{
		int result;
		char* zErrorMsg = 0;

		result = sqlite3_exec(_sqlitedb, sql.c_str(), NULL, NULL, &zErrorMsg);
		if (result != SQLITE_OK)
			CCLOG("insert err");
	}
}

void GameTaskSystem::delTaskStateToTable(int taskid)
{
	std::string sql;
	sql.append(StringUtils::format("delete from TaskState_Table where taskid=%d", taskid));

	if (_sqlitedb)
	{
		int result;
		char* zErrorMsg = 0;

		result = sqlite3_exec(_sqlitedb, sql.c_str(), NULL, NULL, &zErrorMsg);
		if (result != SQLITE_OK)
			CCLOG("insert err");
	}
}

void GameTaskSystem::saveGamePropertyData( void )
{
	std::string sql;
	int result;
	char *zErrorMsg = 0;

	CCASSERT(_sqlitedb, "sqlitedb==null");

	// 清空表
	sql = "delete from gamePro_Table";
	result=sqlite3_exec(_sqlitedb,sql.c_str(),NULL,NULL,&zErrorMsg);


	// 插入数据 
	sql.clear();
	sql = sql.append( "insert into gamePro_Table values (" );
	for ( int i=GamePrKey_killMonNum; i<GamePrKey_Max; i++)
	{
		sql.append( __String::createWithFormat("%d", _GamePropertyArr[i])->getCString() );

		if ( i != GamePrKey_Max-1)
		{
			sql.append( ", ");
		}

	}
	sql.append( ")");				

	result=sqlite3_exec(_sqlitedb,sql.c_str(),NULL,NULL,&zErrorMsg);

	if(result!=SQLITE_OK)
		CCLOG("insert err");

}

void GameTaskSystem::saveDayTaskData( void )
{
	std::string sql;
	int result;
	char *zErrorMsg = 0;

	CCASSERT(_sqlitedb, "sqlitedb==null");

	// 清空表
	sql = "delete from dayTask_Table";
	result=sqlite3_exec(_sqlitedb,sql.c_str(),NULL,NULL,&zErrorMsg);

	auto item = _dayTaskList.begin();
	// 插入数据 
	sql.clear();
	sql = sql.append( "insert into dayTask_Table values " );
	for ( int i=0; i<_dayTaskList.size(); i++)
	{
		int taskid = (*item);item++;
		int taskIsFinish = getGameTaskListWithID( taskid )->TaskState;
		sql.append( __String::createWithFormat("(%d, %d)",taskid, taskIsFinish)->getCString() );

		if ( i != _dayTaskList.size()-1)
		{
			sql.append( ", ");
		}

	}

	result=sqlite3_exec(_sqlitedb,sql.c_str(),NULL,NULL,&zErrorMsg);

	if(result!=SQLITE_OK)
		CCLOG("insert err");
}

void GameTaskSystem::createDayTaskList()
{
	_dayTaskList.clear();
	// 任务随机
	for ( int i = 1; i<=TaskSystem_DayTaskNum; i++ )
	{
		// 随机ID
		int randID = random((int)DayTaskTag_Task1, (int)DayTaskTag_Task9);

		while(1)
		{
			if(std::find(_dayTaskList.begin(), _dayTaskList.end(), randID) != _dayTaskList.end())
			{
				randID = random((int)DayTaskTag_Task1, (int)DayTaskTag_Task9);
			}
			else
			{
				delTaskStateToTable(randID);
				_dayTaskList.push_back( randID );
				break;
			}
		}
	}
}

void GameTaskSystem::saveAchiTaskData(void)
{
	
}

void GameTaskSystem::createAchiTaskList()
{
	_achiTaskList.clear();
	// 任务
	for (int i = DayTaskTag_Task10; i < DayTaskTag_AchiTaskEnd; i++)
	{
		_achiTaskList.push_back(i);
	}
}

std::vector<int> GameTaskSystem::getDayTaskList( void )
{
	updateDayTaskData();
	return _dayTaskList;
}

std::vector<int> GameTaskSystem::getAchiTaskList(void)
{
	updateAchiTaskData();
	return _achiTaskList;
}

csvGameTaskConfig_s* GameTaskSystem::getGameTaskListWithID( int id )
{
	return CSV_MGR()->GetCsvGameTaskConfig()->GetByID( id );
}

void GameTaskSystem::addTaskPropertyWithKey(GamePropertyEnum key, int num)
{
	_GamePropertyArr[key] += num;
}

void GameTaskSystem::setGameTaskProperty( int *data )
{
	_GamePropertyArr[GamePrKey_killMonNum]			+= data[GamePrKey_killMonNum]		;
	_GamePropertyArr[GamePrKey_BGradeNum]			+= data[GamePrKey_BGradeNum]		;
	_GamePropertyArr[GamePrKey_AGradeNum]			+= data[GamePrKey_AGradeNum]		;			
	_GamePropertyArr[GamePrKey_SGradeNum]			+= data[GamePrKey_SGradeNum]		;			
	_GamePropertyArr[GamePrKey_SSGradeNum]			+= data[GamePrKey_SSGradeNum]		;		
	_GamePropertyArr[GamePrKey_getRedBallNum]		+= data[GamePrKey_getRedBallNum]	;		
	_GamePropertyArr[GamePrKey_getBlueBallNum]		+= data[GamePrKey_getBlueBallNum]	;	
	_GamePropertyArr[GamePrKey_getYellowBallNum]	+= data[GamePrKey_getYellowBallNum]	;	
	_GamePropertyArr[GamePrKey_RedKillerNum]		+= data[GamePrKey_RedKillerNum]		;	
	_GamePropertyArr[GamePrKey_BlueKillerNum]		+= data[GamePrKey_BlueKillerNum]	;	
	_GamePropertyArr[GamePrKey_YellowKillerNum]		+= data[GamePrKey_YellowKillerNum]	;	
	_GamePropertyArr[GamePrKey_minKillBossTime]		= data[GamePrKey_minKillBossTime]	;	
	_GamePropertyArr[GamePrKey_RedScore]			+= data[GamePrKey_RedScore]			;
	_GamePropertyArr[GamePrKey_BlueScore]			+= data[GamePrKey_BlueScore]		;
	_GamePropertyArr[GamePrKey_YellowScore]			+= data[GamePrKey_YellowScore]		;

	_GamePropertyArr[GamePrKey_totalKm]				= (_GamePropertyArr[GamePrKey_totalKm] > data[GamePrKey_totalKm])?_GamePropertyArr[GamePrKey_totalKm] : data[GamePrKey_totalKm];

	saveGamePropertyData();
}

void GameTaskSystem::cleanTaskProperty( void )
{
	for ( int i = GamePrKey_killMonNum; i<GamePrKey_Max; i++)
	{
		_GamePropertyArr[i] = 0;
	}
}

void GameTaskSystem::updateAchiTaskData(void)
{
	for (int i = DayTaskTag_Task10; i < DayTaskTag_AchiTaskEnd; i++)
	{
		auto tempTask = CSV_MGR()->GetCsvGameTaskConfig()->GetByID(i);

		switch (i)
		{
			// 通过关卡任务
		case GameTaskSystem::DayTaskTag_Task10:
		case GameTaskSystem::DayTaskTag_Task11:
		case GameTaskSystem::DayTaskTag_Task12:
		case GameTaskSystem::DayTaskTag_Task13:
		case GameTaskSystem::DayTaskTag_Task14:
		case GameTaskSystem::DayTaskTag_Task15:
		case GameTaskSystem::DayTaskTag_Task16:
		case GameTaskSystem::DayTaskTag_Task17:
			tempTask->TaskProgressNum = NewDataMgr::getInstance()->getCurMaxLevel()-1;
			break;

			// 死亡次数
		case GameTaskSystem::DayTaskTag_Task18:
		case GameTaskSystem::DayTaskTag_Task19:
		case GameTaskSystem::DayTaskTag_Task20:
			tempTask->TaskProgressNum = _GamePropertyArr[GamePrKey_DeadNum];
			break;

		case GameTaskSystem::DayTaskTag_Task21:
		{
			if (NewDataMgr::getInstance()->mGameRoleSystem->getRoleConfigWithID(1)->isLock &&
				NewDataMgr::getInstance()->mGameRoleSystem->getRoleConfigWithID(2)->isLock &&
				NewDataMgr::getInstance()->mGameRoleSystem->getRoleConfigWithID(3)->isLock
				)
			{
				tempTask->TaskProgressNum = 1;
			}
		}
			break;

			// 击杀敌方数
		case GameTaskSystem::DayTaskTag_Task22:
		case GameTaskSystem::DayTaskTag_Task23:
		case GameTaskSystem::DayTaskTag_Task24:
			tempTask->TaskProgressNum = _GamePropertyArr[GamePrKey_killMonNum];
			break;

		case GameTaskSystem::DayTaskTag_Task25:
		{
			auto grade = NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(GameGradeMsg::ObjectType_Role);

			if (grade->curGrade >= grade->maxGrade)
			{
				tempTask->TaskProgressNum = 1;
			}
		}
			break;

		case GameTaskSystem::DayTaskTag_Task26:
		{
			auto grade = NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(GameGradeMsg::ObjectType_DaZhao);

			if (grade->curGrade >= grade->maxGrade)
			{
				tempTask->TaskProgressNum = 1;
			}
		}
			break;

		case GameTaskSystem::DayTaskTag_Task27:
		{
			auto grade = NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(GameGradeMsg::ObjectType_Gold);

			if (grade->curGrade >= grade->maxGrade)
			{
				tempTask->TaskProgressNum = 1;
			}
		}
			break;

		case GameTaskSystem::DayTaskTag_Task28:
		{
			auto grade = NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(GameGradeMsg::ObjectType_RedBall);

			if (grade->curGrade >= grade->maxGrade)
			{
				tempTask->TaskProgressNum = 1;
			}
		}
			break;

		case GameTaskSystem::DayTaskTag_Task29:
		{
			auto grade = NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(GameGradeMsg::ObjectType_BlueBall);

			if (grade->curGrade >= grade->maxGrade)
			{
				tempTask->TaskProgressNum = 1;
			}
		}
			break;

		case GameTaskSystem::DayTaskTag_Task30:
		{
			auto grade = NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(GameGradeMsg::ObjectType_YellowBall);

			if (grade->curGrade >= grade->maxGrade)
			{
				tempTask->TaskProgressNum = 1;
			}
		}
			break;

		case GameTaskSystem::DayTaskTag_Task31:
		case GameTaskSystem::DayTaskTag_Task32:
		case GameTaskSystem::DayTaskTag_Task33:
		case GameTaskSystem::DayTaskTag_Task34:
			tempTask->TaskProgressNum = NewDataMgr::getInstance()->getCountStarNum();
			break;

			//////////////DayTaskTag_AchiTaskEnd:

		default:
			break;
		}

		// 任务完成度判断
		if (tempTask->TaskProgressNum >= tempTask->TaskTargetNum)
		{
			if (0 == tempTask->TaskState)
				tempTask->TaskState = 2;
		}
		else
		{
			tempTask->TaskState = 0;
		}
	}
}

void GameTaskSystem::updateDayTaskData( void )
{
	for (std::vector<int>::iterator item = _dayTaskList.begin(); item != _dayTaskList.end(); item++)
	{
		int taskId = (*item);
		auto tempTask = CSV_MGR()->GetCsvGameTaskConfig()->GetByID(taskId);

		switch (taskId)
		{
		case GameTaskSystem::DayTaskTag_Task1:
			tempTask->TaskProgressNum = _GamePropertyArr[GamePrKey_BuyGoldTime];
			break;

		case GameTaskSystem::DayTaskTag_Task2:
			tempTask->TaskProgressNum = _GamePropertyArr[GamePrKey_UseDaZhao];
			break;

		case GameTaskSystem::DayTaskTag_Task3:
			tempTask->TaskProgressNum = _GamePropertyArr[GamePrKey_UseHuDun];
			break;

		case GameTaskSystem::DayTaskTag_Task4:
			tempTask->TaskProgressNum = _GamePropertyArr[GamePrKey_UpgradeObject];
			break;

		case GameTaskSystem::DayTaskTag_Task5:
			tempTask->TaskProgressNum = _GamePropertyArr[GamePrKey_getBlueBallNum];
			break;

		case GameTaskSystem::DayTaskTag_Task6:
			tempTask->TaskProgressNum = _GamePropertyArr[GamePrKey_getRedBallNum];
			break;

		case GameTaskSystem::DayTaskTag_Task7:
			tempTask->TaskProgressNum = _GamePropertyArr[GamePrKey_getYellowBallNum];
			break;

		case GameTaskSystem::DayTaskTag_Task8:
			tempTask->TaskProgressNum = _GamePropertyArr[GamePrKey_FinishGameNum];
			break;

		case GameTaskSystem::DayTaskTag_Task9:
			tempTask->TaskProgressNum = _GamePropertyArr[GamePrKey_killMonNum];
			break;

			///////////// DayTaskTag_DayTaskEnd:

		default:
			break;
		}

		// 任务完成度判断
		if (tempTask->TaskProgressNum >= tempTask->TaskTargetNum)
		{
			if (0 == tempTask->TaskState)
				tempTask->TaskState = 2;
		}
		else
		{
			tempTask->TaskState = 0;
		}
	}
}

int GameTaskSystem::getTaskDate( void )
{
	return UserDefault::getInstance()->getIntegerForKey("TaskDate", 0);
}

void GameTaskSystem::setTaskDate( int date )
{
	UserDefault::getInstance()->setIntegerForKey( "TaskDate", date );
}

int GameTaskSystem::getSignInDate(void)
{
	return UserDefault::getInstance()->getIntegerForKey("SignInDate", 0);
}

void GameTaskSystem::setSignInDate(int date)
{
	UserDefault::getInstance()->setIntegerForKey("SignInDate", date);
}

void GameTaskSystem::saveGameAboutData( void )
{
	log("GameTaskSystem saveGameAboutData");
	saveGamePropertyData();
	saveDayTaskData();
}

void GameTaskSystem::getTaskRewardWithTaskId(int id)
{
	auto goodsInfo = getGameTaskListWithID(id);

	switch (goodsInfo->TaskRewardID)
	{
	case GoodsID_Gold:
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Gold, goodsInfo->RewardNum);
		break;
	case GoodsID_Life:
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Life, goodsInfo->RewardNum);
		break;
	case GoodsID_DaZhao:
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_DaZhao, goodsInfo->RewardNum);
		break;
	case GoodsID_HuDun:
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_HuDun, goodsInfo->RewardNum);
		break;
	default:
		break;
	}

	// 设置为已领取状态
	goodsInfo->TaskState = 1;
	setTaskStateToTable(id);

	// 通知界面更新
	CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_UpdateGold);
	CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_UpdateTaskNum);
	CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskLayer, EProtocolID_GetTaskReward);

}

int GameTaskSystem::getDayTaskFinishNum(void)
{
	int finishTaskNum = 0;

	// 获得日任务表
	std::vector<int> tempTaskList = getDayTaskList();
	std::vector<int>::iterator item = tempTaskList.begin();
	for (; item != tempTaskList.end(); item++)
	{
		auto taskItem = getGameTaskListWithID((*item));
		if (2 == taskItem->TaskState) finishTaskNum += 1;
	}

	return finishTaskNum;
}

int GameTaskSystem::getAchiTaskFinishNum(void)
{
	int finishTaskNum = 0;

	std::vector<int> tempTask2List = getAchiTaskList();
	std::vector<int>::iterator item2 = tempTask2List.begin();
	for (; item2 != tempTask2List.end(); item2++)
	{
		auto taskItem = getGameTaskListWithID((*item2));
		if (2 == taskItem->TaskState) finishTaskNum += 1;
	}

	return finishTaskNum;
}

/*******************************************
* 物品购物系统
*******************************************/

GameBuySystem::GameBuySystem()
	:m_NotionState(false)
{
	CCDirector::getInstance()->getScheduler()->scheduleSelector(
		schedule_selector(GameBuySystem::updateNotion),
		this,
		0.08f,
		CC_REPEAT_FOREVER,
		0.0f,
		false);
}

GameBuySystem::~GameBuySystem()
{

}

void GameBuySystem::updateNotion(float t)
{
	if ( getNotionState())
	{
		setNotionState(false);
		sendBuySuccessMsg();
	}
}

void GameBuySystem::initBuyData( sqlite3* db )
{
	_sqlitedb = db;

	// 添加礼包物品内容
	_packInfoMap.insert(pair<int, PackGoodsInfo >(BuyKey_NewPack,		PackGoodsInfo(2888, 0, 1, 1)));
	_packInfoMap.insert(pair<int, PackGoodsInfo >(BuyKey_OverflowPack,	PackGoodsInfo(50000, 2, 6, 6)));
	_packInfoMap.insert(pair<int, PackGoodsInfo >(BuyKey_TyrantPack,	PackGoodsInfo(150000, 2, 12, 12)));
	_packInfoMap.insert(pair<int, PackGoodsInfo >(BuyKey_BuyPropPack,	PackGoodsInfo(0, 0, 3, 3)));

	// Vip首充获取
	_packInfoMap.insert(pair<int, PackGoodsInfo >(BuyKey_BuyVIP,		PackGoodsInfo(20000, 0, 3, 3)));
	
	// Vip领取物品
	_packInfoMap.insert(pair<int, PackGoodsInfo >(BuyKey_BuyVIPPre,		PackGoodsInfo(10000, 0, 1, 1)));

}

void GameBuySystem::initGoodsData( sqlite3* db )
{

	std::string sql;
	int result;
	char *zErrorMsg = 0;

	CCASSERT(db, "sqlitedb==null");

	// 查询表字段
	sql = "select data1 from initGoods_Table";
	result=sqlite3_exec(db,sql.c_str(),NULL,NULL,&zErrorMsg);

	// 表不存在
	if ( result != SQLITE_OK )
	{
		//	CCASSERT( result == SQLITE_OK, "roleinfo table not existes");

		// 建表
		sql.clear();
		sql="create table if not exists initGoods_Table(";

// 		for ( int i=BuyKey_Dazhao; i<BuyKey_Max; i++)
// 		{
// 			sql.append( "data" );
// 			sql.append(  __String::createWithFormat("%d", i)->getCString() );
// 			sql.append( " integer");
// 
// 			if ( i != BuyKey_Max-1)
// 			{
// 				sql.append( ",");
// 			}
// 
// 		}
		sql.append( ")");

		result=sqlite3_exec(db,sql.c_str(),NULL,NULL,&zErrorMsg);

		if(result == SQLITE_OK)   
		{
			// 插入数据 
			sql.clear();
			sql = sql.append( "insert into initGoods_Table values (" );
// 			for ( int i=BuyKey_Dazhao; i<BuyKey_Max; i++)
// 			{
// 				sql.append( "0" );
// 
// 				if ( i != BuyKey_Max-1)
// 				{
// 					sql.append( ", ");
// 				}
// 
// 			}
			sql.append( ")");				

			result=sqlite3_exec(db,sql.c_str(),NULL,NULL,&zErrorMsg);

			if(result!=SQLITE_OK)
				CCLOG("insert err");

		}		
	}


	// 查询数据 
	sql.clear();
	sql = sql.append( "select " );
// 	for ( int i=BuyKey_Dazhao; i<BuyKey_Max; i++)
// 	{
// 		sql.append( "data" );
// 		sql.append(  __String::createWithFormat("%d", i)->getCString() );
// 
// 		if ( i != BuyKey_Max-1)
// 		{
// 			sql.append( ",");
// 		}
// 
// 	}
	sql.append( " from initGoods_Table");		

	sqlite3_stmt *statement;
	if ( sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr) == SQLITE_OK )
	{
		// 初始化列表
		while (sqlite3_step(statement) == SQLITE_ROW) 
		{		
// 			for ( int i=BuyKey_Dazhao; i<BuyKey_Max; i++)
// 			{
// 				int num			= sqlite3_column_int(statement, i-1);
// 				getBuyGoodsDataWithKey((BuyListKey )i)->number = num;
// 			}
		}
	}
	sqlite3_finalize(statement);
}

void GameBuySystem::addGoodsNumWithKey( int key, int num )
{
	CSV_MGR()->GetCsvBuyList()->GetByID( key )->number += num;
	updateGoodsNumConfig( key );
}

void GameBuySystem::updateGoodsNumConfig( int key )
{
	std::string sql;
	sql.append("update initGoods_Table set  ");

	sql.append("data");
	sql.append(__String::createWithFormat("%d= %d ",key, CSV_MGR()->GetCsvBuyList()->GetByID( (int)key )->number)->getCString());

	if ( _sqlitedb )
	{
		int result;
		char* zErrorMsg = 0;

		result=sqlite3_exec(_sqlitedb,sql.c_str(),NULL,NULL,&zErrorMsg);
		if(result!=SQLITE_OK)
			CCLOG("insert err");
	}
}

csvBuyList_s* GameBuySystem::getBuyGoodsDataWithID(int id)
{
	return CSV_MGR()->GetCsvBuyList()->GetByID(id);
}

bool GameBuySystem::buyGoodsWithKey(int key, int num /*= 1*/, bool isFree /*= false*/)
{
	_curBuykey = key;
	setFreeState(isFree);

	int myGold = NewDataMgr::getInstance()->getGoodsNumWithID(GoodsID_Gold);

	auto item = getBuyGoodsDataWithID(key);
	if ( BuyType_Gold == item->buyType )
	{
		int price = item->Gold*num;

		if (isFree || price <= myGold)
		{
			sendBuySuccessMsg();
		}
		else
		{
			showBuyTips(BuyType_Gold, price);
			return false;

		}
	}
	else if ( BuyType_RMB == item->buyType )
	{
		CppCallJava::getInstance()->JniCallBuyMethod(getGoodsPayCodeWithKey(key), getBuyGoodsDataWithID(_curBuykey)->Price);
		//showBuyTips(BuyType_RMB);
		//addGoodsNumWithKey(key, num);
		return true;
	}
	else if (BuyType_Free == item->buyType)
	{
		sendBuySuccessMsg();
	}
	return true;
}

std::string GameBuySystem::getGoodsPayCodeWithKey(int key)
{
	std::string payCode;
	switch (key)
	{	
	case BuyKey_BuyVIP:
		return goodsPayCode[0][0];
	case BuyKey_OverflowPack:
		return goodsPayCode[1][0];
	case BuyKey_TyrantPack:
		return goodsPayCode[2][0];
	case BuyKey_BuyGold:
		return goodsPayCode[3][0];
	case BuyKey_BuyShield:
		return goodsPayCode[4][0];
	case BuyKey_BuyDaZhao:
		return goodsPayCode[5][0];
	case BuyKey_BuyFuHuo:
		return goodsPayCode[6][0];
	default:
		return goodsPayCode[0][0];
	}
}

bool GameBuySystem::buyGoodsWithPrice(int goodsprice)
{
	int myGold = NewDataMgr::getInstance()->getGoodsNumWithID(GoodsID_Gold);

	int buyType = BuyType_Gold;
	int price = goodsprice;

	if (BuyType_Gold == buyType)
	{
		if (price > myGold)
		{
			showBuyTips(BuyType_Gold, price);
			return false;
		}
		else
		{
			NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Gold, -price);
		}
	}
	else if (BuyType_RMB == buyType)
	{	
		showBuyTips(BuyType_RMB, price);
	}
	else if ( BuyType_Free == buyType)
	{
		sendBuySuccessMsg();
	}


	return true;
}

void GameBuySystem::showBuyTips(BuyType type, int needGold)
{
	if ( BuyType_RMB == type )
	{ 
		CppCallJava::getInstance()->JniCallBuyMethod("ddd", getBuyGoodsDataWithID(_curBuykey)->Price);
	}
	else if ( BuyType_Gold == type )
	{
		int myGold = NewDataMgr::getInstance()->getGoodsNumWithID(GoodsID_Gold);

		if (50000 >= needGold)
		{
			auto showGoodsInfoLayer = PackShowLayer::getPackShowLayer();
			showGoodsInfoLayer->setShowInfoType(PackShowLayer::PackType::InfoType_OverflowPack);
			Director::getInstance()->getRunningScene()->addChild(showGoodsInfoLayer);
		}
		else
		{
			auto showGoodsInfoLayer = PackShowLayer::getPackShowLayer();
			showGoodsInfoLayer->setShowInfoType(PackShowLayer::PackType::InfoType_TyrantPack);
			Director::getInstance()->getRunningScene()->addChild(showGoodsInfoLayer);
		}

// 		auto widget = CommonWidget::create();
// 		widget->setCloseButtonState(false);
// 		widget->setContent("金币不足");
// 		widget->addBtn1ClickEventListener(CC_CALLBACK_0(GameBuySystem::callFuncBuyWidget, this));
// 		Director::getInstance()->getRunningScene()->addChild(widget);
	}

}

void GameBuySystem::callFuncBuyWidget(void)
{
	GameInterfaceMgr::getInstance()->addInterfaceToList(GameInterfaceMgr::InterfaceTag_ShopBuy);
}

void GameBuySystem::sendBuySuccessMsg()
{
	bool needStatics = true;
	if (BuyKey_BuyRolePack <= _curBuykey)
	{
		NewDataMgr::getInstance()->mGameRoleSystem->setRoleLockState(2, true);
		NewDataMgr::getInstance()->mGameRoleSystem->setRoleLockState(3, true);

		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRolePack, EProtocolID_RemoveGoodsInfo);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_UpdateGold);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_BuySuccessEffect);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_UnlockRole);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_BuyRolePackFinish);

	}
	// 道具礼包
	else if (BuyKey_BuyPropPack <= _curBuykey)
	{
		auto packInfo = getPackGoodsInfoWithKey(_curBuykey);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Gold, packInfo->gold, false);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Life, packInfo->life, false);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_HuDun, packInfo->hudun, false);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_DaZhao, packInfo->dazhao);

		// 添加显示Item
		if (packInfo->gold)		ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_Gold, (void*)packInfo->gold);
		if (packInfo->dazhao)	ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_DaZhao, (void*)packInfo->dazhao);
		if (packInfo->hudun)	ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_HuDun, (void*)packInfo->hudun);
		if (packInfo->life)		ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_Life, (void*)packInfo->life);

		firstBuyPackPrc();

		// 发送界面消息
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToGoodsInfoLayer, EProtocolID_RemoveGoodsInfo);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToGameMainLayer, EProtocolID_PropPackBuy);
	}
	// vip
	else if (BuyKey_BuyFuHuo <= _curBuykey)
	{
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Life, getBuyGoodsDataWithID(_curBuykey)->number);

		// 发送界面消息
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToGameFuHuo, EProtocolID_FuHuoBuy);
	}
	else if (BuyKey_BuyVIPPre <= _curBuykey)
	{
		GameInterfaceMgr::getInstance()->closeCurInterface();
		auto packInfo = getPackGoodsInfoWithKey(_curBuykey);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Gold, packInfo->gold, false);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Life, packInfo->life, false);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_HuDun, packInfo->hudun, false);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_DaZhao, packInfo->dazhao);

		// 添加显示Item
		if (packInfo->gold)		ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_Gold, (void*)packInfo->gold);
		if (packInfo->dazhao)	ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_DaZhao, (void*)packInfo->dazhao);
		if (packInfo->hudun)	ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_HuDun, (void*)packInfo->hudun);
		if (packInfo->life)		ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_Life, (void*)packInfo->life);

		// 发送界面消息
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_UpdateGold);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToVIPPresentingLayer, EProtocolID_VipRewardGet);
	}
	else if (BuyKey_BuyVIP <= _curBuykey)
	{
		GameInterfaceMgr::getInstance()->closeCurInterface();
		NewDataMgr::getInstance()->setVipDay(VIPValid_Day);

		auto packInfo = getPackGoodsInfoWithKey(_curBuykey);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Gold, packInfo->gold, false);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Life, packInfo->life, false);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_HuDun, packInfo->hudun, false);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_DaZhao, packInfo->dazhao);

		// 添加显示Item
		if (packInfo->gold)		ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_Gold, (void*)packInfo->gold);
		if (packInfo->dazhao)	ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_DaZhao, (void*)packInfo->dazhao);
		if (packInfo->hudun)	ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_HuDun, (void*)packInfo->hudun);
		if (packInfo->life)		ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_Life, (void*)packInfo->life);

		// 发送界面消息
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_UpdateGold);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_BuySuccessEffect);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_VipBuyCallFunc);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_VipBuyCallFunc);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToVIPLayer, EProtocolID_VipBuyCallFunc);
	}

	// 商城
	else if (BuyKey_BuyLife <= _curBuykey)
	{
		int goodeNum = getBuyGoodsDataWithID(_curBuykey)->number;
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Life, goodeNum);
		// 添加显示Item
		ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_Life, (void*)goodeNum);


		// 发送界面消息
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_BuySuccessEffect);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToGoodsInfoLayer, EProtocolID_RemoveGoodsInfo);
	}
	else if (BuyKey_BuyDaZhao <= _curBuykey)
	{
		int goodeNum = getBuyGoodsDataWithID(_curBuykey)->number;
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_DaZhao, goodeNum);
		// 添加显示Item
		ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_DaZhao, (void*)goodeNum);

		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_BuySuccessEffect);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToGoodsInfoLayer, EProtocolID_RemoveGoodsInfo);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToGameMainLayer, EProtocolID_DaZhaoBuy);
	}
	else if (BuyKey_BuyShield <= _curBuykey)
	{
		int goodeNum = getBuyGoodsDataWithID(_curBuykey)->number;
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_HuDun, goodeNum);
		// 添加显示Item
		ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_HuDun, (void*)goodeNum);


		// 发送界面消息
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_BuySuccessEffect);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToGoodsInfoLayer, EProtocolID_RemoveGoodsInfo);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToGameMainLayer, EProtocolID_DaHudun);
	}
	else if (BuyKey_BuyGold <= _curBuykey)
	{
		NewDataMgr::getInstance()->mGameTaskSystem->addTaskPropertyWithKey(GamePrKey_BuyGoldTime, 1);

		int goodeNum = getBuyGoodsDataWithID(_curBuykey)->number;
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Gold, goodeNum);

		// 添加显示Item
		ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_Gold, (void*)goodeNum);


		// 发送界面消息
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_UpdateGold);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_BuySuccessEffect);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToGoodsInfoLayer, EProtocolID_RemoveGoodsInfo);
	}
	else if (BuyKey_TyrantPack <= _curBuykey)
	{
		auto packInfo = getPackGoodsInfoWithKey(_curBuykey);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Gold, packInfo->gold, false);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Life, packInfo->life, false);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_HuDun, packInfo->hudun, false);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_DaZhao, packInfo->dazhao);

		// 添加显示Item
		if (packInfo->gold)		ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_Gold, (void*)packInfo->gold);
		if (packInfo->dazhao)	ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_DaZhao, (void*)packInfo->dazhao);
		if (packInfo->hudun)	ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_HuDun, (void*)packInfo->hudun);
		if (packInfo->life)		ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_Life, (void*)packInfo->life);

		firstBuyPackPrc();


		// 发送界面消息
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_UpdateGold);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_BuySuccessEffect);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToGoodsInfoLayer, EProtocolID_RemoveGoodsInfo);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToGameMainLayer, EProtocolID_TyrantPackBuy);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToShopLayer, EProtocolID_TyrantPackBuy);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToLevelOptionLayer, EProtocolID_TyrantPackBuy);
	}
	else if (BuyKey_OverflowPack <= _curBuykey)
	{
		auto packInfo = getPackGoodsInfoWithKey(_curBuykey);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Gold, packInfo->gold, false);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Life, packInfo->life, false);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_HuDun, packInfo->hudun, false);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_DaZhao, packInfo->dazhao);

		// 添加显示Item
		if (packInfo->gold)		ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_Gold, (void*)packInfo->gold);
		if (packInfo->dazhao)	ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_DaZhao, (void*)packInfo->dazhao);
		if (packInfo->hudun)	ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_HuDun, (void*)packInfo->hudun);
		if (packInfo->life)		ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_Life, (void*)packInfo->life);

		// 发送界面消息
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_UpdateGold);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_BuySuccessEffect);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToGoodsInfoLayer, EProtocolID_RemoveGoodsInfo);
	}
	else if (BuyKey_NewPack <= _curBuykey)
	{
		auto packInfo = getPackGoodsInfoWithKey(_curBuykey);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Gold, packInfo->gold, false);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Life, packInfo->life, false);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_HuDun, packInfo->hudun, false);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_DaZhao, packInfo->dazhao);

		// 添加显示Item
		if (packInfo->gold)		ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_Gold, (void*)packInfo->gold);
		if (packInfo->dazhao)	ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_DaZhao, (void*)packInfo->dazhao);
		if (packInfo->hudun)	ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_HuDun, (void*)packInfo->hudun);
		if (packInfo->life)		ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_Life, (void*)packInfo->life);

		// 发送界面消息
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_UpdateGold);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_BuySuccessEffect);
	}

	else if (BuyKey_UpgradeYellowBall <= _curBuykey || 
		BuyKey_UpgradeBlueBall <= _curBuykey ||
		BuyKey_UpgradeRedBall <= _curBuykey ||
		BuyKey_UpgradeGold <= _curBuykey ||
		BuyKey_UpgradeDaZhao <= _curBuykey ||
		BuyKey_UpgradeRole <= _curBuykey)
	{
		NewDataMgr::getInstance()->mGameTaskSystem->addTaskPropertyWithKey(GamePrKey_UpgradeObject, 1);

		NewDataMgr::getInstance()->mGameGradeMsg->upgradeobjectWithType(_curBuykey);

		if( !getFreeState() )
			NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Gold, -getBuyGoodsDataWithID(_curBuykey)->Gold);

		// 发送界面消息
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_UpdateGold);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToUpgradeLayer, EProtocolID_UpgradeObject);
	}
	

	else if (BuyKey_VioletCard <= _curBuykey ||
		BuyKey_BlueCard <= _curBuykey)
	{
		needStatics = false;
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Gold, -getBuyGoodsDataWithID(_curBuykey)->Gold);
		NewDataMgr::getInstance()->mGameCardSystem->updateCardLock();

		// 发送界面消息
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_UpdateGold);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToCardInfoLayer, EProtocolID_UnlockCard);
	}
	else if (BuyKey_Role3 <= _curBuykey)
	{
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Gold, -getBuyGoodsDataWithID(_curBuykey)->Gold);
		NewDataMgr::getInstance()->mGameRoleSystem->setRoleLockState(3, true);


		// 发送界面消息
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_UpdateGold);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_BuySuccessEffect);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_UnlockRole);
	}
	else if (BuyKey_Role2 <= _curBuykey)
	{
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Gold, -getBuyGoodsDataWithID(_curBuykey)->Gold);
		NewDataMgr::getInstance()->mGameRoleSystem->setRoleLockState(2, true);

		// 发送界面消息
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_UpdateGold);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_BuySuccessEffect);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_UnlockRole);

	} else {
		needStatics = false;
	}

	if(needStatics) {
		StatisticsHelp::buy(this->_curBuykey);
	}
}

PackGoodsInfo* GameBuySystem::getPackGoodsInfoWithKey(int key)
{
	map<int, PackGoodsInfo>::iterator	it = _packInfoMap.find(key);
	if (it == _packInfoMap.end()){ return nullptr; }
	return &it->second;
}

void GameBuySystem::firstBuyPackPrc(void)
{
	return;
	// 已购买过礼包
	if (NewDataMgr::getInstance()->getTyrantPackBuyState())
	{
		return;
	}

	NewDataMgr::getInstance()->mGameCardSystem->firstBuyUnlockCard();


}

void GameBuySystem::checkFirstBuyPackState(void)
{
	// 首充状态 如果三星卡牌全解锁 设置为已购买 不进行首充送三星
	if (!NewDataMgr::getInstance()->getTyrantPackBuyState())
	{
		if (NewDataMgr::getInstance()->mGameCardSystem->getCardConfigWithId(3)->islock &&
			NewDataMgr::getInstance()->mGameCardSystem->getCardConfigWithId(6)->islock &&
			NewDataMgr::getInstance()->mGameCardSystem->getCardConfigWithId(9)->islock)
		{
			NewDataMgr::getInstance()->setTyrantPackBuyState(true);
		}
	}
}

/*******************************************
* 角色系统
*******************************************/
GameRoleSystem::GameRoleSystem(sqlite3* db)
	:_sqlite(nullptr)
{
	_sqlite = db;
	initRoleInfo(_sqlite);
}

void GameRoleSystem::initRoleInfo( sqlite3* db )
{
	std::string sql;
	int result;
	char *zErrorMsg = 0;

	// 查询表字段
	sql = "select id from roleInfo_Table";
	result=sqlite3_exec(db,sql.c_str(),NULL,NULL,&zErrorMsg);

	// 表不存在
	if ( result != SQLITE_OK )
	{
		//	CCASSERT( result == SQLITE_OK, "roleinfo table not existes");
		sql="create table if not exists roleInfo_Table("\
			"id integer"\
			",roleGrade integer"\
			",isunlock integer"
			")";

		result=sqlite3_exec(db,sql.c_str(),NULL,NULL,&zErrorMsg);

		if(result == SQLITE_OK)   
		{
			for ( int i =1; i<=3; i++ )
			{
				// 插入数据 用户基础数据
				sql.clear();
				sql = __String::createWithFormat("insert into roleInfo_Table values( %d, %d, %d )",
					CSV_MGR()->GetCsvRoleUpgrade()->GetByID(i)->roleId,
					CSV_MGR()->GetCsvRoleUpgrade()->GetByID(i)->roleGrade,
					CSV_MGR()->GetCsvRoleUpgrade()->GetByID(i)->isLock)->getCString();

				result=sqlite3_exec(db,sql.c_str(),NULL,NULL,&zErrorMsg);

				if(result!=SQLITE_OK)
					CCLOG("insert err");
			}
		}
	}

	// 查询语句
	sql = " select id, roleGrade, isunlock from roleInfo_Table";
	sqlite3_stmt *statement;
	if ( sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr) == SQLITE_OK )
	{
		// 初始化角色列表
		while (sqlite3_step(statement) == SQLITE_ROW) 
		{
			// 获得解锁状态
			int id			= sqlite3_column_int(statement, 0);
			int grade		= sqlite3_column_int(statement, 1);
			int islock		= sqlite3_column_int(statement, 2);

			CSV_MGR()->GetCsvRoleUpgrade()->GetByID(id)->roleGrade = grade;
			CSV_MGR()->GetCsvRoleUpgrade()->GetByID(id)->isLock		= islock;

		}
	}

	sqlite3_finalize(statement);
}

csvRoleUpgrade_s* GameRoleSystem::getRoleConfigWithID(int id)
{
	return CSV_MGR()->GetCsvRoleUpgrade()->GetByID(id);
}

int GameRoleSystem::getRolePriceWithID(int id)
{
	int price = 0;
	int buyKey = 0;

	if ( 2 == id )
	{
		buyKey = BuyKey_Role2;
	}
	else if ( 3 == id )
	{
		buyKey = BuyKey_Role3;
	}

	int buyType = NewDataMgr::getInstance()->mGameBuySystem->getBuyGoodsDataWithID(buyKey)->buyType;
	if (BuyType_Gold == buyType)
	{
		price = buyType = NewDataMgr::getInstance()->mGameBuySystem->getBuyGoodsDataWithID(buyKey)->Gold;
	}
	else
	{
		price = buyType = NewDataMgr::getInstance()->mGameBuySystem->getBuyGoodsDataWithID(buyKey)->Price;
	}

	return price;
}

void GameRoleSystem::setRoleLockState(int id, bool state)
{
	getRoleConfigWithID(id)->isLock = state;
	updateTable(TableKey_LockState, id, state);

	ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_Role, (void*)NewDataMgr::getInstance()->getXmlStringWithKey(StringUtils::format("rolename%d", id).c_str()).c_str());

}

void GameRoleSystem::updateTable(TableKey key, int id, int data)
{
	std::string sql;
	sql.append("update roleInfo_Table set  ");

	switch (key)
	{
	case TableKey_LockState:
		sql.append("isunlock=");
		break;
	case TableKey_Grade:
		sql.append("roleGrade=");
		break;

	default:
		break;
	}

	sql.append(__String::createWithFormat("%d ", data)->getCString());
	sql.append(__String::createWithFormat("where id = %d ", id)->getCString());

	if ( _sqlite )
	{
		int result;
		char* zErrorMsg = 0;

		result = sqlite3_exec(_sqlite, sql.c_str(), NULL, NULL, &zErrorMsg);
		if (result != SQLITE_OK)
			CCLOG("insert err");
	}
}

int GameRoleSystem::getRoleGradeWithID(int id)
{
	int grade = NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(id)->curGrade;
	return grade;
}




/*******************************************
* 卡牌系统
*******************************************/

GameCardSystem::GameCardSystem(sqlite3* db)
{
	mNewUnlockCard = 0;
	_sqlite = db;
	initCardInfo(_sqlite);
	log("GameCardSystem1");
	initCardOption(_sqlite);
	log("GameCardSystem2");
}

void GameCardSystem::initCardInfo(sqlite3* db)
{
	std::string sql;
	int result;
	char *zErrorMsg = 0;

	// 查询表字段
	sql = "select id from cardInfo_Table";
	result = sqlite3_exec(db, sql.c_str(), NULL, NULL, &zErrorMsg);

	// 表不存在
	if (result != SQLITE_OK)
	{
		//	CCASSERT( result == SQLITE_OK, "roleinfo table not existes");
		sql = "create table if not exists cardInfo_Table("\
			"id integer"\
			",lockstate integer"\
			")";

		result = sqlite3_exec(db, sql.c_str(), NULL, NULL, &zErrorMsg);

		if (result == SQLITE_OK)
		{
			for (int i = 1; i <= GameData_CardNum; i++)
			{
				// 插入数据 用户基础数据
				sql = __String::createWithFormat("insert into cardInfo_Table values (%d, %d)",
					CSV_MGR()->GetCsvCardAtkPConfig()->GetByID(i)->id, 
					CSV_MGR()->GetCsvCardAtkPConfig()->GetByID(i)->islock
					)->getCString();

				result = sqlite3_exec(db, sql.c_str(), NULL, NULL, &zErrorMsg);

				if (result != SQLITE_OK)
					CCLOG("insert err");
			}
		}
	}

	// 查询语句
	sql = " select id, lockstate from cardInfo_Table";
	sqlite3_stmt *statement;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr) == SQLITE_OK)
	{
		// 初始化角色列表
		while (sqlite3_step(statement) == SQLITE_ROW)
		{
			// 获得星级卡牌数
			int id = sqlite3_column_int(statement, 0);
			CSV_MGR()->GetCsvCardAtkPConfig()->GetByID(id)->islock = sqlite3_column_int(statement, 1);
		}
	}

	sqlite3_finalize(statement);
}

void GameCardSystem::setCardLockWithId(int cardid)
{
	// 更新表
	std::string sql;
	sql.append("update cardInfo_Table set  ");
	sql.append(StringUtils::format("lockstate = %d ", 1));
	sql.append(StringUtils::format("where id = %d ", cardid));

	if (_sqlite)
	{
		int result;
		char* zErrorMsg = 0;

		result = sqlite3_exec(_sqlite, sql.c_str(), NULL, NULL, &zErrorMsg);
		if (result != SQLITE_OK)
			CCLOG("update err");
	}
}

csvCardAtkPConfig_s* GameCardSystem::getCardConfigWithId(int id)
{
	return CSV_MGR()->GetCsvCardAtkPConfig()->GetByID(id);
}

void GameCardSystem::initCardOption(sqlite3* db)
{
	std::string sql;
	int result;
	char *zErrorMsg = 0;

	// 查询表字段
	sql = "select type from cardOption_Table";
	result = sqlite3_exec(db, sql.c_str(), NULL, NULL, &zErrorMsg);

	// 表不存在
	if (result != SQLITE_OK)
	{
		//	CCASSERT( result == SQLITE_OK, "roleinfo table not existes");
		sql = "create table if not exists cardOption_Table("\
			"type integer"\
			",cardid integer"\
			",cardStar integer"\
			")";

		result = sqlite3_exec(db, sql.c_str(), NULL, NULL, &zErrorMsg);

		if (result == SQLITE_OK)
		{
			// 插入数据 
			sql = "insert into cardOption_Table values"\
				"(1, 1, 1),"\
				"(2, 4, 1),"\
				"(3, 7, 1)";

			result = sqlite3_exec(db, sql.c_str(), NULL, NULL, &zErrorMsg);

			if (result != SQLITE_OK)
				CCLOG("insert err");

		}
	}

	// 查询语句
	sql = " select type, cardid , cardStar from cardOption_Table";
	sqlite3_stmt *statement;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr) == SQLITE_OK)
	{
		// 初始化已选择的Card列表
		while (sqlite3_step(statement) == SQLITE_ROW)
		{
			int m = sqlite3_column_int(statement, 0);
			NewDataMgr::getInstance()->mRoleConfig.cardArr[m - 1].cardID = sqlite3_column_int(statement, 1);
			NewDataMgr::getInstance()->mRoleConfig.cardArr[m - 1].cardStarLv = sqlite3_column_int(statement, 2);
		}
	}

	sqlite3_finalize(statement);
}

void GameCardSystem::setOptionCard(int cardType, int cardid)
{
	NewDataMgr::getInstance()->mRoleConfig.cardArr[cardType - 1].cardID = cardid;

	// 更新表
	std::string sql;
	sql.append("update cardOption_Table set  ");
	sql.append(StringUtils::format("cardid = %d ", cardid));
	sql.append(StringUtils::format("where type = %d ", cardType));

	if (_sqlite)
	{
		int result;
		char* zErrorMsg = 0;

		result = sqlite3_exec(_sqlite, sql.c_str(), NULL, NULL, &zErrorMsg);
		if (result != SQLITE_OK)
			CCLOG("update err");
	}

}

int GameCardSystem::getCardPriceWithID(int id)
{
	int cardTrait = getCardConfigWithId(id)->CardId;
	int cardPrice = 0;

	if ( 2 == cardTrait )
	{
		cardPrice = NewDataMgr::getInstance()->mGameBuySystem->getBuyGoodsDataWithID(BuyKey_BlueCard)->Gold;
	}
	else if (3 == cardTrait)
	{
		cardPrice = NewDataMgr::getInstance()->mGameBuySystem->getBuyGoodsDataWithID(BuyKey_VioletCard)->Gold;
	}

	return cardPrice;
}

bool GameCardSystem::buyCardWithID(int id)
{
	_curOptionCardId = id;

	int cardTrait = getCardConfigWithId(id)->CardId;
	int buyKey = 0;

	if (2 == cardTrait)
	{
		buyKey = BuyKey_BlueCard;
	}
	else if (3 == cardTrait)
	{
		buyKey = BuyKey_VioletCard;
	}

	return NewDataMgr::getInstance()->mGameBuySystem->buyGoodsWithKey(buyKey);
}

void GameCardSystem::updateCardLock(void)
{
	std::string cardName = __String::createWithFormat("card_%d",_curOptionCardId)->getCString();
	StatisticsHelp::buy(cardName.c_str(),1,1);
	auto cardItem = getCardConfigWithId(_curOptionCardId);
	cardItem->islock = 1;
	setNewUnlockCard(_curOptionCardId);

	setCardLockWithId(_curOptionCardId);

	ItemRewardTips::getInstance(nullptr)->addItemTips(GoodsID_Card, (void*)getCardConfigWithId(_curOptionCardId)->CardId);

}

void GameCardSystem::unLockCard2()
{
	_curOptionCardId = 2;
	updateCardLock();
}

void GameCardSystem::firstBuyUnlockCard(void)
{
	int cardType = random(1, 3);
	_curOptionCardId = cardType * 3;

	updateCardLock();
}




/*******************************************
* 等级管理
*******************************************/
void GameGradeMsg::initGradeInfo(void)
{
	std::string sql;
	int result;
	char *zErrorMsg = 0;

	// 查询表字段
	sql = "select type from gradeInfo_Table";
	result = sqlite3_exec(_sqlite, sql.c_str(), NULL, NULL, &zErrorMsg);

	// 表不存在
	if (result != SQLITE_OK)
	{
		//	CCASSERT( result == SQLITE_OK, "roleinfo table not existes");
		sql = "create table if not exists gradeInfo_Table("\
			"type integer"\
			",grade integer"\
			",maxgrade integer"\
			")";

		result = sqlite3_exec(_sqlite, sql.c_str(), NULL, NULL, &zErrorMsg);

		if (result == SQLITE_OK)
		{
			// 插入数据 
			sql = "insert into gradeInfo_Table values"\
				"(1, 0, 3),"\
				"(2, 0, 10),"\
				"(3, 0, 5),"\
				"(4, 0, 10),"\
				"(5, 0, 10),"\
				"(6, 0, 10)";

			result = sqlite3_exec(_sqlite, sql.c_str(), NULL, NULL, &zErrorMsg);

			if (result != SQLITE_OK)
				CCLOG("insert err");

		}
	}

	// 查询语句
	sql = " select type, grade , maxgrade from gradeInfo_Table";
	sqlite3_stmt *statement;
	if (sqlite3_prepare_v2(_sqlite, sql.c_str(), -1, &statement, nullptr) == SQLITE_OK)
	{
		while (sqlite3_step(statement) == SQLITE_ROW)
		{
			gradeStr* tempStr = new gradeStr;
			int type = sqlite3_column_int(statement, 0);
			tempStr->curGrade = sqlite3_column_int(statement, 1);
			tempStr->maxGrade = sqlite3_column_int(statement, 2);
			
			_objectGradeMap.insert(pair<int, gradeStr* >(type, tempStr));
		}
	}

	sqlite3_finalize(statement);
}

gradeStr* GameGradeMsg::getObjectGradeWithType(int type)
{
	map<int, gradeStr*>::iterator	it = _objectGradeMap.find(type);
	if (it == _objectGradeMap.end()){ return 0; }
	return it->second;
}

void GameGradeMsg::setObjectGradeWithType(int type, int grade)
{
	map<int, gradeStr*>::iterator	it = _objectGradeMap.find(type);
	if (it != _objectGradeMap.end())
	{
		it->second->curGrade = grade;

		// 更新表
		std::string sql;
		sql.append("update gradeInfo_Table set  ");
		sql.append(StringUtils::format("grade = %d ", grade));
		sql.append(StringUtils::format("where type = %d ", type));

		if (_sqlite)
		{
			int result;
			char* zErrorMsg = 0;

			result = sqlite3_exec(_sqlite, sql.c_str(), NULL, NULL, &zErrorMsg);
			if (result != SQLITE_OK)
				CCLOG("update err");
		}
	}
	
}

csvBuyList_s GameGradeMsg::getObjectUpgradePriceWithType(int type)
{
	auto it = getObjectGradeWithType(type);
	 
	int buyKey;

	switch (type)
	{
	case ObjectType_Role:
		buyKey = BuyKey_UpgradeRole + it->curGrade;
		break;

	case ObjectType_DaZhao:
		buyKey = BuyKey_UpgradeDaZhao + it->curGrade;
		break;

	case ObjectType_Gold:
		buyKey = BuyKey_UpgradeGold + it->curGrade;
		break;

	case ObjectType_RedBall: 
		buyKey = BuyKey_UpgradeRedBall + it->curGrade;
		break;

	case ObjectType_BlueBall:
		buyKey = BuyKey_UpgradeBlueBall + it->curGrade;
		break;

	case ObjectType_YellowBall:
		buyKey = BuyKey_UpgradeYellowBall + it->curGrade;
		break;

	default:
		break;
	}
	if (it->curGrade >= it->maxGrade)	buyKey -= 1;

	auto buyItem = NewDataMgr::getInstance()->mGameBuySystem->getBuyGoodsDataWithID(buyKey);

	return *buyItem;
}

void GameGradeMsg::upgradeobjectWithType(int type)
{
	auto it = getObjectGradeWithType(_curUpgradeType);

	it->curGrade++;

	setObjectGradeWithType(_curUpgradeType, it->curGrade);
}

void GameGradeMsg::buyObjectGradeWithType(int type, bool isFree/* = false*/)
{
	_curUpgradeType = type;
	auto it = getObjectGradeWithType(type);
	int buyKey;

	switch (type)
	{
	case ObjectType_Role:
		buyKey = BuyKey_UpgradeRole + it->curGrade;
		break;

	case ObjectType_DaZhao:
		buyKey = BuyKey_UpgradeDaZhao + it->curGrade;
		break;

	case ObjectType_Gold:
		buyKey = BuyKey_UpgradeGold + it->curGrade;
		break;

	case ObjectType_RedBall:
		buyKey = BuyKey_UpgradeRedBall + it->curGrade;
		break;

	case ObjectType_BlueBall:
		buyKey = BuyKey_UpgradeBlueBall + it->curGrade;
		break;

	case ObjectType_YellowBall:
		buyKey = BuyKey_UpgradeYellowBall + it->curGrade;
		break;

	default:
		break;
	}

	auto buyItem = NewDataMgr::getInstance()->mGameBuySystem->buyGoodsWithKey(buyKey, 1, isFree);

}

int GameGradeMsg::getRewardGoldWithLv(float goldNum)
{
	int lv = getObjectGradeWithType(ObjectType_Gold)->curGrade;
	float rewardNum = 0;
	switch (lv)
	{
	case 0:
		rewardNum = 0;
		break;

	case 1:
		rewardNum = goldNum * 0.2;
		break;

	case 2:
		rewardNum = goldNum * 0.4;
		break;

	case 3:
		rewardNum = goldNum * 0.6;
		break;

	case 4:
		rewardNum = goldNum * 0.8;
		break;

	case 5:
		rewardNum = goldNum;
		break;

	default:
		break;
	}

	return rewardNum;
}

float GameGradeMsg::getObjectAtkWithCurLv(int type)
{
	int lv = getObjectGradeWithType(type)->curGrade;

	switch (type)
	{
	case GameGradeMsg::ObjectType_RedBall:
	case GameGradeMsg::ObjectType_BlueBall:
	case GameGradeMsg::ObjectType_YellowBall:
	case GameGradeMsg::ObjectType_DaZhao:
	{
		return 0.1*lv;
	}
		break;
	default:
		break;
	}
	return 0;
}

