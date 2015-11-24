#ifndef __GAMEPAY_MSG_H__
#define __GAMEPAY_MSG_H__

#include "cocos2d.h"

USING_NS_CC;


/************************************************************************/
/* 游戏支付管理                                                        */
/************************************************************************/

enum PayKay
{
	PayKay_Test1,						// 支付测试
	PayKay_Test2,						// 支付测试
	PayKay_Test3,						// 支付测试
	PayKay_Test4,						// 支付测试
	PayKay_Test5,						// 支付测试
	PayKay_Test6,						// 支付测试
	PayKay_Test7,						// 支付测试
	PayKay_Test8,						// 支付测试
	PayKay_Test9,						// 支付测试

	Paykey_End
};

// 计费信息
struct PayInfoStr
{
	std::string		payCode;	// 计费代码
	std::string		payName;	// 计费名称
	int				payPrice;	// 计费金额
};


class GamePayMsg
{
public:
	static GamePayMsg* getInstance()
	{
		static GamePayMsg Instance;
		return &Instance;
	}

	GamePayMsg();
	~GamePayMsg();

	void GamePay( PayKay key );

	void JniCallJavaPay( std::string  payInfo );

private:

};





#endif