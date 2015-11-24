/********************************************************************
创建于:		2014/12/30  15:19	
文件名: 	E:/Cocos3.3_Project\MyGame\Classes\GameHelp.h 
作  者:		alex(alex)
修改于：	2014/12/30  15:19	

目  的:		游戏帮助/提示
*********************************************************************/
#ifndef __GameHelp_h__
#define __GameHelp_h__


#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

/************************************************************************/
/* 
游戏指引
*/
/************************************************************************/
class GameHelp : public Sprite
{
public:
	static GameHelp* create();

	


private:
	void addMenu();
	void confirmCallback(Ref* sender);
};


#endif // GameHelp_h__