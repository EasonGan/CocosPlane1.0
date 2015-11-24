#include "DemoBullet.h"
#include "../GameCommon/GameData.h"
#include "../GameCommon/EffectsManager.h"
#include "../GamePlaying/SpecialBody.h"
#include "../GamePlaying/BasePlane/PlaneFactory.h"
#include "../GamePlaying/Weapon/weapon.h"


DemoShowLayer::DemoShowLayer()
	:_plane(nullptr)
{

}

DemoShowLayer::~DemoShowLayer()
{
	_plane->setShootAble(false);
	_plane->clearWeapon();
	_plane->removeFromParentAndCleanup(true);
	_plane = nullptr;
}

DemoShowLayer* DemoShowLayer::m_DemoShowLayer = nullptr;
void DemoShowLayer::onEnter()
{
	Layer::onEnter();
	//addEdgeBox();

	DemoShowLayer::m_DemoShowLayer = this;
}

DemoShowLayer* DemoShowLayer::getDemoShowLayer()
{
	return m_DemoShowLayer;
}

void DemoShowLayer::showPlaneDemoWithType(PlaneType type)
{
	int curGrade = NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(GameGradeMsg::ObjectType_Role)->curGrade;


	if (_plane)
	{
		_plane->setShootAble(false);
		_plane->clearWeapon();
		_plane->removeFromParentAndCleanup(true);
	}

	_plane = MainPlane::create(type);
	addChild(_plane);

 	auto weaponFact = GameData::getInstance()->getWeaponFactory();
 	weaponFact->AddMainPlaneWp((int)type, curGrade, _plane);

	_plane->setPosition(Vec2(550, 680));
}



