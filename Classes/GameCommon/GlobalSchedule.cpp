#include "GlobalSchedule.h"


GlobalSchedule* GlobalSchedule::m_pSchedule = NULL; 
float	GlobalSchedule::m_countTime = 0;
float	GlobalSchedule::m_fInterval = 0;

GlobalSchedule::GlobalSchedule(float fInterval, float fDelay) 
{ 
	SCHEDULE->scheduleSelector( 
		schedule_selector(GlobalSchedule::globalUpdate), 
		this, 
		fInterval, 
		CC_REPEAT_FOREVER, 
		fDelay,
		false); 

	m_pSchedule = this; 
} 

GlobalSchedule::~GlobalSchedule() 
{ 
	CCLOG("GlobalSchedule().~()"); 

	SCHEDULE->unscheduleSelector( 
		schedule_selector(GlobalSchedule::globalUpdate), this); 
} 

void GlobalSchedule::globalUpdate(float t) 
{ 
	GlobalSchedule::m_countTime += 1;

	// 这里写全局定时器的逻辑处理代码 
	log("global update [%f]",GlobalSchedule::m_countTime ); 
} 

void GlobalSchedule::start(float fInterval, float fDelay) 
{ 
	GlobalSchedule::m_countTime = 0;
	GlobalSchedule::m_fInterval = fInterval;

	new (std::nothrow) GlobalSchedule(fInterval, fDelay); 
} 

float GlobalSchedule::stop() 
{ 
	CCAssert(m_pSchedule, "no define"); 
	CC_SAFE_DELETE(m_pSchedule); 
	return GlobalSchedule::m_countTime * GlobalSchedule::m_fInterval;
} 

void GlobalSchedule::pause() 
{ 
	CCAssert(m_pSchedule, "no define"); 
	SCHEDULE->pauseTarget(m_pSchedule); 
} 

void GlobalSchedule::resume() 
{ 
	CCAssert(m_pSchedule, "no define"); 
	SCHEDULE->resumeTarget(m_pSchedule); 
} 
