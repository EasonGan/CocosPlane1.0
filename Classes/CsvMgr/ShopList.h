#ifndef __SHOPLIST_H__
#define __SHOPLIST_H__

#include "cocos2d.h"
USING_NS_CC; 
   
struct csvShopList_s{
	int				GoodsID;
	int				BuyType;
	int				BuyNumber;
	int				BuyPrice;
};

class CsvShopList
{
public:

	typedef enum 
	{
	}CSVDataKey;

	CsvShopList();
	~CsvShopList();

	void					SetData( std::vector<std::string> &str );
	csvShopList_s*		GetByID( int id );
	std::map<int, csvShopList_s*> *GetAll();

protected:
	std::map<int, csvShopList_s*>			ShopListMap;
	int		counter;
};
#endif