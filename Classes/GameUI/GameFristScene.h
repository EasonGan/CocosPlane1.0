#ifndef __GAME_FIRST_SCENE_H__
#define __GAME_FIRST_SCENE_H__


#include "cocos2d.h"
USING_NS_CC;


/************************************************************************/
/* 游戏首界面                                                           */
/************************************************************************/

class GameFristScene : public Layer
{
public:
	GameFristScene()
		:_listener(nullptr)
	{

	}
	~GameFristScene()
	{

	}

public:
	CREATE_FUNC(GameFristScene);
	static Scene* createScene();

	virtual void onEnter();
	virtual void onExit(void);
	// touch event
	virtual bool onTouchBegan(Touch *touch, Event *unusedEvent);
	virtual void onTouchMoved(Touch *touch, Event *unusedEvent);
	virtual void onTouchEnded(Touch *touch, Event *unusedEvent);

	// show interface
	void initInterface( void );
	void callFuncPlayEffect(Node* node);

	void addAnimNode(char* name, int animoNum, float delay, Vec2 position, Node* parent);
private:
	EventListenerTouchOneByOne *_listener;

};



#endif