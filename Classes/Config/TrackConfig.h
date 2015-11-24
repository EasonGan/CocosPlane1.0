/********************************************************************
	created:	2015/07/28
	created:	28:7:2015   16:13
	filename: 	E:\cocos3.4_projects\PlaneGame_Danji\Classes\Config\TrackConfig.h
	file path:	E:\cocos3.4_projects\PlaneGame_Danji\Classes\Config
	file base:	TrackConfig
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/
#ifndef TrackConfig_h__
#define TrackConfig_h__
#include "cocos2d.h"
#include "cocos-ext.h"
#include "../GamePlaying/EnemyPlaneController.h"

USING_NS_CC;
USING_NS_CC_EXT;


class EventInfoObj;
class TrajectoryConfigData;
class TrackManager : public Ref
{
public:
	static TrackManager* create(EnemyPlane* plane, const std::string& trackId);
	void purge();

	void startAction();
private:
	void setTarget(EnemyPlane* plane);
	void getTrackProVec( const std::string& id );
	ActionInterval*	getBezierActions(std::vector<Vec2>& vec, Vector<EventInfoObj*> eventSet,float mvTime, float stayTime );
	void OnceTrackAction();
	void LoopTrackAction();
    void runTrackAction(TrajectoryConfigData* data,const std::function<void()> &func);

	void ActionFuncND(Node* node, void* data);

private:
    TrackManager();
    ~TrackManager()
    {
        this->m_TrackProVec.clear();
        this->m_LoopVector.clear();
        this->m_target = nullptr;
    };
    bool haveLoop;
    bool isSpecial;
	EnemyPlane*									m_target;
	Vector<TrajectoryConfigData*>				m_TrackProVec;
	Vector<TrajectoryConfigData*>::iterator		m_TrackProVecIte;
    Vector<TrajectoryConfigData*>               m_LoopVector;
    Vector<TrajectoryConfigData*>::iterator     m_LoopVectorIt;
};


bool TimeCompare(EventInfoObj* ep1, EventInfoObj* ep2 );





#endif // TrackConfig_h__
