/********************************************************************
������:		2015/03/13  15:23	
�ļ���: 	E:\Cocos3.3_Project\PlaneGame2\Classes\GamePlaying\BasePlane\PlaneFactory.h 
��  ��:		alex()
�޸��ڣ�	2015/03/13  15:23	

Ŀ  ��:		�ɻ����칤��
*********************************************************************/
#ifndef __PlaneFactory_H__
#define __PlaneFactory_H__


#include "cocos2d.h"
#include "cocos-ext.h"
#include "BasePlane.h"
#include "../EnemyPlaneController.h"

USING_NS_CC;
USING_NS_CC_EXT;

//�ɻ�
class PlaneFactory : public Ref
{
public:
	static PlaneFactory* create();
	void purge();

	bool init();


	BasePlane* createEnemyPlane(LevelPlaneConfigData* configData);
    BasePlane* createEnemyPlane(int id, float userdata, int gamelvl);
    MainPlane* CreateMainPlane(const PlaneType& tp, bool isMoveUp = true, bool playMusic = true);

	float getBossTimeOut(int gameLvl);

};

#endif // __PlaneFactory_H__