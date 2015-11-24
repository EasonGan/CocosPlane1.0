#include "WeaponConfig.h"
#include "../GamePlaying/EnemyPlaneController.h"

std::vector<WeaponDelegate> WeaponConfig::getWeaponConfigWithId( int id /*= 0*/ )
{
	auto wpConfig = EnemyPlaneController::getInstance()->getWeaponConfigDataFromId(String::createWithFormat("%d", id)->getCString());
	
	std::vector<WeaponDelegate> weaponVec;

	auto wpVec = wpConfig->weapon_vector;
	int wpNum = wpVec.size();
	for (int i = 0; i < wpNum; i++)
	{
		//WeaponDelegate weaponConfig;

		//WeaponInfoObj* wpPro = wpVec.at(i);
		////weaponConfig.setBltResTag(ResTag::Bullet_Self_1);
		//weaponConfig.setBltResTag((ResTag)wpPro->resIndex());
		//weaponConfig.setBltScaleX(wpPro->scaleX());
		//weaponConfig.setBltScaleY(wpPro->scaleY());
		//weaponConfig.setBltSpeed(wpPro->fireSpeed());
		//weaponConfig.setConfigWeaponAngle(wpPro->fireAngle());
		//weaponConfig.setConfigShootInterval(wpPro->barrageFireGap());
		//weaponConfig.setConfigWeaponDuration(wpPro->fireDuration());
		//weaponConfig.setConfigWeaponOffsetX(wpPro->distanceToWeaponX());
		//weaponConfig.setConfigWeaponOffsetY(wpPro->distanceToWeaponY());
		//weaponConfig.setConfigWeaponRotationEachShoot(wpPro->fireChangedAngle());
		//weaponConfig.setConfigWeaponMaxRotattion(wpPro->fireMaxAngle());
		//weaponConfig.setConfigWpNum(wpPro->bulletNumber());
		//weaponConfig.setConfigExDis(wpPro->bulletGap());
		//weaponConfig.setConfigMinDis(wpPro->bulletMinGap());
		//weaponConfig.setConfigZhenFu(wpPro->bulletGapAmplitude());
		////weaponConfig.setConfigIsAmmi(wpPro->);
		//weaponVec.push_back(weaponConfig);
	}

	return weaponVec;
}

