#include "GameRoleScene.h"
#include "GameSceneMgr.h"
#include "CommonWidget.h"
#include "../GameCommon/CircleStageList.h"
#include "GameMainLayer.h"
#include "../GameCommon/NewAudio.h"
#include "../GameCommon/CommonRes.h"
#include "../GameCommon/GameDefine.h"
#include "../JniHelp/CppCallJava.h"
#include <algorithm>
#include "ui/UIScale9Sprite.h"
#include "GameResult.h"
#include "../GamePlaying/SpecialBody.h"
#include "DemoBullet.h"
#include "../GamePlaying/ContactManager.h"
#include "../GamePlaying/MainPlaneShowLayer.h"
#include "../GameCommon/GameData.h"
#include "ItemRewardTips.h"
#include "ui/UIScrollView.h"

using namespace std;

void GameInterfaceMgr::addInterfaceToList( InterfaceTag tag , bool isShow/* = true*/, void* data/* = nullptr*/  )
{
	for ( std::list<InterfaceTag>::iterator item = _interfaceVector.begin(); item != _interfaceVector.end(); item++ )
	{
		if (tag == *item)
		{
			return;
		}
	}

	_interfaceVector.push_back( tag );
	if (isShow)
	{
		_data = data;
		showNextInterface();
	}
}

void GameInterfaceMgr::showNextInterface( void )
{
	if ( _interfaceVector.empty() )
	{
		return;
	}

 	std::list<InterfaceTag>::iterator item = _interfaceVector.end();

	InterfaceTag tag = *(--item);
	setCurInterfaceTag(tag);

	switch (tag)
	{
	case GameInterfaceMgr::InterfaceTag_Task:
		GameTaskList::showLayout(_node, tag);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_ShowButtomBar);
		break;

	case GameInterfaceMgr::InterfaceTag_Upgrade:
		UpgradeLayer::showLayout(_node, tag);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_ShowButtomBar);
		break;

	case GameInterfaceMgr::InterfaceTag_CardOption:
		CardOptionLayout::showLayout( _node, tag );
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_ShowButtomBar);
		break;

	case GameInterfaceMgr::InterfaceTag_ShopBuy:
		ShopBuyLayer::showLayout( _node, tag );
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_ShowButtomBar);
		break;

	case GameInterfaceMgr::InterfaceTag_SignIn:
		SignInLayer::showLayout(_node, tag);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_ShowButtomBar);
		break;

	case GameInterfaceMgr::InterfaceTag_LevelOption:
		LevelOptionLayer::showLayout(_node, tag);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_HideButtomBar);
		break;

	case GameInterfaceMgr::InterfaceTag_VIPBuy:
		VipLayer::showLayout(_node, tag);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_ShowButtomBar);
		break;

	case GameInterfaceMgr::InterfaceTag_VIPReward:
		VipPresentingLayer::showLayout(_node, tag);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_ShowButtomBar);
		break;

	case GameInterfaceMgr::InterfaceTag_ResoutSuc:
		GameResultSuccess::showLayout(_node, tag);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_ShowButtomBar);
		break;

	case GameInterfaceMgr::InterfaceTag_ResoutFail:
		GameResultFail::showLayout(_node, tag);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_ShowButtomBar);
		break;

	case GameInterfaceMgr::InterfaceTag_RewardCard:
		RewardCardLayer::showLayout(_node, tag);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_ShowButtomBar);
		break;
	default:
		break;
	}
}

void GameInterfaceMgr::closeCurInterface( void )
{
	std::list<InterfaceTag>::iterator item = _interfaceVector.end();

	InterfaceTag tag = *(--item);
	_interfaceVector.erase(item);

	auto node = _node->getChildByTag( tag );
	if ( node )	
	{
		node->removeFromParentAndCleanup(true);
	}
		
	CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_UpdateRoleAtk);

	// 回到主界面时主界面动画
	if ( _interfaceVector.empty() )
	{
		setCurInterfaceTag(InterfaceTag_None);
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_ShowButtomBar);
		return;
	}
	else
	{
		item = _interfaceVector.end();
		showNextInterface();
	}
}

void GameInterfaceMgr::closeAllInterface( void )
{
	for ( std::list<InterfaceTag>::iterator item = _interfaceVector.begin(); item != _interfaceVector.end(); item++ )
	{
		auto node = _node->getChildByTag( *item );
		if ( node )	
		{
			node->removeFromParentAndCleanup(true);
		}
	}

	_interfaceVector.clear();
	setCurInterfaceTag(InterfaceTag_None);

}

int GameInterfaceMgr::getCurInterfaceNum(void)
{
	return _interfaceVector.size();
}

/*******************************************
* 主界面
*******************************************/
GameRoleScene::GameRoleScene()
	:_myLayer(nullptr)
	,_winSize(Size(0,0))
	, _curOptionRoleId(0)
{
	_showLayer = nullptr;
}
GameRoleScene::~GameRoleScene()
{
}

Scene* GameRoleScene::createScene()
{
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setGravity(Vec2(0, -500));
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

	auto layer = GameRoleScene::create();
	scene->addChild(layer);

	return scene;
}

bool GameRoleScene::init()
{
	if ( !Layer::init() )
	{
		return false;
	}


	// 鍔犺浇璧勬簮
	Director::getInstance()->getTextureCache()->addImage("animation/UpgradeAnim/upgradeAmin.png");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("animation/UpgradeAnim/upgradeAmin.plist");

	Director::getInstance()->getTextureCache()->addImage("animation/vipAnim/VipAnim.png");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("animation/vipAnim/VipAnim.plist");

	Director::getInstance()->getTextureCache()->addImage("animation/LevelAnim/levelAnim.png");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("animation/LevelAnim/levelAnim.plist");

	return true;
}

void GameRoleScene::onEnter()
{
	Layer::onEnter();

	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(GameRoleScene::onMessageEvent), MsgEventName_ToRoleLayer, nullptr);
	GameInterfaceMgr::getInstance()->setParentNode(this);

	MainSceneTaskBar::showLayout( this );

	initInterface();

	_winSize = Director::getInstance()->getVisibleOrigin();

	int residueDay = NewDataMgr::getInstance()->getVipDay();

	if (NewDataMgr::getInstance()->getFirstEntryState())
	{
		if (!residueDay)
		{
			GameInterfaceMgr::getInstance()->addInterfaceToList(GameInterfaceMgr::InterfaceTag_VIPBuy, false);

		}
		else
		{

			{
				auto showGoodsInfoLayer = PackShowLayer::getPackShowLayer();
				showGoodsInfoLayer->setShowInfoType(PackShowLayer::PackType::InfoType_TyrantPack);
				GameInterfaceMgr::getInstance()->getParentNode()->addChild(showGoodsInfoLayer, 10000);
			}


			// vip
			if (vipRewardJudge())
			{
				GameInterfaceMgr::getInstance()->addInterfaceToList(GameInterfaceMgr::InterfaceTag_VIPReward, false);
				int day = NewDataMgr::getInstance()->getVipDay();
				NewDataMgr::getInstance()->setVipDay(day - 1);
				int today = NewDataMgr::getInstance()->convertTimeStrToNumber(CppCallJava::getInstance()->getSystemTime(), SystemTime_Day);
				NewDataMgr::getInstance()->setVipGetDate(today);
			}
		}
	}
	
	
	// 签到
	if (signInJudge())
	{

		GameInterfaceMgr::getInstance()->addInterfaceToList(GameInterfaceMgr::InterfaceTag_SignIn, false);
		int today = NewDataMgr::getInstance()->convertTimeStrToNumber(CppCallJava::getInstance()->getSystemTime(), SystemTime_Day);
		NewDataMgr::getInstance()->mGameTaskSystem->setSignInDate(today);
	}

	GameInterfaceMgr::getInstance()->showNextInterface();


	// 闊充箰
	NewAudio::getInstance()->playBgMusicForce(Music_Bg_Play);

}

void GameRoleScene::onExit()
{
	Layer::onExit();

	// 移除观察者消息
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void GameRoleScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();

	auto listener = EventListenerKeyboard::create();
	listener->onKeyReleased = CC_CALLBACK_2(GameRoleScene::onKeyReleased,this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool GameRoleScene::signInJudge(void)
{
	int today = NewDataMgr::getInstance()->convertTimeStrToNumber(CppCallJava::getInstance()->getSystemTime(), SystemTime_Day);
	int history = NewDataMgr::getInstance()->mGameTaskSystem->getSignInDate();

	return (today == history) ? false : true;
}

bool GameRoleScene::vipRewardJudge(void)
{
	int getDate = NewDataMgr::getInstance()->getVipGetDate();
	int today = NewDataMgr::getInstance()->convertTimeStrToNumber(CppCallJava::getInstance()->getSystemTime(), SystemTime_Day);
	int residueDay = NewDataMgr::getInstance()->getVipDay();

	return (residueDay && (getDate != today)) ? true : false;
}

void GameRoleScene::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	switch (protocolID)
	{
	case EProtocolID_BuySuccessEffect:
		buySucceedAction();
		break;

	case EProtocolID_UnlockRole:
		{
			if (1 == _curOptionRoleId)
			{
				updateOptionRole(Tag_BtnRole1);
			}
			else if (2 == _curOptionRoleId)
			{
				updateOptionRole(Tag_BtnRole2);
			}
			else if (3 == _curOptionRoleId)
			{
				updateOptionRole(Tag_BtnRole3);
			}
			playUnlockRoleSound();
		}
		break;

	case EProtocolID_UpdateRoleAtk:
		updateRoleAtk();
		updateDemoRole();
		break;

	case EProtocolID_VipBuyCallFunc:
	{
		Button* ButtonVIP = (Button*)GameMainLayer::seekNodeByName(this, "ButtonVIP");
		//	ButtonVIP->setVisible((NewDataMgr::getInstance()->getVipDay()) ? false : true);
		ButtonVIP->setTouchEnabled((NewDataMgr::getInstance()->getVipDay()) ? false : true);

		auto ImgVIpBG = (Sprite*)GameMainLayer::seekNodeByName(this, "ImgVIpBG");
		ImgVIpBG->runAction(MoveTo::create(1.0f, ImgVIpBG->getPosition() + Vec2(0, 210)));
	}
		break;

	case EProtocolID_HelpFeqPlay:
		//HelpFeqLayer::showHelpFeq(HelpFeqLayer::HelpFeqType_ChuJi1,(Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonPlay"));
		break;

	case EProtocolID_BuyRolePackFinish:
	{
		NewDataMgr::getInstance()->setOptionRoleId(3);

		setSoundType(CommonLayer::SoundType_GameStart);
		GameInterfaceMgr::getInstance()->addInterfaceToList(GameInterfaceMgr::InterfaceTag_LevelOption);
	}
		break;

	case EProtocolID_BuyRolePackCancel:
	{
		NewDataMgr::getInstance()->setOptionRoleId(1);

		setSoundType(CommonLayer::SoundType_GameStart);
		GameInterfaceMgr::getInstance()->addInterfaceToList(GameInterfaceMgr::InterfaceTag_LevelOption);
	}
		break;

	default:
		break;
	}
}

void GameRoleScene::initInterface( void )
{
	_myLayer = CSLoader::createNode( Ccs_MainLayer_FilePath );
	Size visibleSize = Director::getInstance()->getVisibleSize();

	if ( !_myLayer )
	{
		return; 
	}

	addChild( _myLayer );
	do 
	{

		Button* ButtonRole1 = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonRole1"); ButtonRole1->setTag(Tag_BtnRole1);
		Button* ButtonRole2 = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonRole2"); ButtonRole2->setTag(Tag_BtnRole2);
		Button* ButtonRole3 = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonRole3"); ButtonRole3->setTag(Tag_BtnRole3);

		Button* ButtonLeftRoll = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonLeftRoll");
		setEventForNode(ButtonLeftRoll, Tag_BtnLeftRoll, nullptr);

		Button* ButtonRightRoll = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonRightRoll");
		setEventForNode(ButtonRightRoll, Tag_BtnRightRoll, nullptr);

		Button* ButtonPlay = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonPlay");
		setEventForNode(ButtonPlay, Tag_Play, nullptr);

		Button* ButtonNewPack = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonNewPack");
		setEventForNode(ButtonNewPack, Tag_BtnNewPack, nullptr);

		auto ImgVipBg = Sprite::create("ccsRes/resMainLayer/zhu58.png");
		ImgVipBg->setPosition(Vec2(620,1069));
		ImgVipBg->setVisible((NewDataMgr::getInstance()->getVipDay()) ? false : true);
		addChild(ImgVipBg, 100, "ImgVIpBG");


		//   Vip 
		std::string vipBtnFile = "ccsRes/resMainSceneTask/VIP3.png";
		auto ButtonVip = Button::create(vipBtnFile, vipBtnFile, vipBtnFile);
		ButtonVip->setPosition(Vec2(105,125));
		ButtonVip->setTouchEnabled((NewDataMgr::getInstance()->getVipDay()) ? false : true);
		setEventForNode(ButtonVip, Tag_BtnVIP, nullptr);
		ImgVipBg->addChild(ButtonVip, 0, "ButtonVIP");
		auto ImgVipName = Sprite::create("ccsRes/resMainSceneTask/VIP4.png");
		ImgVipName->setPosition(Vec2(76, 4));
		ButtonVip->addChild(ImgVipName);
// 		Vector<SpriteFrame*> frame_array;
// 
// 		for (int i = 1; i <= 13; ++i)
// 		{
// 			std::string pic = CCString::createWithFormat("%s%d.png", "VipAnim", i)->getCString();
// 			CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pic.c_str());
// 
// 			frame_array.pushBack(frame);
// 		}
// 
// 		auto p_animation = Animation::createWithSpriteFrames(frame_array, 0.2f);
// 		p_animation->setLoops(-1);
// 
// 		auto animate = Animate::create(p_animation);

// 		auto ImgVipBgNode = Sprite::create("weapon.png");
// 		ImgVipBgNode->setPosition(Vec2(104, 95));
// 		ImgVipBgNode->setScaleY(1.1);
// 		ImgVipBgNode->setScaleX(1.3);
// 		ImgVipBg->addChild(ImgVipBgNode);
// 		ImgVipBgNode->runAction(Sequence::create(
// 			animate,
// 			nullptr));
		// vip end

		auto PageViewRole = (PageView*)GameMainLayer::seekNodeByName(_myLayer, "PageViewRole");
		PageViewRole->addEventListener(CC_CALLBACK_2(GameRoleScene::callFuncPageviewRole, this));

		// 左右翻页动画
		auto spriteLeft = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "SpriteLeft");
		auto SpriteRight = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "SpriteRight");

		auto spriteLeftsize = spriteLeft->getContentSize();

		spriteLeft->runAction(RepeatForever::create(Sequence::create(
			Spawn::create(CCMoveTo::create(0.5f, Vec2(spriteLeftsize.width / 2 + 30, spriteLeftsize.height / 2)), FadeOut::create(0.5f), nullptr),
			DelayTime::create( 1.0f ),
			Spawn::create(MoveTo::create(0.01, Vec2(spriteLeftsize.width / 2 +10, spriteLeftsize.height / 2)), FadeIn::create(0.01f), nullptr),
			nullptr)));

		SpriteRight->runAction(RepeatForever::create(Sequence::create(
			Spawn::create(CCMoveTo::create(0.5f, Vec2(spriteLeftsize.width / 2 + 30, spriteLeftsize.height / 2)), FadeOut::create(0.5f), nullptr),
			DelayTime::create(1.0f),
			Spawn::create(MoveTo::create(0.01, Vec2(spriteLeftsize.width / 2 + 10, spriteLeftsize.height / 2)), FadeIn::create(0.01f), nullptr),
			nullptr)));

		_lastOptionRoleId = _curOptionRoleId = NewDataMgr::getInstance()->mRoleConfig.roleIndex;
		if (1 == _curOptionRoleId)
		{
			updateOptionRole(Tag_BtnRole1);
			PageViewRole->scrollToPage(0);
		}
		else if (2 == _curOptionRoleId)
		{
			updateOptionRole(Tag_BtnRole2);
			PageViewRole->scrollToPage(1);
		}
		else if (3 == _curOptionRoleId)
		{
			updateOptionRole(Tag_BtnRole3);
			PageViewRole->scrollToPage(2);
		}

		Button* BtnRoleShow = (Button*)GameMainLayer::seekNodeByName(_myLayer, "BtnRoleShow");
		std::string filePath = StringUtils::format("ccsRes/resMainLayer/ImgLittleRole%d.png", _curOptionRoleId);
		BtnRoleShow->loadTextures(filePath, filePath, filePath);

		auto ImgFlash3 = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgFlash3");
		ImgFlash3->setOpacity(0);
		ImgFlash3->setScale(1.5);
		ImgFlash3->runAction(RepeatForever::create(Sequence::create(
			DelayTime::create(1.21),
			CCSpawn::create(CCFadeIn::create(0.4), CCScaleTo::create(0.3, 1.5), nullptr),
			CCSpawn::create(CCFadeOut::create(1.2), CCScaleTo::create(0.7, 1.5), nullptr),
			DelayTime::create(1.5),
			nullptr)));

		auto ImgLight1 = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgLight1");
		auto ImgLight2 = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgLight2");
		ImgLight1->runAction(RepeatForever::create(Sequence::create(FadeOut::create(0.8), DelayTime::create(1.2), FadeIn::create(0.3), nullptr)));
		ImgLight2->runAction(RepeatForever::create(Sequence::create(DelayTime::create(0.8), FadeOut::create(0.5), DelayTime::create(1.0), FadeIn::create(0.2), nullptr)));


 		auto ImgFlash1 = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgFlash1");
		ImgFlash1->runAction(RepeatForever::create(Sequence::create(
			CCSpawn::create(MoveTo::create(0.01, Vec2(750, -387)), CCRotateTo::create(0.3, 0), nullptr),
			CCSpawn::create(MoveTo::create(0.3, Vec2(517, -419)), CCRotateTo::create(0.3, -5), nullptr),
			CCSpawn::create(MoveTo::create(0.3, Vec2(231, -490)), CCRotateTo::create(0.3, -10), nullptr),
			CCSpawn::create(MoveTo::create(0.3, Vec2(17, -557)), CCRotateTo::create(0.3, -12), nullptr),
			CCSpawn::create(MoveTo::create(0.3, Vec2(-40, -586)), CCRotateTo::create(0.3, -12), nullptr),
			DelayTime::create(1.5),
			nullptr)));

		
		runAction(Sequence::create(
			DelayTime::create(1.0f),
			CallFunc::create(CC_CALLBACK_0(GameRoleScene::updateDemoRole, this)),
			nullptr));

	} while (0);

}

void GameRoleScene::setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback /*= nullptr*/)
{
	do 
	{
		CC_BREAK_IF( node == nullptr );
		node->setTag( tag );
		if (callback)
			node->addTouchEventListener(callback);
		else
			node->addTouchEventListener(CC_CALLBACK_2(GameRoleScene::callFuncButtonEvent, this));

	} while (0);
}

void GameRoleScene::callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if(!setButtonAction(ref, touchType, SoundType_ButtonStart)) return;

	Button* btn = (Button*)ref;
	int nTag = btn->getTag();

	btn->setTouchEnabled(false);
	btn->runAction(Sequence::create(
		DelayTime::create(0.1f),
		CCCallFuncN::create(CC_CALLBACK_1(GameRoleScene::callFuncDeleyTouch, this)),
		nullptr));

	switch ( nTag )
	{
	case Tag_Play:
		playButtonPro((intptr_t)btn->getUserData());
		break;

	case Tag_BtnNewPack:
		{
			auto node = GameInterfaceMgr::getInstance()->getParentNode();
			auto showGoodsInfoLayer = ShowGoodsLayer::getShowGoodsInfoLayer();
			showGoodsInfoLayer->setShowInfoType(ShowGoodsLayer::showInfoType::InfoType_NewPack);
			node->addChild(showGoodsInfoLayer, LocalZOrde_1000);
		}
		break;

	case Tag_SignIn:
		break;	

	case Tag_BtnLeftRoll:
		{
			auto PageViewRole = (PageView*)GameMainLayer::seekNodeByName(_myLayer, "PageViewRole");
			PageViewRole->scrollToPage(PageViewRole->getCurPageIndex() - 1);
		}
		break;

	case Tag_BtnRightRoll:
		{
			auto PageViewRole = (PageView*)GameMainLayer::seekNodeByName(_myLayer, "PageViewRole");
			PageViewRole->scrollToPage(PageViewRole->getCurPageIndex() + 1);
		}
		break;

	case Tag_BtnVIP:
		GameInterfaceMgr::getInstance()->addInterfaceToList(GameInterfaceMgr::InterfaceTag_VIPBuy);
		break;

	default:
		break;

	}
}

void GameRoleScene::callFuncOptionCard(Ref* ref, Widget::TouchEventType touchType)
{
	if (Widget::TouchEventType::ENDED != touchType) return;

	auto btn = (Button*)ref;
	auto tag = (ButtonTag)btn->getTag();
	updateOptionRole(tag);

	auto PageViewRole = (PageView*)GameMainLayer::seekNodeByName(_myLayer, "PageViewRole");
	switch (tag)
	{
	case Tag_BtnRole1:
		PageViewRole->scrollToPage(0);	
		break;

	case Tag_BtnRole2:
		PageViewRole->scrollToPage(1);
		break;

	case Tag_BtnRole3:
		PageViewRole->scrollToPage(2);
		break;
	default:
		break;
	}
}

void GameRoleScene::callFuncPageviewRole(Ref* ref, PageView::EventType eventType)
{
	auto PageViewRole = (PageView*)ref;
	int roleIndex = PageViewRole->getCurPageIndex();

	switch (roleIndex)
	{
	case 0:
		updateOptionRole(Tag_BtnRole1);
		break;

	case 1:
		updateOptionRole(Tag_BtnRole2);
		break;

	case 2:
		updateOptionRole(Tag_BtnRole3);
		break;
	default:
		break;
	}
	playOptionRoleSound();


}

void GameRoleScene::onKeyReleased( EventKeyboard::KeyCode keyCode,Event * pEvent )
{
// 	if (keyCode != EventKeyboard::KeyCode::KEY_BACKSPACE || keyCode != EventKeyboard::KeyCode::KEY_ESCAPE)
// 		return;


	// 指引中
	if (NewDataMgr::getInstance()->getGuideState())
	{
		return;
	}

	// 物品显示中
	auto ShowGoodsLayer = GameInterfaceMgr::getInstance()->getParentNode()->getChildByName("ShowGoodsLayer");
	if (ShowGoodsLayer)
	{
		ShowGoodsLayer->removeFromParentAndCleanup(true);
		return;
	}

	// 提示中
	auto commonWidgetNode = Director::getInstance()->getRunningScene()->getChildByName("CommonWidget");
	if (commonWidgetNode)
	{
		commonWidgetNode->removeFromParentAndCleanup(true);
		return;
	}

	if (GameInterfaceMgr::getInstance()->getCurInterfaceNum())
	{
		GameInterfaceMgr::getInstance()->closeCurInterface();
		return;
	}	

	// 防止重复show出widget
	if ( getChildByName( "exitWidget" ) ) return;
	
	if (keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE
		|| keyCode == EventKeyboard::KeyCode::KEY_ESCAPE )
	{
		auto widget = CommonWidget::create();
		widget->setContent( "是否确定退出游戏？" );
		widget->addBtn1ClickEventListener( CC_CALLBACK_0(GameRoleScene::callFuncExitGame, this) );
		addChild( widget, 15000, "exitWidget" );
	}
}

void GameRoleScene::callFuncExitGame( void )
{
	Director::getInstance()->end();
}

void GameRoleScene::removeUpgradeAnim( Node* node )
{
	auto animSprite = (Sprite*)node;
	animSprite->removeFromParentAndCleanup(true);
}

void GameRoleScene::showPaySuccee(Ref* sender)
{
	buySucceedAction();
}

void GameRoleScene::buySucceedAction( void )
{
//	NewAudio::getInstance()->playEffect(Music_BuySuccess);
//
//	auto vsize = Director::getInstance()->getVisibleSize();
//	// 特效节点
//	auto node = Node::create();
//	node->setPosition(Vec2(vsize.width/2,vsize.height/4*3));
//	/*Director::getInstance()->getRunningScene()->*/addChild( node , 1001);
//
//	// 购买成功图片动作
//	auto spriteBuy = Sprite::create("particla/BuySucceed/gmcg.png");
//	node->addChild(spriteBuy,-1);
//	spriteBuy->runAction( Sequence::create (
//		ScaleTo::create(0.15f, 1.5f ),
//		ScaleTo::create(0.20f, 1.8f ),
//		Spawn::create(  FadeOut::create(0.5f),ScaleTo::create(0.5f, 2.5f ), nullptr),
//		CallFuncN::create( this, callfuncN_selector(GameRoleScene::callFuncbuySucceed)),
//		nullptr));
//
//	// 礼花绽放
//	auto parcile2 = ParticleSystemQuad::create("particla/BuySucceed/test2.plist");
//	node->addChild(parcile2,-1);
//	auto parcile3 = ParticleSystemQuad::create("particla/BuySucceed/test3.plist");
//	node->addChild(parcile3,-1);
//	auto parcile4 = ParticleSystemQuad::create("particla/BuySucceed/test4.plist");
//	node->addChild(parcile4,-1);


}

void GameRoleScene::callFuncbuySucceed( Node* node )
{
	//node->removeFromParentAndCleanup( true );
}

void GameRoleScene::playUnlockEffect(void)
{
	Button* btn = (Button*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ButtonRole%d", _curOptionRoleId));
	Vec2 pos = btn->getParent()->convertToWorldSpace(btn->getPosition());

	auto vsize = Director::getInstance()->getVisibleSize();
	auto node = Node::create();
	node->setScale(1.5f);
	node->setPosition(pos);
	addChild(node, 1001);

	for (int i = 1; i <= 10; i++)
	{
		auto parcile = ParticleSystemQuad::create(StringUtils::format("particla/RoleLock/test%d.plist", i));
		node->addChild(parcile, -1);

 	}

	auto RoleLock = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("RoleLock%d", _curOptionRoleId));
	RoleLock->setVisible(false);
}

void GameRoleScene::updateOptionRole(ButtonTag tag)
{
	
	Button* ButtonPlay = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonPlay");
	bool curRoleLockState = false;

	for (int i = 1; i <= 3; i++)
	{
		auto butRole = (Button*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ButtonRole%d", i));
		bool roleLockState = NewDataMgr::getInstance()->mGameRoleSystem->getRoleConfigWithID(i)->isLock;

		if (butRole && tag == butRole->getTag())
		{
			_curOptionRoleId = i;
			curRoleLockState = roleLockState;

			butRole->setBright(true);
			// 将当前选择的角色ID传给Play按钮
			ButtonPlay->setUserData((void*)i);

			//auto btnPlay = (Node*)GameMainLayer::seekNodeByName(_myLayer, "ImgPlay");
			auto strUnlockRoleTips = (Text*)GameMainLayer::seekNodeByName(_myLayer, "strUnlockRoleTips");

			//btnPlay->setVisible((curRoleLockState) ? true : false);
			strUnlockRoleTips->setVisible((curRoleLockState) ? false : true);

			// 未解锁 调整价格显示位置
			if (!roleLockState)
			{
				strUnlockRoleTips->setString(StringUtils::format("解锁需消耗%d金币", NewDataMgr::getInstance()->mGameRoleSystem->getRolePriceWithID(i)));

				//auto strSize = AtlasLabelLockPrice->getContentSize();
				//auto ImgGold = (Node*)GameMainLayer::seekNodeByName(_myLayer, "ImgGold");
				//ImgGold->setPosition(Vec2(-5, strSize.height/2));

				ButtonPlay->setBright(true);
			}
			else
			{
				ButtonPlay->setBright(false);
			}
		}
		else
		{
			butRole->setBright(false);
		}
	}

	// 翻页隐藏按钮
	auto PageViewRole = (PageView*)GameMainLayer::seekNodeByName(_myLayer, "PageViewRole");
	int roleIndex = PageViewRole->getCurPageIndex();
	Button* ButtonLeftRoll = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonLeftRoll");
	Button* ButtonRightRoll = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonRightRoll");

	if (_curOptionRoleId != _lastOptionRoleId) 
	{
		Button* BtnRoleShow = (Button*)GameMainLayer::seekNodeByName(_myLayer, "BtnRoleShow");
		BtnRoleShow->runAction(Sequence::create(
			ScaleTo::create(0.1, 1, 0.1)
			, CallFuncN::create(this, callfuncN_selector(GameRoleScene::callFuncChangeRoleDemo))
			, ScaleTo::create(0.1, 1, 1)
			, nullptr));
	}

	switch (roleIndex)
	{
	case 0:
	{
		ButtonLeftRoll->setTouchEnabled(false);
		ButtonLeftRoll->setVisible(false);
		ButtonRightRoll->setTouchEnabled(true);
		ButtonRightRoll->setVisible(true);
	}
		break;

	case 1:
	{
		ButtonLeftRoll->setTouchEnabled(true);
		ButtonLeftRoll->setVisible(true);
		ButtonRightRoll->setTouchEnabled(true);
		ButtonRightRoll->setVisible(true);
	}
		break;

	case 2:
	{
		ButtonRightRoll->setTouchEnabled(false);
		ButtonRightRoll->setVisible(false);
		ButtonLeftRoll->setTouchEnabled(true);
		ButtonLeftRoll->setVisible(true);
	}
		break;
	default:
		break;
	}


	updateRoleAtk();

	if ( _curOptionRoleId != _lastOptionRoleId)
	{
		updateDemoRole();
	}
}

void GameRoleScene::updateDemoRole(void)
{
#if 1

	// 演示
	auto ws = Director::getInstance()->getWinSize();
	if (!_showLayer)
	{
		GameData::getInstance()->setIsJustShowMainPlane(true);
		_showLayer = MainPlaneShowLayer::create();
		_showLayer->setFrameSize(Size(ws.width*0.4, ws.height * 0.4));
		_showLayer->setPosOffset(Vec2(200,100));
		_showLayer->setName("MainPlaneShowLayer");
		addChild(_showLayer);
	}


	int roleStarNum = 0;
	switch (_curOptionRoleId)
	{
	case 1:
		_showLayer->ShowMainPlane(PlaneType::PlaneType_Wind);
		roleStarNum = 2;
		break;
	case 2:
		_showLayer->ShowMainPlane(PlaneType::PlaneType_Fire);
		roleStarNum = 3;
		break;
	case 3:
		_showLayer->ShowMainPlane(PlaneType::PlaneType_Electric);
		roleStarNum = 5;
		break;
	default:
		break;
	}
	
	for (int i = 1; i <= 5; i++)
	{
		auto ImgRole = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ImgStar%d", i));
		ImgRole->setVisible((roleStarNum>=i)?true:false);
	}
#endif
}

void GameRoleScene::updateRoleAtk(void)
{
	auto ImgRole = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgRole");
	ImgRole->setTexture(StringUtils::format("roleImg/ImgName_Role%d.png", _curOptionRoleId));

	// 战斗力
	float planeAtk = NewDataMgr::getInstance()->mGameRoleSystem->getRoleConfigWithID(_curOptionRoleId)->BassAtk;
	planeAtk = planeAtk + planeAtk * ((float)NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(GameGradeMsg::ObjectType_Gold)->curGrade * 0.1
		+ (float)NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(GameGradeMsg::ObjectType_Role)->curGrade * 0.25
		+ (float)NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(GameGradeMsg::ObjectType_DaZhao)->curGrade * 0.5);

	auto curOptionCard = NewDataMgr::getInstance()->mRoleConfig.cardArr;
	float card1Atk = NewDataMgr::getInstance()->mGameCardSystem->getCardConfigWithId(curOptionCard[0].cardID)->CardBassAtk;
	card1Atk = card1Atk + card1Atk * (float)NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(GameGradeMsg::ObjectType_RedBall)->curGrade;

	float card2Atk = NewDataMgr::getInstance()->mGameCardSystem->getCardConfigWithId(curOptionCard[1].cardID)->CardBassAtk;
	card2Atk = card2Atk + card2Atk * (float)NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(GameGradeMsg::ObjectType_BlueBall)->curGrade;

	float card3Atk = NewDataMgr::getInstance()->mGameCardSystem->getCardConfigWithId(curOptionCard[2].cardID)->CardBassAtk;
	card3Atk = card3Atk + card3Atk * (float)NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(GameGradeMsg::ObjectType_YellowBall)->curGrade;

	int atk = planeAtk + card3Atk + card1Atk + card2Atk;
	auto AtlasLabelRoleAtk = (TextAtlas*)GameMainLayer::seekNodeByName(_myLayer, "AtlasLabelRoleAtk");
	AtlasLabelRoleAtk->setString(StringUtils::format("%d", atk));
}

void GameRoleScene::playButtonPro(int roleid)
{
	auto curRole = NewDataMgr::getInstance()->mGameRoleSystem->getRoleConfigWithID(roleid);
	auto Role2Config = NewDataMgr::getInstance()->mGameRoleSystem->getRoleConfigWithID(2);
	auto Role3Config = NewDataMgr::getInstance()->mGameRoleSystem->getRoleConfigWithID(3);
	int maxLevel = NewDataMgr::getInstance()->getCurMaxLevel();

	// 从第4关开始，如玩家没有解锁过新飞机，则每次进入关卡前弹出女神大礼包
	if (4 <= maxLevel && !Role2Config->isLock && !Role3Config->isLock)
	{
		auto rolePackLayer = ShowRolePackLayer::getRolePackLayer();
		auto node = GameInterfaceMgr::getInstance()->getParentNode();
		node->addChild(rolePackLayer, LocalZOrde_1000);
	}
	else if (!curRole->isLock)
	{
		int myGold = NewDataMgr::getInstance()->getGoodsNumWithID(GoodsID_Gold);
		int buyGoodsKey = BuyKey_Role2;

		switch (roleid)
		{
		case 2:
			buyGoodsKey = BuyKey_Role2;
			break;
		case 3:
			buyGoodsKey = BuyKey_Role3;
			break;
		default:
			break;
		}

		int needGold = NewDataMgr::getInstance()->mGameBuySystem->getBuyGoodsDataWithID(buyGoodsKey)->Gold;
		if ( needGold <= myGold )
		{
			NewDataMgr::getInstance()->mGameBuySystem->buyGoodsWithKey(buyGoodsKey);
		}
		else
		{
			auto rolePackLayer = ShowRolePackLayer::getRolePackLayer();
			auto node = GameInterfaceMgr::getInstance()->getParentNode();
			node->addChild(rolePackLayer, LocalZOrde_1000);
		}
	}
	else
	{
		NewDataMgr::getInstance()->setOptionRoleId(roleid);

		setSoundType(CommonLayer::SoundType_GameStart);
		GameInterfaceMgr::getInstance()->addInterfaceToList(GameInterfaceMgr::InterfaceTag_LevelOption);
	}

}

void GameRoleScene::playUnlockRoleSound()
{

	if (1 == _curOptionRoleId)
	{
		NewAudio::getInstance()->playEffect(Music_Role1Unlock);
	}
	else if (2 == _curOptionRoleId)
	{
		NewAudio::getInstance()->playEffect(Music_Role2Unlock);
	}
	else if (3 == _curOptionRoleId)
	{
		NewAudio::getInstance()->playEffect(Music_Role3Unlock);
	}
}

void GameRoleScene::playOptionRoleSound()
{
	if (_curOptionRoleId == _lastOptionRoleId)
	{
		return;
	}
	else
	{
		_lastOptionRoleId = _curOptionRoleId;
	}

	if (1 == _curOptionRoleId)
	{
		NewAudio::getInstance()->playEffect(Music_Role1Option);
	}
	else if (2 == _curOptionRoleId)
	{
		NewAudio::getInstance()->playEffect(Music_Role2Option);
	}
	else if (3 == _curOptionRoleId)
	{
		NewAudio::getInstance()->playEffect(Music_Role3Option);
	}
}

void GameRoleScene::callFuncChangeRoleDemo(Node* node)
{
	auto BtnRoleShow = (Button*)node;

	std::string filePath = StringUtils::format("ccsRes/resMainLayer/ImgLittleRole%d.png", _curOptionRoleId);
	BtnRoleShow->loadTextures(filePath, filePath, filePath);

}

void GameRoleScene::callFuncDeleyTouch(Node* node)
{
	auto btn = (Button*)node;
	btn->setTouchEnabled(true);
}


/*******************************************
* 主界面顶部和底部框
*******************************************/
void MainSceneTaskBar::showLayout( Node* node, bool parm/* = true */ )
{
	if ( node && !node->getChildByName("MainSceneTaskBar"))
	{
		auto layout = MainSceneTaskBar::create();
		layout->_isShowButtomBar = parm;
		node->addChild( layout, LocalZOrde_1000, "MainSceneTaskBar" );
	}
}

MainSceneTaskBar::MainSceneTaskBar ()
	:_mainSceneTaskBar(nullptr)
{

}

MainSceneTaskBar::~MainSceneTaskBar ()
{
}

void MainSceneTaskBar::onEnter()
{
	Layer::onEnter();

	initInterface();
	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(MainSceneTaskBar::onMessageEvent), MsgEventName_ToTaskBarLayer, nullptr);
}

void MainSceneTaskBar::onExit()
{
	Layer::onExit();

	// 移除观察者消息
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void MainSceneTaskBar::setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback)
{
	do 
	{
		CC_BREAK_IF( node == nullptr );

		node->setTag( tag );
		node->addTouchEventListener(CC_CALLBACK_2(MainSceneTaskBar::callFuncButtonEvent, this));

	} while (0);
}

void MainSceneTaskBar::callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType, SoundType_Menu)) return;

	Button* btn = (Button*)ref;
	auto nTag = (ButtonTag)btn->getTag();

	if (Widget::TouchEventType::ENDED != touchType)
		return;

	btn->setTouchEnabled(false);
	btn->runAction(Sequence::create(
		DelayTime::create(0.05f),
		CCCallFuncN::create(CC_CALLBACK_1(MainSceneTaskBar::callFuncDeleyTouch, this)),
		nullptr));

	switch (nTag)
	{
	case MainSceneTaskBar::Tag_BtnUpgrade:
	{
		if (GameInterfaceMgr::getInstance()->getCurInterfaceTag() == GameInterfaceMgr::InterfaceTag_Upgrade) return;
		GameInterfaceMgr::getInstance()->closeAllInterface();
		auto ImgUpgradeFlash = (Sprite*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ImgUpgradeFlash"); ImgUpgradeFlash->setVisible(false);
		auto ParticleMF = (Node*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ParticleMF"); ParticleMF->setVisible(false);

		GameInterfaceMgr::getInstance()->addInterfaceToList(GameInterfaceMgr::InterfaceTag_Upgrade);
	}
		break;

	case MainSceneTaskBar::Tag_BtnCard:
	{
		if (GameInterfaceMgr::getInstance()->getCurInterfaceTag() == GameInterfaceMgr::InterfaceTag_CardOption) return;
		GameInterfaceMgr::getInstance()->closeAllInterface();
		auto ImgCardFlash = (Sprite*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ImgCardFlash"); ImgCardFlash->setVisible(false);
		auto ParticleCard = (Node*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ParticleCard"); ParticleCard->setVisible(false);
		GameInterfaceMgr::getInstance()->addInterfaceToList(GameInterfaceMgr::InterfaceTag_CardOption);
	}
		break;
	case MainSceneTaskBar::Tag_BtnMain:
		if (GameInterfaceMgr::getInstance()->getCurInterfaceTag() == GameInterfaceMgr::InterfaceTag_Task) return;
		GameInterfaceMgr::getInstance()->closeAllInterface();
		break;

	case MainSceneTaskBar::Tag_BuyGole:
	case MainSceneTaskBar::Tag_BtnShop:
		if (GameInterfaceMgr::getInstance()->getCurInterfaceTag() == GameInterfaceMgr::InterfaceTag_ShopBuy) return;
		GameInterfaceMgr::getInstance()->closeAllInterface();
		GameInterfaceMgr::getInstance()->addInterfaceToList(GameInterfaceMgr::InterfaceTag_ShopBuy);
		break;

		break;

	default:
		break;
	}

}

void MainSceneTaskBar::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	switch (protocolID)
	{
	case EProtocolID_ShowButtomBar:
		{
			MsgProShowButtomBar(nullptr);
		}
		break;

	case EProtocolID_HideButtomBar:
		{
			MsgProHideButtomBar(nullptr);
		}
		break;

	case EProtocolID_UpdateTaskNum:
		checkTaskFinish();
		break;

	case EProtocolID_UpdateGold:
		updataShowData();
		break;

	case EProtocolID_VipBuyCallFunc:
		{
			Button* ButtonVIP = (Button*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ButtonVIP");
			//	ButtonVIP->setVisible((NewDataMgr::getInstance()->getVipDay()) ? false : true);
			ButtonVIP->setTouchEnabled((NewDataMgr::getInstance()->getVipDay()) ? false : true);

			auto ImgVIpBG = (Sprite*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ImgVIpBG");
			ImgVIpBG->runAction(MoveTo::create(1.0f, ImgVIpBG->getPosition() + Vec2(0, 210)));

			auto ImgVip = (Sprite*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ImgVip");
			ImgVip->setVisible((NewDataMgr::getInstance()->getVipDay()) ? true : false);
		}
		break;

	case EProtocolID_HelpFeqCard:
		//HelpFeqLayer::showHelpFeq(HelpFeqLayer::HelpFeqType_Card2_1, (Button*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ButtonCard"));
		break;

	default:
		break;
	}
}

void MainSceneTaskBar::initInterface()
{
	_mainSceneTaskBar = CSLoader::createNode( Ccs_mainSceneTaskBar_FilePath );

	if ( !_mainSceneTaskBar )
	{
		return;
	}

	addChild( _mainSceneTaskBar );

	do 
	{
		auto ButtonUpgrade = (Button*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ButtonUpgrade");
		setEventForNode(ButtonUpgrade, Tag_BtnUpgrade, nullptr);

		auto ButtonCard = (Button*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ButtonCard");
		setEventForNode(ButtonCard, Tag_BtnCard, nullptr);

		auto ButtonMain = (Button*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ButtonMain");
		setEventForNode(ButtonMain, Tag_BtnMain, nullptr);

		auto ButtonShop = (Button*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ButtonShop");
		setEventForNode(ButtonShop, Tag_BtnShop, nullptr);

		auto ButtonBuyGold = (Button*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ButtonBuyGold" );
		setEventForNode(ButtonBuyGold, Tag_BuyGole, nullptr);

		//VIP
		auto ImgVip = (Sprite*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ImgVip");
		ImgVip->setVisible(false);

		// 音效
		auto CheckBoxMusic = (CheckBox*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "CheckBoxMusic" );
		CC_BREAK_IF( CheckBoxMusic == nullptr );
		CheckBoxMusic->addEventListener(CC_CALLBACK_2(MainSceneTaskBar::selectedSoundEvent, this));
		CheckBoxMusic->setSelected( !NewAudio::getInstance()->getSoundEffState() );

		// 音乐
		auto CheckBoxSoundEff = (CheckBox*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "CheckBoxSoundEff" );
		CC_BREAK_IF( CheckBoxSoundEff == nullptr );
		CheckBoxSoundEff->addEventListener(CC_CALLBACK_2(MainSceneTaskBar::selectedMusicEvent, this));
		CheckBoxSoundEff->setSelected( !NewAudio::getInstance()->getMusicState() );

		// 金币光
		auto Goldaddguang	= (Sprite*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "Goldaddguang" );
		Goldaddguang->runAction( RepeatForever::create( Sequence::create(CCScaleTo::create(0.01, 0.3f),  CCDelayTime::create(1.0f),Spawn::create(CCScaleTo::create(1.5, 0.6f), CCFadeIn::create(1.5), nullptr),CCFadeOut::create(0.4f),nullptr)));

		// 按钮提示光
		auto ImgUpgradeFlash = (Sprite*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ImgUpgradeFlash"); ImgUpgradeFlash->setVisible(false);
		auto ImgCardFlash = (Sprite*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ImgCardFlash"); ImgCardFlash->setVisible(false);
		auto ParticleMF = (Node*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ParticleMF"); ParticleMF->setVisible(false);
		auto ParticleCard = (Node*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ParticleCard"); ParticleCard->setVisible(false);

		
// 		if (!NewDataMgr::getInstance()->getFirstEntryUpgradeState())
// 		{
// 			ParticleMF->setVisible(true);
// 			ImgUpgradeFlash->setVisible(true);
// 			ImgUpgradeFlash->runAction(RepeatForever::create(Sequence::create(
// 				CCFadeOut::create(0.5f),
// 				CCFadeIn::create(0.5f),
// 				nullptr)));
// 		}
// 
// 		if (!NewDataMgr::getInstance()->getFirstEntryCardState())
// 		{
// 			ParticleCard->setVisible(true);
// 			ImgCardFlash->setVisible(true);
// 			ImgCardFlash->runAction(RepeatForever::create(Sequence::create(
// 				CCFadeOut::create(0.5f),
// 				CCFadeIn::create(0.5f),
// 				nullptr)));
// 		}

		if (NewDataMgr::getInstance()->getVipDay())
		{
//			// 字体流光效果
//			auto spriteWidget1 = Sprite::create("ccsRes/resMainSceneTask/VIP3.png");
//			Size clipSize = spriteWidget1->getContentSize();
//			Sprite* spark = Sprite::create("FirstUIRes/Sptigeguang.png");
//			spark->setScale(0.3, 0.8);
//
//			ClippingNode* clippingNode = ClippingNode::create();
//			clippingNode->setPosition(ImgVip->getPosition());
//			ImgVip->getParent()->addChild(clippingNode);
//			clippingNode->setScale(0.55);
//			clippingNode->setAlphaThreshold(0.05f); //设置alpha闸值  
//			clippingNode->setContentSize(clipSize); //设置尺寸大小  
//
//			clippingNode->setStencil(spriteWidget1);   //设置模板stencil  
//			clippingNode->addChild(spriteWidget1, 1);  //先添加标题,会完全显示出来,因为跟模板一样大小  
//			clippingNode->addChild(spark, 2);       //会被裁减  
//
//			MoveTo* moveAction = MoveTo::create(2.0f, Vec2(clipSize.width, 0));
//			MoveTo* moveBackAction = MoveTo::create(0.01f, Vec2(-clipSize.width, 0));
//			spark->runAction(RepeatForever::create(Sequence::create(CCDelayTime::create(1.5f), moveAction, moveBackAction, NULL)));
		}


		if ( !_isShowButtomBar )
		{
			MsgProHideButtomBar( nullptr );
		}
		
		updataShowData();		
		checkTaskFinish();

	} while (0);
}

void MainSceneTaskBar::selectedMusicEvent( Ref* pSender,CheckBox::EventType type )
{
	switch (type)
	{
	case CheckBox::EventType::UNSELECTED:
		NewAudio::getInstance()->setMusicState( true );
		break;

	case CheckBox::EventType::SELECTED:
		NewAudio::getInstance()->setMusicState( false );
		break;

	default:
		break;
	}
}

void MainSceneTaskBar::selectedSoundEvent( Ref* pSender,CheckBox::EventType type )
{
	switch (type)
	{
	case CheckBox::EventType::UNSELECTED:
		NewAudio::getInstance()->setSoundEffState( true );
		break;

	case CheckBox::EventType::SELECTED:
		NewAudio::getInstance()->setSoundEffState( false );
		break;

	default:
		break;
	}
}

void MainSceneTaskBar::MsgProShowButtomBar( Ref* sender )
{
	auto bottomBar = (Layer*)GameMainLayer::seekNodeByName(_mainSceneTaskBar,"bottomBar");
	bottomBar->setPosition( Vec2(0,0) );

	checkTaskFinish();
}

void MainSceneTaskBar::MsgProHideButtomBar( Ref* sender )
{
	auto bottomBar = (Layer*)GameMainLayer::seekNodeByName(_mainSceneTaskBar,"bottomBar");
	bottomBar->setPosition( Vec2(0,-300) );

	checkTaskFinish();
}

void MainSceneTaskBar::updataShowData( void )
{
	// 金币数
	auto AtlasLabelGoldNum = (TextAtlas*)GameMainLayer::seekNodeByName(_mainSceneTaskBar,"AtlasLabelGoldNum");
	AtlasLabelGoldNum->setString( __String::createWithFormat( "%d", NewDataMgr::getInstance()->mUserData.Gold )->getCString() );
}

void MainSceneTaskBar::MsgProChangeGemGold( Ref* sender )
{
	updataShowData();
}

void MainSceneTaskBar::callFuncDeleyTouch(Node* node)
{
	auto btn = (Button*)node;
	btn->setTouchEnabled(true);
}

void MainSceneTaskBar::checkTaskFinish(void)
{
	return;
	int finishTaskNum = 0;

	// 获得日任务表
	finishTaskNum = NewDataMgr::getInstance()->mGameTaskSystem->getDayTaskFinishNum();
	finishTaskNum += NewDataMgr::getInstance()->mGameTaskSystem->getAchiTaskFinishNum();

	auto ImgTaskTipsBg = (Sprite*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "ImgTaskTipsBg");
	ImgTaskTipsBg->setVisible(false);
	if (finishTaskNum)
	{
		ImgTaskTipsBg->setVisible(true);
		auto AtlasLabelTaskTips = (TextAtlas*)GameMainLayer::seekNodeByName(_mainSceneTaskBar, "AtlasLabelTaskTips");
		AtlasLabelTaskTips->setString(StringUtils::format("%d", finishTaskNum));
	}
}



/*******************************************
* 卡牌选择界面
*******************************************/
void CardOptionLayout::showLayout( Node* node, int tag )
{
	if ( node && !node->getChildByName("CardOptionLayout") )
	{
		auto layout = CardOptionLayout::create();
		layout->setTag( tag );
		node->addChild( layout, LocalZOrde_500, "CardOptionLayout" );
	}
}

void CardOptionLayout::onEnter()
{
	Layer::onEnter();

	auto maxLevel = NewDataMgr::getInstance()->getCurMaxLevel();
	auto cardIsUnlock = checkCardIsUnlock();
	auto isFirstEntry = NewDataMgr::getInstance()->getFirstEntryCardState();
	auto cardUseState = NewDataMgr::getInstance()->getCardUseState();

	initInterface();

	// 没通过第二关，且第一次进入卡牌界面时弹出阶段一引导
// 	if (maxLevel <2 && !isFirstEntry )
// 	{
// 		HelpFeqLayer::showHelpFeq(HelpFeqLayer::HelpFeqType_CardStep1);
// 	}
// 	// 已解锁过2或3星卡牌 通过第2关，且在之前查看过卡牌界面的出现以下指引
// 	else if (2 == maxLevel && !cardIsUnlock && isFirstEntry && !cardUseState)
// 	{
// 		_isGuideUseCard = true;
// 		HelpFeqLayer::showHelpFeq(HelpFeqLayer::HelpFeqType_Card2_2, (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonCard2"));
// 	}
// 	// 胜利通过第2关，且从未进入过卡牌界面的出现以下指引
// 	else if (2 == maxLevel && !isFirstEntry)
// 	{
// 		_isGuideUseCard = true;
// 		HelpFeqLayer::showHelpFeq(HelpFeqLayer::HelpFeqType_CardStep3);
// 	}
	NewDataMgr::getInstance()->setFirstEntryCardState(true);

	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(CardOptionLayout::onMessageEvent), MsgEventName_ToCardLayer, nullptr);
	
// 	auto pLayer = (Layer*)GameMainLayer::seekNodeByName(_cardOptionlayout, "PanelTouch" );
// 
// 	_touchLayer = Layer::create();
// 	_touchLayer->setContentSize( Director::getInstance()->getVisibleSize() );
// 	pLayer->addChild(_touchLayer );
// 
// 	OptionCardTouchListener = EventListenerTouchOneByOne::create();
// 	OptionCardTouchListener->onTouchBegan = CC_CALLBACK_2(CardOptionLayout::onTouchBegan,this);
// 	OptionCardTouchListener->onTouchMoved = CC_CALLBACK_2(CardOptionLayout::onTouchMoved,this);
// 	OptionCardTouchListener->onTouchEnded = CC_CALLBACK_2(CardOptionLayout::onTouchEnded,this);
// 	getEventDispatcher()->addEventListenerWithSceneGraphPriority(OptionCardTouchListener,_touchLayer);
// 
// 	OptionCardTouchListener->setEnabled( false );


}

void CardOptionLayout::onExit()
{
	Layer::onExit();

	// 移除观察者消息
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void CardOptionLayout::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	switch (protocolID)
	{
	case EProtocolID_HelpFeqStep2:
	{
		HelpFeqLayer::showHelpFeq(HelpFeqLayer::HelpFeqType_Card2_2, (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonCard2"));
	}
		break;

	default:
		break;
	}

}

void CardOptionLayout::initInterface()
{

	_myLayer = CSLoader::createNode(Ccs_cardOptionLayer_FilePath);

	if ( !_myLayer )
	{
		return;
	}
	addChild( _myLayer );
	do 
	{
		// 返回
		auto ButtonBack = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonBack" );
		setEventForNode( ButtonBack, Tag_Back ,nullptr);

		// 卡牌1
		auto ButtonCardOption1 = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonCardOption1");
		ButtonCardOption1->setCascadeColorEnabled(true);
		setEventForNode(ButtonCardOption1, Tag_Card1, CC_CALLBACK_2(CardOptionLayout::callFuncCardTypeOptionEvent, this));

		// 卡牌2
		auto ButtonCardOption2 = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonCardOption2");
		ButtonCardOption2->setCascadeColorEnabled(true);
		setEventForNode(ButtonCardOption2, Tag_Card2, CC_CALLBACK_2(CardOptionLayout::callFuncCardTypeOptionEvent, this));

		// 卡牌3
		auto ButtonCardOption3 = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonCardOption3");
		ButtonCardOption3->setCascadeColorEnabled(true);
		setEventForNode(ButtonCardOption3, Tag_Card3, CC_CALLBACK_2(CardOptionLayout::callFuncCardTypeOptionEvent, this));

		updateOptionCard();
		updateCardList();
		

	} while (0);
}

void CardOptionLayout::setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback)
{
	do 
	{
		CC_BREAK_IF( node == nullptr );
		node->setTag( tag );

		if ( callback)
			node->addTouchEventListener(callback);
		else
			node->addTouchEventListener(CC_CALLBACK_2(CardOptionLayout::callFuncButtonEvent, this));

	} while (0);
}

bool CardOptionLayout::onTouchBegan( Touch *touch, Event *unused_event )
{
	return true;
}

void CardOptionLayout::callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType)) return;

	Button* btn = (Button*)ref;
	int nTag = btn->getTag();


	switch ( nTag )
	{
	case Tag_Back:
		{
			GameInterfaceMgr::getInstance()->closeCurInterface();
		}
		break;

	default:
		break;
	}
}

void CardOptionLayout::callFuncCardTypeOptionEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (Widget::TouchEventType::BEGAN != touchType) return;

	Button* btn = (Button*)ref;
	auto nTag = (ButtonTag)btn->getTag();

	//runButtonAction(btn);

	int cardType = 1;
	switch (nTag)
	{
	case CardOptionLayout::Tag_Card1:
		cardType = 1;
		break;
	case CardOptionLayout::Tag_Card2:
		cardType = 2;
		break;
	case CardOptionLayout::Tag_Card3:
		cardType = 3;
		break;
	default:
		break;
	}
	optionCardTypePro(cardType);
}

void CardOptionLayout::callFuncCardOptionEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (Widget::TouchEventType::ENDED != touchType)
		return;

	auto btn = (Button*)ref;
// 	auto newCardbg = btn->getParent()->getChildByName("newCardbg");
// 	auto cardParticle = btn->getParent()->getChildByName("cardParticle");
// 	if (newCardbg) newCardbg->removeFromParentAndCleanup(true);
// 	if (cardParticle)
// 	{
// 		cardParticle->removeFromParentAndCleanup(true);
// 		NewDataMgr::getInstance()->mGameCardSystem->setNewUnlockCard(0);
// 	}

	auto newCardbg = btn->getChildByName("newCardTips");
	if (newCardbg)
	{
		newCardbg->removeFromParentAndCleanup(true);
		NewDataMgr::getInstance()->mGameCardSystem->setNewUnlockCard(0);
	}

	_curSelectCardID = btn->getTag();
	showConfirmOption();
}

void CardOptionLayout::showConfirmOption( void )
{
	auto node = GameInterfaceMgr::getInstance()->getParentNode();
	auto cardInfoLayer = CardInformatLayer::getCardInformatLayer();
	cardInfoLayer->setCardIndex(_curSelectCardID);
	cardInfoLayer->setGuideState(_isGuideUseCard);
	cardInfoLayer->addConfirmCallFunc(CC_CALLBACK_1(CardOptionLayout::callFuncChangeOptionCard, this));
	node->addChild(cardInfoLayer, LocalZOrde_1000);

	CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToCardInfoLayer, EProtocolID_HelpFeqStep2);
}

void CardOptionLayout::runButtonAction( Node* node )
{
	auto action = CCSequence::create(CCEaseSineOut::create(CCScaleTo::create(0.1f, 1.15f)), CCEaseSineIn::create(CCScaleTo::create(0.05f, 1.0f)), nullptr);
	node->runAction( action );
}

void CardOptionLayout::optionCardTypePro(int type)
{

	for (int i = 1; i <= 3; i++)
	{
		auto ImgCard = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ImgCard%d", i));

		auto ButtonCardOption = (Button*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ButtonCardOption%d", i));
		auto nTag = (ButtonTag)ButtonCardOption->getTag();
		std::string filePath = StringUtils::format("%s%d.png", Img_Card_FilePath, NewDataMgr::getInstance()->mRoleConfig.cardArr[i - 1].cardID);

		ImgCard->setTexture(filePath);
		int cardType = 1;
		switch (nTag)
		{
		case CardOptionLayout::Tag_Card1:
			cardType = 1;
			break;
		case CardOptionLayout::Tag_Card2:
			cardType = 2;
			break;
		case CardOptionLayout::Tag_Card3:
			cardType = 3;
			break;
		default:
			break;
		}

		if (cardType != type &&!_isOption)
		{
			ImgCard->runAction(CCTintTo::create(0.1f, Color3B(74, 74, 74)));
		}
		else
		{
			ImgCard->runAction(CCTintTo::create(0.1f, Color3B(255, 255, 255)));
		}
	}


	// 隐藏/显示特效
	for (int i = 1; i <= 3; i++)
	{
		auto btnItem = (Button*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ButtonCard%d", i));

		std::string Cardbg = StringUtils::format("Cardbg%d", i);
		std::string cardParticle = StringUtils::format("cardParticle%d", i);

		auto newCardBg = (Sprite*)btnItem->getParent()->getChildByName(Cardbg);
		auto parcile = (ParticleSystemQuad*)btnItem->getParent()->getChildByName(cardParticle);

		if (type != i && !_isOption)
		{
 			if (newCardBg)	newCardBg->setVisible(false);
 			if (parcile)	parcile->setVisible(false);

		}
		else
		{
			if (newCardBg)	newCardBg->setVisible(true);
			if (parcile)	parcile->setVisible(true);
		}
	}

	for (int i = 1; i <= 9; i++)
	{
		auto btnItem = (Button*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ButtonCard%d", i));

		int curCardType = NewDataMgr::getInstance()->mGameCardSystem->getCardConfigWithId(i)->CardType;
		if (curCardType != type && !_isOption)
		{ 
			btnItem->runAction(CCTintTo::create(0.1f, Color3B(74, 74, 74))); btnItem->setTouchEnabled(false);
		}
		else
		{
			btnItem->runAction(CCTintTo::create(0.1f, Color3B(255, 255, 255))); btnItem->setTouchEnabled(true);
		}

	}

	_isOption = !_isOption;

}

void CardOptionLayout::updateOptionCard(void)
{
	for (int i = 1; i <= 3; i++)
	{
		int useingCardId = NewDataMgr::getInstance()->mRoleConfig.cardArr[i - 1].cardID;
		int cardType = NewDataMgr::getInstance()->mGameCardSystem->getCardConfigWithId(useingCardId)->CardId;

		// 星级
		for (int j = 1; j <= 3; j++)
		{
			auto cardStart = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("cardStart%d_%d", i,j));

			if ( j > cardType )
			{
				cardStart->setVisible(false);
			}
			else
			{
				cardStart->setVisible(true);
			}
		}
		
		// 卡牌图片
		std::string filePath = StringUtils::format("%s%d.png", Img_Card_FilePath, useingCardId);
		auto ImgCard = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ImgCard%d", i));
		ImgCard->setTexture(filePath);

		// 攻击力
		auto cardAtk = (TextAtlas*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("AtlasLabelAtkCardType%d", i));
		int atk = NewDataMgr::getInstance()->mGameCardSystem->getCardConfigWithId(useingCardId)->CardBassAtk;
		cardAtk->setString(StringUtils::format("%d", atk));
	}
}

void CardOptionLayout::updateCardList(void)
{
	int newUnlockCard = NewDataMgr::getInstance()->mGameCardSystem->getNewUnlockCard();
	for (int i = 1; i <= 9; i++)
	{
		bool lockState = NewDataMgr::getInstance()->mGameCardSystem->getCardConfigWithId(i)->islock;

		std::string filePath = StringUtils::format("%s%d.png", Img_CardHead_FilePath, i);
		std::string greyfilePath = StringUtils::format("%s%d.png", Img_CardHead_FilePathGrey, i);
		auto btnItem = (Button*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ButtonCard%d", i));
		btnItem->loadTextures(filePath, filePath, greyfilePath);
		btnItem->setTag(i);
		btnItem->setZOrder(100);
		btnItem->addTouchEventListener(CC_CALLBACK_2(CardOptionLayout::callFuncCardOptionEvent, this));
		btnItem->setBright((lockState) ? true : false);

		if (i == NewDataMgr::getInstance()->mRoleConfig.cardArr[0].cardID)
		{
			setOptionState(btnItem, 1);
		}
		else if (i == NewDataMgr::getInstance()->mRoleConfig.cardArr[1].cardID)
		{
			setOptionState(btnItem, 2);
		}
		else if (i == NewDataMgr::getInstance()->mRoleConfig.cardArr[2].cardID)
		{
			setOptionState(btnItem, 3);
		}

// 使用中提示
// 		btnItem->removeChildByName("userTips");
// 		if (i == NewDataMgr::getInstance()->mRoleConfig.cardArr[0].cardID ||
// 			i == NewDataMgr::getInstance()->mRoleConfig.cardArr[1].cardID ||
// 			i == NewDataMgr::getInstance()->mRoleConfig.cardArr[2].cardID)
// 		{
// 			auto useingTips = Sprite::create("roleImg/ImgCardUseing.png");
// 			useingTips->setPosition(btnItem->getContentSize() / 2);
// 			btnItem->addChild(useingTips, 1, "userTips");
// 			useingTips->runAction(CCRepeatForever::create(CCSequence::create(
// 				CCFadeOut::create(0.4f),
// 				CCFadeIn::create(0.4f),
// 				nullptr)));
// 		}

		// 新卡牌提示
		if (newUnlockCard && newUnlockCard == i)
		{
			auto useingTips = Sprite::create("roleImg/ImgStrNew.png");
			useingTips->setPosition(Vec2(btnItem->getContentSize().width , btnItem->getContentSize().height));
			btnItem->addChild(useingTips, 1, "newCardTips");
			useingTips->runAction(CCRepeatForever::create(CCSequence::create(
				CCFadeOut::create(0.4f),
				CCFadeIn::create(0.4f),
				nullptr)));

// 			auto newCardBg = Sprite::create("particla/newUnlockCard/ImgNewUnlockCard.png");
// 			newCardBg->setPosition(btnItem->getPosition()+Vec2(0,5));
// 			newCardBg->setScale(0.9);
// 			btnItem->getParent()->addChild(newCardBg, -1, "newCardbg");
// 			//newCardBg->runAction(RepeatForever::create(Sequence::create(CCFadeOut::create(1.0), CCFadeIn::create(1.5), nullptr)));
// 			auto parcile = ParticleSystemQuad::create("particla/newUnlockCard/test1.plist");
// 			parcile->setScale(2);
// 			parcile->setPosition(btnItem->getPosition());
// 			btnItem->getParent()->addChild(parcile, -1, "cardParticle");
		}

		auto ImgCardLock = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ImgCardLock%d", i));
		ImgCardLock->setVisible((lockState) ? false : true);

		auto ButtonCardFram = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ButtonCardFram%d", i));

		if ( lockState )
		{
			if (3 >= i)
			{
				ButtonCardFram->setTexture("ccsRes/resCardOption/ImgHeadFream1.png");
			}
			else if (6 >= i)
			{
				ButtonCardFram->setTexture("ccsRes/resCardOption/ImgHeadFream2.png");
			}
			else
			{
				ButtonCardFram->setTexture("ccsRes/resCardOption/ImgHeadFream3.png");
			}
		}
		else
		{
			ButtonCardFram->setTexture("roleImg/ImgHeadFreamEnable.png");
		}
	}
}

void CardOptionLayout::callFuncChangeOptionCard(bool isUse)
{
	if ( !isUse )
	{
		updateCardList();
		return;
	}

	NewAudio::getInstance()->playEffect(Music_UserCard);

	updateOptionCard();
	updateCardList();
	//_eventDispatcher->setEnabled(false);

	int cardType = NewDataMgr::getInstance()->mGameCardSystem->getCardConfigWithId(_curSelectCardID)->CardType;
	auto btnItem = (Button*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ButtonCardOption%d", cardType));

	auto ImgCardFlash = Sprite::create("roleImg/ImgCardFlash.png");
	ImgCardFlash->setPosition(btnItem->getContentSize() / 2);
	btnItem->addChild(ImgCardFlash);

	ImgCardFlash->setOpacity(0);
	ImgCardFlash->runAction(Sequence::create(
		CCFadeIn::create(0.5f),
		CCFadeOut::create(1.0f),
		CallFuncN::create(CC_CALLBACK_1(CardOptionLayout::callFuncRemoveCardFlash, this)),
		nullptr));
}

void CardOptionLayout::callFuncRemoveCardFlash(Node* node)
{
	//_eventDispatcher->setEnabled(true);
	node->removeFromParentAndCleanup(true);

	if (_isGuideUseCard)
	{
		_isGuideUseCard = false;
		HelpFeqLayer::showHelpFeq(HelpFeqLayer::HelpFeqType_Card2_3);
	}

}

bool CardOptionLayout::checkCardIsUnlock(void)
{
	for (int i = 1; i <= 9; i++)
	{
		if (1 == i || 4 == i || 7 == i || 2 == i) continue;

		auto item = NewDataMgr::getInstance()->mGameCardSystem->getCardConfigWithId(i);
		if (item->islock)
		{
			return true;
		}
	}
	return false;
}

void CardOptionLayout::setOptionState(Node* node, int tag)
{
	std::string Cardbg = StringUtils::format("Cardbg%d", tag);
	std::string cardParticle = StringUtils::format("cardParticle%d", tag);

	node->getParent()->removeChildByName(Cardbg);
	node->getParent()->removeChildByName(cardParticle);

	auto newCardBg = Sprite::create("particla/newUnlockCard/ImgNewUnlockCard.png");
	newCardBg->setPosition(node->getPosition() + Vec2(-2,0));
	newCardBg->setScale(0.9);
	node->getParent()->addChild(newCardBg, -1, Cardbg);
	auto parcile = ParticleSystemQuad::create("particla/newUnlockCard/test1.plist");
	parcile->setScale(2);
	parcile->setPosition(node->getPosition());
	node->getParent()->addChild(parcile, -1, cardParticle);
}


/*******************************************
*	卡牌信息界面
*******************************************/
CardInformatLayer* CardInformatLayer::getCardInformatLayer(void)
{
	auto layout = CardInformatLayer::create();

	if ( layout )
	{
		return layout;
	}

	return nullptr;
}

void CardInformatLayer::onEnter()
{
	Layer::onEnter();

	//if (getGuideState())
	//NewDataMgr::getInstance()->setCardUseState(true);

	initInterface();
	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(CardInformatLayer::onMessageEvent), MsgEventName_ToCardInfoLayer, nullptr);
}

void CardInformatLayer::onExit()
{
	Layer::onExit();

	// 绉婚櫎瑙傚療鑰呮秷鎭�
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void CardInformatLayer::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	switch (protocolID)
	{
		case EProtocolID_UnlockCard:
		{
			NewAudio::getInstance()->playEffect(Music_buyCard);
			updateSingleCardInfo();
		}
		break;

		case EProtocolID_HelpFeqStep2:
		{
// 			if ( getGuideState() )
// 				HelpFeqLayer::showHelpFeq(HelpFeqLayer::HelpFeqType_Card2_4, (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonConfirm"));
		}
			break;
	default:
		break;
	}
}

void CardInformatLayer::setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback)
{
	do
	{
		CC_BREAK_IF(node == nullptr);

		node->setTag(tag);

		if (callback)
			node->addTouchEventListener(callback);
		else
			node->addTouchEventListener(CC_CALLBACK_2(CardInformatLayer::callFuncButtonEvent, this));

	} while (0);
}

void CardInformatLayer::callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType)) return;

	Button* btn = (Button*)ref;
	int nTag = btn->getTag();

	switch (nTag)
	{
	case Tag_Close:
		{
			if (_confirmCallFunc)
			{
				_confirmCallFunc(false);
			}
			removeFromParentAndCleanup(true);
		}
		break;

	case Tag_Confirm:
		{
			auto curOptionCard = NewDataMgr::getInstance()->mGameCardSystem->getCardConfigWithId(_curSelectCardID);

			// 已解锁
			if (curOptionCard->islock)
			{
				NewDataMgr::getInstance()->mGameCardSystem->setOptionCard(curOptionCard->CardType, _curSelectCardID);

				if (_confirmCallFunc)
				{
					_confirmCallFunc(true);
				}
				
				removeFromParentAndCleanup(true);
			}
			// 未解锁
			else
			{
				bool buystate = NewDataMgr::getInstance()->mGameCardSystem->buyCardWithID(_curSelectCardID);
				if (!buystate)
				{
					removeFromParentAndCleanup(true);
				}
			}
		}
		break;

	default:
		break;
	}

}

void CardInformatLayer::initInterface()
{
	_myLayer = CSLoader::createNode(Ccs_cardInfoLayer_FilePath);

	if (!_myLayer)
	{
		return;
	}

	addChild(_myLayer);

	do
	{
		// 鍏抽棴
		auto ButtonClose = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonClose");
		setEventForNode(ButtonClose, Tag_Close, nullptr);

		auto ButtonConfirm = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonConfirm");
		setEventForNode(ButtonConfirm, Tag_Confirm, nullptr);

		updateSingleCardInfo();
		
	} while (0);
}

void CardInformatLayer::setCardIndex(int index)
{
	_curSelectCardID = index;
}

void CardInformatLayer::updateSingleCardInfo(void)
{
	auto curOptionCard = NewDataMgr::getInstance()->mGameCardSystem->getCardConfigWithId(_curSelectCardID);

	// 更换演示图
	std::string filePath = StringUtils::format("%s%d.png", Img_CardDemo_FilePath, _curSelectCardID);
	auto ImgCurOptionCard = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgCurOptionCard");
	//ImgCurOptionCard->setTexture(filePath);

	// 更换人物
	filePath = StringUtils::format("%s%d.png", Img_CardRole_FilePath, _curSelectCardID);
	auto ImgRoleCard = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgRoleCard");
	//ImgRoleCard->setTexture(filePath);
	//setCardRolePos();

	SpriteFrame *frame = nullptr;
	switch (curOptionCard->CardType)
	{
	case 1:
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("redWp.png");
		break;
	case 2:
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("blueWp.png");
		break;
	case 3:
		frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("yellowWp.png");
		break;
	default:
		break;
	}
	auto SpriteBall = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "SpriteBall");
	SpriteBall->setScale(0.8);
	SpriteBall->setSpriteFrame(frame);

	// 卡名
	auto TextOptionCardName = (Text*)GameMainLayer::seekNodeByName(_myLayer, "TextOptionCardName");
	TextOptionCardName->setString(curOptionCard->CardName);

	// 说明
	auto TextOptionCardExplain = (Text*)GameMainLayer::seekNodeByName(_myLayer, "TextOptionCardExplain");
	TextOptionCardExplain->setString(curOptionCard->CardDes);

	// 价格
	auto ImgConfirm = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgConfirm");
	auto ImgBuyPrice = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgBuyPrice");
	auto AtlasLabelLockPrice = (TextAtlas*)GameMainLayer::seekNodeByName(_myLayer, "AtlasLabelLockPrice");


	if (curOptionCard->islock)
	{
		ImgConfirm->setVisible(true);
		AtlasLabelLockPrice->setVisible(false);
	}
	else
	{
		ImgConfirm->setVisible(false);
		AtlasLabelLockPrice->setVisible(true);
		AtlasLabelLockPrice->setString(StringUtils::format("%d", NewDataMgr::getInstance()->mGameCardSystem->getCardPriceWithID(_curSelectCardID)));

		ImgBuyPrice->setPosition(Vec2(-5, AtlasLabelLockPrice->getContentSize().height / 2));
	}

	int cardType = NewDataMgr::getInstance()->mGameCardSystem->getCardConfigWithId(_curSelectCardID)->CardId;
	// 星级
	for (int j = 1; j <= 3; j++)
	{
		auto cardStart = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("cardStart%d_%d", 3, j));

		if (j > cardType)
		{
			cardStart->setVisible(false);
		}
		else
		{
			cardStart->setVisible(true);
		}
	}

	// 攻击力
	auto cardAtk = (TextAtlas*)GameMainLayer::seekNodeByName(_myLayer, "AtlasLabelAtk");
	int atk = NewDataMgr::getInstance()->mGameCardSystem->getCardConfigWithId(_curSelectCardID)->CardBassAtk;
	cardAtk->setString(StringUtils::format("%d", atk));
}

void CardInformatLayer::playCardUnlockAnim(void)
{
	return;
	Button* btn = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonCurOptionCard");
	Vec2 pos = btn->getParent()->convertToWorldSpace(btn->getPosition());

	auto vsize = Director::getInstance()->getVisibleSize();
	auto node = Node::create();
	node->setScale(1.5f);
	node->setPosition(pos);
	addChild(node, 1001);

	for (int i = 1; i <= 8; i++)
	{
		auto parcile = ParticleSystemQuad::create(StringUtils::format("particla/CardUnlock/test%d.plist", i));
		node->addChild(parcile, -1);

	}

	auto Lock = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "OptionCardLock");
	Lock->setVisible(false);
}

void CardInformatLayer::addConfirmCallFunc(const ccConfirmCallBack& callfunc)
{
	_confirmCallFunc = callfunc;
}

void CardInformatLayer::setCardRolePos()
{
	auto ImgRoleCard = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgRoleCard");

	switch (_curSelectCardID)
	{
	case 1:
		ImgRoleCard->setPosition(Vec2(-83, 10));
		break;
	case 2:
		ImgRoleCard->setPosition(Vec2(-83, 71));
		break;
	case 3:
		ImgRoleCard->setPosition(Vec2(-83, 138));
		break;
	case 4:
		ImgRoleCard->setPosition(Vec2(-83, 61));
		break;
	case 5:
		ImgRoleCard->setPosition(Vec2(-83, 114));
		break;
	case 6:
		ImgRoleCard->setPosition(Vec2(-83, 80));
		ImgRoleCard->setScale(0.9);
		break;
	case 7:
		ImgRoleCard->setPosition(Vec2(-80, 95));
		ImgRoleCard->setScale(1.02);
		break;
	case 8:
		ImgRoleCard->setPosition(Vec2(-83, 45));
		break;
	case 9:
		ImgRoleCard->setPosition(Vec2(-83, 91));
		break;
	default:
		break;
	}
}



/*******************************************
* 魔法屋
*******************************************/
void UpgradeLayer::showLayout( Node* node, int tag )
{
	if ( node && !node->getChildByName("UpgradeLayer") )
	{
		auto layout = UpgradeLayer::create();
		layout->setTag( tag );
		node->addChild( layout, LocalZOrde_500, "UpgradeLayer" );
	}
}

void UpgradeLayer::onEnter()
{
	Layer::onEnter();

	initInterface();

	// 魔法升级首次进入引导
// 	if ( !NewDataMgr::getInstance()->getFirstEntryUpgradeState() )
// 	{
// 		_isGuideUpGrade = true;
// 		NewDataMgr::getInstance()->setFirstEntryUpgradeState(true);
// 		HelpFeqLayer::showHelpFeq(HelpFeqLayer::HelpFeqType_Upgrade1, (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonItem1"));
// 	}

	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(UpgradeLayer::onMessageEvent), MsgEventName_ToUpgradeLayer, nullptr);
}

void UpgradeLayer::onExit()
{
	Layer::onExit();

	// 移除观察者消息
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void UpgradeLayer::setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback)
{
	do 
	{
		CC_BREAK_IF( node == nullptr );

		node->setTag( tag );

		if (callback)
			node->addTouchEventListener(callback);
		else
			node->addTouchEventListener(CC_CALLBACK_2(UpgradeLayer::callFuncButtonEvent, this));

	} while (0);
}

void UpgradeLayer::callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType)) return;

	Button* btn = (Button*)ref;
	int nTag = btn->getTag();


	switch ( nTag )
	{
	case Tag_Back:
		GameInterfaceMgr::getInstance()->closeCurInterface();
		break;	

	case Tag_Upgrade:
		upgradeItemPro((intptr_t)btn->getUserData());
		break;
	
	default:
		break;
	}
}

void UpgradeLayer::callFuncOptionItemEvent(Ref* ref, Widget::TouchEventType touchType)
{

	if (Widget::TouchEventType::BEGAN != touchType) return;

// 	if(_isGuideUpGrade)
// 		HelpFeqLayer::showHelpFeq(HelpFeqLayer::HelpFeqType_Upgrade3, (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonUpgrade"));


	Button* btn = (Button*)ref;
	auto nTag = (ButtonTag)btn->getTag();

	for (int i = 0; i < 6; i++)
	{
		auto btnItem = (Button*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ButtonItem%d", i + 1));

		btnItem->setBright((nTag == btnItem->getTag()) ? false : true);

		if (nTag == btnItem->getTag())
		{
			_curOptionItemId = i + 1;
		}

// 		auto ImgSign = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ImgSign%d", i + 1));
// 		ImgSign->stopAllActions();
// 		ImgSign->setScale(1.0f);
// 
// 		// 箭头跳动
// 		if (nTag == btnItem->getTag())
// 		{
// 			_curOptionItemId = i + 1;
// 			auto objectIt = NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(_curOptionItemId);
// 
// 
// 			if (objectIt->curGrade >= objectIt->maxGrade)
// 			{
// 				ImgSign->setVisible(false);
// 			}
// 			else
// 			{
// 				ImgSign->runAction(CCRepeatForever::create(CCSequence::create(
// 					CCEaseSineOut::create(CCScaleTo::create(1.0, 1.2, 0.7)),
// 					CCEaseBounceOut::create(CCScaleTo::create(0.4, 1.0, 1.0)),
// 					DelayTime::create(0.5f),
// 					nullptr)));
// 			}
// 		}
	}

	auto ButtonUpgrade = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonUpgrade");
	ButtonUpgrade->setUserData((void*)nTag);

	updateObjectInfo();

}

void UpgradeLayer::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	switch (protocolID)
	{
	case EProtocolID_UpgradeObject:
		{
// 			if (_isGuideUpGrade)
// 			{
// 				_isGuideUpGrade = false;
// 				HelpFeqLayer::showHelpFeq(HelpFeqLayer::HelpFeqType_Upgrade2, nullptr);
// 			}
			updateInterface();
			playUpgradeAnim();
			updateObjectInfo();
		}
		break;

	default:
		break;
	}
}

void UpgradeLayer::initInterface()
{
	_myLayer = CSLoader::createNode(Ccs_UpgradeLayer_FilePath);

	if (!_myLayer)
	{
		return;
	}

	addChild(_myLayer);

	do 
	{
		auto ButtonBack = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonBack");
		setEventForNode( ButtonBack, Tag_Back , nullptr);

		auto ButtonUpgrade = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonUpgrade");
		setEventForNode(ButtonUpgrade, Tag_Upgrade, nullptr);

		// 鍒濆鎻愮ず鐣岄潰
		auto NodeUpgrade = (Node*)GameMainLayer::seekNodeByName(_myLayer, "NodeUpgrade");
		auto TextUpgrdeTips = (Node*)GameMainLayer::seekNodeByName(_myLayer, "TextUpgrdeTips");
		NodeUpgrade->setVisible(false);
		TextUpgrdeTips->setVisible(true);

		TextUpgrdeTips->runAction(CCRepeatForever::create(CCSequence::create(
			CCFadeIn::create(0.2f),
			DelayTime::create(3.0f),
			CCFadeOut::create(0.5f),
			nullptr)));

		updateInterface();

	} while (0);

}

void UpgradeLayer::upgradeItemPro(int tag)
{
	int objectKey = 0;
	
	switch (tag)
	{
	case UpgradeLayer::Tag_ItemRole:
		objectKey = GameGradeMsg::ObjectType_Role;
		break;

	case UpgradeLayer::Tag_ItemDaZhao:
		objectKey = GameGradeMsg::ObjectType_DaZhao;
		break;

	case UpgradeLayer::Tag_ItemGold:
		objectKey = GameGradeMsg::ObjectType_Gold;
		break;

	case UpgradeLayer::Tag_ItemRedBall:
		objectKey = GameGradeMsg::ObjectType_RedBall;
		break;

	case UpgradeLayer::Tag_ItemBlueBall:
		objectKey = GameGradeMsg::ObjectType_BlueBall;
		break;

	case UpgradeLayer::Tag_itemYellowBall:
		objectKey = GameGradeMsg::ObjectType_YellowBall;
		break;

	default:
		break;
	}
	if ( _isGuideUpGrade)
	{
		NewDataMgr::getInstance()->mGameGradeMsg->buyObjectGradeWithType(objectKey, true);
	}
	else
	{
		NewDataMgr::getInstance()->mGameGradeMsg->buyObjectGradeWithType(objectKey);
	}
}

void UpgradeLayer::updateObjectInfo()
{

	// 鏄剧ず/闅愯棌鎻愮ず鏂囧瓧
	auto NodeUpgrade = (Node*)GameMainLayer::seekNodeByName(_myLayer, "NodeUpgrade");
	auto TextUpgrdeTips = (Node*)GameMainLayer::seekNodeByName(_myLayer, "TextUpgrdeTips");
	NodeUpgrade->setVisible(true);
	TextUpgrdeTips->setVisible(false);
	TextUpgrdeTips->stopAllActions();

	// 鏇存柊閫夋嫨鐨勯」鐩彁绀�
	int curItemID = _curOptionItemId;
	auto AtlasLabelUpgradePrice = (TextAtlas*)GameMainLayer::seekNodeByName(_myLayer, "AtlasLabelUpgradePrice");
	auto TextItemUpgradeTips = (Text*)GameMainLayer::seekNodeByName(_myLayer, "TextItemUpgradeTips");
	auto ButtonUpgrade = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonUpgrade");

	auto objectIt = NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(curItemID);
	auto objectBuyIt = NewDataMgr::getInstance()->mGameGradeMsg->getObjectUpgradePriceWithType(curItemID);

	if (objectIt->curGrade < objectIt->maxGrade)
	{
		AtlasLabelUpgradePrice->setString(StringUtils::format("%d", objectBuyIt.Gold));
		TextItemUpgradeTips->setString(objectBuyIt.GoodsDes);
		ButtonUpgrade->setTouchEnabled(true);
		ButtonUpgrade->setColor(Color3B(255, 255, 255));
	}
	else
	{
		AtlasLabelUpgradePrice->setString("0");
		TextItemUpgradeTips->setString(objectBuyIt.GoodsDes);
		ButtonUpgrade->setTouchEnabled(false);
		ButtonUpgrade->setColor(Color3B(74, 74, 74));
	}

	auto ImgSign = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ImgSign%d", curItemID));
	ImgSign->stopAllActions();
	ImgSign->setScale(1.0f);

	if (objectIt->curGrade >= objectIt->maxGrade)
	{
		ImgSign->setVisible(false);
	}
	else
	{
		ImgSign->runAction(CCRepeatForever::create(CCSequence::create(
			CCEaseSineOut::create(CCScaleTo::create(1.0, 1.2, 0.7)),
			CCEaseBounceOut::create(CCScaleTo::create(0.4, 1.0, 1.0)),
			DelayTime::create(0.5f),
			nullptr)));
	}
}

void UpgradeLayer::updateInterface(void)
{
	for (int i = 0, j = 1; i < 6; i++, j++)
	{
		auto btnItem = (Button*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ButtonItem%d", i + 1));
		btnItem->setUserData((void*)j);
		setEventForNode(btnItem, ButtonTag(Tag_ItemRole + i), CC_CALLBACK_2(UpgradeLayer::callFuncOptionItemEvent, this));

		auto objectIt = NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(i+1);

		auto ImgSign = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ImgSign%d", i + 1));
		if (objectIt->curGrade >= objectIt->maxGrade)
		{
			ImgSign->setVisible(false);
		}
	}




	// 初始升级对象数据
	for (int i = 1; i <= 6; i++)
	{
		auto it = NewDataMgr::getInstance()->mGameGradeMsg->getObjectGradeWithType(i);

		for (int j = 1; j <= it->maxGrade; j++)
		{
			auto btnItem = (Button*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ImgStar%d_%d", i, j));
			btnItem->setBright((j <= it->curGrade) ? true : false);
		}

		auto itemGrade = (Text*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("TextItemGrade%d", i));
		itemGrade->setString((it->curGrade < it->maxGrade) ? StringUtils::format("%d", it->curGrade) : "Max");
	}
}

void UpgradeLayer::playUpgradeAnim(void)
{
	NewAudio::getInstance()->playEffect(Music_UpgradeObject);

	Vector<SpriteFrame*> frame_array;

	for (int i = 0; i <= 9; ++i)
	{
		std::string pic = CCString::createWithFormat("%s%d.png", "Upgrade_", i)->getCString();
		CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pic.c_str());

		frame_array.pushBack(frame);
	}
	
	auto p_animation = Animation::createWithSpriteFrames(frame_array, 0.1f);
	p_animation->setLoops(1);

	auto animate = Animate::create(p_animation);

	auto btnItem = (Button*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ButtonItem%d", _curOptionItemId));
	auto upgradeAnimNode = Sprite::create("weapon.png");
	upgradeAnimNode->setScale(2.8f);
	upgradeAnimNode->setPosition(btnItem->getParent()->convertToWorldSpace(btnItem->getPosition()));
	addChild(upgradeAnimNode);

	upgradeAnimNode->runAction(Sequence::create(
		animate,
		CallFuncN::create(CC_CALLBACK_1(UpgradeLayer::removeUpgradeAnim, this)),
		nullptr));
	
	// 闪光
	auto ImgFlash = Sprite::create("roleImg/ImgUpgradeFlash.png");
	ImgFlash->setPosition(btnItem->getContentSize() / 2);
	btnItem->addChild(ImgFlash);

	ImgFlash->setOpacity(0);
	ImgFlash->runAction(Sequence::create(
		DelayTime::create(1.5f),
		CCFadeIn::create(0.5f),
		CCFadeOut::create(1.0f),
		CallFuncN::create(CC_CALLBACK_1(UpgradeLayer::callFuncRemoveFlash, this)),
		nullptr));
}

void UpgradeLayer::removeUpgradeAnim(Node* node)
{
	node->removeFromParentAndCleanup(true);
}

void UpgradeLayer::callFuncRemoveFlash(Node* node)
{	
	node->removeAllChildrenWithCleanup(true);
}


/*******************************************
*	任务列表
*******************************************/
void GameTaskList::showLayout( Node* node, int tag )
{
	if ( node && !node->getChildByName("GameTaskList") )
	{
		auto layout = GameTaskList::create();
		layout->setTag( tag );
		node->addChild(layout, LocalZOrde_500, "GameTaskList");
	}
}

void GameTaskList::onEnter()
{
	Layer::onEnter();

	initInterface();
	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(GameTaskList::onMessageEvent), MsgEventName_ToTaskLayer, nullptr);
}

void GameTaskList::onExit()
{
	Layer::onExit();

	// 移除观察者消息
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void GameTaskList::setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback)
{
	do 
	{
		CC_BREAK_IF( node == nullptr );

		node->setTag( tag );

		if ( callback )
			node->addTouchEventListener(callback);
		else
			node->addTouchEventListener(CC_CALLBACK_2(GameTaskList::callFuncButtonEvent, this));

	} while (0);
}

void GameTaskList::callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType)) return;

	Button* btn = (Button*)ref;
	int nTag = btn->getTag();

	switch ( nTag )
	{

	case Tag_Back:
		GameInterfaceMgr::getInstance()->closeCurInterface();
		break;


	default:
		break;
	}
}

void GameTaskList::callFuncGetRewardEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType, CommonLayer::SoundType_None)) return;

	NewAudio::getInstance()->playEffect(Music_RewardGoods);

	Button* btn = (Button*)ref;
	int nTag = btn->getTag();

	_curGetTaskID = nTag;
	// 获取奖励
	NewDataMgr::getInstance()->mGameTaskSystem->getTaskRewardWithTaskId(_curGetTaskID);

	auto goodsInfo = NewDataMgr::getInstance()->mGameTaskSystem->getGameTaskListWithID(_curGetTaskID);
	ItemRewardTips::getInstance(nullptr)->addItemTips(goodsInfo->TaskRewardID, (void*)goodsInfo->RewardNum);

	CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_UpdateTaskNum);

	// 任务奖励界面
// 	auto node = GameInterfaceMgr::getInstance()->getParentNode();
// 	auto showGoodsInfoLayer = ShowGoodsLayer::getShowGoodsInfoLayer();
// 	showGoodsInfoLayer->setShowInfoType(ShowGoodsLayer::showInfoType::InfoType_TaskReward, _curGetTaskID);
// 	node->addChild(showGoodsInfoLayer, LocalZOrde_1000);
}

void GameTaskList::callFuncLabelEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (Widget::TouchEventType::BEGAN != touchType) return;

	Button* btn = (Button*)ref;

	changeTaskLable(btn->getTag());
}

void GameTaskList::changeTaskLable(int id)
{
	auto ButtonDayTask = (Button*)GameMainLayer::seekNodeByName(_mylayer, "ButtonDayTask");
	auto ButtonOtherTask = (Button*)GameMainLayer::seekNodeByName(_mylayer, "ButtonOtherTask");
	auto ScrollViewDayTask = (cocos2d::ui::ScrollView*)GameMainLayer::seekNodeByName(_mylayer, "ScrollViewDayTask");
	auto ScrollViewSystemTask = (cocos2d::ui::ScrollView*)GameMainLayer::seekNodeByName(_mylayer, "ScrollViewSystemTask");

	auto taskList = getChildByName("taskList");
	if (taskList)
	{
		taskList->removeFromParentAndCleanup(true);
	}

	switch (id)
	{
	case Tag_LabelDayTask:
		{
			ButtonDayTask->setColor(Color3B(255, 255, 255)); 
			ButtonOtherTask->setColor(Color3B(115, 115, 115));

 			auto taskList = TaskListView::create(TaskListView::ListType::ListType_DayTask);
 			taskList->setName("taskList");
 			addChild(taskList);

			//			addTaskItemToDayList();
			//			ScrollViewDayTask->setPosition(Vec2(0, -25));
			// 			ScrollViewSystemTask->setPosition(Vec2(-1000, -25));
		}
		break;
	case Tag_LabelAchTask: 
		{
			ButtonDayTask->setColor(Color3B(115, 115, 115));
			ButtonOtherTask->setColor(Color3B(255, 255, 255)); 

			auto taskList = TaskListView::create(TaskListView::ListType::ListType_AchiTask);
			taskList->setName("taskList");
 			addChild(taskList);

			//			addTaskItemToAchiList();
			//			ScrollViewSystemTask->setPosition(Vec2(0, -25));
			//			ScrollViewDayTask->setPosition(Vec2(-1000, -25));
		}
		break;
	default:
		break;
	}
}

void GameTaskList::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	switch (protocolID)
	{
	case EProtocolID_GetTaskReward:
		{
			checkTaskFinish();
		}
		break;

	default:
		break;
	}
}

void GameTaskList::initInterface()
{
	_mylayer = CSLoader::createNode(Ccs_TaskLayer_FilePath);

	if (!_mylayer)
	{
		return;
	}

	addChild(_mylayer);

	do 
	{
		// 返回
		auto ButtonBack = (Button*)GameMainLayer::seekNodeByName(_mylayer, "ButtonBack");
		setEventForNode( ButtonBack, Tag_Back ,nullptr);

		auto ButtonDayTask = (Button*)GameMainLayer::seekNodeByName(_mylayer, "ButtonDayTask");
		setEventForNode(ButtonDayTask, Tag_LabelDayTask, CC_CALLBACK_2(GameTaskList::callFuncLabelEvent, this));

		auto ButtonOtherTask = (Button*)GameMainLayer::seekNodeByName(_mylayer, "ButtonOtherTask");
		setEventForNode(ButtonOtherTask, Tag_LabelAchTask, CC_CALLBACK_2(GameTaskList::callFuncLabelEvent, this));

		changeTaskLable(Tag_LabelDayTask);
		checkTaskFinish();
		auto ScrollViewList = (cocos2d::ui::ScrollView*)GameMainLayer::seekNodeByName(_mylayer, "ScrollViewDayTask");



	} while (0);
}

class Cmpare
{
public:
	bool operator()(const csvGameTaskConfig_s* st1, const csvGameTaskConfig_s*  st2) const
	{
		return st1->TaskState > st2->TaskState;
	}
};
void GameTaskList::addTaskItemToDayList(void)
{
	auto ScrollViewList = (cocos2d::ui::ScrollView*)GameMainLayer::seekNodeByName(_mylayer, "ScrollViewDayTask");
	ScrollViewList->removeAllChildrenWithCleanup(true);
	ScrollViewList->setSwallowTouches(true);

	// 获得任务表
	std::vector<int> tempTaskList = NewDataMgr::getInstance()->mGameTaskSystem->getDayTaskList();
	std::vector<int>::iterator item = tempTaskList.begin();

	int itemH = 130;
	int itemNum = tempTaskList.size();
	auto scrollSize = ScrollViewList->getInnerContainerSize();
	ScrollViewList->setInnerContainerSize(Size(scrollSize.width, itemH*itemNum));
	scrollSize = ScrollViewList->getInnerContainerSize();

	// 统计已完成任务数
	int finishTaskNum = 0;

	std::list<csvGameTaskConfig_s*> tempList;
	for (int i = 0; i < itemNum; i++, item++)
	{
		auto taskItem = NewDataMgr::getInstance()->mGameTaskSystem->getGameTaskListWithID((*item));
		tempList.push_back(taskItem);
	}
	tempList.sort((Cmpare()));

	auto taskItem = tempList.begin();
	for (int i = 0; i < itemNum; i++, taskItem++)
	{
		auto itemNode = Button::create("TaskRes/ImgItemBg9.png");
		itemNode->setTouchEnabled(false);
		itemNode->setScale9Enabled(true);
		itemNode->setCapInsets(Rect(200, 50, 50, 50));
		itemNode->setContentSize(Size(660, 160));
		itemNode->setPosition(Vec2(scrollSize.width / 2, scrollSize.height - itemH / 2 - i*itemH));
		ScrollViewList->addChild(itemNode);

		// 物品背景
		auto ImgGoodsBg = Sprite::create("TaskRes/ImgGoodsBg.png");
		ImgGoodsBg->setPosition(Vec2(90, 87));
		itemNode->addChild(ImgGoodsBg);

		// 物品
		auto ImgGoods = Sprite::create(getGoodsResWithID((*taskItem)->TaskRewardID));
		ImgGoods->setPosition(Vec2(55, 55));
		ImgGoodsBg->addChild(ImgGoods);

		// 奖励
		auto StrJL = Text::create();
		StrJL->setString("奖励");
		StrJL->setColor(Color3B(134, 23, 15));
		StrJL->setFontSize(27);
		StrJL->setPosition(Vec2(216, 105));
		itemNode->addChild(StrJL);

		// 奖励说明
		auto StrJLExplant = Text::create();
		StrJLExplant->setAnchorPoint(Vec2(0, 0.5));
		StrJLExplant->setString((*taskItem)->TaskRewardDes);
		StrJLExplant->setColor(Color3B(46, 36, 26));
		StrJLExplant->setFontSize(27);
		StrJLExplant->setPosition(Vec2(250, 105));
		itemNode->addChild(StrJLExplant);

		// 任务说明
		auto StrTaskExplant = Text::create();
		StrTaskExplant->setAnchorPoint(Vec2(0.0, 1.0));
		StrTaskExplant->setPosition(Vec2(185, 85));
		StrTaskExplant->setTextHorizontalAlignment(TextHAlignment::LEFT);
		StrTaskExplant->setTextVerticalAlignment(TextVAlignment::CENTER);
		StrTaskExplant->setTextAreaSize(Size(320, 0));
		StrTaskExplant->setString((*taskItem)->TaskDes);
		StrTaskExplant->setColor(Color3B(46, 36, 26));
		StrTaskExplant->setFontSize(22);
		StrTaskExplant->ignoreContentAdaptWithSize(false);
		itemNode->addChild(StrTaskExplant);

		// 已完成未领取
		if (2 == (*taskItem)->TaskState)
		{
			auto filePath = "TaskRes/BtnTaskGet.png";
			auto BtnGet = Button::create(filePath, filePath, filePath);
			BtnGet->setPosition(Vec2(565, 83));
			BtnGet->setTouchEnabled(true);
			itemNode->addChild(BtnGet);
			setEventForNode(BtnGet, (*taskItem)->TaskID, CC_CALLBACK_2(GameTaskList::callFuncGetRewardEvent, this));
		}
		// 未完成
		else if (0 == (*taskItem)->TaskState)
		{
			// 任务进度
			auto StrPar = Text::create();
			StrPar->setAnchorPoint(Vec2(0.5, 0.5));
			StrPar->setString("进度");
			StrPar->setColor(Color3B(134, 23, 15));
			StrPar->setFontSize(27);
			StrPar->setPosition(Vec2(530, 85));
			itemNode->addChild(StrPar);

			// 进度
			float progressNum = (*taskItem)->TaskProgressNum;
			float targetNum = (*taskItem)->TaskTargetNum;
			auto StrProportion = Text::create();
			StrProportion->setAnchorPoint(Vec2(0, 0.5));
			StrProportion->setString(StringUtils::format("%d%s", (int)((progressNum / targetNum)*100), "%"));
			StrProportion->setColor(Color3B(134, 23, 15));
			StrProportion->setFontSize(27);
			StrProportion->setPosition(Vec2(570, 85));
			itemNode->addChild(StrProportion);
		}
		// 已领取
		else if (1 == (*taskItem)->TaskState)
		{
			auto filePath = "TaskRes/ImgFinish.png";
			auto ImgFinish = Sprite::create(filePath);
			ImgFinish->setPosition(Vec2(565, 83));
			itemNode->addChild(ImgFinish);
		}
	}

}

void GameTaskList::addTaskItemToAchiList(void)
{
	auto ScrollViewList = (cocos2d::ui::ScrollView*)GameMainLayer::seekNodeByName(_mylayer, "ScrollViewSystemTask");
	ScrollViewList->removeAllChildrenWithCleanup(true);
	ScrollViewList->setSwallowTouches(true);

	// 获得任务表
	std::vector<int> tempTaskList = NewDataMgr::getInstance()->mGameTaskSystem->getAchiTaskList();
	std::vector<int>::iterator item = tempTaskList.begin();

	int itemH = 130;
	int itemNum = tempTaskList.size();
	auto scrollSize = ScrollViewList->getInnerContainerSize();
	ScrollViewList->setInnerContainerSize(Size(scrollSize.width, itemH*itemNum));
	scrollSize = ScrollViewList->getInnerContainerSize();

	// 统计已完成任务数
	int finishTaskNum = 0;

	std::list<csvGameTaskConfig_s*> tempList;
	for (int i = 0; i < itemNum; i++, item++)
	{
		auto taskItem = NewDataMgr::getInstance()->mGameTaskSystem->getGameTaskListWithID((*item));
		tempList.push_back(taskItem);
	}
	tempList.sort((Cmpare()));

	auto taskItem = tempList.begin();
	for (int i = 0; i < itemNum; i++, taskItem++)
	{
		auto itemNode = Button::create("TaskRes/ImgItemBg9.png");
		itemNode->setTouchEnabled(false);
		itemNode->setScale9Enabled(true);
		itemNode->setCapInsets(Rect(200, 50, 50, 50));
		itemNode->setContentSize(Size(660, 160));
		itemNode->setPosition(Vec2(scrollSize.width / 2, scrollSize.height - itemH / 2 - i*itemH));
		ScrollViewList->addChild(itemNode);

		// 物品背景
		auto ImgGoodsBg = Sprite::create("TaskRes/ImgGoodsBg.png");
		ImgGoodsBg->setPosition(Vec2(90, 87));
		itemNode->addChild(ImgGoodsBg);

		// 物品
		auto ImgGoods = Sprite::create(getGoodsResWithID((*taskItem)->TaskRewardID));
		ImgGoods->setPosition(Vec2(55, 55));
		ImgGoodsBg->addChild(ImgGoods);

		// 奖励
		auto StrJL = Text::create();
		StrJL->setString("奖励");
		StrJL->setColor(Color3B(134, 23, 15));
		StrJL->setFontSize(27);
		StrJL->setPosition(Vec2(216, 105));
		itemNode->addChild(StrJL);

		// 奖励说明
		auto StrJLExplant = Text::create();
		StrJLExplant->setAnchorPoint(Vec2(0, 0.5));
		StrJLExplant->setString((*taskItem)->TaskRewardDes);
		StrJLExplant->setColor(Color3B(46, 36, 26));
		StrJLExplant->setFontSize(27);
		StrJLExplant->setPosition(Vec2(250, 105));
		itemNode->addChild(StrJLExplant);

		// 任务说明
		auto StrTaskExplant = Text::create();
		StrTaskExplant->setAnchorPoint(Vec2(0.0, 1.0));
		StrTaskExplant->setPosition(Vec2(185, 85));
		StrTaskExplant->setTextHorizontalAlignment(TextHAlignment::LEFT);
		StrTaskExplant->setTextVerticalAlignment(TextVAlignment::CENTER);
		StrTaskExplant->setTextAreaSize(Size(320, 0));
		StrTaskExplant->setString((*taskItem)->TaskDes);
		StrTaskExplant->setColor(Color3B(46, 36, 26));
		StrTaskExplant->setFontSize(22);
		StrTaskExplant->ignoreContentAdaptWithSize(false);
		itemNode->addChild(StrTaskExplant);

		// 已完成未领取
		if (2 == (*taskItem)->TaskState)
		{
			auto filePath = "TaskRes/BtnTaskGet.png";
			auto BtnGet = Button::create(filePath, filePath, filePath);
			BtnGet->setPosition(Vec2(565, 83));
			BtnGet->setTouchEnabled(true);
			itemNode->addChild(BtnGet);
			setEventForNode(BtnGet, (*taskItem)->TaskID, CC_CALLBACK_2(GameTaskList::callFuncGetRewardEvent, this));
		}
		// 未完成
		else if (0 == (*taskItem)->TaskState)
		{
			// 任务进度
			auto StrPar = Text::create();
			StrPar->setAnchorPoint(Vec2(0.5, 0.5));
			StrPar->setString("进度");
			StrPar->setColor(Color3B(134, 23, 15));
			StrPar->setFontSize(27);
			StrPar->setPosition(Vec2(530, 85));
			itemNode->addChild(StrPar);

			// 进度
			float progressNum = (*taskItem)->TaskProgressNum;
			float targetNum = (*taskItem)->TaskTargetNum;
			auto StrProportion = Text::create();
			StrProportion->setAnchorPoint(Vec2(0, 0.5));
			StrProportion->setString(StringUtils::format("%d%s", (int)((progressNum / targetNum) * 100), "%"));
			StrProportion->setColor(Color3B(134, 23, 15));
			StrProportion->setFontSize(27);
			StrProportion->setPosition(Vec2(570, 85));
			itemNode->addChild(StrProportion);
		}
		// 已领取
		else if (1 == (*taskItem)->TaskState)
		{
			auto filePath = "TaskRes/ImgFinish.png";
			auto ImgFinish = Sprite::create(filePath);
			ImgFinish->setPosition(Vec2(565, 83));
			itemNode->addChild(ImgFinish);
		}
	}
}

std::string GameTaskList::getGoodsResWithID(int id)
{
	std::string res;
	switch (id)
	{
	case GoodsID_Gold:
		res = "ccsRes/resShopLayer/ImgGold2.png";
		break;
	case GoodsID_Life:
		res = "ccsRes/resShopLayer/ImgLife1.png";
		break;
	case GoodsID_DaZhao:
		res = "ccsRes/resShopLayer/ImgDaZhao2.png";
		break;
	case GoodsID_HuDun:
		res = "ccsRes/resShopLayer/ImgHuDun2.png";
		break;
	default:
		break;
	}

	return res;
}

void GameTaskList::checkTaskFinish(void)
{
	int finishTaskNum = 0;

	// 获得日任务表
	finishTaskNum = NewDataMgr::getInstance()->mGameTaskSystem->getDayTaskFinishNum();
	auto ImgTaskItem1TipsBg = (Sprite*)GameMainLayer::seekNodeByName(_mylayer, "ImgTaskItem1TipsBg");
	ImgTaskItem1TipsBg->setVisible(false);
	if (finishTaskNum)
	{
		ImgTaskItem1TipsBg->setVisible(true);
		auto AtlasLabelTaskTips1 = (TextAtlas*)GameMainLayer::seekNodeByName(_mylayer, "AtlasLabelTaskTips1");
		AtlasLabelTaskTips1->setString(StringUtils::format("%d", finishTaskNum));
	}


	// 获得成就任务表
	finishTaskNum = NewDataMgr::getInstance()->mGameTaskSystem->getAchiTaskFinishNum();

	auto ImgTaskItem2TipsBg = (Sprite*)GameMainLayer::seekNodeByName(_mylayer, "ImgTaskItem2TipsBg");
	ImgTaskItem2TipsBg->setVisible(false);
	if (finishTaskNum)
	{
		ImgTaskItem2TipsBg->setVisible(true);
		auto AtlasLabelTaskTips2 = (TextAtlas*)GameMainLayer::seekNodeByName(_mylayer, "AtlasLabelTaskTips2");
		AtlasLabelTaskTips2->setString(StringUtils::format("%d", finishTaskNum));
	}

}


/*******************************************
*	签到任务
*******************************************/
void SignInLayer::showLayout(Node* node, int tag)
{
	if (node && !node->getChildByName("SignInLayer"))
	{
		auto layout = SignInLayer::create();
		layout->setTag(tag);
		node->addChild(layout, LocalZOrde_1000, "SignInLayer");
	}
}

void SignInLayer::onEnter()
{
	Layer::onEnter();

	initInterface();

	if (NewDataMgr::getInstance()->getGuideNewUser())
		HelpFeqLayer::showHelpFeq(HelpFeqLayer::HelpFeqType_SignIn, (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonGetReward"));

	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(SignInLayer::onMessageEvent), MsgEventName_ToSignInLayer, nullptr);
}

void SignInLayer::onExit()
{
	Layer::onExit();

	// 移除观察者消息
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void SignInLayer::setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback)
{
	do
	{
		CC_BREAK_IF(node == nullptr);

		node->setTag(tag);

		if (callback)
			node->addTouchEventListener(callback);
		else
			node->addTouchEventListener(CC_CALLBACK_2(SignInLayer::callFuncButtonEvent, this));

	} while (0);
}

void SignInLayer::callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType)) return;

	Button* btn = (Button*)ref;
	int nTag = btn->getTag();
	
	switch (nTag)
	{

	case Tag_Close:
		GameInterfaceMgr::getInstance()->closeCurInterface();
		break;

	case Tag_GetReward:
	{
		btn->setTouchEnabled(false);

		addToRewardTips();
// 		int signInday = NewDataMgr::getInstance()->getSignInDay();
// 		auto node = GameInterfaceMgr::getInstance()->getParentNode();
// 		auto showGoodsInfoLayer = ShowGoodsLayer::getShowGoodsInfoLayer();
// 		showGoodsInfoLayer->setShowInfoType(ShowGoodsLayer::showInfoType::InfoType_SignIn, signInday + 1);
// 		node->addChild(showGoodsInfoLayer, LocalZOrde_1000);

		// 设置今日已领取
		if (NewDataMgr::getInstance()->getGuideNewUser())
			CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_HelpFeqPlay);

		GameInterfaceMgr::getInstance()->closeCurInterface();
	}
	default:
		break;
	}
}

void SignInLayer::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	switch (protocolID)
	{
	default:
		break;
	}
}

void SignInLayer::initInterface()
{
	_myLayer = CSLoader::createNode(Ccs_SignInLayer_FilePath);

	if (!_myLayer)
	{
		return;
	}

	addChild(_myLayer);

	do
	{
		// 关闭
		auto ButtonClose = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonClose");
		setEventForNode(ButtonClose, Tag_Close, nullptr);

		// 领取奖励
		auto ButtonGetReward = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonGetReward");
		setEventForNode(ButtonGetReward, Tag_GetReward, nullptr);

		for (int i = 1; i <= 6; i++)
		{
			int signInDay = NewDataMgr::getInstance()->getSignInDay();
			auto BtnDay = (Button*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("BtnDay_%d", i));
			auto SpriteGet = (Button*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("SpriteGet%d", i));

			if ( i <= signInDay )
			{
				BtnDay->setBright(false);
				SpriteGet->setVisible(true);
			}
			else
			{
				SpriteGet->setVisible(false);
			}
		}
	} while (0);
}

void SignInLayer::addToRewardTips(void)
{
	auto parentNode = Director::getInstance()->getRunningScene();

	int signInday = NewDataMgr::getInstance()->getSignInDay();
	auto goodsInfo = NewDataMgr::getInstance()->getSignInRewardInfo(signInday+1);

	if (goodsInfo->gold)	ItemRewardTips::getInstance(parentNode)->addItemTips(GoodsID_Gold, (void*)goodsInfo->gold);
	if (goodsInfo->dazhao)	ItemRewardTips::getInstance(parentNode)->addItemTips(GoodsID_DaZhao, (void*)goodsInfo->dazhao);
	if (goodsInfo->hudun)	ItemRewardTips::getInstance(parentNode)->addItemTips(GoodsID_HuDun, (void*)goodsInfo->hudun);
	if (goodsInfo->life)	ItemRewardTips::getInstance(parentNode)->addItemTips(GoodsID_Life, (void*)goodsInfo->life);

	NewDataMgr::getInstance()->getSignInReward(signInday);

}


/*******************************************
*	商店
*******************************************/
void ShopBuyLayer::showLayout(Node* node, int tag)
{
	if (node && !node->getChildByName("ShopBuyLayer"))
	{
		auto layout = ShopBuyLayer::create();
		layout->setTag(tag);
		node->addChild(layout, LocalZOrde_500, "ShopBuyLayer");
	}
}

void ShopBuyLayer::onEnter()
{
	Layer::onEnter();

	initInterface();
	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(ShopBuyLayer::onMessageEvent), MsgEventName_ToShopLayer, nullptr);
}

void ShopBuyLayer::onExit()
{
	Layer::onExit();
	NotificationCenter::getInstance()->removeAllObservers(this);
}
void ShopBuyLayer::setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback)
{
	do
	{
		CC_BREAK_IF(node == nullptr);

		node->setTag(tag);
		if (callback)
			node->addTouchEventListener(callback);
		else
			node->addTouchEventListener(CC_CALLBACK_2(ShopBuyLayer::callFuncButtonEvent, this));

	} while (0);
}

void ShopBuyLayer::callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType)) return;

	Button* btn = (Button*)ref;
	auto nTag = (ButtonTag)btn->getTag();

	switch (nTag)
	{
	case ShopBuyLayer::Tag_Back:
		GameInterfaceMgr::getInstance()->closeCurInterface();
		break;

	case ShopBuyLayer::Tag_OverflowPack:
	case ShopBuyLayer::Tag_TyrantPack:
	case ShopBuyLayer::Tag_BuyPropPack:
		showGoodsInfo(nTag);
		break;

		// 		NewDataMgr::getInstance()->mGameBuySystem->buyGoodsWithKey(BuyKey_BuyDaZhao);
		// 		NewDataMgr::getInstance()->mGameBuySystem->buyGoodsWithKey(BuyKey_OverflowPack);
		// 		NewDataMgr::getInstance()->mGameBuySystem->buyGoodsWithKey(BuyKey_TyrantPack);
		// 		NewDataMgr::getInstance()->mGameBuySystem->buyGoodsWithKey(BuyKey_BuyGold);
		// 		NewDataMgr::getInstance()->mGameBuySystem->buyGoodsWithKey(BuyKey_BuyShield);
		// 		break;
	default:
		break;
	}

}

void ShopBuyLayer::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	switch (protocolID)
	{
	case EProtocolID_BuyGoodsSucc:
	{
		if (_showGoodsInfoLayer)
		{
			_showGoodsInfoLayer->removeFromParentAndCleanup(true);
			_showGoodsInfoLayer = nullptr;
		}
	}
	case EProtocolID_TyrantPackBuy:
	{
		auto state = NewDataMgr::getInstance()->getTyrantPackBuyState();
		if (!state)
		{
			NewDataMgr::getInstance()->setTyrantPackBuyState(true);
		}
	}
		break;
	default:
		break;
	}
}

void ShopBuyLayer::initInterface()
{
	_widgetLayer = CSLoader::createNode(Ccs_ShopLayer_FilePath);

	if (!_widgetLayer)
	{
		return;
	}

	addChild(_widgetLayer);

	do
	{
		//返回
		auto ButtonBack = (Button*)GameMainLayer::seekNodeByName(_widgetLayer, "ButtonBack");
		setEventForNode(ButtonBack, Tag_Back, nullptr);

		// 超值礼包
		auto ButtonOverflowPack = (Button*)GameMainLayer::seekNodeByName(_widgetLayer, "ButtonOverflowPack");
		setEventForNode(ButtonOverflowPack, Tag_OverflowPack, nullptr);

		// 土豪礼包
		auto ButtonTyrantPack = (Button*)GameMainLayer::seekNodeByName(_widgetLayer, "ButtonZZPack");
		setEventForNode(ButtonTyrantPack, Tag_TyrantPack, nullptr);

		// 道具礼包
		auto ButtonPropPack = (Button*)GameMainLayer::seekNodeByName(_widgetLayer, "ButtonPropPack");
		setEventForNode(ButtonPropPack, Tag_BuyPropPack, nullptr);

		

	} while (0);
}

void ShopBuyLayer::showGoodsInfo(int tag)
{
	_showGoodsInfoLayer = PackShowLayer::getPackShowLayer();

	switch (tag)
	{
	case ShopBuyLayer::Tag_OverflowPack:
		_showGoodsInfoLayer->setShowInfoType(PackShowLayer::InfoType_OverflowPack);
		break;
	case ShopBuyLayer::Tag_TyrantPack:
		_showGoodsInfoLayer->setShowInfoType(PackShowLayer::InfoType_TyrantPack);
		break;
	case ShopBuyLayer::Tag_BuyPropPack:
		_showGoodsInfoLayer->setShowInfoType(PackShowLayer::InfoType_PropPack);
		break;
	
	default:
		break;
	}

	auto node = GameInterfaceMgr::getInstance()->getParentNode();
	node->addChild(_showGoodsInfoLayer, LocalZOrde_1000);
}



/*******************************************
*	关卡选择
*******************************************/
void LevelOptionLayer::showLayout(Node* node, int tag)
{
	if (node && !node->getChildByName("LevelOptionLayer"))
	{
		auto layout = LevelOptionLayer::create();
		layout->setTag(tag);
		node->addChild(layout, LocalZOrde_500, "LevelOptionLayer");
	}
}

void LevelOptionLayer::onEnter()
{
	Layer::onEnter();

	initInterface();

	if (NewDataMgr::getInstance()->getGuideNewUser())
	{
		HelpFeqLayer::showHelpFeq(HelpFeqLayer::HelpFeqType_ChuJi2, (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonStart"));
	}

	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(LevelOptionLayer::onMessageEvent), MsgEventName_ToLevelOptionLayer, nullptr);
}

void LevelOptionLayer::onExit()
{
	Layer::onExit();

	// 移除观察者消息
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void LevelOptionLayer::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	switch (protocolID)
	{
	case EProtocolID_TyrantPackBuy:
		NewDataMgr::getInstance()->setTyrantPackBuyState(true);
		break;

	default:
		break;
	}
}

void LevelOptionLayer::setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback)
{
	do
	{
		CC_BREAK_IF(node == nullptr);

		node->setTag(tag);

		if (callback)
			node->addTouchEventListener(callback);
		else
			node->addTouchEventListener(CC_CALLBACK_2(LevelOptionLayer::callFuncButtonEvent, this));

	} while (0);
}

void LevelOptionLayer::callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType, SoundType_None)) return;

	Button* btn = (Button*)ref;
	int nTag = btn->getTag();

	switch (nTag)
	{
	case Tag_Back:
		setSoundType(CommonLayer::SoundType_Back);
		GameInterfaceMgr::getInstance()->closeCurInterface();
		break;

	case Tag_Start:
		NewDataMgr::getInstance()->setGuideNewUser(false);
		setSoundType(CommonLayer::SoundType_GameStart);
		GameSceneMgr::getInstance()->changeScene(Scene_Type_PlayInterface);
		break;

	case Tag_NewPack:
	case Tag_ThPack:
		setSoundType(CommonLayer::SoundType_Menu);
		showPackInfoWithTag(nTag);
		break;

	case Tag_RightPage:
		{
			auto PageViewLevel = (PageView*)GameMainLayer::seekNodeByName(_myLayer, "PageViewLevel");
			int curPageIndex = PageViewLevel->getCurPageIndex();

			if (0 < curPageIndex)
			{
				PageViewLevel->scrollToPage(curPageIndex - 1);
			}
			
		}
		break;
	case Tag_LeftPage:
		{
			auto PageViewLevel = (PageView*)GameMainLayer::seekNodeByName(_myLayer, "PageViewLevel");
			int curPageIndex = PageViewLevel->getCurPageIndex();

			if (1 > curPageIndex)
			{
				PageViewLevel->scrollToPage(curPageIndex + 1);
			}
		
		}
		break;
	default:
		break;
	}
}

void LevelOptionLayer::callFuncLevelButtonEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType, soundType::SoundType_ButtonStart)) return;

	Button* btn = (Button*)ref;
	int level = btn->getTag();

	setOptionLevelBut(level);
	
}

void LevelOptionLayer::callFuncPageviewRole(Ref* ref, PageView::EventType eventType)
{
	updateScrollPageButton();
}

void LevelOptionLayer::initInterface()
{
	_myLayer = CSLoader::createNode(Ccs_LevelOptionLayer_FilePath);

	if (!_myLayer)
	{
		return;
	}

	addChild(_myLayer);

	do
	{
		// 关闭
		auto ButtonClose = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonBack");
		setEventForNode(ButtonClose, Tag_Back, nullptr);
	
		auto ButtonStart = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonStart");
		setEventForNode(ButtonStart, Tag_Start, nullptr);
		// 新手礼包
		auto ButtonNewPack = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonNewPack");
		setEventForNode(ButtonNewPack, Tag_NewPack, nullptr);
		// 土豪礼包
		auto ButtonThPack = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonThPack");
		setEventForNode(ButtonThPack, Tag_ThPack, nullptr);
		// 左翻页
		auto ButtonLeftPage = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonLeftPage");
		setEventForNode(ButtonLeftPage, Tag_RightPage, nullptr);
		// 右翻页
		auto ButtonRightPage = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonRightPage");
		setEventForNode(ButtonRightPage, Tag_LeftPage, nullptr);

		// 关卡按钮
		int curLevel = NewDataMgr::getInstance()->getCurMaxLevel();
		for (int i = 1; i <= 8; i++)
		{
			auto AtlasLabelLv = (Node*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("AtlasLabelLv%d", i));
			AtlasLabelLv->setZOrder(200);

			auto ButtonLevel = (Button*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ButtonLv%d", i));
			setEventForNode(ButtonLevel, i, CC_CALLBACK_2(LevelOptionLayer::callFuncLevelButtonEvent, this));
			ButtonLevel->setZOrder(100);
			if (i > curLevel)
			{
				// 未开启关卡
				ButtonLevel->setColor(Color3B(74, 74, 74));
				ButtonLevel->setTouchEnabled(false);
			}
			else if (i == curLevel)
			{ 
				ButtonLevel->setBright(false);
			}

			int lvStarGrade = NewDataMgr::getInstance()->getLevelStarGrade(i);
			for (int j = 1; j <= 3; j++)
			{
				auto ImgLvStar = (Button*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ImgStarLv%d_%d", i, j));
				ImgLvStar->setZOrder(151);
				ImgLvStar->setBright((j <= lvStarGrade) ? true : false);
				ImgLvStar->setVisible((i > curLevel) ? false : true);
			}
		}

		// 设置默认关卡
		int maxLevel = NewDataMgr::getInstance()->getCurMaxLevel();
		setOptionLevelBut(maxLevel);

		// 添加翻页触摸
		auto PageViewLevel = (PageView*)GameMainLayer::seekNodeByName(_myLayer, "PageViewLevel");
		PageViewLevel->addEventListener(CC_CALLBACK_2(LevelOptionLayer::callFuncPageviewRole, this));
		PageViewLevel->runAction(Sequence::create(
			DelayTime::create(0.3f),
			CallFuncN::create(CC_CALLBACK_1(LevelOptionLayer::callFuncScrollTo, this)),
			nullptr));

		updateScrollPageButton();
	} while (0);

}

void LevelOptionLayer::callFuncScrollTo(Node* node)
{
	int curLevel = NewDataMgr::getInstance()->getCurMaxLevel();
	auto PageViewLevel = (PageView*)node;
	PageViewLevel->scrollToPage(((curLevel - 1) / 4));

}

void LevelOptionLayer::showPackInfoWithTag(int tag)
{
	auto showGoodsInfoLayer = PackShowLayer::getPackShowLayer();

	switch (tag)
	{
	case Tag_NewPack:
		showGoodsInfoLayer->setShowInfoType(PackShowLayer::InfoType_OverflowPack);
		break;
	case Tag_ThPack:
		showGoodsInfoLayer->setShowInfoType(PackShowLayer::InfoType_TyrantPack);
		break;

	default:
		break;
	}

	auto node = GameInterfaceMgr::getInstance()->getParentNode();
	node->addChild(showGoodsInfoLayer, LocalZOrde_1000);
}

void LevelOptionLayer::updateScrollPageButton(void)
{
	auto pageView = (PageView*)GameMainLayer::seekNodeByName(_myLayer, "PageViewLevel");
	int pageIndex = pageView->getCurPageIndex();

	auto ButtonLeftPage = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonLeftPage");
	auto ButtonRightPage = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonRightPage");
	if (0 == pageIndex)
	{
		ButtonLeftPage->setTouchEnabled(false);
		ButtonLeftPage->setVisible(false);
		ButtonRightPage->setTouchEnabled(true);
		ButtonRightPage->setVisible(true);
	}
	else if (1 == pageIndex)
	{
		ButtonRightPage->setTouchEnabled(false);
		ButtonRightPage->setVisible(false);
		ButtonLeftPage->setTouchEnabled(true);
		ButtonLeftPage->setVisible(true);
	}
	else
	{
		ButtonRightPage->setTouchEnabled(true);
		ButtonRightPage->setVisible(true);
		ButtonLeftPage->setTouchEnabled(true);
		ButtonLeftPage->setVisible(true);
	}
}

void LevelOptionLayer::setOptionLevelBut(int level)
{
	level = (8 <= level) ? 8 : level;

	NewDataMgr::getInstance()->setCurLevel(level);




	auto btnBase = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ImgBase%d", level));
	ParticleSystemQuad *parcile1 = (ParticleSystemQuad*)GameMainLayer::seekNodeByName(_myLayer, "optionEff");
	Sprite *animoNode = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "animoNode");


	auto levelOptionState = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "levelOptionState");

	auto ButtonLevel = (Button*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ButtonLv%d", level));

	if (btnBase)
	{
		if (levelOptionState)
		{
			levelOptionState->removeFromParentAndCleanup(true);
		}

		levelOptionState = Sprite::create(getLevelStateRes(level));
		levelOptionState->setPosition(ButtonLevel->getPosition());
		levelOptionState->setName("levelOptionState");
		ButtonLevel->getParent()->addChild(levelOptionState);
// 		levelOptionState->runAction(CCRepeatForever::create(Sequence::create(
// 			CCFadeOut::create(0.8),
// 			CCFadeIn::create(0.8),
// 			nullptr)));
		levelOptionState->runAction(CCRepeatForever::create(Sequence::create(
			CCScaleTo::create(0.8,0.9),
			CCScaleTo::create(0.8, 1.0),
			nullptr)));

		if (parcile1)
		{
			parcile1->removeFromParentAndCleanup(true);
		}

		if (animoNode)
		{
			animoNode->removeFromParentAndCleanup(true);
		}

		parcile1 = ParticleSystemQuad::create("particla/LevelOption/test1.plist");
		parcile1->setName("optionEff");
		parcile1->setBlendAdditive(true);
		parcile1->setScale(5.0f);
		parcile1->setPositionType(ParticleSystem::PositionType::RELATIVE);
		parcile1->setPosition(Vec2(btnBase->getContentSize().width * 0.5, btnBase->getContentSize().height *0.35));
		btnBase->addChild(parcile1);


		Vector<SpriteFrame*> frame_array;

		for (int i = 1; i <= 26; ++i)
		{
			std::string pic = CCString::createWithFormat("%s%d.png", "ImgStarLv", i)->getCString();
			CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pic.c_str());

			frame_array.pushBack(frame);
		}

		auto p_animation = Animation::createWithSpriteFrames(frame_array, 0.1f);
		p_animation->setLoops(-1);

		auto animate = Animate::create(p_animation);
		auto AnimNode = Sprite::create("weapon.png");
		AnimNode->setPosition( Vec2(-65, -7));
		AnimNode->setPosition(Vec2(btnBase->getContentSize().width * 0.5, btnBase->getContentSize().height));
		btnBase->addChild(AnimNode, 8888, "animoNode");

		AnimNode->runAction(animate);
	}

	// 更改Boss属性
	auto bossItem = NewDataMgr::getInstance()->getBossProConfigWithId(level);

	auto bossName = (Text*)GameMainLayer::seekNodeByName(_myLayer, "TextBossName");
	bossName->setString(bossItem->bossName);

	for (int i = 1; i <= 5; i++)
	{
		// 生命
		auto lifeStar = (Text*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ImgStar1_%d", i));

		if ( i <= bossItem->lifeLevel )
		{
			lifeStar->setVisible(true);
			lifeStar->setBright(true);
		}
		else if ( i-0.5 == bossItem->lifeLevel)
		{
			lifeStar->setVisible(true);
			lifeStar->setBright(false);
		}
		else
		{
			lifeStar->setVisible(false);
		}

		// 攻击
		auto atkStar = (Text*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ImgStar2_%d", i));

		if (i <= bossItem->atkLevel)
		{
			atkStar->setVisible(true);
			atkStar->setBright(true);
		}
		else if (i - 0.5 == bossItem->atkLevel)
		{
			atkStar->setVisible(true);
			atkStar->setBright(false);
		}
		else
		{
			atkStar->setVisible(false);
		}

		// 速度
		auto speedStar = (Text*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ImgStar3_%d", i));

		if (i <= bossItem->speedLevel)
		{
			speedStar->setVisible(true);
			speedStar->setBright(true);
		}
		else if (i - 0.5 == bossItem->speedLevel)
		{
			speedStar->setVisible(true);
			speedStar->setBright(false);
		}
		else
		{
			speedStar->setVisible(false);
		}
	}

	updateAnimWithLevel(level);
}

void LevelOptionLayer::updateAnimWithLevel(int level)
{
	// 添加boss动画
	auto nodeBoss = (Node*)GameMainLayer::seekNodeByName(_myLayer, "NodeaBossAnim");

	nodeBoss->removeAllChildrenWithCleanup(true);

	Armature *bossAnim = nullptr;
	switch (level)
	{
		// 机抢女
	case 1:
	case 5:
	case 9:
	case 13:
	{
		bossAnim = Armature::create("kaijia");
		bossAnim->getAnimation()->playByIndex(0);
	}
		break;

	case 2:
	case 6:
	case 10:
	case 14:
	{
		bossAnim = Armature::create("boss-siyan");
		bossAnim->getAnimation()->playByIndex(2);
		bossAnim->setPosition(Vec2(0, 60));
		bossAnim->setScale(0.8);
	}
		break;

	case 3:
	case 7:
	case 11:
	case 15:
	{
		bossAnim = Armature::create("BoneWarrior");
		bossAnim->getAnimation()->playByIndex(0);
		bossAnim->setPosition(Vec2(0, 80));
		bossAnim->setScale(0.8);
	}
		break;

	case 4:
	case 8:
	case 12:
	case 16:
	{
		bossAnim = Armature::create("boss-yizhi");
		bossAnim->getAnimation()->playByIndex(7);
		bossAnim->setPosition(Vec2(0, 60));
		bossAnim->setScale(0.9);
	}
		break;

	default:
		break;
	}

	nodeBoss->addChild(bossAnim);
}

std::string LevelOptionLayer::getLevelStateRes(int lv)
{
	switch (lv)
	{
	case 1:
	case 2:
	case 3:
	case 4:
		return "roleImg/imgLevelFlash1.png";

	case 5:
	case 6:
	case 7:
	case 8:
		return "roleImg/imgLevelFlash2.png";

	case 9:
	case 10:
	case 11:
	case 12:
		return "roleImg/imgLevelFlash3.png";

	case 13:
	case 14:
	case 15:
	case 16:
		return "roleImg/imgLevelFlash4.png";

	default:
		break;
	}
}

/*******************************************
*	VIP
*******************************************/
void VipLayer::showLayout(Node* node, int tag)
{
	if (node && !node->getChildByName("VipLayer"))
	{
		auto layout = VipLayer::create();
		layout->setTag(tag);
		node->addChild(layout, LocalZOrde_1000, "VipLayer");
	}
}

void VipLayer::onEnter()
{
	Layer::onEnter();

	initInterface();
	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(VipLayer::onMessageEvent), MsgEventName_ToVIPLayer, nullptr);
}

void VipLayer::onExit()
{
	Layer::onExit();

	// 绉婚櫎瑙傚療鑰呮秷鎭�
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void VipLayer::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	switch (protocolID)
	{
	case EProtocolID_VipBuyCallFunc:
		removeFromParentAndCleanup(true);
		break;
	default:
		break;
	}
}

void VipLayer::setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback)
{
	do
	{
		CC_BREAK_IF(node == nullptr);

		node->setTag(tag);

		if (callback)
			node->addTouchEventListener(callback);
		else
			node->addTouchEventListener(CC_CALLBACK_2(VipLayer::callFuncButtonEvent, this));

	} while (0);
}

void VipLayer::callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType)) return;

	Button* btn = (Button*)ref;
	int nTag = btn->getTag();

	switch (nTag)
	{
	case Tag_Close:
		GameInterfaceMgr::getInstance()->closeCurInterface();
		break;
		
	case Tag_Buy:
		NewDataMgr::getInstance()->mGameBuySystem->buyGoodsWithKey(BuyKey_BuyVIP);
		break;
	default:
		break;
	}

}

void VipLayer::initInterface()
{
	_myLayer = CSLoader::createNode(Ccs_VIPLayer_FilePath);

	if (!_myLayer)
	{
		return;
	}

	addChild(_myLayer);

	do
	{
		// 鍏抽棴
		auto ButtonClose = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonClose");
		setEventForNode(ButtonClose, Tag_Close, nullptr);

		auto ButtonBuy = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonBuy");
		setEventForNode(ButtonBuy, Tag_Buy, nullptr);
		

	} while (0);
}


/*******************************************
*	VIP奖励
*******************************************/
void VipPresentingLayer::showLayout(Node* node, int tag)
{
	if (node && !node->getChildByName("VipPresentingLayer"))
	{
		auto layout = VipPresentingLayer::create();
		layout->setTag(tag);
		node->addChild(layout, LocalZOrde_1000, "VipPresentingLayer");
	}
}

void VipPresentingLayer::onEnter()
{
	Layer::onEnter();

	initInterface();
	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(VipPresentingLayer::onMessageEvent), MsgEventName_ToVIPLayer, nullptr);
}

void VipPresentingLayer::onExit()
{
	Layer::onExit();

	// 绉婚櫎瑙傚療鑰呮秷鎭�
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void VipPresentingLayer::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	switch (protocolID)
	{
	case EProtocolID_VipRewardGet:
		removeFromParentAndCleanup(true);
		break;

	default:
		break;
	}
}

void VipPresentingLayer::setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback)
{
	do
	{
		CC_BREAK_IF(node == nullptr);

		node->setTag(tag);

		if (callback)
			node->addTouchEventListener(callback);
		else
			node->addTouchEventListener(CC_CALLBACK_2(VipPresentingLayer::callFuncButtonEvent, this));

	} while (0);
}

void VipPresentingLayer::callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType)) return;

	Button* btn = (Button*)ref;
	int nTag = btn->getTag();

	switch (nTag)
	{
	case Tag_Close:
		GameInterfaceMgr::getInstance()->closeCurInterface();
		break;

	case Tag_Get:
		NewDataMgr::getInstance()->mGameBuySystem->buyGoodsWithKey(BuyKey_BuyVIPPre);
		break;

	default:
		break;
	}

}

void VipPresentingLayer::initInterface()
{
	_myLayer = CSLoader::createNode(Ccs_VipPreLayer_FilePath);

	if (!_myLayer)
	{
		return;
	}

	addChild(_myLayer);

	do
	{
		// 鍏抽棴
		auto ButtonClose = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonClose");
		setEventForNode(ButtonClose, Tag_Close, nullptr);

		auto ButtonGet = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonGet");
		setEventForNode(ButtonGet, Tag_Get, nullptr);

		int vipDay = NewDataMgr::getInstance()->getVipDay();
		auto TextVIPDay = (Text*)GameMainLayer::seekNodeByName(_myLayer, "TextVIPDay");
		TextVIPDay->setString(StringUtils::format("%d", vipDay));

	} while (0);
}


/*******************************************
*	卡牌奖励
*******************************************/
void RewardCardLayer::showLayout(Node* node, int tag)
{
	if (node && !node->getChildByName("RewardCardLayer"))
	{
		auto layout = RewardCardLayer::create();
		layout->setTag(tag);
		node->addChild(layout, LocalZOrde_1000, "RewardCardLayer");
	}
}

void RewardCardLayer::onEnter()
{
	Layer::onEnter();

	initInterface();
}

void RewardCardLayer::onExit()
{
	Layer::onExit();
}

void RewardCardLayer::initInterface()
{
	_myLayer = CSLoader::createNode(Ccs_RewardCard_FilePath);

	if (!_myLayer)
	{
		return;
	}

	addChild(_myLayer);

	do
	{
		auto ButtonCard = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonCard");

		ButtonCard->setScale(0.3f);
		ButtonCard->runAction(CCSpawn::create(
			CCScaleTo::create(1.0f, 1.0f),
			CCSequence::create(
				CCOrbitCamera::create(0.1, 1, 0, 0, 90, 0, 0),
				CCCallFuncN::create(CC_CALLBACK_1(RewardCardLayer::callFuncReverse, this)),
				CCOrbitCamera::create(0.2, 1, 0, -90, -90, 0, 0),
				//CCCallFuncN::create(CC_CALLBACK_1(RewardCardLayer::callFuncFront, this)),
				CCOrbitCamera::create(0.3, 1, 0, 0, 90, 0, 0),
				CCCallFuncN::create(CC_CALLBACK_1(RewardCardLayer::callFuncFront, this)),
				CCOrbitCamera::create(0.4, 1, 0, -90, -90, 0, 0),
				//CCCallFuncN::create(CC_CALLBACK_1(RewardCardLayer::callFuncFront, this)),
				CCOrbitCamera::create(0.6, 1, 0, 0, 90, 0, 0),
				CCCallFuncN::create(CC_CALLBACK_1(RewardCardLayer::callFuncReverse, this)),
				CCOrbitCamera::create(0.7, 1, 0, -90, -90, 0, 0),
				//CCCallFuncN::create(CC_CALLBACK_1(RewardCardLayer::callFuncFront, this)),
				CCOrbitCamera::create(0.8, 1, 0, 0, 90, 0, 0),
				CCCallFuncN::create(CC_CALLBACK_1(RewardCardLayer::callFuncReverse, this)),
				CCOrbitCamera::create(0.8, 1, 0, -90, -90, 0, 0),
				//CCCallFuncN::create(CC_CALLBACK_1(RewardCardLayer::callFuncFront, this)),
				nullptr),
			nullptr));


	} while (0);
}

void RewardCardLayer::callFuncFront(Node* node)
{
	auto PanelFront = (Node*)GameMainLayer::seekNodeByName(_myLayer, "PanelFront");
	PanelFront->setVisible(false);

	auto ButtonCard = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonCard");
	ButtonCard->setBright(false);
	//ButtonCard->setFlipX(true);

}

void RewardCardLayer::callFuncReverse(Node* node)
{
	auto PanelFront = (Node*)GameMainLayer::seekNodeByName(_myLayer, "PanelFront");
	PanelFront->setVisible(true);

	auto ButtonCard = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonCard");
	ButtonCard->setBright(true);
	ButtonCard->setFlipX(true);
}



TaskListView* TaskListView::create(ListType type)
{
	TaskListView *pRet = new(std::nothrow) TaskListView(); 
	if (pRet)
	{
		pRet->setMyListType(type);
		pRet->init();
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

void TaskListView::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	switch (protocolID)
	{
	case EProtocolID_GetTaskReward:
	{

	}
		break;

	default:
		break;
	}
}

bool TaskListView::init(void)
{
	if (!Layer::init())
	{
		return false;
	}

	initTaskList();

	TableView* tableView = TableView::create(this, Size(640, 850));
	tableView->setDirection(cocos2d::extension::ScrollView::Direction::VERTICAL);
	tableView->setDelegate(this);
	tableView->setPosition(Vec2(40, 200));
	tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
	this->addChild(tableView);
	tableView->reloadData();

	return true;
}

void TaskListView::initTaskList(void)
{
	// 获得任务表
	std::vector<int> tempTaskList;
	
	if (ListType_AchiTask == getMyListType())
	{
		tempTaskList = NewDataMgr::getInstance()->mGameTaskSystem->getAchiTaskList();
	}
	else
	{
		tempTaskList = NewDataMgr::getInstance()->mGameTaskSystem->getDayTaskList();
	}

	std::vector<int>::iterator item = tempTaskList.begin();
	int itemNum = tempTaskList.size();

	std::list<csvGameTaskConfig_s*> tempList;
	for (int i = 0; i < itemNum; i++, item++)
	{
		auto taskItem = NewDataMgr::getInstance()->mGameTaskSystem->getGameTaskListWithID((*item));
		tempList.push_back(taskItem);
	}
	tempList.sort((Cmpare()));

	auto listItme = tempList.begin();
	for (int i = 0; listItme != tempList.end(); listItme++, i++)
	{
		int taskid = (*listItme)->TaskID;
		_taskMap.insert(pair<int, int >(i, taskid));
	}
}

void TaskListView::scrollViewDidZoom(cocos2d::extension::ScrollView* view)
{

}

void TaskListView::tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell)
{

}

cocos2d::Size TaskListView::tableCellSizeForIndex(cocos2d::extension::TableView *table, ssize_t idx)
{
	return Size(600, 130);
}

cocos2d::extension::TableViewCell* TaskListView::tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx)
{
	int taskId = 0;

// 	if (ListType_AchiTask == getMyListType())
// 	{
// 		taskId = GameTaskSystem::DayTaskTag_Task10 + idx;
// 	}
// 	else
// 	{
// 		taskId = GameTaskSystem::DayTaskTag_Task1 + idx;
// 	}

	map<int, int>::iterator	it = _taskMap.find(idx);
	taskId = it->second;

	auto taskItem = NewDataMgr::getInstance()->mGameTaskSystem->getGameTaskListWithID(taskId);

	TableViewCell *cell = table->dequeueCell();

	if (!cell)
	{

		cell = new (std::nothrow) TableViewCell();
		cell->setTag(taskId);
		cell->autorelease();
		cell->setTag(idx);

		auto itemNode = Button::create("TaskRes/ImgItemBg9.png");
		itemNode->setTouchEnabled(false);
		itemNode->setScale9Enabled(true);
		itemNode->setCapInsets(Rect(200, 50, 50, 50));
		itemNode->setContentSize(Size(660, 160));
		cell->addChild(itemNode);
		itemNode->setPosition(itemNode->getPosition() + Vec2(getContentSize().width / 2-40, 55));

		// 物品背景
		auto ImgGoodsBg = Sprite::create("TaskRes/ImgGoodsBg.png");
		ImgGoodsBg->setPosition(Vec2(90, 87) + Vec2(-5, -25));
		cell->addChild(ImgGoodsBg);

		// 物品
		auto ImgGoods = Sprite::create(getGoodsResWithID(taskItem->TaskRewardID));
		ImgGoods->setPosition(Vec2(90, 87) + Vec2(-5, -25));
		ImgGoods->setName("ImgGoods");
		ImgGoods->setTag(1101);
		cell->addChild(ImgGoods);

		// 奖励
		auto StrJL = Text::create();
		StrJL->setString("奖励");
		StrJL->setColor(Color3B(134, 23, 15));
		StrJL->setFontSize(27);
		StrJL->setPosition(Vec2(216, 105) + Vec2(-5, -25));
		cell->addChild(StrJL);

		// 奖励说明
		auto StrJLExplant = Text::create();
		StrJLExplant->setAnchorPoint(Vec2(0, 0.5));
		StrJLExplant->setString(taskItem->TaskRewardDes);
		StrJLExplant->setColor(Color3B(46, 36, 26));
		StrJLExplant->setFontSize(27);
		StrJLExplant->setPosition(Vec2(250, 105) + Vec2(-5, -25));
		StrJLExplant->setTag(1109);
		cell->addChild(StrJLExplant);

		// 任务说明
		auto StrTaskExplant = Text::create();
		StrTaskExplant->setAnchorPoint(Vec2(0.0, 1.0));
		StrTaskExplant->setPosition(Vec2(185, 85) + Vec2(-5, -25));
		StrTaskExplant->setTextHorizontalAlignment(TextHAlignment::LEFT);
		StrTaskExplant->setTextVerticalAlignment(TextVAlignment::CENTER);
		StrTaskExplant->setTextAreaSize(Size(320, 0));
		StrTaskExplant->setString(taskItem->TaskDes);
		StrTaskExplant->setColor(Color3B(46, 36, 26));
		StrTaskExplant->setFontSize(22);
		StrTaskExplant->setName("StrTaskExplant");
		StrTaskExplant->setTag(1103);
		StrTaskExplant->ignoreContentAdaptWithSize(false);
		cell->addChild(StrTaskExplant);

		// 领取按钮
		auto filePath = "TaskRes/BtnTaskGet.png";
		auto BtnGet = Button::create(filePath, filePath, filePath);
		BtnGet->setPosition(Vec2(565, 83) + Vec2(-15, -25));
		BtnGet->setUserData((void*)taskItem->TaskID);
		BtnGet->setTouchEnabled(false);
		BtnGet->setVisible(false);
		BtnGet->setTag(1104);
		BtnGet->addTouchEventListener(CC_CALLBACK_2(TaskListView::callFuncGetRewardEvent, this));
		cell->addChild(BtnGet);


		// 任务进度
		auto StrPar = Text::create();
		StrPar->setAnchorPoint(Vec2(0.5, 0.5));
		StrPar->setString("进度");
		StrPar->setColor(Color3B(134, 23, 15));
		StrPar->setFontSize(27);
		StrPar->setPosition(Vec2(530, 85) + Vec2(-15, -25));
		StrPar->setVisible(false);
		StrPar->setTag(1105);
		cell->addChild(StrPar);

		// 进度
		float progressNum = taskItem->TaskProgressNum;
		float targetNum = taskItem->TaskTargetNum;
		auto StrProportion = Text::create();
		StrProportion->setAnchorPoint(Vec2(0, 0.5));
		StrProportion->setString(StringUtils::format("%d%s", (int)((progressNum / targetNum) * 100), "%"));
		StrProportion->setColor(Color3B(134, 23, 15));
		StrProportion->setFontSize(27);
		StrProportion->setPosition(Vec2(570, 85) + Vec2(-15, -25));
		StrProportion->setVisible(false);
		StrProportion->setTag(1106);
		cell->addChild(StrProportion);

		// 已领取
		auto ImgFinish = Sprite::create("TaskRes/ImgFinish.png");
		ImgFinish->setPosition(Vec2(565, 83) + Vec2(-5, -25));
		ImgFinish->setVisible(false);
		ImgFinish->setTag(1107);
		cell->addChild(ImgFinish);

		// 已完成未领取
		if (2 == taskItem->TaskState)
		{
			BtnGet->setVisible(true);
			BtnGet->setTouchEnabled(true);
		}
		// 未完成
		else if (0 == taskItem->TaskState)
		{
			StrPar->setVisible(true);
			StrProportion->setVisible(true);
		}
		// 已领取
		else if (1 == taskItem->TaskState)
		{
			ImgFinish->setVisible(true);
		}

	}
	else
	{
		auto StrTaskExplant = (Text*)cell->getChildByTag(1103);
		StrTaskExplant->setString(taskItem->TaskDes);

		auto StrJLExplant = (Text*)cell->getChildByTag(1109);
		StrJLExplant->setString(taskItem->TaskRewardDes);

		auto ImgGoods = (Sprite*)cell->getChildByTag(1101);
 		ImgGoods->setTexture(getGoodsResWithID(taskItem->TaskRewardID));

		auto BtnGet = (Button*)cell->getChildByTag(1104);
		BtnGet->setTouchEnabled(false);
		BtnGet->setVisible(false);
		BtnGet->setUserData((void*)taskItem->TaskID);

		auto StrPar = (Text*)cell->getChildByTag(1105);
		StrPar->setVisible(false);

		float progressNum = taskItem->TaskProgressNum;
		float targetNum = taskItem->TaskTargetNum;
		auto StrProportion = (Text*)cell->getChildByTag(1106);
		StrProportion->setVisible(false);
		StrProportion->setString(StringUtils::format("%d%s", (int)((progressNum / targetNum) * 100), "%"));

		auto ImgFinish = (Sprite*)cell->getChildByTag(1107);
		ImgFinish->setVisible(false);


		// 已完成未领取
		if (2 == taskItem->TaskState)
		{
			BtnGet->setVisible(true);
			BtnGet->setTouchEnabled(true);
		}
		// 未完成
		else if (0 == taskItem->TaskState)
		{
			StrPar->setVisible(true);
			StrProportion->setVisible(true);
		}
		// 已领取
		else if (1 == taskItem->TaskState)
		{
			ImgFinish->setVisible(true);
		}
	}
	return cell;

}

ssize_t TaskListView::numberOfCellsInTableView(cocos2d::extension::TableView *table)
{
	int tableNumber = 0;
	auto listtype = getMyListType();
	if (ListType_AchiTask == listtype)
	{
		tableNumber = NewDataMgr::getInstance()->mGameTaskSystem->getAchiTaskList().size();
	}
	else
	{
		tableNumber = NewDataMgr::getInstance()->mGameTaskSystem->getDayTaskList().size();
	}
	return tableNumber;

}

std::string TaskListView::getGoodsResWithID(int id)
{
	std::string res;
	switch (id)
	{
	case GoodsID_Gold:
		res = "ccsRes/resShopLayer/ImgGold2.png";
		break;
	case GoodsID_Life:
		res = "ccsRes/resShopLayer/ImgLife1.png";
		break;
	case GoodsID_DaZhao:
		res = "ccsRes/resShopLayer/ImgDazhao.png";
		break;
	case GoodsID_HuDun:
		res = "ccsRes/resShopLayer/ImgHuDun2.png";
		break;
	default:
		break;
	}

	return res;

}

void TaskListView::callFuncGetRewardEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType, CommonLayer::SoundType_None)) return;

	NewAudio::getInstance()->playEffect(Music_RewardGoods);

	Button* btn = (Button*)ref;
	int nTag = btn->getTag();

	int _curGetTaskID = (intptr_t)btn->getUserData();

	auto cell = (TableViewCell *)btn->getParent();

	// 获取奖励
	NewDataMgr::getInstance()->mGameTaskSystem->getTaskRewardWithTaskId(_curGetTaskID);

	auto goodsInfo = NewDataMgr::getInstance()->mGameTaskSystem->getGameTaskListWithID(_curGetTaskID);
	ItemRewardTips::getInstance(nullptr)->addItemTips(goodsInfo->TaskRewardID, (void*)goodsInfo->RewardNum);

	updateTableItem(cell, _curGetTaskID);
}

void TaskListView::updateTableItem(TableViewCell *cell, int taskid)
{
	auto taskItem = NewDataMgr::getInstance()->mGameTaskSystem->getGameTaskListWithID(taskid);

	auto StrTaskExplant = (Text*)cell->getChildByTag(1103);
	StrTaskExplant->setString(taskItem->TaskDes);

	auto StrJLExplant = (Text*)cell->getChildByTag(1109);
	StrJLExplant->setString(taskItem->TaskRewardDes);

	auto ImgGoods = (Sprite*)cell->getChildByTag(1101);
	ImgGoods->setTexture(getGoodsResWithID(taskItem->TaskRewardID));

	auto BtnGet = (Button*)cell->getChildByTag(1104);
	BtnGet->setTouchEnabled(false);
	BtnGet->setVisible(false);
	BtnGet->setUserData((void*)taskItem->TaskID);

	auto StrPar = (Text*)cell->getChildByTag(1105);
	StrPar->setVisible(false);

	float progressNum = taskItem->TaskProgressNum;
	float targetNum = taskItem->TaskTargetNum;
	auto StrProportion = (Text*)cell->getChildByTag(1106);
	StrProportion->setVisible(false);
	StrProportion->setString(StringUtils::format("%d%s", (int)((progressNum / targetNum) * 100), "%"));

	auto ImgFinish = (Sprite*)cell->getChildByTag(1107);
	ImgFinish->setVisible(false);


	// 已完成未领取
	if (2 == taskItem->TaskState)
	{
		BtnGet->setVisible(true);
		BtnGet->setTouchEnabled(true);
	}
	// 未完成
	else if (0 == taskItem->TaskState)
	{
		StrPar->setVisible(true);
		StrProportion->setVisible(true);
	}
	// 已领取
	else if (1 == taskItem->TaskState)
	{
		ImgFinish->setVisible(true);
	}
}
