#include "MainPlaneShowLayer.h"
#include "SpecialBody.h"
#include "../GameCommon/GameDefine.h"
#include "../GameCommon/NewDataMgr.h"
#include "../GameCommon/GameData.h"
#include "BasePlane/PlaneFactory.h"
#include "ContactManager.h"


MainPlaneShowLayer::MainPlaneShowLayer()
{
	m_Offset = Vec2::ZERO;
	m_winSize = Director::getInstance()->getWinSize();
}

void MainPlaneShowLayer::onEnter()
{
	Layer::onEnter();
	ContactManager* cm = ContactManager::create();
	addChild(cm);

	/*auto scene = Director::getInstance()->getRunningScene();
	int mask = scene->getPhysicsWorld()->getDebugDrawMask();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);*/

	initEdge();
}

void MainPlaneShowLayer::initEdge()
{
	float exDis = 0.1;
	auto smallEdge = ScreenEdge::create(Size(m_FrameSize.width + 40, m_FrameSize.height + 40));
	smallEdge->setPosition(Vec2(m_winSize.width*0.5, m_winSize.height*0.5) + m_Offset);

	smallEdge->getBasePro().setMyId(UnitId::eScreenEdgeSmall);
	addChild(smallEdge);
}

void MainPlaneShowLayer::ShowMainPlane( PlaneType pt)
{
	Node* node = getChildByName("mainPlane");
	if (node)
	{
		node->removeFromParentAndCleanup(true);
	}
	auto plane = GameData::getInstance()->getPlaneFactory()->CreateMainPlane(pt,false,false);
	plane->setPosition(Vec2(m_winSize.width*0.5, m_winSize.height*0.4) + m_Offset);
	plane->setName("mainPlane");
	addChild(plane,1000);
}

void MainPlaneShowLayer::setPosOffset( const Vec2& pos )
{
	m_Offset = pos;
}

void MainPlaneShowLayer::setFrameSize( const Size& size )
{
	m_FrameSize = size;
}

