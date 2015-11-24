//
//  EnemyPlayController.h
//  PlaneGame2
//
//  Created by 叶飞 on 15/7/23.
//
//

#ifndef __PlaneGame2__EnemyPlayController__
#define __PlaneGame2__EnemyPlayController__

#include "cocostudio/CocoStudio.h"
#include "BasePlane/BasePlane.h"
#include "DropGoods.h"

USING_NS_CC;

#define SG_M(TYPE, M)\
public:\
void set_##M( const TYPE& in_##M ) {\
M##_ = in_##M;\
}\
TYPE M() {\
return M##_;\
}\
private:\
TYPE M##_;

#define MY_CREATE_FUNC(__TYPE__) \
static __TYPE__* create() \
{ \
__TYPE__ *pRet = new(std::nothrow) __TYPE__(); \
if (pRet) \
{ \
pRet->autorelease(); \
return pRet; \
} \
else \
{ \
delete pRet; \
pRet = NULL; \
return NULL; \
} \
}

class WeaponInfoObj : public cocos2d::Ref {
    ~WeaponInfoObj() {
        if(this->childBarrageRule != nullptr) {
            this->childBarrageRule->release();
        }
    }
public:
    MY_CREATE_FUNC(WeaponInfoObj);
    bool initData(rapidjson::Value& value);
   
    SG_M(int, barrageType);
    SG_M(int, fireCount);
    SG_M(float, fireGap);
    SG_M(float, fireAngle);
    SG_M(int, bulletNumber);
    SG_M(float, barrageFireGap);
    SG_M(float, fireSpeed);
    SG_M(float, fireChangedAngle);
    SG_M(float, fireMaxAngle);
    SG_M(float, bulletAngle);
    SG_M(std::string, bulletNumberList);
    SG_M(float, bulletMinGap);
    SG_M(float, bulletGapAmplitude);
    SG_M(float, bulletGap);
    SG_M(float, bulletYGap);
    SG_M(float, distanceToWeaponX);
    SG_M(float, distanceToWeaponY);
    SG_M(bool, fireRightNow);
    SG_M(bool, followWeaponFlag);
    SG_M(float , fireDuration);
    SG_M(float, waitDuration);
    SG_M(std::string, bullet_resIndex);
    SG_M(float, bullet_scaleX);
    SG_M(float, bullet_scaleY);
    SG_M(float, bullet_lifeTime);
    SG_M(float, bullet_rotateSpeed);
    SG_M(float, bullet_rotateSpeedPublic);
    SG_M(float, bullet_firstDuration);
    SG_M(float, bullet_waitDuration);
    SG_M(float, bullet_firstAcceleration);
    SG_M(float, bullet_firstMostSpeed);
    SG_M(float, bullet_secondSpeed);
    SG_M(float, bullet_secondAcceleration);
    SG_M(float, bullet_secondMostSpeed);
    SG_M(int , bullet_shrinkNumber);
    SG_M(float, bullet_invisibleDuration);
    SG_M(float, bullet_invisibleSubDuration);
    SG_M(float, x);
    SG_M(float, y);
public:
    WeaponInfoObj* childBarrageRule;
};

class WeaponInfoObjList : public cocos2d::Ref
{
    ~WeaponInfoObjList();
public:
    MY_CREATE_FUNC(WeaponInfoObjList);
    Vector<WeaponInfoObj*> weapon_vector;
};

class EventInfoObj : public cocos2d::Ref
{
public:
    MY_CREATE_FUNC(EventInfoObj);
    SG_M(float, event_time);
    SG_M(std::string, func_str);
};

class TrajectoryConfigData : public cocos2d::Ref
{
    ~TrajectoryConfigData();
public:
    MY_CREATE_FUNC(TrajectoryConfigData);
    bool initData(rapidjson::Value& value);
    SG_M(std::vector<Vec2>, point_list);
    SG_M(float,time);
    SG_M(bool, isLoop);
    SG_M(float,stayTime);
    SG_M(Vector<EventInfoObj*>, event_list);
};

class TrajectoryConfigDataList : public cocos2d::Ref
{
    ~TrajectoryConfigDataList();
public:
    MY_CREATE_FUNC(TrajectoryConfigDataList);
    SG_M(Vector<TrajectoryConfigData*>,tre_vector);
};

class LevelPlaneConfigData : public cocos2d::Ref
{
public:
    MY_CREATE_FUNC(LevelPlaneConfigData);
    bool initData(rapidjson::Value& value);
    SG_M (float, attack);
    SG_M (std::string, barrageArr);
    SG_M (std::string, dropArr);
    SG_M (int, health);
    SG_M (int, name);
    SG_M (int, score);
    SG_M (float, speed);
    SG_M (Vec2, startPoint);
    SG_M (std::string, trajectoryID);
};

class LevelPlaneConfigDataList : public cocos2d::Ref
{
    ~LevelPlaneConfigDataList();
public:
    MY_CREATE_FUNC(LevelPlaneConfigDataList);
    Vector<LevelPlaneConfigData*> config_vector;
    bool initData(rapidjson::Value& value);

protected:
//    int directNextBatch;
};

class EnemyPlaneController : public cocos2d::Ref
{
protected:
    static const unsigned int GAME_STATE_PLAYING = 1;
    static const unsigned int GAME_STATE_INIT = 2;
    static const unsigned int GAME_STATE_PAUSE = 3;
    static const unsigned int GAME_STATE_DESTROY = 4;
    static const unsigned int GAME_STATE_FIGHTING = 5;

    float start_wait_time;
    float waitTime;
    float matrix_fly_speed;
    float fly_size;
    bool sendPause;
    int curLevel;
    float gift_plane_send_interval;
    int gift_plane_num;
    Vector<LevelPlaneConfigDataList*> level_plane_config_vector;
    Map<std::string,TrajectoryConfigDataList*> trajectory_map;
    Map<std::string,WeaponInfoObjList*> weapon_map;

    Scheduler* _scheduler;
    void pauseLoop();
    void resumeLoop();
    void endAllSend();
    void endLoop();
    unsigned int game_state;
    bool parserTrajectoryFile();
    bool parserSpecialTrajectoryFile();
    bool parseTrajectory(std::string file);
    bool parserWeaponFile();
    std::string getTrackConfigFileName(int levelNum);
    std::string getSpecialTrackConfigFileName(int levelNum);
    std::string getEnemyConfigFileName(int levelNum);
    std::string getSpecialEnemyConfigFileName(int levelNum);
    Vector<LevelPlaneConfigDataList*>::iterator config_index;
    Vector<LevelPlaneConfigData*> special_vectory;
    Vector<LevelPlaneConfigData*> remove_special_vectory;
    void createPlaneWitchConfig(LevelPlaneConfigData* config);
    void sendPlane();
    void levelPaneEmpty();
    void sendGiftPlane();
    void createOneGiftPlane(int planeId,int actionId,int curLevel,int drop);
    void setGiftPlaneMoveIn(Node* node);
    void updateUserFlySize(float dt);
    void updateSpecialSize(float dt);
    void sendGiftPlaneLoop(float dt);
    void startSpecialLevelWithConfig();
    void overTheGame(bool win);
    void delayOverTheGame(bool win);
    bool startLevel(std::string filePath);
    void pauseSend();
    void resumeSend();
public:
    bool isGuideLevel();
    void pauseSendGiftPlane();
    void resumeSendGiftPlane();
    EnemyPlaneController();
    ~EnemyPlaneController();
    Vector<BasePlane*> send_plane_vector;
    Vector<BasePlane*> send_gift_plane_vector;
    bool isFirst;
    bool isPause;
    bool pauseSendGift;
    bool isSpecialLevel;

    bool isFirstDrop;
    int groupNum;
    int guide_group_num_1;
    int guide_group_num_2;

    bool isNeedGuide();
    void startGuide();

    void startGuideHudun();
    void startGuideDazhao();

    void pauseSelfAndChild(Node* node);
    void resumeSelfAndChild(Node* node);
    void mainPlaneDeathPause();
    void mainPlaneDeathResume();
    static void destroyInstance();
    void endGiftPlaneSend(Ref* obj);
    TrajectoryConfigDataList* getTrajectoryConfigDataFromId(std::string id);
    WeaponInfoObjList* getWeaponConfigDataFromId(std::string id);
    void exitGame();
    void waitSend(float time);
    static EnemyPlaneController* getInstance();
    bool startLevelWithConfigFile(int levelNum = 1,bool isFirst = true);
    void destroyCallBack(BasePlane* basePlane);
    void giftPlaneDestroyCallBack(BasePlane* basePlane);
    BasePlane* getTheClosePlane(const Vec2 &pos);
    bool isMoreClose(const Vec2 &pos,const Vec2 &targetPos,const Vec2 &targetPos2);
};

#endif /* defined(__PlaneGame2__EnemyPlayController__) */
