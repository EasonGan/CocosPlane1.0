#include "ShopList.h"
#include "CsvCommon.h"

using namespace std;
using namespace cocos2d;

CsvShopList::CsvShopList() 
{
}

CsvShopList::~CsvShopList() 
{
	for ( map<int, csvShopList_s*>::iterator it = ShopListMap.begin(); it != ShopListMap.end(); it++ ) {
		delete it->second;
	}
}

void CsvShopList::SetData( vector<string> &str ) 
{
	csvShopList_s   *item = new csvShopList_s;
	counter = 0;

	item->GoodsID = StringToInt( str[counter] );
	counter++; 
	item->BuyType = StringToInt( str[counter] );
	counter++; 
	item->BuyNumber = StringToInt( str[counter] );
	counter++; 
	item->BuyPrice = StringToInt( str[counter] );
	counter++; 

	ShopListMap.insert( pair<int,csvShopList_s*>( atoi( str[0].c_str() ), item ) ); 
}
   
csvShopList_s *CsvShopList::GetByID( int id ) 
{
	map<int, csvShopList_s*>::iterator	it = ShopListMap.find( id ); 
	if( it == ShopListMap.end() ){ return NULL; } 
	return it->second;
}

map<int, csvShopList_s*> *CsvShopList::GetAll() 
{
	return &ShopListMap;
}

