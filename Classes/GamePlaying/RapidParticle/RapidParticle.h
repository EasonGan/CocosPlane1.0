/********************************************************************
	created:	2015/07/28
	created:	28:7:2015   16:15
	filename: 	E:\cocos3.4_projects\PlaneGame_Danji\Classes\GamePlaying\RapidParticle\RapidParticle.h
	file path:	E:\cocos3.4_projects\PlaneGame_Danji\Classes\GamePlaying\RapidParticle
	file base:	RapidParticle
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/
#ifndef RapidParticle_h__
#define RapidParticle_h__
#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;




class RapidParticle : public ParticleSystemQuad
{
public:
	static RapidParticle* create(const std::string& file);	//正常的创建方式，从文件中读取
	static RapidParticle* createWithExsitParticle(RapidParticle* particle);


	bool initParticlePro(RapidParticle* particle );
};











#endif // RapidParticle_h__
