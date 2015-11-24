#include "QuickStoneTable.h"
#include "CsvCommon.h"

using namespace std;
using namespace cocos2d;

CsvQuickStoneTable::CsvQuickStoneTable() 
{
}

CsvQuickStoneTable::~CsvQuickStoneTable() 
{
	for ( map<int, csvQuickStoneTable_s*>::iterator it = QuickStoneTableMap.begin(); it != QuickStoneTableMap.end(); it++ ) {
		delete it->second;
	}
}

void CsvQuickStoneTable::SetData( vector<string> &str ) 
{
	csvQuickStoneTable_s   *item = new csvQuickStoneTable_s;
	counter = 0;

	item->id = StringToInt( str[counter] );
	counter++; 
	item->grade = StringToInt( str[counter] );
	counter++; 
	item->implantPrice = StringToInt( str[counter] );
	counter++; 
	item->compoundPrice = StringToInt( str[counter] );
	counter++; 
	item->compoundNeedNum = StringToInt( str[counter] );
	counter++; 
	item->haveNum = StringToInt( str[counter] );
	counter++;
	item->atk = StringToInt(str[counter]);
	counter++;

	QuickStoneTableMap.insert( pair<int,csvQuickStoneTable_s*>( atoi( str[0].c_str() ), item ) ); 
}

csvQuickStoneTable_s *CsvQuickStoneTable::GetByID( int id ) 
{
	map<int, csvQuickStoneTable_s*>::iterator	it = QuickStoneTableMap.find( id ); 
	if( it == QuickStoneTableMap.end() ){ return NULL; } 
	return it->second;
}

map<int, csvQuickStoneTable_s*> *CsvQuickStoneTable::GetAll() 
{
	return &QuickStoneTableMap;
}

