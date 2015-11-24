#include "TrackConfig.h"
#include "../GameCommon/GameData.h"
#include "../GamePlaying/AI.h"

bool TimeCompare( EventInfoObj* ep1, EventInfoObj* ep2 )
{
	if (ep1->event_time() > ep2->event_time())
	{
		return true;
	}
	return false;
}

TrackManager* TrackManager::create(EnemyPlane* plane, const std::string& trackId)
{
	TrackManager* tc = new (std::nothrow) TrackManager();
	if (tc)
	{
		tc->retain();
		tc->setTarget(plane);
		tc->getTrackProVec(trackId);
		tc->autorelease();
		return tc;
	}
	CC_SAFE_DELETE(tc);
	return nullptr;
}

TrackManager::TrackManager()
{
    this->haveLoop = false;
    this->isSpecial = false;
}

void TrackManager::purge()
{
	release();
}

void TrackManager::setTarget( EnemyPlane* plane )
{
	m_target = plane;
}

void TrackManager::getTrackProVec( const std::string& id )
{
	auto dataObj = EnemyPlaneController::getInstance()->getTrajectoryConfigDataFromId(id);
	if (dataObj)
	{
		m_TrackProVec = dataObj->tre_vector();
		m_TrackProVecIte = m_TrackProVec.begin();
	}
}

void TrackManager::startAction()
{

    if(m_TrackProVec.size() == 1) {
        if(this->m_target) {
            this->m_target->m_isAutoFire = true;
        }
    }

	OnceTrackAction();
}

ActionInterval* TrackManager::getBezierActions( std::vector<Vec2>& PosVec, Vector<EventInfoObj*> eventSet, float mvTime, float stayTime )
{
	ccBezierConfig config;
	config.controlPoint_1 = PosVec.at(0);
	config.controlPoint_2 = PosVec.at(1);
	config.endPosition = PosVec.at(2);
	
	BezierBy* bezier = BezierBy::create(mvTime, config);

	EventInfoObj* ep = nullptr;
	const char* str = "";
	float shootTime = 0.0f;

	ActionInterval* tempAction = nullptr;
	ActionInterval* spw = nullptr;

    sort(eventSet.begin(), eventSet.end(), TimeCompare);

	for (int i = 0; i < eventSet.size(); i++)
	{
		ep = eventSet.at(i);
		shootTime = ep->event_time();

		//int DanMuId = atoi(ep->func_str().c_str());
		int DanMuId = 0;
		void* data = (void*)DanMuId;
		auto wait = DelayTime::create(shootTime);
		auto func = __CCCallFuncND::create(this, callfuncND_selector(TrackManager::ActionFuncND), data);
		auto seq = Sequence::create(wait, func, nullptr);
		if (tempAction)
		{
			spw = Sequence::createWithTwoActions(tempAction, seq);
		}
		else
		{
			spw = seq;
		}
		tempAction = spw;
	}
    
    if(eventSet.size() != 0) {
        auto stay = DelayTime::create(stayTime);
        
        if (spw)
        {
            spw = Sequence::createWithTwoActions(bezier,spw);
            auto onceAction = Sequence::create(spw, stay, nullptr);
            return onceAction;
        }
        else
        {
            auto onceAction = Sequence::create(bezier, stay - 2, nullptr);
            
            return onceAction;
        }
    } else {
        int DanMuId = 0;
        void* data = (void*)DanMuId;
        auto onceAction = Sequence::create(bezier,CallFunc::create(CC_CALLBACK_0(TrackManager::ActionFuncND,this,m_target,data)), nullptr);
        this->isSpecial = true;
        return onceAction;
    }
}

void TrackManager::OnceTrackAction()
{
	if (m_TrackProVec.empty())
	{
		return;
	}
	if (m_TrackProVecIte != m_TrackProVec.end())
	{
		TrajectoryConfigData* tp = (*m_TrackProVecIte);

        if(this->haveLoop) {
            this->m_LoopVector.pushBack(tp);
        }
        if(tp->isLoop()) {
            this->haveLoop = true;
        }
        this->runTrackAction(tp, CC_CALLBACK_0(TrackManager::OnceTrackAction, this));
		m_TrackProVecIte++;
	}
	else
	{
		if(this->m_LoopVector.size() > 0) {
            this->m_LoopVectorIt = this->m_LoopVector.begin();
            LoopTrackAction();
        } else {
            float speed = -80;
            if(this->isSpecial) {
                speed = -800;
            }
            this->m_target->getPhysicsBody()->setVelocity(Vec2(0,speed));
        }
	}
}

void TrackManager::LoopTrackAction()
{
    if(this->m_LoopVectorIt >= this->m_LoopVector.end()) {
        this->m_LoopVectorIt = this->m_LoopVector.begin();
    }
    this->runTrackAction(*this->m_LoopVectorIt, CC_CALLBACK_0(TrackManager::LoopTrackAction, this));
    this->m_LoopVectorIt++;
}

void TrackManager::runTrackAction(TrajectoryConfigData* data,const std::function<void()> &func)
{
    std::vector<Vec2> posVec = data->point_list();
    Vector<EventInfoObj*> eventVec = data->event_list();
    float stayTime = data->stayTime();
    auto mvShoot = getBezierActions(posVec, eventVec, data->time(), stayTime);
    auto next = CallFunc::create(func);
    auto seq = Sequence::create(mvShoot, next, nullptr);
    this->m_target->runAction(seq);
}

void TrackManager::ActionFuncND( Node* node, void* data )
{
    if(this->m_target) {
        if(this->m_target->getIsMoveIn())
        {
            this->m_target->setIsMoveIn(false);
        }
        m_target->StartFire();
    }
}
