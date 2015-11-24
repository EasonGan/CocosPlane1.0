#ifndef __CSV_COMMON_H__
#define __CSV_COMMON_H__

#include "cocos2d.h"

USING_NS_CC;

/************************************************************************/
/* CSV文件读写公共函数                                                  */
/************************************************************************/

// 字符串拆分
void SplitString( std::string &str, char splitter, std::vector<std::string> *vec );
// 字符串转整形
int StringToInt( std::string &str );
// 字符串转浮点
float StringToFloat( std::string &str );
// 字符串转CCPoint
void StringToCCPoint( std::string &str, cocos2d::CCPoint *point );
// 字符串转整形vector
void StringToIntVec( std::string &str, char splitter, std::vector<int> *vec );
void StringToIntVec( std::string &str, char splitter, std::vector<int> &vec );
// 字符串转浮点vector
void StringToFloatVec( std::string &str, char splitter, std::vector<float> *vec );
// 字符串转字符串vector
void StringToStringVec( std::string &str, char splitter, std::vector<std::string> *vec );

#endif	//! __CSV_COMMON_H__