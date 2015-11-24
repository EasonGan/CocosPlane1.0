

#include "JavaCallCpp.h"
#include "../GameCommon/ActionRecord.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "cocos2d.h"
#include "GameCommon/ActionRecord.h"
#include "GameCommon/NewDataMgr.h"
#include "GameCommon/GameData.h"
#include "GameUI/GameSceneMgr.h"
USING_NS_CC;

extern "C"
{
	char* jstringTostring(JNIEnv* env, jstring jstr) 
	{ 
		char* rtn = NULL;
		if(!jstr)
			return NULL;
		jclass clsstring = env->FindClass("java/lang/String");
		jstring strencode = env->NewStringUTF("utf-8");
		jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
		jbyteArray barr= (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
		jsize alen = env->GetArrayLength(barr);
		jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
		if (alen > 0)
		{ 
			rtn = (char*)malloc(alen + 1);
			memcpy(rtn, ba, alen);
			rtn[alen] = 0;
		} 
		env->ReleaseByteArrayElements(barr, ba, 0);
		return rtn;
	}

	JNIEXPORT void JNICALL Java_org_weme_game_JniHelp_JavaCallCpp_passDevicesInfo
		(JNIEnv *env, jclass thiz, jstring devicesId)
	{
		char *nDevicesId = jstringTostring(env, devicesId);

		log("javacallcpp	devicesId[%s]", nDevicesId );
		userAction::getInstance()->setDevicesId( nDevicesId );

		free(nDevicesId);
	}

	JNIEXPORT void JNICALL Java_org_weme_game_JniHelp_JavaCallCpp_buySuccessNotion
		(JNIEnv *env, jclass thiz)
	{
		if (Scene_Type_PlayInterface == GameSceneMgr::getInstance()->getCurScen())
		{
			GameData::getInstance()->GameResume();
		}
		NewDataMgr::getInstance()->mGameBuySystem->setNotionState(true);
		log("javacallcpp buySuccessNotion");
	}
}


#endif
