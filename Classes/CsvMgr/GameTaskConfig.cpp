#include "GameTaskConfig.h"
#include "CsvCommon.h"

using namespace std;
using namespace cocos2d;

CsvGameTaskConfig::CsvGameTaskConfig() 
{
}

CsvGameTaskConfig::~CsvGameTaskConfig() 
{
	for ( map<int, csvGameTaskConfig_s*>::iterator it = GameTaskConfigMap.begin(); it != GameTaskConfigMap.end(); it++ ) {
		delete it->second;
	}
}

void CsvGameTaskConfig::SetData( vector<string> &str ) 
{
	csvGameTaskConfig_s   *item = new csvGameTaskConfig_s;
	counter = 0;

	item->TaskID = StringToInt( str[counter] );
	counter++; 
	item->TaskDes =  str[counter];
	counter++; 
	item->TaskRewardDes =  str[counter];
	counter++; 
	item->TaskType = StringToInt(str[counter]);
	counter++;
	item->RewardNum = StringToInt( str[counter] );
	counter++; 
	item->TaskTargetNum = StringToInt( str[counter] );
	counter++; 
	item->TaskProgressNum = StringToInt( str[counter] );
	counter++; 
	item->TaskState = StringToInt( str[counter] );
	counter++; 
	item->TaskRewardID = StringToInt( str[counter] );
	counter++; 

	GameTaskConfigMap.insert( pair<int,csvGameTaskConfig_s*>( atoi( str[0].c_str() ), item ) ); 
}

csvGameTaskConfig_s *CsvGameTaskConfig::GetByID( int id ) 
{
	map<int, csvGameTaskConfig_s*>::iterator	it = GameTaskConfigMap.find( id ); 
	if( it == GameTaskConfigMap.end() ){ return NULL; } 
	return it->second;
}

map<int, csvGameTaskConfig_s*> *CsvGameTaskConfig::GetAll() 
{
	return &GameTaskConfigMap;
}

