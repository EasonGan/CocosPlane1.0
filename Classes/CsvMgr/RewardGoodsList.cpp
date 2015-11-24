#include "RewardGoodsList.h"
#include "CsvCommon.h"

using namespace std;
using namespace cocos2d;

CsvRewardGoodsList::CsvRewardGoodsList() 
{
}

CsvRewardGoodsList::~CsvRewardGoodsList() 
{
	for ( map<int, csvRewardGoodsList_s*>::iterator it = RewardGoodsListMap.begin(); it != RewardGoodsListMap.end(); it++ ) {
		delete it->second;
	}
}

void CsvRewardGoodsList::SetData( vector<string> &str ) 
{
	csvRewardGoodsList_s   *item = new csvRewardGoodsList_s;
	counter = 0;

	item->GoodsID = StringToInt( str[counter] );
	counter++; 
	item->GoodsNum = StringToInt( str[counter] );
	counter++; 
	item->GoodsProb = StringToInt( str[counter] );
	counter++; 
	item->GoodsName =  str[counter] ;
	counter++; 

	RewardGoodsListMap.insert( pair<int,csvRewardGoodsList_s*>( atoi( str[0].c_str() ), item ) ); 
}

csvRewardGoodsList_s *CsvRewardGoodsList::GetByID( int id ) 
{
	map<int, csvRewardGoodsList_s*>::iterator	it = RewardGoodsListMap.find( id ); 
	if( it == RewardGoodsListMap.end() ){ return NULL; } 
	return it->second;
}

map<int, csvRewardGoodsList_s*> *CsvRewardGoodsList::GetAll() 
{
	return &RewardGoodsListMap;
}

