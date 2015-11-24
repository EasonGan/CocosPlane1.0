

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	#ifndef _Included_JavaCallCpp
	#define _Included_JavaCallCpp
#include "../../cocos/platform/android/jni/JniHelper.h"

		#ifdef __cplusplus
		extern "C" {
		#endif

 
			JNIEXPORT void JNICALL Java_org_weme_game_JniHelp_JavaCallCpp_passDevicesInfo
				(JNIEnv *env, jclass thiz, jstring devicesId);

			JNIEXPORT void JNICALL Java_org_weme_game_JniHelp_JavaCallCpp_buySuccessNotion
				(JNIEnv *env, jclass thiz);

		#ifdef __cplusplus
		}
		#endif
		
	#endif
	

#endif