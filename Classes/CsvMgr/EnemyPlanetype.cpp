#include "EnemyPlanetype.h"
#include "CsvCommon.h"

using namespace std;
using namespace cocos2d;

CsvEnemyPlanetype::CsvEnemyPlanetype() 
{
}

CsvEnemyPlanetype::~CsvEnemyPlanetype() 
{
	for ( map<int, csvEnemyPlanetype_s*>::iterator it = EnemyPlanetypeMap.begin(); it != EnemyPlanetypeMap.end(); it++ ) {
		delete it->second;
	}
}

void CsvEnemyPlanetype::SetData( vector<string> &str ) 
{
	csvEnemyPlanetype_s   *item = new csvEnemyPlanetype_s;
	counter = 0;

	item->EnemyType = StringToInt( str[counter] );
	counter++; 
	item->AtkCoe1 = StringToFloat( str[counter] );
	counter++; 
	item->AtkCoe2 = StringToFloat( str[counter] );
	counter++; 
	item->HpCoe1 = StringToFloat( str[counter] );
	counter++; 
	item->HpCoe2 = StringToFloat( str[counter] );
	counter++; 
	item->ScoreCoe1 = StringToFloat( str[counter] );
	counter++; 
	item->ScoreCoe2 = StringToFloat( str[counter] );
	counter++; 

	EnemyPlanetypeMap.insert( pair<int,csvEnemyPlanetype_s*>( atoi( str[0].c_str() ), item ) ); 
}

csvEnemyPlanetype_s *CsvEnemyPlanetype::GetByID( int id ) 
{
	map<int, csvEnemyPlanetype_s*>::iterator	it = EnemyPlanetypeMap.find( id ); 
	if( it == EnemyPlanetypeMap.end() ){ return NULL; } 
	return it->second;
}

map<int, csvEnemyPlanetype_s*> *CsvEnemyPlanetype::GetAll() 
{
	return &EnemyPlanetypeMap;
}

