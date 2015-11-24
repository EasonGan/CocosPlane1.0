#pragma once

#include "CppCallJava.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)//预编译 判断是否是android平台
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#endif

#include "GameCommon/NewDataMgr.h"
#include "GameUI/GameSceneMgr.h"
#include "GameCommon/GameData.h"


void CppCallJava::vibrate( long time )
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo minfo;
	
	bool isHave = JniHelper::getStaticMethodInfo(minfo,"org/weme/game/JniHelp/CppCallJava","vibrate","(I)V");
	if (isHave)
	{
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, time);
		minfo.env->DeleteLocalRef(minfo.classID);
	}
	else 
	{
		CCLog("### java func vibrate() is not exist ### ");
	}

#endif
}

void CppCallJava::vibrateWithPattern()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo minfo;

	bool isHave = JniHelper::getStaticMethodInfo(minfo,"org/weme/game/JniHelp/CppCallJava","vibrateWithPattern","()V");
	if (isHave)
	{
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, time);
		minfo.env->DeleteLocalRef(minfo.classID);
	}
	else 
	{
		CCLog("### java func vibrateWithPattern() is not exist ### ");
	}

#endif
}

std::string  CppCallJava::getSystemTime( void )
{
	std::string curTime;

	//CCLog("### java func getSystemTime()### ");
#if ( CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 )

	time_t curtime=time(0); 
	tm tim =*localtime(&curtime); 
	int  day, mon, year, hour, min, sec; 
	day = tim.tm_mday;
	mon = tim.tm_mon;
	year = tim.tm_year;
	hour = tim.tm_hour;
	min = tim.tm_min;
	sec = tim.tm_sec;
	
	char curTimeBuffer[256];
	sprintf_s( curTimeBuffer, sizeof(curTimeBuffer), "%d||%d||%d||%d||%d||%d||", year, mon, day, hour, min, sec );
	curTime = curTimeBuffer;

#elif ( CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID )
	JniMethodInfo minfo;

	CCLog("### java func getSystemTime()### ");
	if ( JniHelper::getStaticMethodInfo(minfo,"org/weme/game/JniHelp/CppCallJava","getCurSystemTime","()Ljava/lang/String;"))
	{
		CCLog("### java func getSystemTime()### ");
		jstring time = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		curTime = (char*)minfo.env->GetStringUTFChars(time, NULL);
		minfo.env->DeleteLocalRef(minfo.classID);
	}
	else 
	{
		CCLog("### java func getSystemTime() is not exist ### ");
	}

#else
	curTime = "2015||5||17||12||16||32||";
#endif
	
	CCLOG("CppCallJava::getSystemTime time:%s", curTime.c_str());
	return curTime;
}

void CppCallJava::JniCallBuyMethod(std::string payCode, int price)
{
#if ( CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID )

	JniMethodInfo minfo;

	log("### java func JniCallBuyInterface()### ");
	if (JniHelper::getStaticMethodInfo(minfo, 
		"org/weme/game/JniHelp/CppCallJava", 
		"toBuyMethod", 
		"(Ljava/lang/String;I)V"))
	{
		jstring  l_strPayCode = minfo.env->NewStringUTF(payCode.c_str());

		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID,
			l_strPayCode,
			price);
		minfo.env->DeleteLocalRef(l_strPayCode);
		minfo.env->DeleteLocalRef(minfo.classID);
	}
	else
	{
		log("### java func JniCallBuyInterface() is not exist ### ");
	}

#else
	if (Scene_Type_PlayInterface == GameSceneMgr::getInstance()->getCurScen())
	{
		GameData::getInstance()->GameResume();
	}
	//NewDataMgr::getInstance()->mGameBuySystem->sendBuySuccessMsg();
	NewDataMgr::getInstance()->mGameBuySystem->setNotionState(true);

#endif
}

