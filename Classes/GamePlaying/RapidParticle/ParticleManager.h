/********************************************************************
	created:	2015/07/20
	created:	20:7:2015   18:32
	filename: 	E:\cocos3.4_projects\PlaneGame\Classes\GamePlaying\ParticleManager.h
	file path:	E:\cocos3.4_projects\PlaneGame\Classes\GamePlaying
	file base:	ParticleManager
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/
#include "cocos2d.h"
#include "cocos-ext.h"
#include "RapidParticle.h"

USING_NS_CC;
USING_NS_CC_EXT;



class ParticleManager
{
public:
	static ParticleManager* getInstance()
	{
		static ParticleManager instance;
		return &instance;
	}

	void purge();
	void releaseAllParticle();

	RapidParticle* getParticle( const std::string& file );

private:
	Map<std::string, RapidParticle*>   m_ParticleFileMap;

};