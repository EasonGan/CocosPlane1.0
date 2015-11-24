/********************************************************************
	created:	2015/07/30
	created:	30:7:2015   16:39
	filename: 	E:\cocos3.4_projects\PlaneGame_Danji\Classes\Config\WeaponConfig.h
	file path:	E:\cocos3.4_projects\PlaneGame_Danji\Classes\Config
	file base:	WeaponConfig
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/
#ifndef WeaponConfig_h__
#define WeaponConfig_h__
#include "cocos2d.h"
#include "cocos-ext.h"
#include "../GamePlaying/Weapon/WeaponDelegate.h"

USING_NS_CC;
USING_NS_CC_EXT;


class WeaponConfig : public Ref
{
public:
	static std::vector<WeaponDelegate> getWeaponConfigWithId(int id = 0);
	std::vector<WeaponDelegate>		m_WeaponConfigVec;
};













#endif // WeaponConfig_h__
