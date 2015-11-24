#ifndef __GAMETASKCONFIG_H__
#define __GAMETASKCONFIG_H__

#include "cocos2d.h"
USING_NS_CC; 

struct csvGameTaskConfig_s{
	int				TaskID;
	std::string		TaskDes;
	std::string		TaskRewardDes;
	int				RewardNum;
	int				TaskType;
	int				TaskTargetNum;
	int				TaskProgressNum;
	int				TaskState;			// 0-未完成 1-已完成未领取 2-已完成已领取
	int				TaskRewardID;		// 奖励物品ID
};

class CsvGameTaskConfig
{
public:

	typedef enum 
	{
	}CSVDataKey;

	CsvGameTaskConfig();
	~CsvGameTaskConfig();

	void					SetData( std::vector<std::string> &str );
	csvGameTaskConfig_s*		GetByID( int id );
	std::map<int, csvGameTaskConfig_s*> *GetAll();

protected:
	std::map<int, csvGameTaskConfig_s*>			GameTaskConfigMap;
	int		counter;
};
#endif