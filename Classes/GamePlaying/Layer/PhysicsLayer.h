#ifndef __PHYSICS_LAYER_H__
#define __PHYSICS_LAYER_H__

#include "cocos2d.h"
#include "../EnemyPlaneController.h"

USING_NS_CC;

class BasePlane;
class MyPlane;
class PhysicsLayer : public cocos2d::Layer
{
public:
	PhysicsLayer();
	~PhysicsLayer();
	static Scene* createScene();
	CREATE_FUNC(PhysicsLayer);

    virtual bool init();
	virtual void onEnter();
	virtual void onExit();
	virtual bool onTouchBegan(Touch *touch, Event *unused_event); 

    void update(float dt);
	// 返回键暂停处理
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	void doPause( void );
	void callFuncButtonEvent( void );
	void callFuncButtonExitEvent( void );

	void addPlane();
	void addPlaneWithProtect();
	void delayAddPlane();
	void delayAddPlaneWithProtect();

	//添加围墙
	void addEdgeBox();
	
	//BOSS出场提示
	void BossIn();
	void addWarning();
	void BossInCB(Node* node);

	//主角受到伤害提示
	void BeHurtWarning(float dur = 1.0f);
	void removeContactEff(Node* node);

	//变黑一段时间
	void ToBlack(const float& duration);
	void BlackCallback(Node* node );

	//各个关卡对应的胜负判断
	void CheckGameResult();

	//生存
	void SurvivalLvl(float dt);

	//收集
	void CollectLvl(float dt);

	//击杀
	void KillLvl(float dt);

	//游戏帮助提示
	void addTips();
	void TipsCallback();

	//部分关卡陨石掉落
	void addFallingStone();

	void initEdge();
    PhysicsWorld*		m_physicsWorld;
private:
	EventDispatcher*	m_eventDispatcher;
	std::vector<Vec2>	m_EdgePosVec;
};



#endif // __HELLOWORLD_SCENE_H__
