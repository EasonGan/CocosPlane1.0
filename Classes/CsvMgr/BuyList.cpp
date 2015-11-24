#include "BuyList.h"
#include "CsvCommon.h"

using namespace std;
using namespace cocos2d;

CsvBuyList::CsvBuyList() 
{
}

CsvBuyList::~CsvBuyList() 
{
	for ( map<int, csvBuyList_s*>::iterator it = BuyListMap.begin(); it != BuyListMap.end(); it++ ) {
		delete it->second;
	}
}

void CsvBuyList::SetData( vector<string> &str ) 
{
	csvBuyList_s   *item = new (std::nothrow) csvBuyList_s;
	counter = 0;

	item->GoodsId = StringToInt( str[counter] );
	counter++; 
	item->GoodsName =  str[counter];
	counter++; 
	item->Price = StringToFloat( str[counter] );
	counter++; 
	item->Gold = StringToInt( str[counter] );
	counter++; 
	item->data =  StringToInt( str[counter] );;
	counter++; 
	item->number = StringToInt( str[counter] );
	counter++; 
	item->buyType = StringToInt( str[counter] );
	counter++; 
	item->GoodsDes =  str[counter];
	counter++; 

	BuyListMap.insert( pair<int,csvBuyList_s*>( atoi( str[0].c_str() ), item ) ); 
}

csvBuyList_s *CsvBuyList::GetByID( int id ) 
{
	map<int, csvBuyList_s*>::iterator	it = BuyListMap.find( id ); 
	if( it == BuyListMap.end() ){ return NULL; } 
	return it->second;
}

map<int, csvBuyList_s*> *CsvBuyList::GetAll() 
{
	return &BuyListMap;
}

