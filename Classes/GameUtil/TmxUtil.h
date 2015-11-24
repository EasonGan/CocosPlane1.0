/********************************************************************
创建于:		2015/02/05  11:55	
文件名: 	E:\Cocos3.3_Project\PlaneGame\Classes\Util\TmxUtil.h 
作  者:		alex(alex)
修改于：	2015/02/05  11:55	

目  的:		Tilemap 读取敌机组合工具
*********************************************************************/
#ifndef __TmxUtil_H__
#define __TmxUtil_H__


#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class TmxUtil
{
public:
	static std::vector<std::string> SplitToStr(const std::string &s, char delim);
	static std::vector<int> SplitToInt(const std::string &s, char delim);
	static std::vector<float> SplitToFloat(const std::string &s, char delim);

	static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

	//解析TMX的“名称”中，解析出数据，int，float
	static int getIntDataByRate(const std::string& str1, const std::string& str2);
	static float getFloatDataByRate(const std::string& str1, const std::string& str2);

	//得到每个类型对应的概率
	static int getRateIndex(std::vector<float> vec);

	//以#分割出字符串数组
	static std::vector<std::string> getStrVec( const std::string& str);


};

#endif // __TmxUtil_H__