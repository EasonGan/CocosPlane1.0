#ifndef __BULLETCONFIG_H__
#define __BULLETCONFIG_H__

#include "cocos2d.h"
USING_NS_CC; 

struct csvBulletConfig_s{
	int				id;
	int				resId;
	float			scaleX;
	float			scaleY;
	float			attackCoe;
};

class CsvBulletConfig
{
public:

	typedef enum 
	{
	}CSVDataKey;

	CsvBulletConfig();
	~CsvBulletConfig();

	void					SetData( std::vector<std::string> &str );
	csvBulletConfig_s*		GetByID( int id );
	std::map<int, csvBulletConfig_s*> *GetAll();

protected:
	std::map<int, csvBulletConfig_s*>			BulletConfigMap;
	int		counter;
};
#endif