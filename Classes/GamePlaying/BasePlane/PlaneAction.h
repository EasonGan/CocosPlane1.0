/********************************************************************
创建于:		2014/12/08  11:13	
文件名: 	E:\Cocos3.3_Project\MyGame\Classes\EnterAction.h 
作  者:		alex(alex)
修改于：	2014/12/08  11:13	

目  的:		敌方飞机入场的动作
*********************************************************************/
#ifndef _PLANE_ACTION_H_
#define _PLANE_ACTION_H_


#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

extern float LinePos[][2];

class BasePlane;
class EnterAction : public Ref
{
public:
	static EnterAction* create();
	void purge();

	bool init();

	ActionInterval* AddEnemyActionByIdx(int ActionId,BasePlane* pl);

	//按预先设定的轨迹飞行(方向：左上)
	ActionInterval* CreateCardinalSplineL2U(BasePlane* pl);

	//按预先设定的轨迹飞行(方向：左下)
	ActionInterval* CreateCardinalSplineL2B(BasePlane* pl);

	//按预先设定的轨迹飞行(方向：右上)
	ActionInterval* CreateCardinalSplineR2U(BasePlane* pl);

	//按预先设定的轨迹飞行(方向：右下)
	ActionInterval* CreateCardinalSplineR2B(BasePlane* pl);
	
	//向下飞行屏幕上面边缘
	ActionInterval* CreateMoveToUpEdge(BasePlane* pl);

	//向下飞行到屏幕上部的随机位置
	ActionInterval* CreateMoveToScreen(BasePlane* pl);

	//从左/右边出现值屏幕随机位置
	ActionInterval* CreateHorizonMoveToScreen(BasePlane* pl);

	//从左/右出平移屏幕的固定位置
	ActionInterval* CreateMoveToFasteningPos(BasePlane* pl);

	/************************************/
	//从右边出来，由上至下绕回来
	ActionInterval* CreateBezierFromUpToBackAtRight(BasePlane* pl);
	
	//从右边出来，由下至上绕回来
	ActionInterval* CreateBezierFromBottomToBackAtRight(BasePlane* pl);

	//从左边出来，由上至下绕回来
	ActionInterval* CreateBezierFromUpToBackAtLeft(BasePlane* pl);

	//从左边出来，由下至上绕回来
	ActionInterval* CreateBezierFromBottomToBackAtLeft(BasePlane* pl);

	//从右边出来，曲线运动到左边
	ActionInterval* CreateBezierRight2Left(BasePlane* pl);

	//从左边出来，曲线运动到右边
	ActionInterval* CreateBezierLeft2Right(BasePlane* pl);

	//从左上运动到中间，回到右上
	ActionInterval* CreateBezierFromLP2RP(BasePlane* pl);

	//从右上运动到中间，回到左上
	ActionInterval* CreateBezierFromRP2LP(BasePlane* pl);

	//从右边出来，走波浪线
	ActionInterval* CreateBezierWaveAtRight(BasePlane* pl);

	//从左边出来，走波浪线
	ActionInterval* CreateBezierWaveAtLeft(BasePlane* pl);

private:
	Size		m_winSize;
};


#endif