#ifndef __CSV_MGR_H__
#define __CSV_MGR_H__

#include <vector>
#include <map>
#include "CardAtkPConfig.h"
#include "BuyList.h"
#include "RoleUpgrade.h"
#include "RewardGoodsList.h"
#include "GameTaskConfig.h"
#include "EnemyPlanetype.h"
#include "ShopList.h"
#include "BulletConfig.h"
#include "BossProConfig.h"
#include "../SQLite/sqlite3.h"

/************************************************************************/
/* CSV文件读写管理                                                      */
/************************************************************************/

// 单件声明模板，在类一开始声明时调用，最后的访问权限仍为private
#define SINGLETON_DECLEAR( className )	public:\
	static className	*CreateInstance(){ if( inst == NULL ) { inst = new (std::nothrow) className; } return inst; }\
	static className	*GetInst(){ if( inst == NULL ) { CreateInstance(); }	return inst; }\
	static void			DestroyInstance(){ if( inst != NULL ) { delete inst;	inst = NULL; } }\
										private:\
										static className	*inst;

// 单件定义模板。在cpp开始调用											
#define SINGLETON_DEFINE( className )	className	*className::inst = NULL;

//===================================================== csv
// csv 实现声明
#define CSV_DECLEAR( csvName )		public:\
										Csv##csvName *		GetCsv##csvName();\
									protected:\
										void ParseCsv##csvName( std::string &str ); \
										Csv##csvName		csv##csvName;\
									public:\

// csv 实现定义
#define CSV_DEFINE( csvName )		void CsvMgr::ParseCsv##csvName( std::string &str ) {\
										SplitString( str, ',', &splitted );\
										csv##csvName.SetData( splitted );\
									}\
									Csv##csvName *CsvMgr::GetCsv##csvName() {\
										return &csv##csvName;\
									}

// 加入csv文件
#define ADD_CSV( csvFileName, csvName )		csvFiles.push_back( csvFileName );\
	csvObj[csvFileName] = &CsvMgr::ParseCsv##csvName;

// 字符串长度
#define STR_LEN		1024


class CsvMgr {
SINGLETON_DECLEAR( CsvMgr );
// 文件名
CSV_DECLEAR( CardAtkPConfig );
CSV_DECLEAR( BuyList		);
CSV_DECLEAR( RoleUpgrade	);
CSV_DECLEAR( RewardGoodsList);
CSV_DECLEAR( GameTaskConfig );
CSV_DECLEAR( EnemyPlanetype );
CSV_DECLEAR( ShopList		);
CSV_DECLEAR( BulletConfig	);
CSV_DECLEAR( BossProConfig	);

public:
	// 搜索csv文件，找到csv的文件名
	void				FindAllCsv();

protected:
	CsvMgr();
	~CsvMgr();

	typedef void (CsvMgr::*func)( std::string& );	// 函数指针
	std::map<std::string, func>		csvObj;		// 文件名与响应函数对应map
	
	unsigned char *					tmpStr;		// 临时用的string
	std::vector<std::string>		splitted;	// 分割后的字符串vec
	
	// csv文件序列
	std::vector<std::string>		csvFiles;

private:
	// 读取csv文件
	void LoadCsv( const std::string &fileName );

	// 从buffer中读取一行
	// 返回当前的buff位置
	const unsigned char* GetLineFromBuffer( unsigned char *outBuffer, const unsigned char *buffer, int maxLen );
};

#define CSV_MGR()		CsvMgr::GetInst()

#define SW_LOCALIZED_STR(key) CSV_MGR()->GetCsvUiText()->GetByID(key)->txt

#endif	//! __CSV_MGR_H__