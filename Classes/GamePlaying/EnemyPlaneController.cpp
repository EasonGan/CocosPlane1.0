//
//  EnemyPlayController.cpp
//  PlaneGame2
//
//  Created by 叶飞 on 15/7/23.
//
//

#include "Layer/UIController.h"
#include "Layer/PhysicsLayer.h"
#include "../GameCommon/GameData.h"
#include "BasePlane/PlaneFactory.h"
#include "BasePlane/PlaneAction.h"
#include "../GameUI/GameMainLayer.h"
#include "Weapon/EditorWeapon.h"
#include <stdlib.h>
#include "../GameUtil/StatisticsHelp.h"


bool WeaponInfoObj::initData(rapidjson::Value &value) {
    if(!value["barrageType"].IsNull()) {
        this->set_barrageType(value["barrageType"].GetInt());
    }
    if(!value["fireSpeed"].IsNull()) {
        this->set_fireSpeed(value["fireSpeed"].GetInt());
    }
    if(!value["fireCount"].IsNull()) {
        this->set_fireCount(value["fireCount"].GetInt());
    }
    if(!value["fireGap"].IsNull()) {
        this->set_fireGap(value["fireGap"].GetDouble());
    }
    if(!value["fireAngle"].IsNull()) {
        this->set_fireAngle(value["fireAngle"].GetDouble());
    }
    if(!value["followWeaponFlag"].IsNull()) {
        this->set_followWeaponFlag(value["followWeaponFlag"].GetBool());
    }
    if(!value["bulletNumber"].IsNull()) {
        this->set_bulletNumber(value["bulletNumber"].GetInt());
    }
    if(!value["barrageFireGap"].IsNull()) {
        this->set_barrageFireGap(value["barrageFireGap"].GetDouble());
    }
    if(!value["fireChangedAngle"].IsNull()) {
        this->set_fireChangedAngle(value["fireChangedAngle"].GetDouble());
    }
    if(!value["fireMaxAngle"].IsNull()) {
        this->set_fireMaxAngle(value["fireMaxAngle"].GetDouble());
    }
    if(!value["bulletAngle"].IsNull()) {
        this->set_bulletAngle(value["bulletAngle"].GetDouble());
    }
    if(!value["bulletNumberList"].IsNull()) {
        this->set_bulletNumberList(value["bulletNumberList"].GetString());
    }
    if(!value["bulletMinGap"].IsNull()) {
        this->set_bulletMinGap(value["bulletMinGap"].GetDouble());
    }
    if(!value["bulletGapAmplitude"].IsNull()) {
        this->set_bulletGapAmplitude(value["bulletGapAmplitude"].GetDouble());
    }
    if(!value["bulletGap"].IsNull()) {
        this->set_bulletGap(value["bulletGap"].GetDouble());
    } else {
        this->set_bulletGap(0);
    }
    if(!value["bulletYGap"].IsNull()) {
        this->set_bulletYGap(value["bulletYGap"].GetDouble());
    } else {
        this->set_bulletYGap(0);
    }
    if(!value["distanceToWeaponX"].IsNull()) {
        this->set_distanceToWeaponX(value["distanceToWeaponX"].GetDouble());
    }
    if(!value["distanceToWeaponY"].IsNull()) {
        this->set_distanceToWeaponY(value["distanceToWeaponY"].GetDouble());
    }
    if(!value["fireRightNow"].IsNull()) {
        this->set_fireRightNow(value["fireRightNow"].GetBool());
    }
    if(!value["fireDuration"].IsNull()) {
        this->set_fireDuration(value["fireDuration"].GetDouble());
    }
    if(!value["waitDuration"].IsNull()) {
        this->set_waitDuration(value["waitDuration"].GetDouble());
    }

    rapidjson::Value &bulletRule = value["bulletRule"];

    if(!bulletRule["resIndex"].IsNull()) {
        this->set_bullet_resIndex(bulletRule["resIndex"].GetString());
    }
    if(!bulletRule["scaleX"].IsNull()) {
        this->set_bullet_scaleX(bulletRule["scaleX"].GetDouble());
    }
    if(!bulletRule["scaleY"].IsNull()) {
        this->set_bullet_scaleY(bulletRule["scaleY"].GetDouble());
    }
    if(!bulletRule["lifeTime"].IsNull()) {
        this->set_bullet_lifeTime(bulletRule["lifeTime"].GetDouble());
    }
    if(!bulletRule["rotateSpeed"].IsNull()) {
        this->set_bullet_rotateSpeed(bulletRule["rotateSpeed"].GetDouble());
    }
    if(!bulletRule["rotateSpeedPublic"].IsNull()) {
        this->set_bullet_rotateSpeedPublic(bulletRule["rotateSpeedPublic"].GetDouble());
    } else {
        this->set_bullet_rotateSpeedPublic(0);
    }
    if(!bulletRule["firstDuration"].IsNull()) {
        this->set_bullet_firstDuration(bulletRule["firstDuration"].GetDouble());
    }
    if(!bulletRule["waitDuration"].IsNull()) {
        this->set_bullet_waitDuration(bulletRule["waitDuration"].GetDouble());
    }
    if(!bulletRule["firstAcceleration"].IsNull()) {
        this->set_bullet_firstAcceleration(bulletRule["firstAcceleration"].GetDouble());
    } else {
        this->set_bullet_firstAcceleration(0);
    }
    if(!bulletRule["firstMostSpeed"].IsNull()) {
        this->set_bullet_firstMostSpeed(bulletRule["firstMostSpeed"].GetDouble());
    }
    if(!bulletRule["secondSpeed"].IsNull()) {
        this->set_bullet_secondSpeed(bulletRule["secondSpeed"].GetDouble());
    }
    if(!bulletRule["secondAcceleration"].IsNull()) {
        this->set_bullet_secondAcceleration(bulletRule["secondAcceleration"].GetDouble());
    } else {
        this->set_bullet_secondAcceleration(0);
    }
    if(!bulletRule["secondMostSpeed"].IsNull()) {
        this->set_bullet_secondMostSpeed(bulletRule["secondMostSpeed"].GetDouble());
    }
    if(!bulletRule["shrinkNumber"].IsNull()) {
        this->set_bullet_shrinkNumber(bulletRule["shrinkNumber"].GetInt());
    }
    if(!bulletRule["invisibleDuration"].IsNull()) {
        this->set_bullet_invisibleDuration(bulletRule["invisibleDuration"].GetDouble());
    }
    if(!bulletRule["invisibleSubDuration"].IsNull()) {
        this->set_bullet_invisibleSubDuration(bulletRule["invisibleSubDuration"].GetDouble());
    }
    if(!value["x"].IsNull()) {
        this->set_x(value["x"].GetDouble());
    }
    if(!value["y"].IsNull()) {
        this->set_y(value["y"].GetDouble());
    }
    if(!value["childBarrageRule"].IsNull()) {
        WeaponInfoObj* info = WeaponInfoObj::create();
        if(info->initData(value["childBarrageRule"])) {
            this->childBarrageRule = info;
            this->childBarrageRule->retain();
        } else {
            return false;
        }
    } else {
        this->childBarrageRule = nullptr;
    }
    return true;
};


WeaponInfoObjList::~WeaponInfoObjList() {
    this->weapon_vector.clear();
}

bool TrajectoryConfigData::initData(rapidjson::Value &value) {
    rapidjson::Value &pointList = value["pointList"];
    if (pointList.IsNull() || !pointList.IsArray()) {
        CCLOG("pointList err %s", "");
        return false;
    }

    rapidjson::Value &eventList = value["eventList"];
    if (pointList.IsNull() || !pointList.IsArray()) {
        CCLOG("eventList err %s", "");
        return false;
    }

    std::vector<Vec2> p_list;

    for (unsigned int i = 0; i < pointList.Size(); i++) {
        float x = pointList[i]["x"].GetDouble();
        float y = pointList[i]["y"].GetDouble();
        p_list.push_back(Vec2(x, y));
    }
    this->set_point_list(p_list);
    this->set_time(value["time"].GetDouble());
    this->set_stayTime(value["stayTime"].GetDouble());
    this->set_isLoop(value["isLoop"].GetBool());

    Vector<EventInfoObj *> e_list;
    for (unsigned int i = 0; i < eventList.Size(); i++) {
        EventInfoObj *event = EventInfoObj::create();
        rapidjson::Value &e = eventList[i];
        if (e["eventName"].IsNull() || e["eventTime"].IsNull()) {
            return false;
        }
        event->set_event_time(e["eventTime"].GetDouble());
        event->set_func_str(e["eventName"].GetString());
        e_list.pushBack(event);
    }
    this->set_event_list(e_list);
    return true;
};

TrajectoryConfigData::~TrajectoryConfigData() {
    this->point_list_.clear();
    this->event_list_.clear();
}

bool LevelPlaneConfigData::initData(rapidjson::Value &value) {
    this->set_attack(value["attack"].GetDouble());
    this->set_barrageArr(value["barrageArr"].GetString());
    this->set_dropArr(value["dropArr"].GetString());
    this->set_health(value["health"].GetInt());
    this->set_name(value["name"].GetInt());
    this->set_score(value["score"].GetInt());
    this->set_speed(value["speed"].GetDouble());
    this->set_startPoint(Vec2(value["startPoint"]["x"].GetDouble(), value["startPoint"]["y"].GetDouble()));
    this->set_trajectoryID(value["trajectoryID"].GetString());
    return true;
}

TrajectoryConfigDataList::~TrajectoryConfigDataList() {
    this->tre_vector_.clear();
}

bool LevelPlaneConfigDataList::initData(rapidjson::Value &value) {

    rapidjson::Value &plane_list = value["plane_list"];
    if (plane_list.IsNull() || !plane_list.IsArray()) {
        CCLOG("plane_list is err ==================> %s ", "");
        return false;
    }

//    this->directNextBatch = value["directNextBatch"].GetInt();
    for (unsigned int i = 0; i < plane_list.Size(); i++) {
        LevelPlaneConfigData *oneData = LevelPlaneConfigData::create();
        rapidjson::Value &planeValue = plane_list[i];
        if (!oneData->initData(planeValue)) {
            return false;
        }
        this->config_vector.pushBack(oneData);
    }
    return true;
}

LevelPlaneConfigDataList::~LevelPlaneConfigDataList() {
    this->config_vector.clear();
}


static EnemyPlaneController *controller_singleInstance = nullptr;

EnemyPlaneController *EnemyPlaneController::getInstance() {
    if (!controller_singleInstance) {
        controller_singleInstance = new(std::nothrow) EnemyPlaneController();
    }
    return controller_singleInstance;
}

void EnemyPlaneController::destroyInstance() {
    CC_SAFE_DELETE(controller_singleInstance);
}

EnemyPlaneController::EnemyPlaneController() {
    this->gift_plane_num = 5;
    this->isFirst = false;
    this->isPause = false;
    this->isSpecialLevel = false;
    this->waitTime = -1;
    this->fly_size = 1280;
    this->start_wait_time = 3;
    this->gift_plane_send_interval = 0;
    this->game_state = this->GAME_STATE_INIT;
    this->sendPause = false;
    this->pauseSendGift = false;

    this->guide_group_num_1 = 5;
    this->guide_group_num_2 = 7;

    this->parserWeaponFile();
    this->curLevel = 0;
    this->isFirstDrop = true;
	this->_scheduler = Director::getInstance()->getScheduler();
    NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(EnemyPlaneController::endGiftPlaneSend), Msg_BossDead, nullptr);
}

void EnemyPlaneController::endGiftPlaneSend(Ref *obj)
{
    this->_scheduler->unschedule(CC_SCHEDULE_SELECTOR(EnemyPlaneController::sendGiftPlaneLoop), this);
}

EnemyPlaneController::~EnemyPlaneController() {
    this->endLoop();
    this->level_plane_config_vector.clear();
    this->send_plane_vector.clear();
    this->trajectory_map.clear();
    EditorBulletFactory::destroyInstance();
    NotificationCenter::getInstance()->removeObserver(this, Msg_BossDead);
}

bool EnemyPlaneController::startLevelWithConfigFile(int levelNum, bool isFirst) {

    StatisticsHelp::startLevel(levelNum);
    this->curLevel = levelNum;
    this->groupNum = 0;
    this->isFirst = isFirst;
    this->parserTrajectoryFile();
    std::string filePath = this->getEnemyConfigFileName(this->curLevel);
    return this->startLevel(filePath);
}

bool EnemyPlaneController::parserWeaponFile() {
    auto fileUtils = cocos2d::FileUtils::getInstance();
    std::string thePath = "level_config/barrage.json";
    if (!fileUtils->isFileExist(thePath)) {
        CCLOG("cant nont find weapon file ==================> %s ", thePath.c_str());
        return false;
    }

    rapidjson::Document d;
    std::string str = cocos2d::FileUtils::getInstance()->getStringFromFile(thePath);

    d.Parse<rapidjson::kParseDefaultFlags>(str.c_str());

    if (d.HasParseError()) {
        CCLOG("the level weapon file erro \nGetParseError %s", d.GetParseError());
        return false;
    }

    for (auto iter = d.MemberonBegin(); iter != d.MemberonEnd(); ++iter) {
        auto key = (iter->name).GetString();

        WeaponInfoObjList *weaponList = WeaponInfoObjList::create();
        for (unsigned int i = 0; i < d[key].Size(); i++) {
            WeaponInfoObj *data = WeaponInfoObj::create();
            rapidjson::Value &value = d[key][i];
            if (!data->initData(value)) {
                return false;
            }
            weaponList->weapon_vector.pushBack(data);
        }
        this->weapon_map.insert(key, weaponList);
    }
    return true;
}

bool EnemyPlaneController::parserTrajectoryFile()
{
    std::string thePath = this->getTrackConfigFileName(this->curLevel);
    return this->parseTrajectory(thePath);
}

bool EnemyPlaneController::parserSpecialTrajectoryFile()
{
//    std::string thePath = this->getSpecialTrackConfigFileName(this->curLevel);
//    return this->parseTrajectory(thePath);
    return false;
}

bool EnemyPlaneController::parseTrajectory(std::string thePath) {
    this->trajectory_map.clear();
    auto fileUtils = cocos2d::FileUtils::getInstance();
    if (!fileUtils->isFileExist(thePath)) {
        CCLOG("cant nont find Trahectory file ==================> %s ", thePath.c_str());
        return false;
    }

    rapidjson::Document d;
    std::string str = cocos2d::FileUtils::getInstance()->getStringFromFile(thePath);

    d.Parse<rapidjson::kParseDefaultFlags>(str.c_str());

    if (d.HasParseError()) {
        CCLOG("the level Trahectory file erro \nGetParseError %s", d.GetParseError());
        return false;
    }

    for (auto iter = d.MemberonBegin(); iter != d.MemberonEnd(); ++iter) {
        auto key = (iter->name).GetString();
        TrajectoryConfigDataList *list = TrajectoryConfigDataList::create();
        Vector<TrajectoryConfigData *> temp;
        for (unsigned int i = 0; i < d[key].Size(); i++) {
            TrajectoryConfigData *data = TrajectoryConfigData::create();
            rapidjson::Value &value = d[key][i];
            if (!data->initData(value)) {
                return false;
            }
            temp.pushBack(data);
        }
        list->set_tre_vector(temp);
        this->trajectory_map.insert(key, list);
    }
    return true;
}

TrajectoryConfigDataList *EnemyPlaneController::getTrajectoryConfigDataFromId(std::string id) {
    TrajectoryConfigDataList *trajectoryList = nullptr;
    if (trajectory_map.find(id) != this->trajectory_map.end()) {
        trajectoryList = this->trajectory_map.at(id);
    }
    return trajectoryList;
}

WeaponInfoObjList *EnemyPlaneController::getWeaponConfigDataFromId(std::string id) {
    WeaponInfoObjList *weapon_list = nullptr;
    if (this->weapon_map.find(id) != this->weapon_map.end()) {
        weapon_list = this->weapon_map.at(id);
    }
    return weapon_list;
}

void EnemyPlaneController::exitGame() {
    this->endAllSend();
//    StatisticsHelp::failLevel(this->curLevel);
    this->destroyInstance();
}

void EnemyPlaneController::pauseLoop() {
    this->_scheduler->pauseTarget(this);
}

void EnemyPlaneController::resumeLoop() {
    this->_scheduler->resumeTarget(this);
}

void EnemyPlaneController::pauseSelfAndChild(Node* node)
{
	node->pause();

    Vector<Node*> vec = node->getChildren();
    if(vec.size() > 0) 
	{
        for(Vector<Node*>::iterator it = vec.begin(); it != vec.end(); it ++)
		{
			Node* child = (*it); 
			if (child->getTag() != RemoveTag)
			{
				this->pauseSelfAndChild(*it);
			}
        }
    }
}

void EnemyPlaneController::resumeSelfAndChild(Node* node)
{
    node->resume();
    Vector<Node*> vec = node->getChildren();
    if(vec.size() > 0) {
        for(Vector<Node*>::iterator it = vec.begin(); it != vec.end(); it ++) {
            this->resumeSelfAndChild(*it);
        }
    }
}

void EnemyPlaneController::mainPlaneDeathPause()
{
    this->isPause = true;
    this->pauseLoop();
    if(this->send_plane_vector.size() > 0) {
        for(Vector<BasePlane*>::iterator it = this->send_plane_vector.begin();it != this->send_plane_vector.end(); it++)
        {
            this->pauseSelfAndChild(*it);
        }
    }
    auto stone = UIController::getInstance()->getPhysicsLayer()->getChildByTag(20150924);
    if(stone) {
        stone->pause();
    }
}

void EnemyPlaneController::mainPlaneDeathResume()
{
    if(!this->isPause) {
        return;
    }
    this->isPause = false;
    this->resumeLoop();
    if(this->send_plane_vector.size() > 0) {
        for(Vector<BasePlane*>::iterator it = this->send_plane_vector.begin();it != this->send_plane_vector.end(); it++)
        {
            this->resumeSelfAndChild(*it);
        }
    }
    auto stone = UIController::getInstance()->getPhysicsLayer()->getChildByTag(20150924);
    if(stone) {
        stone->resume();
    }
}


void EnemyPlaneController::endLoop() {
    if (this->game_state != this->GAME_STATE_DESTROY) {
        this->game_state = this->GAME_STATE_DESTROY;
        this->_scheduler->unscheduleAllForTarget(this);
    }
}

void EnemyPlaneController::endAllSend()
{
    this->_scheduler->unscheduleAllForTarget(this);
}

void EnemyPlaneController::destroyCallBack(BasePlane *basePlane) {
    CCASSERT(basePlane, "basePlane be non-nullptr");
	this->send_plane_vector.eraseObject(basePlane);

    if (this->send_plane_vector.size() == 0) {
        this->resumeSend();
    }
}

void EnemyPlaneController::giftPlaneDestroyCallBack(BasePlane* basePlane)
{
    CCASSERT(basePlane, "basePlane be non-nullptr");
	this->send_gift_plane_vector.eraseObject(basePlane);
}

void EnemyPlaneController::pauseSend() {
    this->sendPause = true;
}

void EnemyPlaneController::resumeSend() {
    this->sendPause = false;
}

void EnemyPlaneController::pauseSendGiftPlane()
{
    this->pauseSendGift = true;
}

bool EnemyPlaneController::isGuideLevel()
{
    return (1 == this->curLevel);
}

void EnemyPlaneController::resumeSendGiftPlane()
{
    this->pauseSendGift = false;
}

void EnemyPlaneController::waitSend(float time) {
    CCASSERT(time, "wait time be non-nullptr");
    if (time >= 0) {
        this->waitTime = time;
    }
}

std::string EnemyPlaneController::getEnemyConfigFileName(int levelNum) {

    return __String::createWithFormat("level_config/level_%d.json.enemy",levelNum)->getCString();
}

std::string EnemyPlaneController::getSpecialEnemyConfigFileName(int levelNum)
{
    return  "level_config/level_special.json.enemy";
}

std::string EnemyPlaneController::getTrackConfigFileName(int levelNum) {
    return __String::createWithFormat("level_config/level_%d.json.track",levelNum)->getCString();
}

std::string EnemyPlaneController::getSpecialTrackConfigFileName(int levelNum)
{
    return  "level_config/level_special.json.track";
}

void EnemyPlaneController::sendGiftPlane()
{
    this->gift_plane_send_interval = 2.0f + rand_0_1() * 2.0f;
    int planeId = 1;
    switch (this->curLevel)
    {
        case 1:
            planeId = 1;
            break;
        case 2:
            planeId = 6;
            break;
        case 3:
            planeId = 12;
            break;
        case 0:
            planeId = 16;
            break;
        default:
            break;
    }
    int actionId = random(9,18);
    float delayTime = 0.3;

    int dropAllNum = random(1, 2);

    for(int i = 0; i< this->gift_plane_num;i++) {
        int dropId = -1;
        if(dropAllNum > 0) {
            int dropRandom = random(1,99);
            dropId = floor((dropRandom - 1) / 33) + 1;
        }
        dropAllNum--;
        Node* node = Node::create();
        UIController::getInstance()->getPhysicsLayer()->addChild(node);
        node->runAction(Sequence::create(DelayTime::create(delayTime * i),CallFunc::create(CC_CALLBACK_0(EnemyPlaneController::createOneGiftPlane, this,planeId,actionId,this->curLevel,dropId)),RemoveSelf::create(true), nullptr));
    }
}

void EnemyPlaneController::createOneGiftPlane(int planeId,int actionId,int curLevel,int drop)
{
    BasePlane* pl = GameData::getInstance()->getPlaneFactory()->createEnemyPlane(planeId, 0.5f, curLevel);
    EnemyPlane* enemyPlane = dynamic_cast<EnemyPlane*>(pl);
    if(enemyPlane)
        enemyPlane->setIsMoveIn(true);
    auto action = GameData::getInstance()->getPlaneAction()->AddEnemyActionByIdx(actionId,pl);
    pl->setDestroyCallFun(CC_CALLBACK_1(EnemyPlaneController::giftPlaneDestroyCallBack,this));
    UIController::getInstance()->getPhysicsLayer()->addChild(pl);
	pl->runAction(action);
    pl->runAction(Sequence::create(DelayTime::create(0.5),CallFunc::create(CC_CALLBACK_0(EnemyPlaneController::setGiftPlaneMoveIn,this,pl)), nullptr));
    this->send_gift_plane_vector.pushBack(pl);
    if(drop > 0) {
        DropReward dropReward;
        dropReward.DropTp = (DropGoodsType)drop;
        pl->getPlanePro().DropVec.push_back(dropReward);
    }
}

void EnemyPlaneController::setGiftPlaneMoveIn(Node *node)
{
    EnemyPlane* enemyPlane = dynamic_cast<EnemyPlane*>(node);
    if(enemyPlane)
        enemyPlane->setIsMoveIn(false);
}

void EnemyPlaneController::startSpecialLevelWithConfig()
{
//    this->parserSpecialTrajectoryFile();
//    std::string path = this->getSpecialEnemyConfigFileName(this->curLevel);
//
//    if(!this->startLevel(path)) {
//        this->delayOverTheGame(true);
//    }
}

void EnemyPlaneController::levelPaneEmpty()
{
	MainPlane* mainPlane = GameData::getInstance()->getMainPlane();
	if(mainPlane) 
	{
		mainPlane->FlyUp();
	}
	return;

   // if(this->isFirst && !this->isSpecialLevel) {
   //     this->isSpecialLevel  = true;
   //     MainPlane* mainPlane = GameData::getInstance()->getMainPlane();
   //     if(mainPlane) {
   //         mainPlane->addSpeedUpEffect();
			////mainPlane->setInvincibleAbsolutely();
			////mainPlane->setTouchAble(true);
   //         CallFunc* callFunc =  CallFunc::create(CC_CALLBACK_0(EnemyPlaneController::startSpecialLevelWithConfig, this));
   //         mainPlane->runAction(Sequence::create(DelayTime::create(4),callFunc,nullptr));
   //     }
   // } else {
   //     this->delayOverTheGame(true);
   // }

}

void EnemyPlaneController::overTheGame(bool win)
{
    if(win) {
//        StatisticsHelp::finishLevel(this->curLevel);
    } else {
//        StatisticsHelp::failLevel(this->curLevel);
    }
    GameData::getInstance()->GameOver(win);
}

void EnemyPlaneController::delayOverTheGame(bool win)
{
    Node* node = Node::create();
    UIController::getInstance()->getPhysicsLayer()->addChild(node);
    node->runAction(Sequence::create(DelayTime::create(1.1),CallFunc::create(CC_CALLBACK_0(EnemyPlaneController::overTheGame,this,win)), nullptr));
}

bool EnemyPlaneController::startLevel(std::string filePath)
{
    this->level_plane_config_vector.clear();
    auto fileUtils = cocos2d::FileUtils::getInstance();
    if (!fileUtils->isFileExist(filePath)) {
        CCLOG("cant nont find level config file ==================> %s ", filePath.c_str());
        return false;
    }

    rapidjson::Document d;
    std::string str = cocos2d::FileUtils::getInstance()->getStringFromFile(filePath);

    d.Parse<rapidjson::kParseDefaultFlags>(str.c_str());

    if (d.HasParseError()) {
        CCLOG("the level config file erro \nGetParseError %s", d.GetParseError());
        return false;
    }

    rapidjson::Value &list = d["list"];
    if (list.IsNull() || !list.IsArray()) {
        CCLOG("the level config data is erro %s", "");
        return false;
    }
    for (unsigned int i = 0; i < list.Size(); i++) {
        rapidjson::Value &onePosData = list[i];
        LevelPlaneConfigDataList *data = LevelPlaneConfigDataList::create();
        if (!data->initData(onePosData)) {
            this->level_plane_config_vector.clear();
            return false;
        }
        this->level_plane_config_vector.pushBack(data);
    }
    this->config_index = this->level_plane_config_vector.begin();
    if(!this->isSpecialLevel) {
        this->_scheduler->schedule(CC_SCHEDULE_SELECTOR(EnemyPlaneController::updateUserFlySize), this, 0.1, false);
        this->_scheduler->schedule(CC_SCHEDULE_SELECTOR(EnemyPlaneController::sendGiftPlaneLoop), this, 0.1, false);
    } else {
        this->_scheduler->schedule(CC_SCHEDULE_SELECTOR(EnemyPlaneController::updateSpecialSize), this, 0.1, false);
        LevelPlaneConfigDataList *data = *this->config_index;
        this->special_vectory = data->config_vector;
    }
    this->game_state = this->GAME_STATE_PLAYING;
    return true;
}

void EnemyPlaneController::sendPlane() {
    if (this->config_index >= this->level_plane_config_vector.end()) {
//        NewDataMgr::getInstance()->setLevelFinishState(this->curLevel,true);
        this->endAllSend();
        this->levelPaneEmpty();
        return;
    }


    LevelPlaneConfigDataList *data = *this->config_index;
    Vector<LevelPlaneConfigData*> temp = data->config_vector;
    for (Vector<LevelPlaneConfigData*>::iterator it = temp.begin(); it != temp.end(); it++)
	{
        this->createPlaneWitchConfig(*it);
    }
    this->pauseSend();
    this->config_index++;

//    this->groupNum++;
//    if(this->isNeedGuide()) {
//        this->startGuide();
//    }
}


bool EnemyPlaneController::isNeedGuide()
{
//    if(this->curLevel == 0 && (this->groupNum == this->guide_group_num_1 || this->groupNum == this->guide_group_num_2)) {
//        return true;
//    } else {
//        return false;
//    }
	return false;
}

void EnemyPlaneController::startGuide()
{
    if(this->groupNum == this->guide_group_num_1) {
        Node* node = Node::create();
        UIController::getInstance()->getPhysicsLayer()->addChild(node);
        node->runAction(Sequence::create(DelayTime::create(2), CallFunc::create(CC_CALLBACK_0(EnemyPlaneController::startGuideHudun,this)),nullptr));
    } else if(this->groupNum == this->guide_group_num_2) {
        Node* node = Node::create();
        UIController::getInstance()->getPhysicsLayer()->addChild(node);
        node->runAction(Sequence::create(DelayTime::create(1.5), CallFunc::create(CC_CALLBACK_0(EnemyPlaneController::startGuideDazhao,this)),nullptr));
    } else {
        this->isFirstDrop = false;
        UserDefault::getInstance()->setBoolForKey("noGui", true);
        UIController::getInstance()->getMainUILayer()->helpFeqCard();
    }
}

void EnemyPlaneController::startGuideHudun()
{
    UIController::getInstance()->getMainUILayer()->helpFeqHuDun();
}

void EnemyPlaneController::startGuideDazhao()
{
    UIController::getInstance()->getMainUILayer()->helpFeqDaZhao();
}

void EnemyPlaneController::sendGiftPlaneLoop(float dt) {
    if(this->pauseSendGift) {
        return;
    }

    if (this->game_state != this->GAME_STATE_PLAYING) {
        return;
    }

    if (this->start_wait_time > 0) {
        return;
    }

    if (this->waitTime >= 0) {
        return;
    }

    if(this->gift_plane_send_interval > 0) {
        this->gift_plane_send_interval -= dt;
    } else {
        this->sendGiftPlane();
    }
}


void EnemyPlaneController::createPlaneWitchConfig(LevelPlaneConfigData* config)
{
    GameData* gameData = GameData::getInstance();
    auto basePlane = gameData->getPlaneFactory()->createEnemyPlane(config);
    EnemyPlane* plane = dynamic_cast<EnemyPlane*>(basePlane);
    if(plane) {
        plane->setIsMoveIn(true);
    }
    basePlane->setPosition(config->startPoint());
    basePlane->setDestroyCallFun(CC_CALLBACK_1(EnemyPlaneController::destroyCallBack,this));
    UIController::getInstance()->getPhysicsLayer()->addChild(basePlane, -1000);
    this->send_plane_vector.pushBack(basePlane);
}

void EnemyPlaneController::updateSpecialSize(float dt)
{
    if(this->special_vectory.size() <= 0 && this->send_plane_vector.size() == 0) {
        this->endAllSend();
        this->levelPaneEmpty();
        return;
    }
    this->fly_size += 550 * dt;
    for (Vector<LevelPlaneConfigData*>::iterator it = this->special_vectory.begin(); it != this->special_vectory.end(); it++)
    {
        LevelPlaneConfigData* config = *it;
        if(this->fly_size >= config->startPoint().y) {
            this->remove_special_vectory.pushBack(config);
            config->set_startPoint(Vec2(config->startPoint().x,1368));
            this->createPlaneWitchConfig(config);
        }
    }

    for (Vector<LevelPlaneConfigData*>::iterator it2 = this->remove_special_vectory.begin(); it2 != this->remove_special_vectory.end(); it2++)
    {
        this->special_vectory.eraseObject(*it2);
    }

    if(this->remove_special_vectory.size() > 0) {
        this->remove_special_vectory.clear();
    }
}

void EnemyPlaneController::updateUserFlySize(float dt) {
    if (this->game_state != this->GAME_STATE_PLAYING) {
        return;
    }

    if (this->start_wait_time > 0) {
        this->start_wait_time -= dt;
        return;
    }

    if (this->waitTime >= 0) {
        this->waitTime -= dt;
        return;
    }

    if(!this->sendPause) {
        this->sendPlane();
    }
}

bool EnemyPlaneController::isMoreClose(const Vec2 &pos,const Vec2 &targetPos,const Vec2 &targetPos2)
{
    if(pos.distance(targetPos) > pos.distance(targetPos2))
    {
        return true;
    } else {
        return false;
    }

}

BasePlane* EnemyPlaneController::getTheClosePlane(const Vec2 &pos) {
    BasePlane* closePlane = nullptr;
    if(this->send_plane_vector.size() > 0)
    {
        for (Vector<BasePlane *>::iterator it = this->send_plane_vector.begin(); it != this->send_plane_vector.end(); it++)
        {
            if (closePlane == nullptr || this->isMoreClose(pos,closePlane->getPhysicsBody()->getPosition(),(*it)->getPhysicsBody()->getPosition()))
            {
                closePlane = *it;
            }
        }
    }
    else if(this->send_gift_plane_vector.size() > 0)
    {
        for (Vector<BasePlane *>::iterator it = this->send_gift_plane_vector.begin(); it != this->send_gift_plane_vector.end(); it++)
        {
            if (closePlane == nullptr || this->isMoreClose(pos,closePlane->getPhysicsBody()->getPosition(),(*it)->getPhysicsBody()->getPosition()))
            {
                closePlane = *it;
            }
        }
    }
    return closePlane;
}
