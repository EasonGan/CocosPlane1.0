/********************************************************************
	created:	2015/09/16
	created:	16:9:2015   17:39
	filename: 	E:\cocos3.4_projects\PlaneGame_Danji\Classes\GamePlaying\MainPlaneShowLayer.h
	file path:	E:\cocos3.4_projects\PlaneGame_Danji\Classes\GamePlaying
	file base:	MainPlaneShowLayer
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/
#ifndef MainPlaneShowLayer_h__
#define MainPlaneShowLayer_h__
#include "cocos2d.h"
#include "cocos-ext.h"
#include "../GameCommon/GameDefine.h"

USING_NS_CC;
USING_NS_CC_EXT;

class MainPlaneShowLayer : public Layer
{
public:
	MainPlaneShowLayer();
	CREATE_FUNC(MainPlaneShowLayer);

	virtual void onEnter();
	void initEdge();
	void setPosOffset(const Vec2& pos);
	void ShowMainPlane(PlaneType pt);
	void setFrameSize(const Size& size);
private:
	Vec2			m_Offset;
	Size			m_winSize;
	Size			m_FrameSize;

};













#endif // MainPlaneShowLayer_h__
