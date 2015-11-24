 //
// Created by 叶飞 on 15/8/19.
//

#include "EditorWeapon.h"
#include "../Layer/UIController.h"
#include "../Layer/PhysicsLayer.h"
#include "../../GameCommon/GameData.h"
#include "../../GameCommon/EffectsManager.h"
#include "../AI.h"

USING_NS_CC;
USING_NS_CC_EXT;

EditorBulletFactory::EditorBulletFactory()
{
    this->bulletIndex = 0;
}

EditorBulletFactory::~EditorBulletFactory()
{
    this->bullet_recover_vector.clear();
}

static EditorBulletFactory* factory_singleInstance = nullptr;

EditorBulletFactory *EditorBulletFactory::getInstance() {
    if (!factory_singleInstance) {    
        factory_singleInstance = new(std::nothrow) EditorBulletFactory();
    }
    return factory_singleInstance;
}

void EditorBulletFactory::destroyInstance() {
    CC_SAFE_DELETE(factory_singleInstance);
}

void EditorBulletFactory::recoveryEditorBullet(EditorBullet* bullet)
{
    this->bullet_recover_vector.pushBack(bullet);
    bullet->removeFromParentAndCleanup(true);
}

EditorBullet* EditorBulletFactory::getEditorFromRecoveryVector()
{
    EditorBullet* bullet = nullptr;
//    if(!this->bullet_recover_vector.empty()) {
//        bullet = this->bullet_recover_vector.front();
//        bullet->retain();
//        this->bullet_recover_vector.eraseObject(bullet);
   
//    } else {
//        this->bulletIndex++;
        bullet = EditorBullet::create();
//        bullet->index = this->bulletIndex;
//    }
    return bullet;
}

EditorWeapon::~EditorWeapon() {
//    this->releaseSelf();
    this->unschedule(CC_SCHEDULE_SELECTOR(EditorWeapon::mainLoop));
    this->info = nullptr;
}

EditorWeapon::EditorWeapon() {
    this->isAim = 0;
    this->fireTotalTime = 0;
    this->fireTimes = 0;
    this->fireOnceWaitTime = 100;
    this->isShoot = false;

    this->fireGroupWaitTime = 0;
    this->fireGroupDuration = 0;
    this->fireCount = 0;
    this->fireOnceBulletNum = 0;
    this->fireAngle = 0;
    this->fireSpeed = 0;
    this->isChild = false;
    this->particalPath = "";
    this->setBelongTo(UnitId::DefaultIdx);
    //无需初始化
    this->fireOnceInterval = 0;
    this->fireGroupInterval = 0;
}

bool EditorWeapon::init(WeaponInfoObj *info) {
    if (info == nullptr) {
        return false;
    } else {
        this->info = info;
        this->needInitFireAngle = true;
        this->fireGroupWaitTime = info->waitDuration();
        this->fireGroupDuration = info->fireDuration();
        this->fireCount = info->fireCount();
        this->fireOnceBulletNum = info->bulletNumber();
        this->baseAngle = info->fireAngle();
        this->fireSpeed = info->fireSpeed();
        this->bulletPos = Vec2(info->distanceToWeaponX(), info->distanceToWeaponY());
        this->initWeapon();
        return true;
    }
}

void EditorWeapon::setParticalPath(std::string path)
{
    this->particalPath = path;
}

void EditorWeapon::setIsOneTime(bool isOneTime) {
    this->isOneTime = isOneTime;
}

void EditorWeapon::setCouldChangeGold(bool could) {
    this->couldChangeGold = could;
}

void EditorWeapon::onEnter() {
    Node::onEnter();
    startWeaponLoop();
}

void EditorWeapon::startWeaponLoop() {
//    this->scheduleUpdate();
    if(!this->isChild) {
        this->schedule(CC_SCHEDULE_SELECTOR(EditorWeapon::mainLoop));
    }
}

Vec2 EditorWeapon::getVelocityFromAngle(float angle) {
    Vec2 vec = Vec2(0, 1).forAngle((90 - angle) * Angle2pi);
    vec.normalize();
    return vec * this->fireSpeed;
}

void EditorWeapon::update(float dt) {
//    this->mainLoop(dt);
}

void EditorWeapon::mainLoop(float dt) {
    if(EnemyPlaneController::getInstance()->isPause) {
        return;
    }
    this->fireGroupWaitTime -= dt;
    if (this->fireGroupWaitTime > 0) {
//        log("fireGroupWaitTime --------------- %f",fireGroupWaitTime);
        return;
    }
    if (this->isShoot || this->fireGroupInterval <= 0) {

        if(this->needInitFireAngle) {
            this->needInitFireAngle = false;
            this->initFireAngle();
        }
        if ((!this->fireGroupDuration || this->fireGroupDuration == 0 || this->fireTotalTime < this->fireGroupDuration) && this->fireTimes < this->fireCount) {
            this->fireOnceWaitTime += dt;
            this->fireTotalTime += dt;
            if (this->fireOnceWaitTime >= this->fireOnceInterval) {
                if(this->isChild) {
//                    log("fire one time -+-+-+-+-+-+-+  %s","");
                }
                this->fireOnce();
                this->fireOnceWaitTime = 0;
                this->fireTimes++;
            } else {
                if(this->isChild) {
//                    log("fireOnceWaitTime time [][][][][][][]  %f",this->fireOnceWaitTime);
                }
            }

            if(this->fireOnceInterval <= 0) {
                this->fireOnceInterval = this->info->fireGap();
            }
        } else {
            this->isShoot = false;
            this->fireTimes = 0;
            this->fireTotalTime = 0;
            this->fireOnceWaitTime = 0;
            this->groupFireEnd();
            if(this->isOneTime) {
                return;
            }
        }
        if(this->fireGroupInterval <= 0) {
            this->isShoot = true;
            this->fireGroupInterval = this->info->barrageFireGap();
        }
        this->fireGroupInterval -= dt;
        if(this->isChild) {
//                    log("fireGroupInterval ============== %f",fireGroupInterval);
        }
    } else {
        this->needInitFireAngle = true;
        this->fireGroupInterval -= dt;
        if(this->isChild) {
//            log("fireGroupInterval ============== %f",fireGroupInterval);
        }
    }
}


void EditorWeapon::initFireAngle() {
    this->fireAngle = this->baseAngle;
    if(this->isAim) {
        Vec2 parentPos = this->getParent()->getParent()->getPosition();
        auto size = Director::getInstance()->getVisibleSize();
        Vec2 mainPos = Vec2(size.width * 0.5,0);
        auto mainPlane = GameData::getInstance()->getMainPlane();
        if(mainPlane)
        {
            mainPos = mainPlane->convertToWorldSpaceAR(Vec2::ZERO);
            auto v = mainPos - parentPos;
            float angle =  -(v.getAngle() * Pi2Angle) + 90;
            this->fireAngle = angle + (this->fireAngle - 180);
        }
    }

    if(this->info->followWeaponFlag()) {
        this->fireAngle = this->getParent()->getParent()->getRotation();
    } else {
        this->fireAngle += this->getParent()->getParent()->getRotation();
    }
}

void EditorWeapon::releaseSelf() {
}

void EditorWeapon::initWeapon() {

}

void EditorWeapon::groupFireEnd() {
    EnemyPlane* plane = dynamic_cast<EnemyPlane*>(this->getParent()->getParent());
    if(plane && plane->m_isAutoFire && !plane->m_haveCallBack) {
        plane->m_haveCallBack = true;
        plane->runAction(Sequence::create(DelayTime::create(this->fireGroupInterval),CallFunc::create(CC_CALLBACK_0(EnemyPlane::StartFire,plane)), nullptr));
    }

    if(this->isOneTime) {
//        this->unscheduleUpdate();
        this->unschedule(CC_SCHEDULE_SELECTOR(EditorWeapon::mainLoop));
        this->removeFromParent();
    }
}

void EditorWeapon::fireOnce() {
//    log("fire once barrage index %d", this->barrageIndex);
}

void EditorWeapon::addBullet(Vec2 pos, float rotation, Vec2 velocity) {
    EditorBullet *bullet = EditorBulletFactory::getInstance()->getEditorFromRecoveryVector();
//    EditorBullet *bullet = EditorBullet::create();
    UIController::getInstance()->getEffectsLayer()->addChild(bullet,9999);
    bullet->release();
    bullet->initEditorBulletFrame(this->info);
    bullet->setBelongTo(this->getBelongTo());
    bullet->setBulletVelocity(velocity);
    if(this->isChild) {
        bullet->setBodyPosition(pos);
    } else {
        bullet->setBodyPosition(Vec2(pos.x,pos.y - bullet->getContentSize().height * 0.5));
    }
    if(this->particalPath != "") {
        Node* particleNode = Node::create();
        particleNode->setPosition(Vec2(bullet->getContentSize().width*0.5,bullet->getContentSize().height*0.5));
        particleNode->setTag(1001);
        bullet->addChild(particleNode);
        EffectsManager::addParticle(this->particalPath,particleNode,Vec2(0,0),false);
    }
    bullet->setCouldChange(this->couldChangeGold);
    bullet->initEditorBullet();
	bullet->getPhysicsBody()->setRotation(rotation);
}

void NormalEditorWeapon::initWeapon() {
    this->x_padding = 0;
    this->x_gap = this->info->bulletGap();
    this->y_gap = this->info->bulletYGap();
    this->x_move_speed = this->info->bulletGapAmplitude();
    this->x_padding_min = this->info->bulletMinGap();
    this->x_padding += x_gap;
}

void NormalEditorWeapon::fireOnce() {

    EditorWeapon::fireOnce();
//    Vec2 basePoint = this->bulletPos;
    Vec2 basePoint = this->getPosition();
    Vec2 worldPos = this->getParent()->getParent()->getPosition();
    if(this->bullet_wdith == 0) {
        EditorBullet* bullet = EditorBullet::create();
        bullet->initEditorBulletFrame(this->info);
        this->bullet_wdith = bullet->getContentSize().width;
        bullet->release();
    }
    
    float paddingX = x_padding + this->bullet_wdith;
    float planeRotate = this->getParent()->getRotation();
    float worldAngle = planeRotate + this->fireAngle;
    float baseX = 0;
    float baseX2 = 0;
    float baseY = 0;
    int allNum = this->fireOnceBulletNum;
    if (allNum <= 0)
        return;

    if (0 != allNum % 2) {
        allNum -= 1;
        Vec2 pos = Vec2(basePoint.x, basePoint.y);
        if(this->isAim) {
            Node* node = Node::create();
            node->setPosition(basePoint + worldPos);
            UIController::getInstance()->getPhysicsLayer()->addChild(node);
            Node* childNode = Node::create();
            childNode->setPosition(pos);
            node->addChild(childNode);

            node->setRotation(worldAngle-180);
            pos = childNode->convertToWorldSpaceAR(Vec2::ZERO);
        } else {
            pos += worldPos;
        }

        this->addBullet(pos, worldAngle, this->getVelocityFromAngle(worldAngle));
        baseX = basePoint.x + paddingX;
        baseX2 = basePoint.x - paddingX;
    } else {

        baseX = basePoint.x + paddingX * 0.5;
        baseX2 = basePoint.x - paddingX * 0.5;
    }
    baseY = basePoint.y - this->y_gap;

    for (int i = 0; i < allNum / 2; i++) {
//        float x = baseX + this->x_paddingX * i;
        float y = baseY - this->y_gap * i;
//        Vec2 pos = Vec2(x,y) + worldPos;
        Vec2 pos1 = Vec2(baseX + paddingX * i,y);
        Vec2 pos2 = Vec2(baseX2 - paddingX * i,y);

        if(this->isAim) {
            Node* node = Node::create();
            node->setPosition(basePoint + worldPos);
            UIController::getInstance()->getPhysicsLayer()->addChild(node);

            Node* childNode1 = Node::create();
            Node* childNode2 = Node::create();
            childNode1->setPosition(pos1);
            childNode2->setPosition(pos2);
            node->addChild(childNode1);
            node->addChild(childNode2);
            node->setRotation(worldAngle-180);
            pos1 = childNode1->convertToWorldSpaceAR(Vec2::ZERO);
            pos2 = childNode2->convertToWorldSpaceAR(Vec2::ZERO);
            node->removeFromParent();
        } else {
            pos1 += worldPos;
            pos2 += worldPos;
        }


        this->addBullet(pos1, worldAngle, this->getVelocityFromAngle(worldAngle));
        this->addBullet(pos2, worldAngle, this->getVelocityFromAngle(worldAngle));
    }

    if ((this->x_padding_min != 0 && this->x_padding <= this->x_padding_min) || this->x_padding <= 0) {
        this->isUp = true;
        this->isDown = false;
    }

    if (this->x_gap != 0 && this->x_padding >= this->x_gap) {
        this->isDown = true;
        this->isUp = false;
    }

    if (this->isUp) {
        this->x_padding += this->x_move_speed;
    }

    if (this->isDown) {
        this->x_padding -= this->x_move_speed;
    }

}

void ScatterEditorWeapon::initWeapon() {
    this->bulletAngle = this->info->bulletAngle();
    this->bulletTotalNum = this->info->bulletNumber();
}

void ScatterEditorWeapon::fireOnce() {
    EditorWeapon::fireOnce();
//    return;
    Vec2 basePoint = this->getPosition();
    Vec2 worldPos = this->getParent()->getParent()->getPosition();
    float baseAngle = 0;
    float baseAngle2 = 0;
    int allNum = this->bulletTotalNum;
    if (allNum <= 0)
        return;

    float planeRotate = this->getParent()->getRotation();
    float worldAngle = planeRotate + this->fireAngle;

    Vec2 sendPos = Vec2(basePoint.x, basePoint.y) + worldPos;
    if (0 != allNum % 2) {
        allNum -= 1;
        this->addBullet(sendPos, worldAngle, this->getVelocityFromAngle(worldAngle));
        baseAngle = worldAngle + this->bulletAngle;
        baseAngle2 = worldAngle - this->bulletAngle;
    } else {
        baseAngle = worldAngle + this->bulletAngle * 0.5;
        baseAngle2 = worldAngle - this->bulletAngle * 0.5;
    }
    for (int i = 0; i < allNum / 2; i++) {
        float angle1 = baseAngle + this->bulletAngle * i;
        float angle2 = baseAngle2 - this->bulletAngle * i;

        this->addBullet(sendPos, angle1, this->getVelocityFromAngle(angle1));
        this->addBullet(sendPos, angle2, this->getVelocityFromAngle(angle2));
    }
}

void StrafeEditorWeapon::initFireAngle()
{
    EditorWeapon::initFireAngle();
    if(this->isAim) {
        this->weaponAngle = this->fireAngle;
    }

    this->beginAngle = this->weaponAngle;
}

void StrafeEditorWeapon::groupFireEnd() {
    EditorWeapon::groupFireEnd();
    if(this->maxChangeAngle == 0) {
        this->weaponAngle = this->baseAngle - this->bulletAngle;
        this->weaponAngle = this->baseAngle;
    }
//    log("--------------------------------------------","");
}

void StrafeEditorWeapon::initWeapon() {
    this->bulletAngle = this->info->fireChangedAngle();
    this->weaponAngle = this->baseAngle;
    this->maxChangeAngle = this->info->fireMaxAngle();
}

void StrafeEditorWeapon::fireOnce() {
    EditorWeapon::fireOnce();
    Vec2 basePoint = this->getPosition();
    Vec2 worldPos = this->getParent()->getParent()->getPosition();
    float planeRotate = this->getParent()->getRotation();
    float worldAngle = planeRotate + this->weaponAngle;
    Vec2 sendPos = Vec2(basePoint.x, basePoint.y) + worldPos;

    this->addBullet(sendPos, worldAngle, this->getVelocityFromAngle(worldAngle));
    this->weaponAngle += this->bulletAngle;
//    log("the angle is => %f",this->weaponAngle);
//    log("weaponAngle is %f           beginAngle is %f",this->weaponAngle,this->beginAngle);
    if(this->maxChangeAngle != 0 && fabs(this->weaponAngle - this->beginAngle) >= this->maxChangeAngle) {
        this->beginAngle = this->weaponAngle;
        this->bulletAngle = -this->bulletAngle;
    }
}


void CrossEditorWeapon::releaseSelf() {
    this->number_vector.clear();
}

void CrossEditorWeapon::initWeapon() {
    this->bulletAngle = this->info->bulletAngle();
    std::string str = this->info->bulletNumberList();
    std::stringstream ss(str);
    std::string sub_str;
    while (getline(ss, sub_str, ',')) {
        int temp = atoi(sub_str.c_str());
        if (temp != 0) {
            this->number_vector.push_back(temp);
        }
    }

    this->it = this->number_vector.begin();
}

void CrossEditorWeapon::fireOnce() {
    EditorWeapon::fireOnce();
    if (this->it >= this->number_vector.end())
        this->it = this->number_vector.begin();
    Vec2 basePoint = this->getPosition();
    Vec2 worldPos = this->getParent()->getParent()->getPosition();
    float baseAngle = 0;
    float baseAngle2 = 0;
    int allNum = *this->it;
    if (allNum <= 0)
        return;

    float planeRotate = this->getParent()->getRotation();
    float worldAngle = planeRotate + this->fireAngle;

    Vec2 sendPos = Vec2(basePoint.x, basePoint.y) + worldPos;
    if (0 != allNum % 2) {
        allNum -= 1;
        this->addBullet(sendPos, worldAngle, this->getVelocityFromAngle(worldAngle));
        baseAngle = worldAngle + this->bulletAngle;
        baseAngle2 = worldAngle - this->bulletAngle;
    } else {
        baseAngle = worldAngle + this->bulletAngle * 0.5;
        baseAngle2 = worldAngle - this->bulletAngle * 0.5;
    }
    for (int i = 0; i < allNum / 2; i++) {
        float angle1 = baseAngle + this->bulletAngle * i;
        float angle2 = baseAngle2 - this->bulletAngle * i;

        this->addBullet(sendPos, angle1, this->getVelocityFromAngle(angle1));
        this->addBullet(sendPos, angle2, this->getVelocityFromAngle(angle2));
    }
    this->it++;
}

void EditorWeaponFactory::createAndAddWeapon(Node *node,std::string barrageArray,bool isOneTime) {

    std::string str = barrageArray;
    std::stringstream ss(str);
    std::string sub_str;

	if(str.find("#") != -1) 
	{
        while (getline(ss, sub_str, '#')) 
		{
            EditorWeaponFactory::createWeaponWithString(node,sub_str,isOneTime);
        }
    } 
	else 
	{
        EditorWeaponFactory::createWeaponWithString(node,barrageArray,isOneTime);
    }
	//log("--------sub_str id = %s", sub_str.c_str());
	//log("--------barrageArray id = %s", barrageArray.c_str());
}

void EditorWeaponFactory::createWeaponWithString(Node* node,std::string barrage,bool isOneTime)
{
    int index = 0;
	Node* parentNode = node->getChildByTag(20150824);
	if(!parentNode) 
	{
		parentNode = Node::create();
		parentNode->setTag(20150824);
		node->addChild(parentNode);
	}
    std::stringstream ss(barrage);
    std::string sub_str;
    float x = 0;
    float y = 0;
    std::string id = "";
    int aim = 0;
    while (getline(ss, sub_str, ',')) 
	{
        switch (index) 
		{

            case 1:
                id = sub_str;
                break;
            case 4:
                x = atoi(sub_str.c_str());
                break;
            case 5:
				y = atoi(sub_str.c_str());
                break;
            case 6:
				aim = atoi(sub_str.c_str());
                break;
        }
        index++;
    }
	if (atoi(id.c_str()) >= 10000 && atoi(id.c_str()) <= 10003)
	{
		GameData::getInstance()->getPlaneAiFactory()->AddEnemyAiByIdx((BasePlane*)node,atoi(id.c_str()));				//飞机自己的AI
	}
	else
	{
		WeaponInfoObjList* list = EnemyPlaneController::getInstance()->getWeaponConfigDataFromId(id);
		if(list != nullptr) {
			for(Vector<WeaponInfoObj*>::iterator it = list->weapon_vector.begin();it < list->weapon_vector.end();it++) {
				auto* weapon = EditorWeaponFactory::createWeaponWithType(*it,isOneTime, true);
				weapon->isAim = aim;
				weapon->setPosition(x,y);
				parentNode->addChild(weapon);
			}
		}
	}
}



EditorWeapon* EditorWeaponFactory::createWeaponWithType(WeaponInfoObj* infoObj,bool isOneTime,bool couldChangeGold,bool isChild)
{
    EditorWeapon* weapon;
    switch (infoObj->barrageType()) {
        case 0:
            weapon = NormalEditorWeapon::create();
            break;
        case 1:
            weapon = ScatterEditorWeapon::create();
            break;
        case 2:
            weapon = StrafeEditorWeapon::create();
            break;
        case 3:
            weapon = CrossEditorWeapon::create();
            break;
    }
    if(weapon != nullptr) {
        weapon->isChild = isChild;
        weapon->setIsOneTime(isOneTime);
        weapon->setCouldChangeGold(couldChangeGold);
        weapon->init(infoObj);
    }
    return weapon;

}