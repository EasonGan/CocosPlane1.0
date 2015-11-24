#ifndef __ROLEUPGRADE_H__
#define __ROLEUPGRADE_H__

#include "cocos2d.h"
USING_NS_CC; 

struct csvRoleUpgrade_s{
	int				roleId;
	int				roleGrade;
	float			BassAtk;
	int				isLock;
	int				grade;
};

class CsvRoleUpgrade
{
public:

	typedef enum 
	{
	}CSVDataKey;

	CsvRoleUpgrade();
	~CsvRoleUpgrade();

	void					SetData( std::vector<std::string> &str );
	csvRoleUpgrade_s*		GetByID( int id );
	std::map<int, csvRoleUpgrade_s*> *GetAll();

protected:
	std::map<int, csvRoleUpgrade_s*>			RoleUpgradeMap;
	int		counter;
};
#endif