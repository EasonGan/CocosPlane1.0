#ifndef __BOSSPROCONFIG_H__
#define __BOSSPROCONFIG_H__

#include "cocos2d.h"
USING_NS_CC; 

struct csvBossProConfig_s{
	int				id;
	float			atkLevel;
	float			lifeLevel;
	float			speedLevel;
	std::string		bossName;
	int				grade1;
	int				grade2;
	int				grade3;
	float			timeOut;
};

class CsvBossProConfig
{
public:

	typedef enum 
	{
	}CSVDataKey;

	CsvBossProConfig();
	~CsvBossProConfig();

	void					SetData( std::vector<std::string> &str );
	csvBossProConfig_s*		GetByID( int id );
	std::map<int, csvBossProConfig_s*> *GetAll();

protected:
	std::map<int, csvBossProConfig_s*>			BossProConfigMap;
	int		counter;
};
#endif