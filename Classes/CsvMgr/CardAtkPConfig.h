#ifndef __CARDATKPCONFIG_H__
#define __CARDATKPCONFIG_H__

#include "cocos2d.h"
USING_NS_CC; 

struct csvCardAtkPConfig_s{
	int				id;
	int				CardType;
	int				CardId;
	float			CardBassAtk;
	int				islock;		// 1为解锁 0为锁定
	int				grade;
	std::string		CardDes;
	std::string		CardName;
};

class CsvCardAtkPConfig
{
public:


	CsvCardAtkPConfig();
	~CsvCardAtkPConfig();

	void					SetData( std::vector<std::string> &str );
	csvCardAtkPConfig_s*		GetByID( int id );
	std::map<int, csvCardAtkPConfig_s*> *GetAll();

protected:
	std::map<int, csvCardAtkPConfig_s*>			CardAtkPConfigMap;
	int		counter;
};
#endif