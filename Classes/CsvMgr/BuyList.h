#ifndef __BUYLIST_H__
#define __BUYLIST_H__

#include "cocos2d.h"
USING_NS_CC; 

struct csvBuyList_s{
	int				GoodsId;
	std::string		GoodsName;
	std::string		GoodsDes;
	float			Price;
	int				Gold;
	int				data;
	int				number;
	int				buyType;
};

class CsvBuyList
{
public:

	typedef enum 
	{
	}CSVDataKey;

	CsvBuyList();
	~CsvBuyList();

	void					SetData( std::vector<std::string> &str );
	csvBuyList_s*		GetByID( int id );
	std::map<int, csvBuyList_s*> *GetAll();

protected:
	std::map<int, csvBuyList_s*>			BuyListMap;
	int		counter;
};
#endif