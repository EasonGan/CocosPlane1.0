#include "CommonWidget.h"
#include "GameMainLayer.h"
#include "../GameCommon/CommonRes.h"
#include "GameRoleScene.h"
#include "../GameCommon/GameDefine.h"
#include "../GameCommon/GameData.h"
#include "GameSceneMgr.h"
#include "../GameCommon/EffectsManager.h"
#include "../GamePlaying/Layer/UIController.h"

/*******************************************
*	widget公用方法
*******************************************/
bool CommonLayer::setButtonAction(Ref* ref, Widget::TouchEventType touchType, soundType sound )
{
	Button* btn = (Button*)ref;
	int nTag = btn->getTag();

	if (Widget::TouchEventType::BEGAN == touchType)
	{
		//btn->runAction(CCEaseSineIn::create(CCScaleTo::create(0.1f, 1.1f)));
		btn->setColor(Color3B(80, 80, 80));

		// 音效处理
		switch (sound)
		{
		case CommonLayer::SoundType_Back:
			NewAudio::getInstance()->playEffect(Music_ButtonBack);
			break;
		case CommonLayer::SoundType_ButtonStart:
			NewAudio::getInstance()->playEffect(Music_ButtonStart);
			break;
		case CommonLayer::SoundType_Menu:
			NewAudio::getInstance()->playEffect(Music_Menu);
			break;
		default:
			break;
		}
	}
	else if (Widget::TouchEventType::ENDED == touchType)
	{
		//btn->runAction(CCEaseSineOut::create(CCScaleTo::create(0.1f, 1.0f)));
		btn->setColor(Color3B(255, 255, 255));
		return true;
	}
	else if (Widget::TouchEventType::CANCELED == touchType)
	{
		//btn->runAction(CCEaseSineOut::create(CCScaleTo::create(0.1f, 1.0f)));
		btn->setColor(Color3B(255, 255, 255));
	}

	return false;
}

void CommonLayer::setSoundType(soundType sound /*= SoundType_None*/)
{
	// 音效处理
	switch (sound)
	{
	case CommonLayer::SoundType_Close:
		NewAudio::getInstance()->playEffect(Music_ButtonClose);
		break;
	case CommonLayer::SoundType_Back:
		NewAudio::getInstance()->playEffect(Music_ButtonBack);
		break;
	case CommonLayer::SoundType_ButtonStart:
		NewAudio::getInstance()->playEffect(Music_ButtonStart);
		break;
	case CommonLayer::SoundType_Menu:
		NewAudio::getInstance()->playEffect(Music_Menu);
		break;
	case CommonLayer::SoundType_GameStart:
		NewAudio::getInstance()->playEffect(Music_GameStart);
	default:
		break;
	}
}


/********************************************/
/* 公共小弹框   
/*******************************************/
bool CommonWidget::init()
{
	if ( !Layer::init() )
	{
		return false;
	}

	setName("CommonWidget");
	initInterface();

	return true;
}

void CommonWidget::onEnter()
{
	Layer::onEnter();

}

void CommonWidget::onExit( void )
{
	Layer::onExit();
}

void CommonWidget::initInterface( void )
{
	_commonWidget = CSLoader::createNode( Ccs_GameWidgetLayer_FilePath );

	if ( !_commonWidget )
	{
		return;
	}
	addChild( _commonWidget );

	do 
	{
		auto butExit = (Button*)GameMainLayer::seekNodeByName(_commonWidget,"ButtonClose");
		CC_BREAK_IF( butExit == nullptr );
		butExit->setTag( Tag_ExitWidget );
		butExit->addClickEventListener(CC_CALLBACK_1(CommonWidget::callFuncButtonEvent, this));

		auto butBuy = (Button*)GameMainLayer::seekNodeByName(_commonWidget,"ButtonConfirm");
		CC_BREAK_IF( butBuy == nullptr );
		butBuy->setTag( Tag_Btn1 );
		butBuy->addClickEventListener(CC_CALLBACK_1(CommonWidget::callFuncButtonEvent, this));

		auto textContene1			= (Text*)GameMainLayer::seekNodeByName(_commonWidget, "TextWidgetTips" );
		textContene1->setString( "" );

		auto textContene2			= (Text*)GameMainLayer::seekNodeByName(_commonWidget, "TextWidgetTips2" );
		textContene2->setString( "" );

	} while (0);

}

void CommonWidget::setCloseButtonState(bool state)
{
	auto butExit = (Button*)GameMainLayer::seekNodeByName(_commonWidget, "ButtonClose");
	if (butExit)
	{
		butExit->setVisible((state) ? true : false);
		butExit->setTouchEnabled((state) ? true : false);
	}

}

void CommonWidget::setContent(const  char* content1 /*= nullptr*/, const char* content2 /*= nullptr*/ )
{
	auto textContene1			= (Text*)GameMainLayer::seekNodeByName(_commonWidget, "TextWidgetTips" );
	if ( textContene1 && content1 )
	{
		textContene1->setString( content1 );
	}

	auto textContene2			= (Text*)GameMainLayer::seekNodeByName(_commonWidget, "TextWidgetTips2" );
	if ( textContene2 && content2 )
	{
		textContene2->setString( content2 );
	}
}

void CommonWidget::addBtn1ClickEventListener( const ccCommonWidgetBtnCallBack& callback )
{
	this->callFuncBtn1 = callback;
}

void CommonWidget::addBtn2ClickEventListener( const ccCommonWidgetBtnCallBack& callback )
{
	this->callFuncBtn2 = callback;
}

void CommonWidget::addBtnExitClickEventListener( const ccCommonWidgetBtnCallBack& callback )
{
	this->callFuncBtnExit = callback;
}

void CommonWidget::callFuncButtonEvent( Ref* ref )
{
	Button* btn = (Button*)ref;
	int nTag = btn->getTag();

	switch ( nTag )
	{
		// 退出
	case Tag_ExitWidget:
		{
			if ( !callFuncBtnExit )
			{
				removeFromParentAndCleanup(true);
			}
			else
			{
				callFuncBtnExit();
				removeFromParentAndCleanup(true);
			}
		}
		break;

	case Tag_Btn1:
		{
			if ( !callFuncBtn1 )
			{
				removeFromParentAndCleanup(true);
			}
			else
			{
				callFuncBtn1();
				removeFromParentAndCleanup(true);
			}
		}
		break;

	case Tag_Btn2:
		{
			if ( !callFuncBtn2 )
			{
				removeFromParentAndCleanup(true);
			}
			else
			{
				callFuncBtn2();
				removeFromParentAndCleanup(true);
			}
		}

	default:
		break;
	}


}




/********************************************/
/* 奖励节点
/*******************************************/
RewardGoodsNode* RewardGoodsNode::create( RewardGoodsKey key )
{
	RewardGoodsNode *node = new (std::nothrow) RewardGoodsNode();
	if (node && node->initWithKey( key ))
	{
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return nullptr;
}
RewardGoodsNode* RewardGoodsNode::clone() const
{
	RewardGoodsNode *node = new (std::nothrow) RewardGoodsNode();
	if ( node )
	{
		node->_nodeTag = this->_nodeTag;
		node->initWithKey( _goodeskey );
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return nullptr;
}

bool RewardGoodsNode::initWithKey( RewardGoodsKey key )
{
	if ( 0 == _nodeTag) setTag( -1 );
	_goodeskey = key;

	switch ( key )
	{
	case RewardKey_1:
		{
			int randCardIndex = random(1,9);

			if ( 0 != _nodeTag)	randCardIndex = _nodeTag;
			setTag( randCardIndex );

			std::string cardFilePath = __String::createWithFormat("%s%d.png", Img_Card_FilePath, randCardIndex )->getCString();
			auto goods = Sprite::create( cardFilePath );
			goods->setAnchorPoint( Vec2(0.5, 0.5) );
			goods->setScale( 0.40f );
			goods->setPosition( Vec2(0, 25 ));
			goods->setName( "goods" );
			goods->setCascadeOpacityEnabled(true);
			addChild(goods);

			auto goodsK = Sprite::create( StringUtils::format("ccsRes/cardOptionLayer/CardFrame%d.png", (randCardIndex-1)/3+1) );
			goodsK->setScale( 1.7f);
			goodsK->setPosition(goods->getContentSize() / 2);
			goodsK->setCascadeOpacityEnabled(true);
			goods->addChild(goodsK);

			for ( int i = 0; i<4; i++ )
			{
				auto goodsStar = Sprite::create( "ccsRes/CompoundCardLayer/hecheng14.png" );
				goodsStar->setPosition( Vec2( 309 - i*50, 408) );
				goodsStar->setCascadeOpacityEnabled(true);
				goods->addChild(goodsStar);
			}
		}
		break;

	case RewardKey_2:
		{
			int randCardIndex = random(1,9);
			if ( 0 != _nodeTag)	randCardIndex = _nodeTag;
			setTag( randCardIndex );

			std::string cardFilePath = __String::createWithFormat("%s%d.png", Img_Card_FilePath, randCardIndex )->getCString();
			auto goods = Sprite::create( cardFilePath );
			goods->setAnchorPoint( Vec2(0.5, 0.5) );
			goods->setScale( 0.40f );
			goods->setPosition( Vec2(0, 25 ));
			goods->setName( "goods" );
			goods->setCascadeOpacityEnabled(true);
			addChild(goods);

			auto goodsK = Sprite::create( StringUtils::format("ccsRes/cardOptionLayer/CardFrame%d.png", (randCardIndex-1)/3+1) );
			goodsK->setScale( 1.7f);
			goodsK->setPosition( goods->getContentSize()/2 );
			goodsK->setCascadeOpacityEnabled(true);
			goods->addChild(goodsK);

			for ( int i = 0; i<3; i++ )
			{
				auto goodsStar = Sprite::create( "ccsRes/CompoundCardLayer/hecheng14.png" );
				goodsStar->setPosition( Vec2( 309 - i*50, 408) );
				goodsStar->setCascadeOpacityEnabled(true);
				goods->addChild(goodsStar);
			}

		}
		break;

	case RewardKey_3:
	case RewardKey_24:
		{
			auto goods = Sprite::create( "ccsRes/roleOptionLayer/juese5.png" );
			goods->setName( "goods" );
			goods->setPosition( Vec2(0, 10) );
			goods->setScale( 1.3f );
			addChild( goods );
		}
		break;

	case RewardKey_4:
	case RewardKey_5:
	case RewardKey_14:
	case RewardKey_21:
	case RewardKey_22:
	case RewardKey_23:
		{
			auto goods = Sprite::create( "ccsRes/ShopLayer/goumai10.png" );
			goods->setPosition( Vec2(0, 5) );
			goods->setName( "goods" );
			addChild( goods );
		}
		break;

	case RewardKey_6:
		{
			auto goods = Sprite::create( "ccsRes/ShopLayer/goumai21.png" );
			goods->setPosition( Vec2(0, 15) );
			goods->setName( "goods" );
			addChild( goods );
		}
		break;

	case RewardKey_7:
	case RewardKey_15:
		{
			int randCardIndex = random(1,9);
			if ( 0 != _nodeTag)	randCardIndex = _nodeTag;
			setTag( randCardIndex );

			std::string cardFilePath = __String::createWithFormat("%s%d.png", Img_Card_FilePath, randCardIndex )->getCString();
			auto goods = Sprite::create( cardFilePath );
			goods->setAnchorPoint( Vec2(0.5, 0.5) );
			goods->setScale( 0.40f );
			goods->setPosition( Vec2(0, 25 ));
			goods->setName( "goods" );
			goods->setCascadeOpacityEnabled(true);
			addChild(goods);

			auto goodsK = Sprite::create( StringUtils::format("ccsRes/cardOptionLayer/CardFrame%d.png", (randCardIndex-1)/3+1) );
			goodsK->setScale( 1.7f);
			goodsK->setPosition( goods->getContentSize()/2 );
			goodsK->setCascadeOpacityEnabled(true);
			goods->addChild(goodsK);

			for ( int i = 0; i<1; i++ )
			{
				auto goodsStar = Sprite::create( "ccsRes/CompoundCardLayer/hecheng14.png" );
				goodsStar->setPosition( Vec2( 309 - i*50, 408) );
				goodsStar->setCascadeOpacityEnabled(true);
				goods->addChild(goodsStar);
			}

		}
		break;

	case RewardKey_8:
	case RewardKey_19:
	case RewardKey_20:
		{
			int randCardIndex = random(1,9);
			if ( 0 != _nodeTag)	randCardIndex = _nodeTag;
			setTag( randCardIndex );

			std::string cardFilePath = __String::createWithFormat("%s%d.png", Img_Card_FilePath, randCardIndex )->getCString();
			auto goods = Sprite::create( cardFilePath );
			goods->setAnchorPoint( Vec2(0.5, 0.5) );
			goods->setScale( 0.40f );
			goods->setPosition( Vec2(0, 25 ));
			goods->setName( "goods" );
			goods->setCascadeOpacityEnabled(true);
			addChild(goods);

			auto goodsK = Sprite::create( StringUtils::format("ccsRes/cardOptionLayer/CardFrame%d.png", (randCardIndex-1)/3+1) );
			goodsK->setScale( 1.7f);
			goodsK->setPosition( goods->getContentSize()/2 );
			goodsK->setCascadeOpacityEnabled(true);
			goods->addChild(goodsK);

			for ( int i = 0; i<2; i++ )
			{
				auto goodsStar = Sprite::create( "ccsRes/CompoundCardLayer/hecheng14.png" );
				goodsStar->setPosition( Vec2( 309 - i*50, 408) );
				goodsStar->setCascadeOpacityEnabled(true);
				goods->addChild(goodsStar);
			}
		}
		break;

	case RewardKey_9:
		{
			auto goods = Sprite::create( "ccsRes/roleOptionLayer/juese3.png" );
			goods->setPosition( Vec2(0, 10) );
			goods->setName( "goods" );
			goods->setScale( 1.3f );
			addChild( goods );
		}
		break;

	case RewardKey_10:
	case RewardKey_16:
	case RewardKey_17:
	case RewardKey_18:
		{
			auto goods = Sprite::create( "ccsRes/roleOptionLayer/juese4.png" );
			goods->setPosition( Vec2(0, 10) );
			goods->setScale( 1.3f );
			goods->setName( "goods" );
			addChild( goods );
		}
		break;

	case RewardKey_11:
		{
			auto goods = Sprite::create( "ccsRes/WujinModeLayer/goodschongci.png" );
			goods->setPosition( Vec2(0, 25) );
			goods->setName( "goods" );
			addChild( goods );
		}
		break;

	case RewardKey_12:
		{
			auto goods = Sprite::create( "ccsRes/WujinModeLayer/goodsHudun.png" );
			goods->setPosition( Vec2(0, 25) );
			goods->setName( "goods" );
			addChild( goods );
		}
		break;

	case RewardKey_13:
		{
			auto goods = Sprite::create( "ccsRes/WujinModeLayer/dazhao.png" );
			goods->setPosition( Vec2(0, 25) );
			goods->setName( "goods" );
			addChild( goods );
		}
		break;

	default:
		break;
	}

	_nodeTag = getTag();

	// 数量
// 	auto goodsNum = TextAtlas::create( __String::createWithFormat(".%d", NewDataMgr::getInstance()->getRewardGoodsListWithKey( key )->GoodsNum )->getCString(), 
// 		"ccsRes/AtlasLabel/jiesuan22.png", 20,28, ".");
// 	goodsNum->setPosition(Vec2(0, -60));
// 	addChild( goodsNum );

	return true;
}



/*******************************************
*	物品展示
*******************************************/
ShowGoodsLayer* ShowGoodsLayer::getShowGoodsInfoLayer(void)
{
	auto layout = ShowGoodsLayer::create();

	if (layout)
	{
		return layout;
	}
	return nullptr;
}

void ShowGoodsLayer::onEnter()
{
	Layer::onEnter();

	setName("ShowGoodsLayer");
	initInterface();
	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(ShowGoodsLayer::onMessageEvent), MsgEventName_ToGoodsInfoLayer, nullptr);
}

void ShowGoodsLayer::onExit()
{
	Layer::onExit();

	// 移除观察者消息
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void ShowGoodsLayer::setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback)
{
	do
	{
		CC_BREAK_IF(node == nullptr);

		node->setTag(tag);

		if (callback)
			node->addTouchEventListener(callback);
		else
			node->addTouchEventListener(CC_CALLBACK_2(ShowGoodsLayer::callFuncButtonEvent, this));

	} while (0);
}

void ShowGoodsLayer::callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType)) return;

	Button* btn = (Button*)ref;
	int nTag = btn->getTag();

	switch (nTag)
	{

	case Tag_Close:
	{
		if (Scene_Type_PlayInterface == GameSceneMgr::getInstance()->getCurScen())
		{
			PauseLayer* layer = PauseLayer::create();
			layer->setActivateType(PauseLayer::ActivateTag_None);
			Director::getInstance()->getRunningScene()->addChild(layer, 10000);
		}

		if (InfoType_SignIn == _curShowType)
		{
			if(NewDataMgr::getInstance()->getGuideNewUser())
				CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_HelpFeqPlay);
		}

		removeFromParentAndCleanup(true);



	}
		break;

	case Tag_Buy:
		NewDataMgr::getInstance()->mGameBuySystem->buyGoodsWithKey(_curBuyType);
		break;

	default:
		break;
	}
}

void ShowGoodsLayer::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	switch (protocolID)
	{
	case EProtocolID_RemoveGoodsInfo:
		removeFromParentAndCleanup(true);
		break;

	default:
		break;
	}
}

void ShowGoodsLayer::onTouchEnded(Touch *touch, Event *unusedEvent)
{
	if (InfoType_SignIn == _curShowType)
		removeFromParentAndCleanup(true);
}

void ShowGoodsLayer::initInterface()
{
	_myLayer = CSLoader::createNode(Ccs_ShowGoodsLayer_FilePath);

	if (!_myLayer)
	{
		return;
	}

	addChild(_myLayer);

	do
	{
		// 关闭
		auto ButtonBack = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonClose");
		setEventForNode(ButtonBack, Tag_Close, nullptr);

		// 购买
		auto ButtonBuy = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonBuy");
		setEventForNode(ButtonBuy, Tag_Buy, nullptr);

		auto PanelTouch = (Node*)GameMainLayer::seekNodeByName(_myLayer, "PanelTouch");

		// 添加触摸处理
		auto listener = EventListenerTouchOneByOne::create();
		listener->onTouchBegan = CC_CALLBACK_2(ShowGoodsLayer::onTouchBegan, this);
		listener->onTouchMoved = CC_CALLBACK_2(ShowGoodsLayer::onTouchMoved, this);
		listener->onTouchEnded = CC_CALLBACK_2(ShowGoodsLayer::onTouchEnded, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, PanelTouch);

		// 物品显示添加
		switch (_curShowType)
		{
		case ShowGoodsLayer::InfoType_NewPack:
		{
			_curBuyType = BuyKey_NewPack;
			auto packinfo = NewDataMgr::getInstance()->mGameBuySystem->getPackGoodsInfoWithKey(_curBuyType);

			if (packinfo->gold)		addItemInfoToList(InfoType_BuyGold, packinfo->gold, "金币");
			if (packinfo->dazhao)	addItemInfoToList(InfoType_BuyDaZhao, packinfo->dazhao, "必杀");
			if (packinfo->hudun)	addItemInfoToList(InfoType_BuyShield, packinfo->hudun, "护盾");
			if (packinfo->life)		addItemInfoToList(InfoType_BuyLife, packinfo->life, "生命");
		}

			break;

		case ShowGoodsLayer::InfoType_OverflowPack:
		{
			_curBuyType = BuyKey_OverflowPack;
			auto packinfo = NewDataMgr::getInstance()->mGameBuySystem->getPackGoodsInfoWithKey(_curBuyType);

			if (packinfo->gold)		addItemInfoToList(InfoType_BuyGold, packinfo->gold, "金币");
			if (packinfo->dazhao)	addItemInfoToList(InfoType_BuyDaZhao, packinfo->dazhao, "必杀");
			if (packinfo->hudun)	addItemInfoToList(InfoType_BuyShield, packinfo->hudun, "护盾");
			if (packinfo->life)		addItemInfoToList(InfoType_BuyLife, packinfo->life, "生命");
		}
			break;

		case ShowGoodsLayer::InfoType_TyrantPack:
		{
			_curBuyType = BuyKey_TyrantPack;
			auto packinfo = NewDataMgr::getInstance()->mGameBuySystem->getPackGoodsInfoWithKey(_curBuyType);

			if (packinfo->gold)		addItemInfoToList(InfoType_BuyGold, packinfo->gold, "金币");
			if (packinfo->dazhao)	addItemInfoToList(InfoType_BuyDaZhao, packinfo->dazhao, "必杀");
			if (packinfo->hudun)	addItemInfoToList(InfoType_BuyShield, packinfo->hudun, "护盾");
			if (packinfo->life)		addItemInfoToList(InfoType_BuyLife, packinfo->life, "生命");
		}
			break;

		case ShowGoodsLayer::InfoType_BuyGold:
		{
			_curBuyType = BuyKey_BuyGold;
			auto goodsInfo = NewDataMgr::getInstance()->mGameBuySystem->getBuyGoodsDataWithID(_curBuyType);
			addItemInfoToList(InfoType_BuyGold, goodsInfo->number, goodsInfo->GoodsName);
		}
			break;

		case ShowGoodsLayer::InfoType_BuyShield:
		{
			_curBuyType = BuyKey_BuyShield;
			auto goodsInfo = NewDataMgr::getInstance()->mGameBuySystem->getBuyGoodsDataWithID(_curBuyType);
			addItemInfoToList(InfoType_BuyShield, goodsInfo->number, goodsInfo->GoodsName);
		}
			break;

		case ShowGoodsLayer::InfoType_BuyDaZhao:
		{
			_curBuyType = BuyKey_BuyDaZhao;
			auto goodsInfo = NewDataMgr::getInstance()->mGameBuySystem->getBuyGoodsDataWithID(_curBuyType);
			addItemInfoToList(InfoType_BuyDaZhao, goodsInfo->number, goodsInfo->GoodsName);
		}
			break;

		case ShowGoodsLayer::InfoType_BuyLife:
		{
			_curBuyType = BuyKey_BuyLife;
			auto goodsInfo = NewDataMgr::getInstance()->mGameBuySystem->getBuyGoodsDataWithID(_curBuyType);
			addItemInfoToList(InfoType_BuyLife, goodsInfo->number, goodsInfo->GoodsName);
		}
			break;

		case ShowGoodsLayer::InfoType_SignIn:
		{
			auto goodsInfo = NewDataMgr::getInstance()->getSignInRewardInfo(_data);

			if (goodsInfo->gold)	addItemInfoToList(InfoType_BuyGold, goodsInfo->gold, "金币");
			if (goodsInfo->dazhao)	addItemInfoToList(InfoType_BuyDaZhao, goodsInfo->dazhao, "必杀");
			if (goodsInfo->hudun)	addItemInfoToList(InfoType_BuyShield, goodsInfo->hudun, "护盾");
			if (goodsInfo->life)	addItemInfoToList(InfoType_BuyLife, goodsInfo->life, "生命");
		}
			break;

		case ShowGoodsLayer::InfoType_TaskReward:
		{
			auto goodsInfo = NewDataMgr::getInstance()->mGameTaskSystem->getGameTaskListWithID(_data);

			switch (goodsInfo->TaskRewardID)
			{
			case GoodsID_Gold:
				addItemInfoToList(InfoType_BuyGold, goodsInfo->RewardNum, "金币");
				break;
			case GoodsID_Life:
				addItemInfoToList(InfoType_BuyLife, goodsInfo->RewardNum, "生命");
				break;
			case GoodsID_DaZhao:
				addItemInfoToList(InfoType_BuyDaZhao, goodsInfo->RewardNum, "必杀");
				break;
			case GoodsID_HuDun:
				addItemInfoToList(InfoType_BuyShield, goodsInfo->RewardNum, "护盾");
				break;
			default:
				break;
			}

		}
			break;

		default:
			break;
		}

		updateInerface();

	} while (0);
}

void ShowGoodsLayer::setShowInfoType(int type, int data)
{
	_curShowType = type;
	_data = data;
}

void ShowGoodsLayer::addItemInfoToList(showInfoType type, int num, std::string name)
{
	ItemStr item;
	item.type = type;
	item.num = num;
	item.name = name;

	_goodsInfoList.push_back(item);
}

void ShowGoodsLayer::updateInerface()
{
	auto size = Director::getInstance()->getVisibleSize();
	int panelHight = 300 + _goodsInfoList.size() * 100;
	int goodsDistanceH = 100;

	if (InfoType_SignIn == _curShowType || InfoType_TaskReward == _curShowType)
	{
		panelHight -= 100;
		goodsDistanceH = 60;

	}
	// 框体大小
	auto PanelShowGoods = (Node*)GameMainLayer::seekNodeByName(_myLayer, "PanelShowGoods");
	PanelShowGoods->setContentSize(Size(600, panelHight));
	PanelShowGoods->setPosition(Vec2(size.width / 2, size.height / 2 + panelHight / 2));

	// 标题栏
	auto ImgTitleBg = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgTitleBg");
	auto ImgInterfaceTitle = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgInterfaceTitle");
	auto ImgPack = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgPack");
	auto ButtonBuy = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonBuy");

	ImgInterfaceTitle->setTexture(getTitleImgPathWithType((showInfoType)_curShowType));
	ImgTitleBg->setPosition(Vec2(600 / 2, panelHight));
	ImgPack->setPosition(Vec2(600 / 2, panelHight + 50));
	ButtonBuy->setPosition(Vec2(600 / 2, 120));

	ImgPack->setVisible((
		InfoType_SignIn == _curShowType || 
		InfoType_NewPack == _curShowType || 
		InfoType_OverflowPack == _curShowType || 
		InfoType_TyrantPack == _curShowType) ? true : false);

	auto PanelPriceBg = (Layer*)GameMainLayer::seekNodeByName(_myLayer, "PanelPriceBg");
	PanelPriceBg->setVisible(false);

	// 签到界面按钮处理
	if (InfoType_SignIn == _curShowType || InfoType_TaskReward == _curShowType)
	{
		//PanelShowGoods->setContentSize(Size(560, panelHight - 100));
		ButtonBuy->setTouchEnabled(false);
		ButtonBuy->setVisible(false);
	}
	else
	{
		// 价格框

		auto TextPrice = (Text*)GameMainLayer::seekNodeByName(_myLayer, "TextPrice");

		auto buyItem = NewDataMgr::getInstance()->mGameBuySystem->getBuyGoodsDataWithID(_curBuyType);
		if (buyItem->Price)
		{
			PanelPriceBg->setPosition(Vec2(600 / 2, panelHight - 100));
			PanelPriceBg->setVisible(true);

			//TextPrice->setString(StringUtils::format("%.1f元 ", buyItem->Price));
			TextPrice->setString(StringUtils::format("%d元 ", (int)buyItem->Price));
		}
	}

	
	auto listItem = _goodsInfoList.begin();
	for (int i = 1; listItem != _goodsInfoList.end(); listItem++, i++)
	{
		auto imgGoods = Sprite::create(getGoodsImgPathWithType(listItem->type));
		imgGoods->setPosition(Vec2(140, panelHight - goodsDistanceH - 90 * i));
		PanelShowGoods->addChild(imgGoods);

		auto textGoodsName = Text::create();
		textGoodsName->setString(listItem->name);
		textGoodsName->setPosition(Vec2(140 + 100, panelHight - goodsDistanceH - 90 * i));
		textGoodsName->setFontSize(38);
		textGoodsName->setColor(Color3B(97, 74, 37));
		PanelShowGoods->addChild(textGoodsName);

		auto textGoodsNum = Text::create();
		textGoodsNum->setAnchorPoint(Vec2(0, 0.5));
		textGoodsNum->setString(StringUtils::format("X%d", listItem->num));
		textGoodsNum->setPosition(Vec2(140 + 200, panelHight - goodsDistanceH - 90 * i));
		textGoodsNum->setFontSize(36);
		textGoodsNum->setColor(Color3B(121, 67, 7));
		PanelShowGoods->addChild(textGoodsNum);

	}
}

std::string ShowGoodsLayer::getGoodsImgPathWithType(showInfoType type)
{
	std::string imgName;
	switch (type)
	{
	case ShowGoodsLayer::InfoType_BuyGold:
		imgName = "ccsRes/resShopLayer/ImgGold1.png";
		break;
	case ShowGoodsLayer::InfoType_BuyShield:
		imgName = "ccsRes/resShopLayer/ImgHuDun1.png";
		break;
	case ShowGoodsLayer::InfoType_BuyDaZhao:
		imgName = "ccsRes/resShopLayer/ImgDazhao.png";
		break;
	case ShowGoodsLayer::InfoType_BuyLife:
		imgName = "ccsRes/resShopLayer/ImgLife1.png";
		break;

	default:
		break;
	}

	return imgName;
}

std::string ShowGoodsLayer::getTitleImgPathWithType(showInfoType type)
{
	std::string imgName;
	switch (type)
	{
	case ShowGoodsLayer::InfoType_OverflowPack:
		imgName = "roleImg/ImgOverflowPack.png";
		break;
	case ShowGoodsLayer::InfoType_TyrantPack:
		imgName = "roleImg/ImgTyrantPack.png";
		break;
	case ShowGoodsLayer::InfoType_BuyGold:
		imgName = "roleImg/ImgBuyGold.png";
		break;
	case ShowGoodsLayer::InfoType_BuyShield:
		imgName = "roleImg/ImgBuyShield.png";
		break;
	case ShowGoodsLayer::InfoType_BuyDaZhao:
		imgName = "roleImg/ImgBuyDaZhao.png";
		break;
	case ShowGoodsLayer::InfoType_TaskReward:
	case ShowGoodsLayer::InfoType_SignIn:
		imgName = "roleImg/ImgGetReward.png";
		break;
	default:
		break;
	}

	return imgName;
}

void ShowGoodsLayer::onTouchMoved(Touch *touch, Event *unused_event)
{

}

bool ShowGoodsLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
	return true;
}


/*******************************************
*	礼包展示
*******************************************/
PackShowLayer* PackShowLayer::getPackShowLayer(void)
{
	auto layout = PackShowLayer::create();

	if (layout)
	{
		return layout;
	}
	return nullptr;
}

void PackShowLayer::onEnter()
{
	Layer::onEnter();

	setName("PackShowLayer");
	initInterface();
	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(PackShowLayer::onMessageEvent), MsgEventName_ToGoodsInfoLayer, nullptr);
}

void PackShowLayer::onExit()
{
	Layer::onExit();

	// 移除观察者消息
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void PackShowLayer::setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback)
{
	do
	{
		CC_BREAK_IF(node == nullptr);

		node->setTag(tag);

		if (callback)
			node->addTouchEventListener(callback);
		else
			node->addTouchEventListener(CC_CALLBACK_2(PackShowLayer::callFuncButtonEvent, this));

	} while (0);
}

void PackShowLayer::callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType)) return;

	Button* btn = (Button*)ref;
	int nTag = btn->getTag();

	switch (nTag)
	{

	case Tag_Close:
	{
		if (Scene_Type_PlayInterface == GameSceneMgr::getInstance()->getCurScen())
		{
			PauseLayer* layer = PauseLayer::create();
			layer->setActivateType(PauseLayer::ActivateTag_None);
			Director::getInstance()->getRunningScene()->addChild(layer, 10000);
		}

		removeFromParentAndCleanup(true);
	}
		break;

	case Tag_Buy:
		NewDataMgr::getInstance()->mGameBuySystem->buyGoodsWithKey(_curBuyType);
		break;

	default:
		break;
	}
}

void PackShowLayer::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	switch (protocolID)
	{
	case EProtocolID_RemoveGoodsInfo:
		removeFromParentAndCleanup(true);
		break;

	default:
		break;
	}
}

void PackShowLayer::initInterface()
{
	_myLayer = CSLoader::createNode(Ccs_ShowPackLayer_FilePath);

	if (!_myLayer)
	{
		return;
	}

	addChild(_myLayer);

	do
	{
		// 关闭
		auto ButtonBack = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonClose");
		setEventForNode(ButtonBack, Tag_Close, nullptr);

		// 购买
		auto ButtonBuy = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonBuyPack");
		setEventForNode(ButtonBuy, Tag_Buy, nullptr);

		// 物品显示添加
		switch (_curShowType)
		{
		case PackType::InfoType_PropPack:
		{
			_curBuyType = BuyKey_BuyPropPack;
			auto packinfo = NewDataMgr::getInstance()->mGameBuySystem->getPackGoodsInfoWithKey(_curBuyType);

			addItemInfoToList(InfoType_BuyGold, packinfo->gold, "金币");
			addItemInfoToList(InfoType_BuyDaZhao, packinfo->dazhao, "必杀");
			addItemInfoToList(InfoType_BuyShield, packinfo->hudun, "护盾");
			addItemInfoToList(InfoType_BuyLife, packinfo->life, "生命");
		}

			break;

		case PackType::InfoType_OverflowPack:
		{
			_curBuyType = BuyKey_OverflowPack;
			auto packinfo = NewDataMgr::getInstance()->mGameBuySystem->getPackGoodsInfoWithKey(_curBuyType);

			addItemInfoToList(InfoType_BuyGold, packinfo->gold, "金币");
			addItemInfoToList(InfoType_BuyDaZhao, packinfo->dazhao, "必杀");
			addItemInfoToList(InfoType_BuyShield, packinfo->hudun, "护盾");
			addItemInfoToList(InfoType_BuyLife, packinfo->life, "生命");
		}
			break;

		case PackType::InfoType_TyrantPack:
		{
			_curBuyType = BuyKey_TyrantPack;
			auto packinfo = NewDataMgr::getInstance()->mGameBuySystem->getPackGoodsInfoWithKey(_curBuyType);

			addItemInfoToList(InfoType_BuyGold, packinfo->gold, "金币");
			addItemInfoToList(InfoType_BuyDaZhao, packinfo->dazhao, "必杀");
			addItemInfoToList(InfoType_BuyShield, packinfo->hudun, "护盾");
			addItemInfoToList(InfoType_BuyLife, packinfo->life, "生命");
		}
			break;

		default:
			break;
		}

		updateInerface();

	} while (0);
}

void PackShowLayer::setShowInfoType(int type, int data)
{
	_curShowType = type;
	_data = data;
}

void PackShowLayer::addItemInfoToList(PackType type, int num, std::string name)
{
	ItemStr item;
	item.type = type;
	item.num = num;
	item.name = name;

	_goodsInfoList.push_back(item);
}

void PackShowLayer::updateInerface()
{
	
	auto buyItem = NewDataMgr::getInstance()->mGameBuySystem->getBuyGoodsDataWithID(_curBuyType);
	// 标题栏
	auto TitlePack = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "TitlePack");
	auto IcoPack = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "IcoPack");
	auto TextBuyTips = (Text*)GameMainLayer::seekNodeByName(_myLayer, "TextBuyTips");

	TitlePack->setTexture(getPackTitlePathWithType((PackType)_curShowType));
	IcoPack->setTexture(getPackIcoPathWtihType((PackType)_curShowType));
	TextBuyTips->setString(StringUtils::format("点击“领取”即确定购买，信息费用%d元。是否确定立即支付购买？客服：400660448", (int)buyItem->Price));

	auto listItem = _goodsInfoList.begin();
	for (int i = 1; listItem != _goodsInfoList.end(); listItem++, i++)
	{
		auto propNum = listItem->num;
		switch (listItem->type)
		{
		case InfoType_BuyGold:		
		{
			auto ImgGoods = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgGold");
			auto NumGoods = (TextAtlas*)GameMainLayer::seekNodeByName(_myLayer, "NumGold");
			NumGoods->setString(StringUtils::format("%d", listItem->num));
			ImgGoods->setVisible((propNum) ? true : false);
			NumGoods->setVisible((propNum) ? true : false);
		}
			break;
		case InfoType_BuyShield:
		{
			auto ImgGoods = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgHudun");
			auto NumGoods = (TextAtlas*)GameMainLayer::seekNodeByName(_myLayer, "NumHudun");
			NumGoods->setString(StringUtils::format("%d", listItem->num));
			ImgGoods->setVisible((propNum) ? true : false);
			NumGoods->setVisible((propNum) ? true : false);
		}
			break;
		case InfoType_BuyDaZhao:
		{
			auto ImgGoods = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgDaZhao");
			auto NumGoods = (TextAtlas*)GameMainLayer::seekNodeByName(_myLayer, "NumDazhao");
			NumGoods->setString(StringUtils::format("%d", listItem->num));
			NumGoods->setVisible((propNum) ? true : false);
			ImgGoods->setVisible((propNum) ? true : false);
		}
			break;
		case InfoType_BuyLife:
		{
			auto ImgGoods = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgLife");
			auto NumGoods = (TextAtlas*)GameMainLayer::seekNodeByName(_myLayer, "NumLIfe");
			NumGoods->setString(StringUtils::format("%d", listItem->num));
			ImgGoods->setVisible((propNum) ? true : false);
			NumGoods->setVisible((propNum) ? true : false);
		}
			break;
		default:
			break;
		}
	}
}


std::string PackShowLayer::getPackIcoPathWtihType(PackType type)
{
	std::string imgName;
	switch (type)
	{
	case PackType::InfoType_OverflowPack:
		imgName = "ccsRes/resShopLayer/imgCJPack.png";
		break;
	case PackType::InfoType_TyrantPack:
		imgName = "ccsRes/resShopLayer/imgZZPack.png";
		break;
	case PackType::InfoType_PropPack:
		imgName = "ccsRes/resShopLayer/imgDJPack.png";
		break;
	default:
		break;
	}

	return imgName;
}

std::string PackShowLayer::getPackTitlePathWithType(PackType type)
{
	std::string imgName;
	switch (type)
	{
	case PackType::InfoType_OverflowPack:
		imgName = "ccsRes/resShopLayer/imgCJPackStr.png";
		break;
	case PackType::InfoType_TyrantPack:
		imgName = "ccsRes/resShopLayer/imgZZPackStr.png";
		break;
	case PackType::InfoType_PropPack:
		imgName = "ccsRes/resShopLayer/imgDJPackStr.png";
		break;

	default:
		break;
	}

	return imgName;
}


/*******************************************
*	女神礼包展示
*******************************************/
ShowRolePackLayer* ShowRolePackLayer::getRolePackLayer(void)
{
	auto layout = ShowRolePackLayer::create();

	if (layout)
	{
		return layout;
	}
	return nullptr;
}

void ShowRolePackLayer::onEnter()
{
	Layer::onEnter();

	setName("ShowRolePackLayer");
	initInterface();
	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(ShowRolePackLayer::onMessageEvent), MsgEventName_ToRolePack, nullptr);
}

void ShowRolePackLayer::onExit()
{
	Layer::onExit();

	// 移除观察者消息
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void ShowRolePackLayer::setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback)
{
	do
	{
		CC_BREAK_IF(node == nullptr);

		node->setTag(tag);

		if (callback)
			node->addTouchEventListener(callback);
		else
			node->addTouchEventListener(CC_CALLBACK_2(ShowRolePackLayer::callFuncButtonEvent, this));

	} while (0);
}

void ShowRolePackLayer::callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType)) return;

	Button* btn = (Button*)ref;
	int nTag = btn->getTag();

	switch (nTag)
	{

	case Tag_Close:
	{
		CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToRoleLayer, EProtocolID_BuyRolePackCancel);
		removeFromParentAndCleanup(true);
	}
		break;

	case Tag_Buy:
		NewDataMgr::getInstance()->mGameBuySystem->buyGoodsWithKey(BuyKey_BuyRolePack);
		break;

	default:
		break;
	}
}

void ShowRolePackLayer::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	switch (protocolID)
	{
	case EProtocolID_RemoveGoodsInfo:
		removeFromParentAndCleanup(true);
		break;

	default:
		break;
	}
}

void ShowRolePackLayer::initInterface()
{
	_myLayer = CSLoader::createNode(Ccs_RolePackLayer_FilePath);

	if (!_myLayer)
	{
		return;
	}

	addChild(_myLayer);

	do
	{
		// 关闭
		auto ButtonBack = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonClose");
		setEventForNode(ButtonBack, Tag_Close, nullptr);

		// 购买
		auto ButtonBuy = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonBuyPack");
		setEventForNode(ButtonBuy, Tag_Buy, nullptr);


	} while (0);
}


/*******************************************
*	新手指引
*******************************************/

void HelpFeqLayer::showHelpFeq(HelpFeqType type, Node* node /*= nullptr*/)
{
	NewDataMgr::getInstance()->setGuideState(true);

	Node* parentNode = nullptr;
	if (GameSceneType::Scene_Type_RoleInterface == GameSceneMgr::getInstance()->getCurScen() )
	{
		parentNode = GameInterfaceMgr::getInstance()->getParentNode();
	}
	else if (GameSceneType::Scene_Type_PlayInterface == GameSceneMgr::getInstance()->getCurScen())
	{
		parentNode = UIController::getInstance()->getMainUILayer();
	}

	auto helpFeqlayout = HelpFeqLayer::create();
	helpFeqlayout->setGuideNode(node);
	helpFeqlayout->setHelpType(type);
	helpFeqlayout->setIsShowArrows((node) ? true : false);
	parentNode->addChild(helpFeqlayout, 1200);
}


HelpFeqLayer* HelpFeqLayer::getHelpFeqLayerLayer(void)
{
	auto layout = HelpFeqLayer::create();

	if (layout)
	{
		return layout;
	}
	return nullptr;
}

void HelpFeqLayer::onEnter()
{
	Layer::onEnter();

	checkAddList();
	if (getGuideNode())
	{
		// 设置需要指引的按钮
		auto FeqButton = getGuideNode();
		auto buttonPoint = FeqButton->getParent()->convertToWorldSpace(FeqButton->getPosition());
		auto buttonContentSize = FeqButton->getContentSize();

		setArrowsPos(buttonPoint);

#if 0
		// 设置裁剪节点
		auto clip = ClippingNode::create();
		clip->setInverted(true);
		clip->setAlphaThreshold(0.0f);
		addChild(clip);

		// 在裁剪节点添加一个灰色的透明层
		auto layerColor = LayerColor::create(Color4B(0, 0, 0, 150));
		clip->addChild(layerColor);

		Vec2 point[4];
		point[0] = Vec2(buttonPoint.x - buttonContentSize.width / 2, buttonPoint.y - buttonContentSize.height / 2);
		point[1] = Vec2(buttonPoint.x + buttonContentSize.width / 2, buttonPoint.y - buttonContentSize.height / 2);
		point[2] = Vec2(buttonPoint.x + buttonContentSize.width / 2, buttonPoint.y + buttonContentSize.height / 2);
		point[3] = Vec2(buttonPoint.x - buttonContentSize.width / 2, buttonPoint.y + buttonContentSize.height / 2);

		auto mStencil = CCDrawNode::create();
		mStencil->drawPolygon(point, 4, ccColor4F(1, 1, 1, 1), 3, ccColor4F(1, 1, 0, 1));
		clip->setStencil(mStencil);
#else
		// 在裁剪节点添加一个灰色的透明层
		CCSize size = CCDirector::getInstance()->getWinSize();
		auto layerColor = LayerColor::create(Color4B(0, 0, 0, 200));

		Vec2 point[5];
		// 左下
		point[0] = Vec2(buttonPoint.x - buttonContentSize.width / 2 , buttonPoint.y - buttonContentSize.height / 2);
		// 右下
		point[2] = Vec2(buttonPoint.x + buttonContentSize.width / 2, buttonPoint.y - buttonContentSize.height / 2);
		// 右上
		point[3] = Vec2(buttonPoint.x + buttonContentSize.width / 2, buttonPoint.y + buttonContentSize.height / 2);
		// 左上
		point[4] = Vec2(buttonPoint.x - buttonContentSize.width / 2, buttonPoint.y + buttonContentSize.height / 2);



		//  画个圆角矩形
		int segments = 50;		//圆角等分个数
		float radius = 20;		//圆角半径
		Vec2 origin, destination;
		origin = Vec2(buttonPoint.x - buttonContentSize.width / 2, buttonPoint.y - buttonContentSize.height / 2);
		destination = Vec2(buttonPoint.x + buttonContentSize.width / 2, buttonPoint.y + buttonContentSize.height / 2);

		if (HelpFeqType_Card2_2 == getHelpType())
		{
			origin = Vec2(buttonPoint.x - buttonContentSize.width / 2 - 8, buttonPoint.y - buttonContentSize.height / 20 - 45);
			destination = Vec2(buttonPoint.x + buttonContentSize.width / 2, buttonPoint.y + buttonContentSize.height / 2 + 15);
		}
		const float coef = 0.5f * (float)M_PI / segments;
		Point * vertices = new Point[segments + 1];
		Point * thisVertices = vertices;
		for (unsigned int i = 0; i <= segments; ++i, ++thisVertices)
		{
			float rads = (segments - i)*coef;
			thisVertices->x = (int)(radius * sinf(rads));
			thisVertices->y = (int)(radius * cosf(rads));
		}
		//
		Point tagCenter;
		float minX = MIN(origin.x, destination.x);
		float maxX = MAX(origin.x, destination.x);
		float minY = MIN(origin.y, destination.y);
		float maxY = MAX(origin.y, destination.y);

		unsigned int dwPolygonPtMax = (segments + 1) * 4;
		Point * pPolygonPtArr = new Point[dwPolygonPtMax];
		Point * thisPolygonPt = pPolygonPtArr;
		int aa = 0;
		//左上角
		tagCenter.x = minX + radius;
		tagCenter.y = maxY - radius;
		thisVertices = vertices;
		for (unsigned int i = 0; i <= segments; ++i, ++thisPolygonPt, ++thisVertices)
		{
			thisPolygonPt->x = tagCenter.x - thisVertices->x;
			thisPolygonPt->y = tagCenter.y + thisVertices->y;
			++aa;
		}
		//右上角
		tagCenter.x = maxX - radius;
		tagCenter.y = maxY - radius;
		thisVertices = vertices + segments;
		for (unsigned int i = 0; i <= segments; ++i, ++thisPolygonPt, --thisVertices)
		{
			thisPolygonPt->x = tagCenter.x + thisVertices->x;
			thisPolygonPt->y = tagCenter.y + thisVertices->y;
			++aa;
		}
		//右下角
		tagCenter.x = maxX - radius;
		tagCenter.y = minY + radius;
		thisVertices = vertices;
		for (unsigned int i = 0; i <= segments; ++i, ++thisPolygonPt, ++thisVertices)
		{
			thisPolygonPt->x = tagCenter.x + thisVertices->x;
			thisPolygonPt->y = tagCenter.y - thisVertices->y;
			++aa;
		}
		//左下角
		tagCenter.x = minX + radius;
		tagCenter.y = minY + radius;
		thisVertices = vertices + segments;
		for (unsigned int i = 0; i <= segments; ++i, ++thisPolygonPt, --thisVertices)
		{
			thisPolygonPt->x = tagCenter.x - thisVertices->x;
			thisPolygonPt->y = tagCenter.y - thisVertices->y;
			++aa;
		}

		ccBlendFunc blend;
		blend.src = GL_ONE;
		blend.dst = GL_ZERO;

		auto mStencil = CCDrawNode::create();
		mStencil->drawPolygon(pPolygonPtArr, dwPolygonPtMax, ccColor4F(0, 0, 0, 0), 3, ccColor4F(0, 0, 0, 0));
		mStencil->setBlendFunc(blend);

		CCRenderTexture* texture = CCRenderTexture::create(size.width, size.height, kCCTexture2DPixelFormat_RGBA8888);
		if (texture)
		{
			texture->begin();
			layerColor->visit();
			mStencil->visit();
			texture->end();
			CCSprite* ret = CCSprite::createWithTexture(texture->getSprite()->getTexture());
			ret->setFlipY(true);
			ret->setPosition(Vec2(size.width / 2, size.height / 2));
			addChild(ret);
		}

		CC_SAFE_DELETE_ARRAY(vertices);
		CC_SAFE_DELETE_ARRAY(pPolygonPtArr);
#endif
		


		// 添加一个layer拦截触摸
		auto listen_layer = Layer::create();
		listen_layer->setContentSize(Director::getInstance()->getVisibleSize());
		addChild(listen_layer);
		auto listener = EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = [=](Touch* touch, Event* event){
			CCPoint touchPoint = touch->getLocation();

			//判断点是否在矩形中
			auto btnPoint = FeqButton->getParent()->convertToWorldSpace(FeqButton->getPosition());
			auto btnSize = FeqButton->getContentSize();
			Rect rect = Rect(btnPoint.x - btnSize.width / 2, btnPoint.y - btnSize.height / 2, btnSize.width, btnSize.height);
			if (rect.containsPoint(touchPoint))
			{
				Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
				NewDataMgr::getInstance()->setGuideState(false);
				removeFromParentAndCleanup(true);

				if (GameSceneType::Scene_Type_PlayInterface == GameSceneMgr::getInstance()->getCurScen())
				{
					GameData::getInstance()->GameResume();
				}

				return false;
			}
			return true;
		};

		Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, listen_layer);
	}
	else
	{
		std::list<HelpFeqType> nextTipsList;

		// 在裁剪节点添加一个灰色的透明层
		auto layerColor = LayerColor::create(Color4B(0, 0, 0, 150));
		addChild(layerColor);

		// 添加一个layer拦截触摸
		auto listen_layer = Layer::create();
		listen_layer->setContentSize(Director::getInstance()->getVisibleSize());
		addChild(listen_layer);
		auto listener = EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = [=](Touch* touch, Event* event){

			if (_helpFeqList.empty() )
			{
				NewDataMgr::getInstance()->setGuideState(false);

				NewDataMgr::getInstance()->setGuideState(false);
				runAction(Sequence::create(
					DelayTime::create(0.1),
					CCCallFuncN::create(CC_CALLBACK_1(HelpFeqLayer::delayRemoveSelf, this)),
					nullptr));

				if (GameSceneType::Scene_Type_PlayInterface == GameSceneMgr::getInstance()->getCurScen())
				{
					GameData::getInstance()->GameResume();
				}
			}
			else
			{
				auto item = _helpFeqList.begin();

				if (HelpFeqType_Card2_2 == (*item))
				{
					CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToCardLayer, EProtocolID_HelpFeqStep2);
					_helpFeqList.clear();

					NewDataMgr::getInstance()->setGuideState(false);
					runAction(Sequence::create(
						DelayTime::create(0.1),
						CCCallFuncN::create(CC_CALLBACK_1(HelpFeqLayer::delayRemoveSelf,this)), 
						nullptr));
					
				}
				else
				{
					setTipsText((*item));
					_helpFeqList.erase(item);
				}

			}
			return true;
			
		};

		Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, listen_layer);
	}
	initInterface();

}

void HelpFeqLayer::delayRemoveSelf(Node* node)
{
	removeFromParentAndCleanup(true);
	//Director::getInstance()->getEventDispatcher()->removeEventListener(listener);
}

void HelpFeqLayer::setShowInfoType(int type )
{

}

void HelpFeqLayer::initInterface()
{
	_myLayer = CSLoader::createNode(Ccs_HelpFeq_FilePath);

	if (!_myLayer)
	{
		return;
	}

	addChild(_myLayer);

	do 
	{
		auto size = Director::getInstance()->getVisibleSize();
		auto PanelHelpFeq = (Node*)GameMainLayer::seekNodeByName(_myLayer, "PanelHelpFeq");
		
		auto ImgTips = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgTips");
		ImgTips->setVisible((getIsShowArrows()) ? true : false);

		auto TextTips = (Text*)GameMainLayer::seekNodeByName(_myLayer, "TextTips");
		auto TextTouchContinue = (Text*)GameMainLayer::seekNodeByName(_myLayer, "TextTouchContinue");
		TextTouchContinue->setVisible((getIsShowArrows()) ? false : true);

		auto PanelBallTips = (Node*)GameMainLayer::seekNodeByName(_myLayer, "PanelBallTips");
		PanelBallTips->setVisible(false);

		int moveOffset = 640;
		switch (getHelpType())
		{
		case HelpFeqLayer::HelpFeqType_SignIn:
			ImgTips->setPosition(getArrowsPos() + Vec2(0, -150));
			ImgTips->setFlippedY(true);
			break;

		case HelpFeqLayer::HelpFeqType_ChuJi1:
			ImgTips->setPosition(getArrowsPos() + Vec2(0, -150));
			ImgTips->setFlippedY(true);
			break;

		case HelpFeqLayer::HelpFeqType_ChuJi2:
			ImgTips->setPosition(getArrowsPos() + Vec2(0, 150));
			ImgTips->setFlippedY(false);
			break;
		case HelpFeqLayer::HelpFeqType_CardTips:
		{
			TextTips->setVisible(false);
			PanelBallTips->setVisible(true);


			auto ImgFlash1 = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgFlash1");
			auto ImgFlash2 = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgFlash2");
			auto ImgFlash3 = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgFlash3");

			ImgFlash1->runAction(CCRepeatForever::create(Sequence::create(
				CCFadeOut::create(0.6),
				CCFadeIn::create(0.6),
				nullptr)));

			ImgFlash2->runAction(CCRepeatForever::create(Sequence::create(
				CCFadeOut::create(0.6),
				CCFadeIn::create(0.6),
				nullptr)));

			ImgFlash3->runAction(CCRepeatForever::create(Sequence::create(
				CCFadeOut::create(0.6),
				CCFadeIn::create(0.6),
				nullptr)));

			auto Imgkuang_1 = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "kuang_1");
			auto Imgkuang_2 = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "kuang_2");
			auto Imgkuang_3 = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "kuang_3");

			auto Imgball1 = Sprite::createWithSpriteFrameName("blueWp.png");
			Imgball1->setPosition(Imgkuang_1->getContentSize()/2);
			Imgkuang_1->addChild(Imgball1);
			auto Imgball2 = Sprite::createWithSpriteFrameName("redWp.png");
			Imgball2->setPosition(Imgkuang_2->getContentSize() / 2);
			Imgkuang_2->addChild(Imgball2);
			auto Imgball3 = Sprite::createWithSpriteFrameName("yellowWp.png");
			Imgball3->setPosition(Imgkuang_3->getContentSize() / 2);
			Imgkuang_3->addChild(Imgball3);
		}
			break;
		case HelpFeqLayer::HelpFeqType_UseHudun:
			ImgTips->setPosition(getArrowsPos() + Vec2(0, 150));
			break;
		case HelpFeqLayer::HelpFeqType_UseDazhao:
			ImgTips->setPosition(getArrowsPos() + Vec2(0, 150));
			break;
		case HelpFeqLayer::HelpFeqType_Upgrade1:
			moveOffset = size.height * 0.7;
			ImgTips->setPosition(getArrowsPos() + Vec2(0, 150));
			ImgTips->setFlippedY(false);
			break;
		case HelpFeqLayer::HelpFeqType_Upgrade2:
			break;
		case HelpFeqLayer::HelpFeqType_Upgrade3:
			ImgTips->setPosition(getArrowsPos() + Vec2(0, 150));
			ImgTips->setFlippedY(false);
			break;
		case HelpFeqLayer::HelpFeqType_Card1_1:
			break;
		case HelpFeqLayer::HelpFeqType_Card1_2:
			break;
		case HelpFeqLayer::HelpFeqType_Card1_3:
			break;
		case HelpFeqLayer::HelpFeqType_Card2_1:
			ImgTips->setPosition(getArrowsPos() + Vec2(0, 150));
			ImgTips->setFlippedY(false);
			break;
		case HelpFeqLayer::HelpFeqType_Card2_2:
			ImgTips->setPosition(getArrowsPos() + Vec2(0, 150));
			ImgTips->setFlippedY(false);
			break;
		case HelpFeqLayer::HelpFeqType_Card2_3:
			ImgTips->setPosition(getArrowsPos() + Vec2(0, 150));
			ImgTips->setFlippedY(false);
			break;
		case HelpFeqLayer::HelpFeqType_Card2_4:
			ImgTips->setPosition(getArrowsPos() + Vec2(0, -150));
			ImgTips->setFlippedY(true);
			break;
		default:
			break;
		}

		PanelHelpFeq->runAction(EaseSineOut::create(MoveBy::create(0.1, Vec2(0, -moveOffset))));

		ImgTips->runAction(RepeatForever::create(Sequence::create(
			MoveBy::create(0.3, Vec2(0, 15)),
			MoveBy::create(0.3, Vec2(0, -15)),
			nullptr)));

		setTipsText(getHelpType());

	} while (0);
}

void HelpFeqLayer::setTipsArrowsPos(Vec2 pos)
{

}

void HelpFeqLayer::setTipsText(HelpFeqType type)
{
	auto TextTips = (Text*)GameMainLayer::seekNodeByName(_myLayer, "TextTips");
	switch (type)
	{
	case HelpFeqLayer::HelpFeqType_SignIn:
		TextTips->setString(NewDataMgr::getInstance()->getXmlStringWithKey("helpFeq_signIn"));
		break;

	case HelpFeqLayer::HelpFeqType_ChuJi1:
		TextTips->setString(NewDataMgr::getInstance()->getXmlStringWithKey("helpFeq_ChuJi"));
		break;

	case HelpFeqLayer::HelpFeqType_ChuJi2:
		TextTips->setString(NewDataMgr::getInstance()->getXmlStringWithKey("helpFeq_ChuJi2"));
		break;

	case HelpFeqLayer::HelpFeqType_UseHudun:
		TextTips->setString(NewDataMgr::getInstance()->getXmlStringWithKey("helpFeq_UseHudun"));
		break;

	case HelpFeqLayer::HelpFeqType_UseDazhao:
		TextTips->setString(NewDataMgr::getInstance()->getXmlStringWithKey("helpFeq_UseDazhao"));
		break;

	case HelpFeqLayer::HelpFeqType_Upgrade1:
		TextTips->setString(NewDataMgr::getInstance()->getXmlStringWithKey("helpFeq_Upgrade1"));
		break;

	case HelpFeqLayer::HelpFeqType_Upgrade2:
		TextTips->setString(NewDataMgr::getInstance()->getXmlStringWithKey("helpFeq_Upgrade2"));
		break;

	case HelpFeqLayer::HelpFeqType_Upgrade3:
		TextTips->setString(NewDataMgr::getInstance()->getXmlStringWithKey("helpFeq_Upgrade3"));
		break;

	case HelpFeqLayer::HelpFeqType_CardStep1:
	case HelpFeqLayer::HelpFeqType_CardStep3:
	case HelpFeqLayer::HelpFeqType_Card1_1:
		TextTips->setString(NewDataMgr::getInstance()->getXmlStringWithKey("helpFeq_Card1_1"));
		break;

	case HelpFeqLayer::HelpFeqType_Card1_2:
		TextTips->setString(NewDataMgr::getInstance()->getXmlStringWithKey("helpFeq_Card1_2"));
		break;

	case HelpFeqLayer::HelpFeqType_Card1_3:
		TextTips->setString(NewDataMgr::getInstance()->getXmlStringWithKey("helpFeq_Card1_3"));
		break;

	case HelpFeqLayer::HelpFeqType_Card2_1:
		TextTips->setString(NewDataMgr::getInstance()->getXmlStringWithKey("helpFeq_Card2_1"));
		break;

	case HelpFeqLayer::HelpFeqType_Card2_2:
		TextTips->setString(NewDataMgr::getInstance()->getXmlStringWithKey("helpFeq_Card2_2"));
		break;

	case HelpFeqLayer::HelpFeqType_Card2_3:
		TextTips->setString(NewDataMgr::getInstance()->getXmlStringWithKey("helpFeq_Card2_3"));
		break;

	case HelpFeqLayer::HelpFeqType_Card2_4:
		TextTips->setString(NewDataMgr::getInstance()->getXmlStringWithKey("helpFeq_Card2_4"));
		break;
	default:
		break;
	}
}

void HelpFeqLayer::checkAddList()
{
	if ( HelpFeqType_CardStep1 == getHelpType() )
	{
		_helpFeqList.push_back(HelpFeqType_Card1_2);
		_helpFeqList.push_back(HelpFeqType_Card1_3);
	}

	if ( HelpFeqType_CardStep3 == getHelpType() )
	{
		_helpFeqList.push_back(HelpFeqType_Card1_2);
		_helpFeqList.push_back(HelpFeqType_Card2_2);
	}
}
