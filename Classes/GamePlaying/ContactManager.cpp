#include "ContactManager.h"
#include "PhysicsUnit.h"

void ContactManager::onEnter()
{
	Layer::onEnter();

	//添加碰撞监听
	m_contactListener = EventListenerPhysicsContact::create();
	m_contactListener->onContactBegin = CC_CALLBACK_1(ContactManager::onContactBegin,this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_contactListener,this);
}

void ContactManager::onExit()
{
	Layer::onExit();
	_eventDispatcher->removeEventListener(m_contactListener);
}

bool ContactManager::onContactBegin( const PhysicsContact& contact )
{
	PhysicsBody* bodyA = contact.getShapeA()->getBody();
	PhysicsBody* bodyB = contact.getShapeB()->getBody();
	if (!bodyA || !bodyB)
	{
		return true;
	}
	PhysicsUnit* puA = (PhysicsUnit*)(bodyA->getNode());
	PhysicsUnit* puB = (PhysicsUnit*)(bodyB->getNode());
	if (!puA || !puB)
	{
		return true;
	}
	Vec2 posA = puA->convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 posB = puB->convertToWorldSpaceAR(Vec2::ZERO);

	BasePro proA = puA->getBasePro();
	BasePro proB = puB->getBasePro();
	proA.setContactPos(posA);
	proB.setContactPos(posB);

	puA->ChangeProperty(proB);
	puB->ChangeProperty(proA);

	return true;
}


//----------------------------------
void PhysicsUnitManager::AddUnit( PhysicsUnit* pu )
{
	m_id++;
	m_id %= 1000000000;
	pu->getBasePro().setId(m_id);

	m_PhysicsUnitVec.insert(m_id, pu);
	//log("------- unit nums = %d", m_PhysicsUnitVec.size());
}

void PhysicsUnitManager::DelUnit( PhysicsUnit* pu )
{
	int id = pu->getBasePro().getId();
	auto it = m_PhysicsUnitVec.find(id);
	if (it == m_PhysicsUnitVec.end())
	{
		//没找到
		log("************** PhysicsUnit has been removed ******************");
	}
	else
	{
		it = m_PhysicsUnitVec.erase(it);
	}
}

void PhysicsUnitManager::DelUnit( int id )
{
	auto it = m_PhysicsUnitVec.find(id);
	if (it == m_PhysicsUnitVec.end())
	{
		//没找到
		log("************** PhysicsUnit has been removed ******************");
	}
	else
	{
		it = m_PhysicsUnitVec.erase(it);
	}
}

void PhysicsUnitManager::ClearUnits()
{
	m_PhysicsUnitVec.clear();
}
