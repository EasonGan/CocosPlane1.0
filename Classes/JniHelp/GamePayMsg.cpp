#include "GamePayMsg.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)//预编译 判断是否是android平台
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#endif

PayInfoStr payInfo[Paykey_End] = 
{
	{"001","金币", 400},
	{"002","金币", 400},
	{"003","金币", 400},
	{"004","金币", 400},
	{"005","金币", 400},
	{"006","金币", 400},
	{"007","金币", 400},
	{"008","金币", 400},
	{"009","金币", 400}
};

GamePayMsg::GamePayMsg()
{

}

GamePayMsg::~GamePayMsg()
{

}

void GamePayMsg::GamePay( PayKay key )
{
	switch (key)
	{
	case PayKay_Test1:
		break;
	case PayKay_Test2:
		break;
	case PayKay_Test3:
		break;
	case PayKay_Test4:
		break;
	case PayKay_Test5:
		break;
	case PayKay_Test6:
		break;
	case PayKay_Test7:
		break;
	case PayKay_Test8:
		break;
	case PayKay_Test9:
		break;
	case Paykey_End:
		break;
	default:
		break;
	}
	char payPriceStr[5]; /*itoa(  ,payPriceStr, 10);*/
	sprintf(payPriceStr, "%d", payInfo[key].payPrice);

	std::string curPayInfo = payInfo[key].payCode + payPriceStr;
	JniCallJavaPay( curPayInfo );
}

void GamePayMsg::JniCallJavaPay( std::string payInfo )
{
#if ( CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_IOS )

#elif ( CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID )
	JniMethodInfo minfo;

	CCLog("### java func JniCallJavaPay()### ");
	if ( JniHelper::getStaticMethodInfo(minfo,"org/weme/game/JniHelp/CppCallJava","JniCallJavaPay","(Ljava/lang/String;)V"))
	{
		CCLog("### java func JniCallJavaPay()### ");
		jstring  j_payinfo = minfo.env->NewStringUTF( payInfo.c_str() ); 
		minfo.env->CallStaticObjectMethod(
			minfo.classID, 
			minfo.methodID, 
			j_payinfo);
		minfo.env->DeleteLocalRef(minfo.classID);
		minfo.env->DeleteLocalRef(j_payinfo);    
	}
	else 
	{
		CCLog("### java func JniCallJavaPay() is not exist ### ");
	}


#endif
	//CCLog( "CppCallJava::JniCallJavaPay" );
}

