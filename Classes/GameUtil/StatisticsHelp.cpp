//
// Created by 叶飞 on 15/10/20.
//

#include "StatisticsHelp.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    #include "../../cocos2d/cocos/platform/android/jni/JniHelper.h"
#endif

void StatisticsHelp::buy(int item, int num, int price)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        char key[5];
        sprintf(key, "%d", item);
        StatisticsHelp::callJava("buy",key,"1","1");
#endif
}

void StatisticsHelp::use(int item, int num, int price)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        char key[5];
        sprintf(key, "%d", item);
        StatisticsHelp::callJava("use",key,"1","1");
#endif
}

void StatisticsHelp::buy(const char* item,int num,int price)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    StatisticsHelp::callJava("buy",item,"1","1");
#endif
}
void StatisticsHelp::use(const char* item,int num,int price)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    StatisticsHelp::callJava("use",item,"1","1");
#endif
}

void StatisticsHelp::startLevel(int level)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    char lv[5];
    sprintf(lv, "%d", level);
    StatisticsHelp::callJava("startLevel",lv,"","");
#endif
}

void StatisticsHelp::failLevel(int level)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    char lv[5];
    sprintf(lv, "%d", level);
    StatisticsHelp::callJava("failLevel",lv,"","");
#endif
}

void StatisticsHelp::finishLevel(int level)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    char lv[5];
    sprintf(lv, "%d", level);
    StatisticsHelp::callJava("finishLevel",lv,"","");
#endif
}

void StatisticsHelp::pay(int money,int gameMoney,int payChannel)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    char m[5];
    sprintf(m, "%d", money);
    StatisticsHelp::callJava("pay",m,"1","5");
#endif
}

void StatisticsHelp::callJava(const char* p1,const char* p2,const char* p3,const char* p4) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

//        JniMethodInfo info;
//
//        if (JniHelper::getStaticMethodInfo(info,
//            "org/weme/game/JniHelp/CppCallJava",
//            "callFunction",
//            "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
//        {
//            jstring str1 = info.env->NewStringUTF(p1);
//            jstring str2 = info.env->NewStringUTF(p2);
//            jstring str3 = info.env->NewStringUTF(p3);
//            jstring str4 = info.env->NewStringUTF(p4);
//
//            info.env->CallStaticVoidMethod(info.classID, info.methodID,
//                str1,str2,str3,str4);
//            info.env->DeleteLocalRef(str1);
//            info.env->DeleteLocalRef(str2);
//            info.env->DeleteLocalRef(str3);
//            info.env->DeleteLocalRef(str4);
//            info.env->DeleteLocalRef(info.classID);
//        }
#endif
}
