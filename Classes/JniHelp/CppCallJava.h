/********************************************************************
创建于:		2015/02/28  14:09	
文件名: 	E:\Cocos3.3_Project\PlaneGame\Classes\Util\CppCallJava.h 
作  者:		alex()
修改于：	2015/02/28  14:09	

目  的:		C++调用JAVA的功能处理
*********************************************************************/
#ifndef __CppCallJava_H__
#define __CppCallJava_H__


#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class CppCallJava : public Ref
{
public:
	static CppCallJava* getInstance()
	{
		static CppCallJava instance;
		return &instance;
	}

	//手机震动,持续时间
	void vibrate(long time);

	//手机震动，震动2次
	void vibrateWithPattern();

	//获得系统时间
	std::string getSystemTime( void );

	// 第三方购买调用
	void JniCallBuyMethod(std::string payCode, int price);

};

#endif // __CppCallJava_H__