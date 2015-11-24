//
// Created by 叶飞 on 15/8/19.
//


#ifndef __EditorWeapon_H_
#define __EditorWeapon_H_

#include "../EnemyPlaneController.h"
#include "../Bullet/EditorBullet.h"

class EditorBulletFactory : public cocos2d::Ref{
public:
    int bulletIndex;
    EditorBulletFactory();
    ~EditorBulletFactory();

    static EditorBulletFactory* getInstance();
    static void destroyInstance();
    Vector<EditorBullet*>bullet_recover_vector;
    void recoveryEditorBullet(EditorBullet* bullet);
    EditorBullet* getEditorFromRecoveryVector();
};

class EditorWeapon : public cocos2d::Node {
private:
    float fireTotalTime;
    float fireTimes;
    float fireOnceWaitTime;
    float isOneTime;
    float couldChangeGold;

    float fireGroupWaitTime;
    float fireGroupDuration;
    float fireCount;
    float fireOnceInterval;
    float fireGroupInterval;
    float tempFireGroupInterval;
    bool isShoot;
    std::string particalPath;
public:
    int isAim;
    ~EditorWeapon();
    EditorWeapon();

    void setParticalPath(std::string path);
    bool isChild;
    MY_CREATE_FUNC(EditorWeapon);
    bool init(WeaponInfoObj* info);
    WeaponInfoObj* info;
    int fireOnceBulletNum;
    float fireAngle;
    float baseAngle;
    float fireSpeed;
    float needInitFireAngle;
    Vec2 bulletPos;
//    static EditorWeapon* create(WeaponInfoObj* info);
    void startWeaponLoop();
    void update(float dt);
    void setIsOneTime(bool isOneTime);
    void setCouldChangeGold(bool could);
    float refreshTime;
    void mainLoop(float dt);
    void addBullet(Vec2 pos,float rotation,Vec2 velocity);
    Vec2 getVelocityFromAngle(float angle);
    virtual void initFireAngle();
    virtual void groupFireEnd();
    virtual void fireOnce();
    virtual void initWeapon();
    virtual void onEnter();
    virtual void releaseSelf();
    CC_SYNTHESIZE(UnitId,belongTo,BelongTo);
};

class NormalEditorWeapon : public EditorWeapon 
{
	NormalEditorWeapon()
	{
		isDown = false;
		isUp = false;
        bullet_wdith = 0;
	}
    virtual void initWeapon();
    float x_padding;
    float x_gap;
    float y_gap;
    float x_move_speed;
    float x_padding_min;
    bool isDown;
    bool isUp;
    void fireOnce();
    float bullet_wdith;
public:
    MY_CREATE_FUNC(NormalEditorWeapon);
};

//散射

class ScatterEditorWeapon : public EditorWeapon {
    float bulletAngle;
    float bulletTotalNum;
    virtual void initWeapon();
    void fireOnce();
public:
    MY_CREATE_FUNC(ScatterEditorWeapon);
};

//扫射

class StrafeEditorWeapon : public EditorWeapon {
    float bulletAngle;
    float weaponAngle;
    float maxChangeAngle;
    float beginAngle;
    virtual void initFireAngle();
    virtual void groupFireEnd();
    virtual void initWeapon();
    void fireOnce();
public:
    MY_CREATE_FUNC(StrafeEditorWeapon);
};

//交叉

class CrossEditorWeapon : public EditorWeapon {
    float bulletAngle;
    std::vector<int> number_vector;
    std::vector<int>::iterator it;
    virtual void initWeapon();
    virtual void releaseSelf();
    void fireOnce();
public:
    MY_CREATE_FUNC(CrossEditorWeapon);
};

class EditorWeaponFactory : public Ref
{
public:
    static void createAndAddWeapon(Node* node,std::string barrageArray,bool isOneTime = false);
    static void createWeaponWithString(Node* node,std::string barrage,bool isOneTime = false);
//    static void createAndAddWeapon(Node* node,WeaponInfoObj* infoObj,Vec2 pos);
    static EditorWeapon* createWeaponWithType(WeaponInfoObj* infoObj,bool isOneTime = false,bool couldChangeGold = false,bool isChildWeapon = false);
};

#endif //__EditorWeapon_H_
