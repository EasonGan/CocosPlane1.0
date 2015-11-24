#include "PlaneSkin.h"
#include "../../GameCommon/GameData.h"
#include "../Weapon/WeaponFactory.h"

Sprite* PlaneSkin::createSkinWithPicture( const std::string &name )
{
	auto skin = Sprite::createWithSpriteFrameName(name);
	return skin;
}

Armature* PlaneSkin::createSkinWithArmatureById( int id )
{

	return nullptr;
}

//----------------------------------
PlaneArmature* PlaneArmature::create( const std::string& name, EnemyPlane* target)
{
	PlaneArmature *armature = new (std::nothrow) PlaneArmature();
	if (armature && armature->init(name))
	{
		armature->setPlaneTarget(target);
		armature->autorelease();
		return armature;
	}
	CC_SAFE_DELETE(armature);
	return nullptr;
}

void PlaneArmature::setPlaneTarget( EnemyPlane* target )
{
	m_Target = target;
}

void PlaneArmature::playRestAnimation( int idx /*= 0*/ )
{
	getAnimation()->playWithIndex(idx);
}

void PlaneArmature::playShootAnimation( int idx /*= 1*/ )
{
	auto animat = getAnimation();
	animat->playWithIndex(idx);
	animat->setFrameEventCallFunc(this, frameEvent_selector(PlaneArmature::onFrameEventOfShoot));
}

void PlaneArmature::playHackAnimation( int idx /*= 2*/ )
{
	getAnimation()->playWithIndex(idx);
	auto animat = getAnimation();
	animat->playWithIndex(idx);
	animat->setFrameEventCallFunc(this, frameEvent_selector(PlaneArmature::onFrameEventOfHack));
}

void PlaneArmature::onFrameEventOfShoot( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "shoot")
	{
		StartShoot();
	}
	else if(evt == "end")
	{
		playRestAnimation();
	}
}

void PlaneArmature::onFrameEventOfHack( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex )
{
	if (evt == "hack")
	{
		
	}
	else if(evt == "end")
	{
		playRestAnimation();
	}
}

void PlaneArmature::StartShoot()
{
	m_Target->addConfigWeapon(true);
}
