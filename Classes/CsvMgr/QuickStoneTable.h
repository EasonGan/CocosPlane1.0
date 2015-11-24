#ifndef __QUICKSTONETABLE_H__
#define __QUICKSTONETABLE_H__

#include "cocos2d.h"
USING_NS_CC; 

struct csvQuickStoneTable_s{
	int				id;
	int				grade;
	int				implantPrice;
	int				compoundPrice;
	int				compoundNeedNum;
	int				haveNum;
	int				atk;
};

class CsvQuickStoneTable
{
public:

	typedef enum 
	{
	}CSVDataKey;

	CsvQuickStoneTable();
	~CsvQuickStoneTable();

	void					SetData( std::vector<std::string> &str );
	csvQuickStoneTable_s*		GetByID( int id );
	std::map<int, csvQuickStoneTable_s*> *GetAll();

protected:
	std::map<int, csvQuickStoneTable_s*>			QuickStoneTableMap;
	int		counter;
};
#endif