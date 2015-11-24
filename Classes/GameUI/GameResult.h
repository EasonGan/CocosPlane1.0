#ifndef _GAME_RESULT_H_
#define _GAME_RESULT_H_


#include "cocos2d.h"
#include "cocos-ext.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "../GamePlaying/Layer/UIController.h"
#include "GameRoleScene.h"

USING_NS_CC;
USING_NS_CC_EXT;


using namespace cocostudio::timeline;
using namespace cocos2d::ui;

class UIController;


/*******************************************
*	胜利结算界面
*******************************************/
class GameResultSuccess : public Layer ,public CommonLayer
{
public:

	enum ButtonTag
	{
		Tag_Back,

		Tag_Continut,
		Tag_Again,
	};

	GameResultSuccess()
		:_myLayer(nullptr)
	{

	}

	~GameResultSuccess()
	{

	}

	static void showLayout(Node* node, int tag);
	virtual void onEnter();
	virtual void onExit();

	CREATE_FUNC(GameResultSuccess);

private:
	virtual void onMessageEvent(Ref* sender);
	virtual void setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback);
	virtual void callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType);

	void initInterface();

	void playTextFlash(void);
	void playStarAction( void );
	void playWingAction(void);
	void playStarGradeAction(Node* star, float delay);
	void callFuncPlayStarAction(Node* node);
	void callFuncMainStarAction(Node* node);
	void callFuncGuideCard( void );

	void deleyGuideCard(void);

private:
	Node*	_myLayer;
	
};

/*******************************************
*	失败结算界面
*******************************************/
class GameResultFail : public Layer, public CommonLayer
{
public:

	enum ButtonTag
	{
		Tag_Back,

		Tag_Continut,
		Tag_Again,
	};

	GameResultFail()
		:_myLayer(nullptr)
	{

	}

	~GameResultFail()
	{

	}

	static void showLayout(Node* node, int tag);
	virtual void onEnter();
	virtual void onExit();

	CREATE_FUNC(GameResultFail);

private:
	virtual void onMessageEvent(Ref* sender);
	virtual void setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback);
	virtual void callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType);

	void initInterface();
	void playStarGradeAction(Node* star, int delay);
	void callFuncPlayStarAction(Node* node);

private:
	Node*	_myLayer;

};

#endif