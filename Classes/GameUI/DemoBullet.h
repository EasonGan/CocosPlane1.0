#pragma once
#include "cocos2d.h"
#include "../GameCommon/GameDefine.h"
#include "../GamePlaying/Weapon/weapon.h"
#include "../GamePlaying/BasePlane/BasePlane.h"

USING_NS_CC;

class DemoShowLayer : public cocos2d::Layer
{
public:
	DemoShowLayer();
	~DemoShowLayer();

	static DemoShowLayer* getDemoShowLayer();

	CREATE_FUNC(DemoShowLayer);

	virtual void onEnter();

	void showPlaneDemoWithType(PlaneType type);
	CC_SYNTHESIZE(PlaneType, mPlaneType, MainPlaneType);

/*	void initDemo*/
private:
	static DemoShowLayer* m_DemoShowLayer;

	MainPlane* _plane;
};
	