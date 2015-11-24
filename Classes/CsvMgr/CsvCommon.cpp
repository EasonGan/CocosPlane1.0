#include "CsvCommon.h"
using namespace std;
using namespace cocos2d;

// 为了方便拆分字符串，定义以下全局变量
int				splitPos = 0;	// 新字段所在位置
vector<string>	tmpVec;			// 临时vec。用来存储中间过程的字符串

// SplitString
void SplitString( string &str, char splitter, vector<string> *vec ) {
	vec->clear();
	splitPos = 0;
	for( unsigned int i = 0; i <= str.size(); i++ )
	{
		splitPos = str.find( splitter, i );
		// 找不到匹配字符，说明已经是最后一个字段
		if( splitPos == -1 ) {
			// 注意此处 -i ,保证长度不会出错
			vec->push_back( str.substr( i, str.size() - i ) );
			break;
		}
		// 非尾端字段
		vec->push_back( str.substr( i, splitPos - i ) );

		// 准备拆解下一个字段
		i = splitPos;
	}
}

// StringToInt
int StringToInt( string &str ) {
	return atoi( str.c_str() );
}

// StringToFloat
float StringToFloat( string &str ) {
	return (float)atof( str.c_str() );
}

// StringToCCPoint
void StringToCCPoint( string &str, CCPoint *point ) {
	// 空则直接返回 ( 0, 0 )
	if( str == "" ) {
		point->x = 0;
		point->y = 0;
		return;
	}

	vector<float>	vec;
	StringToFloatVec( str, '|', &vec );
	point->x = vec[0];
	point->y = vec[1];
}

// StringToIntVec
void StringToIntVec( string &str, char splitter, vector<int> *vec ) {
	vec->clear();
	tmpVec.clear();

	// 拆分
	SplitString( str, splitter, &tmpVec );
	// 转int
	for( unsigned int i = 0; i< tmpVec.size(); i++ ) {
		vec->push_back( StringToInt( tmpVec[i] ) );
	}
}
void StringToIntVec( string &str, char splitter, vector<int> &vec )
 {
	int xxx = vec.size();
	vec.clear();
	tmpVec.clear();

	// 拆分
	SplitString( str, splitter, &tmpVec );
	// 转int
	for( unsigned int i = 0; i< tmpVec.size(); i++ ) 
	{
		vec.push_back( StringToInt( tmpVec[i] ) );
	}
}
// StringToFloatVec
void StringToFloatVec( string &str, char splitter, vector<float> *vec ) {
	vec->clear();
	tmpVec.clear();

	SplitString( str, splitter, &tmpVec );
	for( unsigned int i = 0; i < tmpVec.size(); i++ ) {
		vec->push_back( StringToFloat( tmpVec[i] ) );
	}
}

// StringToStringVec
void StringToStringVec( string &str, char splitter, vector<string> *vec ) {
	vec->clear();
	SplitString( str, splitter, vec );
}