#include "PhysicsLayer.h"
#include "UIController.h"
#include "MenuLayer.h"
#include "RollingBg.h"
#include "../ContactManager.h"
#include "../SpecialBody.h"
#include "../BasePlane/BasePlane.h"
#include "../BasePlane/PlaneManager.h"
#include "../BossTimeOut.h"
#include "../../GameCommon/EffectsManager.h"
#include "../../GameCommon/GameData.h"
#include "../../GameCommon/NewAudio.h"
#include "../../GameUI/CommonWidget.h"
#include "../../GameUI/GameSceneMgr.h"
#include "../../GameUI/GameMainLayer.h"
#include "../../GameUtil/TimeInterval.h"
#include "../../GameCommon/NewDataMgr.h"
#include "CloudLayer.h"
#include "../BasePlane/PlaneFactory.h"
#include "../RapidParticle/ParticleManager.h"
#include "../../GameUI/ItemRewardTips.h"

using namespace experimental;
USING_NS_CC;

#define CreateGroupActionTag	2015210


PhysicsLayer::PhysicsLayer()
{
    this->scheduleUpdate();
}

PhysicsLayer::~PhysicsLayer()
{
	ParticleManager::getInstance()->releaseAllParticle();
}


void PhysicsLayer::update(float dt)
{
    this->m_physicsWorld->step(dt);
}

Scene* PhysicsLayer::createScene()
{
    auto scene = Scene::createWithPhysics();
#if WIN32
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
#endif
	scene->getPhysicsWorld()->setGravity(Vec2(0,-500));
    scene->getPhysicsWorld()->setAutoStep(false);
	GameData::getInstance()->GameStart();

	//地图层
	auto rollingbg = RollingBg::create();
	scene->addChild(rollingbg,-100);
	UIController::getInstance()->setGameBgMgr(rollingbg);

	//云层
	auto cloudLayer = CloudLayer::create();
	scene->addChild(cloudLayer, -99);
	UIController::getInstance()->setCloudLayer(cloudLayer);

	//碰撞监听层
	ContactManager* cm = ContactManager::create();
	scene->addChild(cm);

	//物理层
    auto Physicslayer = PhysicsLayer::create();
    Physicslayer->m_physicsWorld = scene->getPhysicsWorld();
    scene->getPhysicsWorld()->setAutoStep(false);
    scene->addChild(Physicslayer);
	UIController::getInstance()->setPhysicsLayer(Physicslayer);

	//菜单层
	auto menuLayer = MenuLayer::create();
	scene->addChild(menuLayer,100);
	UIController::getInstance()->setMenuLayer(menuLayer);

	//特效层
	auto effectslayer = EffectsManager::create();
	scene->addChild(effectslayer,200);

	GameMainLayer* mainLayer = GameMainLayer::create();
	scene->addChild( mainLayer,300 );

	UIController::getInstance()->setMainUILayer( mainLayer );
	UIController::getInstance()->setEffectsLayer(effectslayer);

	GameData::getInstance()->setisDisabledKey(false);

	return scene;
}

bool PhysicsLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    return true;
}

void PhysicsLayer::onEnter()
{
	Layer::onEnter();
	CCSize s = Director::sharedDirector()->getWinSize();
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(PhysicsLayer::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(PhysicsLayer::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(PhysicsLayer::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	auto listener = EventListenerKeyboard::create();
	listener->onKeyReleased = CC_CALLBACK_2(PhysicsLayer::onKeyReleased,this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	addPlane();
	
	//游戏指引
	//addTips();
	//addFallingStone();
	addEdgeBox();
	initEdge();
	CheckGameResult();

	NewAudio::getInstance()->playBgMusicForce(Music_Bg_Play);

	//飞行距离
	FlyDistance* flydis = FlyDistance::create();
	addChild(flydis);

	//schedule(schedule_selector(PhysicsLayer::refreshEmemyByNums),0.5f);
	
	//GameData::getInstance()->getPlaneManager()->Rest(100000.0f);
	int lvl = NewDataMgr::getInstance()->getCurLevel();
    EnemyPlaneController::getInstance()->startLevelWithConfigFile(lvl,!NewDataMgr::getInstance()->getLevelFinishState(lvl));

	//加载第一个TMX文件
//	GameData::getInstance()->getPlaneManager()->InitLvlData(1);
//	GameData::getInstance()->getPlaneManager()->AddGiftPlaneGroup();
}

void PhysicsLayer::onExit()
{
	Layer::onExit();
	ItemRewardTips::getInstance(nullptr)->stop();
	EnemyPlaneController::getInstance()->exitGame();//停止更新敌机
	PhysicsUnitManager::getInstance()->ClearUnits();
	GameData::getInstance()->resetGameData();
}

void PhysicsLayer::onKeyReleased( EventKeyboard::KeyCode keyCode, Event* event )
{
	bool temp = GameData::getInstance()->getisDisabledKey(); 
	if ( temp )
	{
		return;
	}

	bool ispause = GameData::getInstance()->isGamePause();
	if (ispause)
	{
		return;
	}
	
	if (keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE
		|| keyCode == EventKeyboard::KeyCode::KEY_ESCAPE )
	{
		// 界面暂停
		doPause();
	}
}

void PhysicsLayer::doPause( void )
{
// 	Size size = Director::getInstance()->getVisibleSize();
// 	CCRenderTexture *renderTexture = CCRenderTexture::create(size.width, size.height);
// 
// 	//遍历Game类的所有子节点信息，画入renderTexture中。
// 	//这里类似截图。
// 	renderTexture->begin(); 
// 	this->getParent()->visit();
// 	renderTexture->end();
// 
// 	//将游戏界面暂停，压入场景堆栈。并切换到GamePause界面
// 	CCDirector::sharedDirector()->pushScene(GamePauseScene::create(renderTexture)

	// 检测重复加载
	if( getChildByName("pauseWidget") )
		return;

	GameMainLayer::DelayPuse(this);
	 
// 	auto widget = CommonWidget::create();
// 	widget->setContent("是否退出？");
// 	widget->addBtn1ClickEventListener(CC_CALLBACK_0(PhysicsLayer::callFuncButtonEvent, this));
// 	widget->addBtnExitClickEventListener(CC_CALLBACK_0(PhysicsLayer::callFuncButtonExitEvent, this));
	//UIController::getInstance()->getMainUILayer()->addChild( widget ,15000, "pauseWidget");
}

void PhysicsLayer::callFuncButtonEvent( void )
{
	GameData::getInstance()->GameResume();
	GameSceneMgr::getInstance()->changeScene( Scene_Type_RoleInterface );
}

void PhysicsLayer::callFuncButtonExitEvent( void )
{
	GameData::getInstance()->GameResume();
}

void PhysicsLayer::addPlane()
{
	auto delay = DelayTime::create(0.5);
	auto func = CallFunc::create(this,callfunc_selector(PhysicsLayer::delayAddPlane));
	auto seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void PhysicsLayer::addPlaneWithProtect()
{
	auto delay = DelayTime::create(0.5);
	auto func = CallFunc::create(this,callfunc_selector(PhysicsLayer::delayAddPlaneWithProtect));
	auto seq = Sequence::create(delay, func, nullptr);
	runAction(seq);
}

void PhysicsLayer::delayAddPlane()
{
	int curRole = NewDataMgr::getInstance()->mRoleConfig.roleIndex;
	auto plane = GameData::getInstance()->getPlaneFactory()->CreateMainPlane((PlaneType) curRole);
	plane->setNeedTrackParticle(true);
	UIController::getInstance()->getPhysicsLayer()->addChild(plane);
	//GameData::getInstance()->setMainPlane(plane);

	auto mainpl = GameData::getInstance()->getMainPlane();
    if(UIController::getInstance()->getMainUILayer()->shieldIsEnough())
	{
		if (mainpl)
		{
			if(EnemyPlaneController::getInstance()->isGuideLevel()) {
			 	mainpl->setInvincibleAbsolutely(true);
			}

            mainpl->AddProtect();
//            mainpl->setShootAble(false);
		}
	}
}

void PhysicsLayer::delayAddPlaneWithProtect()
{
	int curRole = NewDataMgr::getInstance()->mRoleConfig.roleIndex;
	auto plane = GameData::getInstance()->getPlaneFactory()->CreateMainPlane((PlaneType) curRole);
	plane->setNeedTrackParticle(true);
	UIController::getInstance()->getPhysicsLayer()->addChild(plane);
	GameData::getInstance()->setMainPlane(plane);

	//加个护盾
	MainPlane* pl = GameData::getInstance()->getMainPlane();
	pl->AddProtect();
}

void PhysicsLayer::addEdgeBox()
{
	int houdu = 3000;
	Size s = Director::getInstance()->getWinSize();
	Wall* bottomWall = Wall::create(Size(houdu,houdu),PhysicsMaterial(0.0,1.0,0.0));
	bottomWall->setPosition(ccp(s.width * 0.5, -houdu * 0.5));
	bottomWall->setUnitId(UnitId::eBottomWall);
	addChild(bottomWall);

	Wall* upWall = Wall::create(Size(houdu,houdu));
	upWall->setPosition(ccp(s.width * 0.5, s.height + houdu * 0.5 ));
	upWall->setUnitId(UnitId::eUpWall);
	addChild(upWall);

	Wall* leftWall = Wall::create(Size(houdu,houdu));
	leftWall->setPosition(ccp(-houdu * 0.5, s.height * 0.5));
	leftWall->setUnitId(UnitId::eLeftWall);
	addChild(leftWall);

	Wall* rightWall = Wall::create(Size(houdu,houdu));
	rightWall->setPosition(ccp(s.width + houdu * 0.5, s.height * 0.5));
	rightWall->setUnitId(UnitId::eRightWall);
	addChild(rightWall);
}

bool PhysicsLayer::onTouchBegan( Touch *touch, Event *unused_event )
{
	bool isPause = GameData::getInstance()->isGamePause();
	if (isPause)
	{
		return false;
	}

	//static TimeInterval timeIn;
	//timeIn.AddNewTime();
	//if (timeIn.IsTimeFree(0.2))
	//{
	//	return false;
	//}
	////log(" touch interval > 1s");
	//timeIn.ClearTime();

	////能量条是否充满
	//bool isFull = GameData::getInstance()->getisEnergyFull();
	//if (!isFull)
	//{
	//	return false;
	//}
	
	//UIController::getInstance()->addHelpTipsForContact();

	return true;
}

void PhysicsLayer::BossIn()
{
	addWarning();
}

void PhysicsLayer::BossInCB( Node* node )
{
	node->removeFromParentAndCleanup(true);
}

void PhysicsLayer::addWarning()
{
	NewAudio::getInstance()->playEffect(Music_BossIn);
	WarningOfBoss* layer = WarningOfBoss::create();
	addChild(layer);
}

void PhysicsLayer::ToBlack(const float& duration )
{
	auto layer = LayerColor::create(ccc4(0,0,0,0));
	addChild(layer,-2000);

	float timescale = Director::getInstance()->getScheduler()->getTimeScale();

	ActionInterval* fadein = FadeIn::create(duration * 0.01 * timescale);
	ActionInterval* delay = DelayTime::create(duration * 0.5 * timescale);
	ActionInterval* fadeout = FadeOut::create(duration * 0.4 * timescale);
	CallFuncN* funcN = CallFuncN::create(this,callfuncN_selector(PhysicsLayer::BlackCallback));
	ActionInterval* seq = Sequence::create(fadein,delay,fadeout,funcN,nullptr);
	layer->runAction(seq);
}

void PhysicsLayer::BlackCallback( Node* node )
{
	node->removeFromParentAndCleanup(true);
}

void PhysicsLayer::CheckGameResult()
{
	auto gametype = 1;
	switch (gametype)
	{
	case 2:	//BOSS
		break;
	case 1:	//生存
		schedule(schedule_selector(PhysicsLayer::SurvivalLvl));
		break;
	case 3: //收集
		schedule(schedule_selector(PhysicsLayer::CollectLvl));
		break;
	case 4: //击杀
		schedule(schedule_selector(PhysicsLayer::KillLvl));
		break;
	default:
		break;
	}
}

void PhysicsLayer::SurvivalLvl( float dt )
{
	bool isPlay = GameData::getInstance()->getisGamePlaying();
	if (!isPlay)
	{
		return;
	}

	bool isend = GameData::getInstance()->isGameEnd();
	if (isend)
	{
		GameData::getInstance()->GameOver(true);
	}
	
}

void PhysicsLayer::CollectLvl( float dt )
{
	bool isPlay = GameData::getInstance()->getisGamePlaying();
	if (!isPlay)
	{
		return;
	}
	bool isend = GameData::getInstance()->isGameEnd();
	
}

void PhysicsLayer::KillLvl( float dt )
{
	bool isPlay = GameData::getInstance()->getisGamePlaying();
	if (!isPlay)
	{
		return;
	}
	bool isend = GameData::getInstance()->isGameEnd();
	
	//敌机是否全部出现并死亡，
	if (isend)
	{
		//是否击杀完成，完成则胜利，否则失败
		bool isDone = 0;//UIController::getInstance()->getMainUILayer()->updateCollectNum();
		if (isDone)
		{
			GameData::getInstance()->GameOver(true);
		}
		else
		{
			GameData::getInstance()->GameOver(false);
		}
	}
}

void PhysicsLayer::addTips()
{
	ActionInterval* delay = DelayTime::create(1.0f);
	CallFunc* func = CallFunc::create(this,callfunc_selector(PhysicsLayer::TipsCallback));
	ActionInterval* seq = Sequence::create(delay,func,nullptr);
	runAction(seq);
}

void PhysicsLayer::TipsCallback()
{
	
}

void PhysicsLayer::addFallingStone()
{
	int curLvl = GameData::getInstance()->getPlaneManager()->getCurLvl();
	int delayTime = 10000;
	switch (curLvl)
	{
	case 5:
		delayTime = 40.0f;
		break;
	case 7:
		delayTime = 50.0f;
		break;
	case 8:
		delayTime = 60.0f;
		break;
	default:
		break;
	}

	BossTimeOut* timeCd = BossTimeOut::create();
	timeCd->setMaxTime(delayTime);
	timeCd->setInterval(3.0f);
	addChild(timeCd);
}

void PhysicsLayer::BeHurtWarning(float dur)
{
	Node* node = getChildByTag(1504251128);
	auto size = Director::getInstance()->getWinSize();
	if (!node)
	{
		node = Node::create();
		node->setPosition(size.width*0.5,size.height*0.5);
		addChild(node,-100,1504251128);
	}
	auto kuang = Sprite::create("Effect/contactEff/screenEff2.png");
	kuang->setScale(size.width/kuang->getContentSize().width, size.height/kuang->getContentSize().height);
	node->addChild(kuang);

	auto content = Sprite::create("Effect/contactEff/screenEff1.png");
	content->setScale(size.width/content->getContentSize().width, size.height/content->getContentSize().height);
	node->addChild(content);

	{
		ActionInterval* tintTo = FadeIn::create(0.5);
		ActionInterval* tintRed = FadeOut::create(0.5);
		ActionInterval* seq = Sequence::create(tintRed,tintTo,tintRed,nullptr);
		ActionInterval* repeat = Repeat::create(seq,1);
		CallFuncN* funcN = CallFuncN::create(this,callfuncN_selector(PhysicsLayer::removeContactEff));
		ActionInterval* seq1 = Sequence::create(repeat,funcN,nullptr);
		kuang->runAction(seq1);
	}
	{
		ActionInterval* tintTo = FadeIn::create(0.25);
		ActionInterval* tintRed = FadeOut::create(0.25);
		ActionInterval* seq = Sequence::create(tintRed,tintTo,nullptr);
		ActionInterval* repeat = Repeat::create(seq,dur/0.5 - 1);
		ActionInterval* seq1 = Sequence::create(repeat,nullptr);
		content->runAction(seq1);
	}
}

void PhysicsLayer::removeContactEff(Node* node)
{
	node->getParent()->removeFromParentAndCleanup(true);
}

void PhysicsLayer::initEdge()
{
	auto ws = Director::getInstance()->getWinSize();

	float exDis = 0.1;
	auto smallEdge = ScreenEdge::create(Size(ws.width + 100, ws.height + 100));
	//auto smallEdge = ScreenEdge::create(Size(ws.width * 0.8, ws.height * 0.8)); //测试
	auto bigEdge = ScreenEdge::create(Size(ws.width * 4, ws.height * 3));
	smallEdge->setPosition(ws.width*0.5, ws.height*0.5);
	bigEdge->setPosition(ws.width*0.5, ws.height*0.5);

	smallEdge->getBasePro().setMyId(UnitId::eScreenEdgeSmall);
	bigEdge->getBasePro().setMyId(UnitId::eScreenEdgeBig);
	addChild(smallEdge);
	addChild(bigEdge);
}
