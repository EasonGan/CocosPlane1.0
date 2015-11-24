#ifndef __ENEMYPLANETYPE_H__
#define __ENEMYPLANETYPE_H__

#include "cocos2d.h"
USING_NS_CC; 

struct csvEnemyPlanetype_s{
	int				EnemyType;
	float			AtkCoe1;
	float			AtkCoe2;
	float			HpCoe1;
	float			HpCoe2;
	float			ScoreCoe1;
	float			ScoreCoe2;
};

class CsvEnemyPlanetype
{
public:

	typedef enum 
	{
	}CSVDataKey;

	CsvEnemyPlanetype();
	~CsvEnemyPlanetype();

	void					SetData( std::vector<std::string> &str );
	csvEnemyPlanetype_s*		GetByID( int id );
	std::map<int, csvEnemyPlanetype_s*> *GetAll();

protected:
	std::map<int, csvEnemyPlanetype_s*>			EnemyPlanetypeMap;
	int		counter;
};
#endif