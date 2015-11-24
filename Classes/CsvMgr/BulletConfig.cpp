#include "BulletConfig.h"
#include "CsvCommon.h"

using namespace std;
using namespace cocos2d;

CsvBulletConfig::CsvBulletConfig() 
{
}

CsvBulletConfig::~CsvBulletConfig() 
{
	for ( map<int, csvBulletConfig_s*>::iterator it = BulletConfigMap.begin(); it != BulletConfigMap.end(); it++ ) {
		delete it->second;
	}
}

void CsvBulletConfig::SetData( vector<string> &str ) 
{
	csvBulletConfig_s   *item = new csvBulletConfig_s;
	counter = 0;

	item->id = StringToInt( str[counter] );
	counter++; 
	item->resId = StringToInt( str[counter] );
	counter++; 
	item->scaleX = StringToFloat( str[counter] );
	counter++; 
	item->scaleY = StringToFloat(str[counter]);
	counter++;
	item->attackCoe = StringToFloat( str[counter] );
	counter++; 

	BulletConfigMap.insert( pair<int,csvBulletConfig_s*>( atoi( str[0].c_str() ), item ) ); 
}

csvBulletConfig_s *CsvBulletConfig::GetByID( int id ) 
{
	map<int, csvBulletConfig_s*>::iterator	it = BulletConfigMap.find( id ); 
	if( it == BulletConfigMap.end() ){ return NULL; } 
	return it->second;
}

map<int, csvBulletConfig_s*> *CsvBulletConfig::GetAll() 
{
	return &BulletConfigMap;
}

