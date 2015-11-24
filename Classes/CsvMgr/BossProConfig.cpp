#include "BossProConfig.h"
#include "CsvCommon.h"

using namespace std;
using namespace cocos2d;

CsvBossProConfig::CsvBossProConfig() 
{
}

CsvBossProConfig::~CsvBossProConfig() 
{
	for ( map<int, csvBossProConfig_s*>::iterator it = BossProConfigMap.begin(); it != BossProConfigMap.end(); it++ ) {
		delete it->second;
	}
}

void CsvBossProConfig::SetData( vector<string> &str ) 
{
	csvBossProConfig_s   *item = new csvBossProConfig_s;
	counter = 0;

	item->id = StringToInt( str[counter] );
	counter++; 
	item->atkLevel = StringToFloat( str[counter] );
	counter++; 
	item->lifeLevel = StringToFloat( str[counter] );
	counter++; 
	item->speedLevel = StringToFloat( str[counter] );
	counter++; 
	item->bossName = str[counter];

	counter++;
	item->grade1 = StringToInt(str[counter]);
	counter++;
	item->grade2 = StringToInt(str[counter]);
	counter++;
	item->grade3 = StringToInt(str[counter]);
	counter++;
	item->timeOut = StringToFloat(str[counter]);
	
	BossProConfigMap.insert( pair<int,csvBossProConfig_s*>( atoi( str[0].c_str() ), item ) ); 
}

csvBossProConfig_s *CsvBossProConfig::GetByID( int id ) 
{
	map<int, csvBossProConfig_s*>::iterator	it = BossProConfigMap.find( id ); 
	if( it == BossProConfigMap.end() ){ return NULL; } 
	return it->second;
}

map<int, csvBossProConfig_s*> *CsvBossProConfig::GetAll() 
{
	return &BossProConfigMap;
}

