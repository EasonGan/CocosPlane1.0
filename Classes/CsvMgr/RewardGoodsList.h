#ifndef __REWARDGOODSLIST_H__
#define __REWARDGOODSLIST_H__

#include "cocos2d.h"
USING_NS_CC; 

struct csvRewardGoodsList_s{
	int				GoodsID;
	int				GoodsNum;
	int				GoodsProb;
	std::string				GoodsName;
};

class CsvRewardGoodsList
{
public:

	typedef enum 
	{
	}CSVDataKey;

	CsvRewardGoodsList();
	~CsvRewardGoodsList();

	void					SetData( std::vector<std::string> &str );
	csvRewardGoodsList_s*		GetByID( int id );
	std::map<int, csvRewardGoodsList_s*> *GetAll();

protected:
	std::map<int, csvRewardGoodsList_s*>			RewardGoodsListMap;
	int		counter;
};
#endif