#include "CsvMgr.h"
#include "CsvCommon.h"
#include "../GameUtil/DecryptUtil.h"

#include "platform/CCFileUtils.h"

using namespace cocos2d;
using namespace std;

SINGLETON_DEFINE( CsvMgr )

CSV_DEFINE( CardAtkPConfig	);
CSV_DEFINE( BuyList			);
CSV_DEFINE( RoleUpgrade		);
CSV_DEFINE( RewardGoodsList );
CSV_DEFINE( GameTaskConfig  );
CSV_DEFINE( EnemyPlanetype  );
CSV_DEFINE( ShopList  );
CSV_DEFINE( BulletConfig );
CSV_DEFINE( BossProConfig );

CsvMgr::CsvMgr() 
{
	ADD_CSV( "CardAtkPConfig"		,CardAtkPConfig );
	ADD_CSV( "BuyList"				,BuyList		);
	ADD_CSV( "RoleUpgrade"			,RoleUpgrade	);
	ADD_CSV( "RewardGoodsList"		,RewardGoodsList);
	ADD_CSV( "GameTaskConfig"		,GameTaskConfig );
	ADD_CSV( "EnemyPlanetype"		,EnemyPlanetype );
	ADD_CSV( "ShopList"				,ShopList		);
	ADD_CSV( "BulletConfig"			,BulletConfig	);
	ADD_CSV( "BossProConfig"		,BossProConfig	);

	tmpStr = new (std::nothrow) unsigned char[STR_LEN];

	FindAllCsv();

}

CsvMgr::~CsvMgr() 
{
	delete []tmpStr;
}

void CsvMgr::FindAllCsv() 
{
	for (string it:csvFiles)
	{
		LoadCsv( it.c_str() );
	}

// 	for( vector<string>::iterator it = csvFiles.begin(); it != csvFiles.end(); it++ ) 
// 	{
// 		LoadCsv( (*it).c_str() );
// 	}
}

void CsvMgr::LoadCsv( const string &fileName2 )
{
	string name( fileName2 );
    //路径
	string fileName = "csv/";
	fileName += fileName2 + ".csv";
	ssize_t len;
	Data data;
	if(DecryptUtil::needDecrypt()) {
		data = DecryptUtil::decryptFile(FileUtils::getInstance()->getDataFromFile(fileName.c_str()));
	} else {
		data = FileUtils::getInstance()->getDataFromFile(fileName.c_str());
	}
	const unsigned char *buf = data.getBytes();
	len = data.getSize();
//	const unsigned char *buf = CCFileUtils::getInstance()->getFileData(
//		fileName.c_str(), "r", &len );
	//CCLog(CCFileUtils::sharedFileUtils()->fullPathForFilename(fileName.c_str()).c_str());
	if (len == 0)
	{
		//CCLog("swCsvMgr::LoadCsv open file failed : %s", fileName.c_str() );
		return;
	}
	const unsigned char *firstBuffer = buf;

	// 前两行不用读取
	buf = GetLineFromBuffer( tmpStr, buf, STR_LEN );
	buf = GetLineFromBuffer( tmpStr, buf, STR_LEN );

	func parseFunc = csvObj[name];
	string	splitStr;	// 存储需要拆分的字符串
	while ( *buf != '\0' && (buf - firstBuffer) < len ) {
		buf = GetLineFromBuffer( tmpStr, buf, STR_LEN );

		splitStr = reinterpret_cast<char *>(tmpStr);
		// 通过函数指针找到对应文件名的处理函数
		(this->*parseFunc)( splitStr );
	}

//	delete []firstBuffer;
}

const unsigned char* CsvMgr::GetLineFromBuffer( unsigned char *outBuffer,
	const unsigned char *buffer, int maxLen )
{
	// buffer hander
	CCAssert(outBuffer != NULL, "");
	CCAssert(buffer != NULL, "");

	int index = 0;
	const unsigned char *p = buffer;
	while ( (*p != '\0') && (index < maxLen - 1) )
	{
		// 读到行尾
		if (*p == '\n'){
			p ++;
			break;
		}

		*(outBuffer + index) = *p;
		index ++;
		p ++;
	}
	// 添加末尾
	*(outBuffer + index) = '\0';
	// 如果末尾有\r, 也去掉
	if (index > 0)
	{
		if (*(outBuffer + index - 1) == '\r')
		{
			*(outBuffer + index - 1) = '\0';
		}
	}
	return p;
}
