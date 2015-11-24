//
// Created by 叶飞 on 15/8/19.
//


#ifndef __EditorBullet_H_
#define __EditorBullet_H_

#include "Bullet.h"
#include "../EnemyPlaneController.h"
#include "../Weapon/EditorWeapon.h"

class EditorWeapon;

class EditorBullet : public Bullet
{
private:
    bool isRemoveIng;
    Vec2 velocity;
    Vec2 startPoint;
    float firstDuration;
    float firstTempTime;
    float lifeTime;
    float tempLifeTime;
    int times;
    float speed;
    bool firstIsEnd;
    bool secondAnimationIsRun;
    bool secondSpeedIsSet;
    EditorWeapon* childWeapon;
public:

    int index;
    const static int ACTION_TAG_FIRST_WAIT = 102;

    static EditorBullet* create();
    void initEditorBulletFrame(WeaponInfoObj* infoObj);
    EditorBullet();
    ~EditorBullet();
    void setBulletVelocity(Vec2 velocity);
    void startSecond(float dt);
    void resetVelocity();
    void firstAcceleration(float dt);
    void secondAcceleration(float dt);
    void startSecondAcceleration(float dt);
    void addChildWeapon();
    void initBody();
    void initEditorBullet();
    void setCouldChange(bool could);
    void toRecovery();
    void startBulletAction();
    void bulletMainLoop(float dt);
    void update(float dt);

    void setBodyPosition(Vec2 pos);
    void fixAngle(float dt);

    virtual void RemoveMyself();
    WeaponInfoObj* info;

    CC_SYNTHESIZE(UnitId,belongTo,BelongTo);
};


#endif //__EditorBullet_H_
