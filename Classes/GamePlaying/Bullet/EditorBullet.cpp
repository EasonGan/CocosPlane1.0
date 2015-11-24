//
// Created by 叶飞 on 15/8/19.
//

#include "../Weapon/EditorWeapon.h"

EditorBullet *EditorBullet::create() {
    EditorBullet *bullet = new(std::nothrow) EditorBullet();
    if (bullet && bullet->init()) {
		bullet->initBullet();
        return bullet;
    }
    else {
        CC_SAFE_DELETE(bullet);
        return nullptr;
    }
}

void EditorBullet::initEditorBulletFrame(WeaponInfoObj* infoObj)
{
    this->info = infoObj;
    this->isRemoveIng = false;
    std::string res = this->info->bullet_resIndex();
    std::string frameName = String::createWithFormat("bullet_%s.png", res.c_str())->getCString();
    this->initWithSpriteFrameName(frameName);
}

EditorBullet::EditorBullet()
{
    this->index = 0;
}

EditorBullet::~EditorBullet() {
    this->toRecovery();
}

void EditorBullet::setCouldChange(bool could) {
    this->couldChange = could;
}

void EditorBullet::initEditorBullet() {
    if (this->info == nullptr) {
        return;
    }
    this->resetHurt(-1);
    int hp = this->m_BasePro.getMaxHp();
    this->m_BasePro.setCurHp(hp);
    this->childWeapon = nullptr;
    this->firstDuration = 0;
    this->firstTempTime = 0;
    this->lifeTime = -1;
    this->tempLifeTime = 0;
    this->firstIsEnd = true;
    this->secondAnimationIsRun = false;
    this->secondSpeedIsSet = false;
    this->startPoint = this->getPosition();
    this->times = this->info->bullet_shrinkNumber();
    this->speed = this->info->fireSpeed();
    this->initBody();
    if (this->info->bullet_scaleX() > 0) {
        this->setScaleX(this->info->bullet_scaleX());
    }

    if (this->info->childBarrageRule != nullptr) {
        this->addChildWeapon();
    }

    if (this->info->bullet_scaleY() > 0) {
        this->setScaleY(this->info->bullet_scaleY());
    }
    this->getPhysicsBody()->setMass(1);
    this->setBitmask(ENEMY_AMMO_CATEGORYBITMASK, ENEMY_AMMO_COLLISIONBITMASK, ENEMY_AMMO_CONTACTTESTBITMASK);
    this->lifeTime = this->info->bullet_lifeTime();

    this->getPhysicsBody()->setPosition(this->getPosition());
    if (this->info->bullet_waitDuration() > 0) {
        auto action = Sequence::create(DelayTime::create(this->info->bullet_waitDuration()), CallFunc::create(CC_CALLBACK_0(EditorBullet::startBulletAction, this)), nullptr);
        action->setTag(EditorBullet::ACTION_TAG_FIRST_WAIT);
        this->runAction(action);
    } else {
        this->startBulletAction();
    }

    this->firstDuration = this->info->bullet_firstDuration();
    if (this->firstDuration > 0) {
        this->firstIsEnd = false;
    }
}

void EditorBullet::setBulletVelocity(Vec2 velocity) {
    this->velocity = velocity;
}

void EditorBullet::addChildWeapon() {
    this->childWeapon = EditorWeaponFactory::createWeaponWithType(this->info->childBarrageRule, false, true, true);
    Node *node = Node::create();
    node->setTag(100);
    node->setPosition(Vec2(this->getContentSize().width * 0.5, this->getContentSize().height * 0.5));
    node->addChild(this->childWeapon);
    this->addChild(node);
}

void EditorBullet::initBody() {
//    Bullet::initBody();
    auto body = PhysicsBody::createBox(Size(1, 1));
    body->setGravityEnable(false);
//    body->setGroup(1);
    setPhysicsBody(body);
}

void EditorBullet::startBulletAction() {
//    this->scheduleUpdate();
    this->schedule(CC_SCHEDULE_SELECTOR(EditorBullet::bulletMainLoop));
    this->getPhysicsBody()->setVelocity(this->velocity);
    if (this->info->bullet_rotateSpeed() > 0) {
        this->runAction(RepeatForever::create(RotateBy::create(1, this->info->bullet_rotateSpeed())));
    }
}


void EditorBullet::bulletMainLoop(float dt) {

    if (!this->firstIsEnd) {
        if (this->firstTempTime >= this->firstDuration) {
            this->firstIsEnd = true;
            this->firstTempTime = 0;
        } else {
            this->firstTempTime += dt;
        }

        float firstAcceleration = this->info->bullet_firstAcceleration();
        if (firstAcceleration != 0) {
            this->speed = this->info->fireSpeed();
            this->firstAcceleration(dt);
        }
    } else {
        this->startSecond(dt);
    }


    if (this->info->bullet_rotateSpeedPublic() != 0) {
        this->fixAngle(dt);
    }

    if(this->childWeapon != nullptr) {
        this->childWeapon->mainLoop(dt);
    }

    if(this->lifeTime > 0 ) {
        if(this->tempLifeTime > this->lifeTime) {
            this->DeathForce();
        }
        this->tempLifeTime += dt;
//        log("life time is %f",tempLifeTime);
    }
}

void EditorBullet::update(float dt) {
    this->bulletMainLoop(dt);
}

void EditorBullet::fixAngle(float dt) {
    if (this->info->bullet_rotateSpeedPublic() != 0) {
        Vec2 base = this->getPhysicsBody()->getPosition() - this->startPoint;
        float length = base.length();
        base.normalize();
        Vec2 vecLine = base * this->speed;
        float value = sin(-this->info->bullet_rotateSpeedPublic() * Angle2pi);
        Vec2 vec2 = base.forAngle((base.getAngle() * Pi2Angle - 90) * Angle2pi) * length * value;
        this->getPhysicsBody()->setVelocity(vec2 + vecLine);
    }
}

void EditorBullet::firstAcceleration(float dt) {
    float maxSpeed = this->info->bullet_firstAcceleration();
    float firstAcceleration = this->info->bullet_firstAcceleration();
    if (firstAcceleration != 0) {
        if (firstAcceleration > 0) {
            if (this->speed >= maxSpeed) {
                this->speed = maxSpeed;
            } else {
                this->speed += firstAcceleration * dt;
            }
        } else {
            if (this->speed <= maxSpeed) {
                this->speed = maxSpeed;
            } else {
                this->speed += firstAcceleration * dt;
            }
        }
    }
    this->resetVelocity();
}

void EditorBullet::secondAcceleration(float dt) {
    float maxSpeed = this->info->bullet_secondMostSpeed();
    float secondAcceleration = this->info->bullet_secondAcceleration();
    if (secondAcceleration != 0) {
        if (secondAcceleration > 0) {
            if (this->speed >= maxSpeed) {
                this->speed = maxSpeed;
            } else {
                this->speed += secondAcceleration * dt;
            }
        } else {
            if (this->speed <= maxSpeed) {
                this->speed = maxSpeed;
            } else {
                this->speed += secondAcceleration * dt;
            }
        }
    }
    this->resetVelocity();
}

void EditorBullet::startSecondAcceleration(float dt) {
    float secondAcceleration = this->info->bullet_secondAcceleration();
    if (secondAcceleration != 0) {
        this->secondAcceleration(dt);
    }
}

void EditorBullet::resetVelocity() {
    Vec2 base = this->getPhysicsBody()->getPosition() - this->startPoint;
    base.normalize();
    Vec2 vecLine = base * this->speed;
    this->getPhysicsBody()->setVelocity(vecLine);
}

void EditorBullet::startSecond(float dt) {
    if (this->times > 0 && this->info->bullet_firstDuration() > 0) {
        this->times--;
        this->getPhysicsBody()->setVelocity(-this->velocity);
        this->firstIsEnd = false;
        return;
    }

    if(!this->secondSpeedIsSet) {
        this->secondSpeedIsSet = true;
        float secondSpeed = this->info->bullet_secondSpeed();
        if (secondSpeed > 0) {
            this->speed = secondSpeed;
            if (this->info->bullet_rotateSpeedPublic() == 0) {
                Vec2 vec = this->velocity;
                vec.normalize();
                this->getPhysicsBody()->setVelocity(vec * this->speed);
            }
        }
    }

    this->startSecondAcceleration(dt);

    if(!this->secondAnimationIsRun) {
        if (this->info->bullet_invisibleSubDuration() > 0) {
            this->runAction(FadeOut::create(this->info->bullet_invisibleSubDuration()));
        }

        if (this->info->bullet_invisibleDuration() > 0) {
            this->runAction(Sequence::create(DelayTime::create(this->info->bullet_invisibleDuration()), FadeIn::create(0.1), nullptr));
        }
    }
}

void EditorBullet::toRecovery() {
    this->stopAllActions();
    this->unschedule(CC_SCHEDULE_SELECTOR(EditorBullet::bulletMainLoop));
    if (this->info != nullptr && this->info->childBarrageRule != nullptr) {
        if (this->getChildByTag(100) != nullptr) {
            this->removeChildByTag(100, true);
        }
    }

    if(this->getChildByTag(1001)) {
        this->removeChildByTag(1001);
    }
    this->setRotation(0);
    this->info = nullptr;
    this->childWeapon = nullptr;
}

void EditorBullet::setBodyPosition(Vec2 pos) {
    this->setPosition(pos);
}

void EditorBullet::RemoveMyself()
{
//    if(this->isRemoveIng) {
//        return;
//    }
//    this->isRemoveIng = true;
    Bullet::RemoveMyself();
//    this->toRecovery();
//    EditorBulletFactory::getInstance()->recoveryEditorBullet(this);
}