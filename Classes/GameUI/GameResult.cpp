#include "GameResult.h"
#include "GameRoleScene.h"
#include "../GameCommon/CommonRes.h"
#include "GameMainLayer.h"
#include "../GameCommon/GameData.h"
#include "GameSceneMgr.h"

/*******************************************
*	胜利结算界面
*******************************************/
void GameResultSuccess::showLayout(Node* node, int tag)
{
	if (node && !node->getChildByName("GameResultSuccess"))
	{
		auto layout = GameResultSuccess::create();
		layout->setTag(tag);
		node->addChild(layout, LocalZOrde_500, "GameResultSuccess");
	}
}

void GameResultSuccess::onEnter()
{
	Layer::onEnter();

	int curLevel = NewDataMgr::getInstance()->getCurLevel();
	if (3 >= curLevel)
	{
		auto showGoodsInfoLayer = PackShowLayer::getPackShowLayer();
		showGoodsInfoLayer->setShowInfoType(PackShowLayer::PackType::InfoType_OverflowPack);
		GameInterfaceMgr::getInstance()->getParentNode()->addChild(showGoodsInfoLayer, LocalZOrde_1000);
	}
	else
	{
		auto showGoodsInfoLayer = PackShowLayer::getPackShowLayer();
		showGoodsInfoLayer->setShowInfoType(PackShowLayer::PackType::InfoType_TyrantPack);
		GameInterfaceMgr::getInstance()->getParentNode()->addChild(showGoodsInfoLayer, LocalZOrde_1000);
	}
	initInterface();
	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(GameResultSuccess::onMessageEvent), MsgEventName_ToSucResult, nullptr);
}

void GameResultSuccess::onExit()
{
	Layer::onExit();

	// 绉婚櫎瑙傚療鑰呮秷鎭�
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void GameResultSuccess::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	switch (protocolID)
	{
	default:
		break;
	}
}

void GameResultSuccess::setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback)
{
	do
	{
		CC_BREAK_IF(node == nullptr);

		node->setTag(tag);

		if (callback)
			node->addTouchEventListener(callback);
		else
			node->addTouchEventListener(CC_CALLBACK_2(GameResultSuccess::callFuncButtonEvent, this));

	} while (0);
}

void GameResultSuccess::callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType)) return;

	Button* btn = (Button*)ref;
	int nTag = btn->getTag();

	switch (nTag)
	{
	case Tag_Back:
		GameInterfaceMgr::getInstance()->closeAllInterface();
		break;

	case Tag_Continut:
		GameInterfaceMgr::getInstance()->closeAllInterface();
		GameInterfaceMgr::getInstance()->addInterfaceToList(GameInterfaceMgr::InterfaceTag_LevelOption);
		break;

	case Tag_Again:
		GameInterfaceMgr::getInstance()->closeAllInterface();
		GameSceneMgr::getInstance()->changeScene(GameSceneType::Scene_Type_PlayInterface);
		break;

	default:
		break;
	}

}

void GameResultSuccess::initInterface()
{
	_myLayer = CSLoader::createNode(Ccs_ResoutSucLayer_FilePath);

	if (!_myLayer)
	{
		return;
	}

	addChild(_myLayer);

	do
	{
		// 返回
		auto ButtonBack = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonBack");
		setEventForNode(ButtonBack, Tag_Back, nullptr);

		// 鍏抽棴
		auto ButtonContinut = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonContinut");
		setEventForNode(ButtonContinut, Tag_Continut, nullptr);

		auto ButtonAgain = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonAgain");
		setEventForNode(ButtonAgain, Tag_Again, nullptr);

		int curLevel = NewDataMgr::getInstance()->getCurLevel();
		auto AtlasLabelLevel = (TextAtlas*)GameMainLayer::seekNodeByName(_myLayer, "AtlasLabelLevel");
		AtlasLabelLevel->setString(StringUtils::format(".%d/", curLevel));

		auto AtlasLabelScore = (TextAtlas*)GameMainLayer::seekNodeByName(_myLayer, "AtlasLabelScore");
		auto AtlasLabelGold = (TextAtlas*)GameMainLayer::seekNodeByName(_myLayer, "AtlasLabelGold");

		int goldNum = GameData::getInstance()->getGoldNum();
		AtlasLabelScore->setString(StringUtils::format("%d", GameData::getInstance()->getScoreNum()));
		AtlasLabelGold->setString(StringUtils::format("%d", goldNum));

		auto ImgMainStar = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgMainStar");
		ImgMainStar->runAction(Sequence::create(
			DelayTime::create(0.5f),
			CCCallFuncN::create(this, callfuncN_selector(GameResultSuccess::callFuncMainStarAction)),
			nullptr));


		int rewardGoldNum = NewDataMgr::getInstance()->mGameGradeMsg->getRewardGoldWithLv(goldNum);
		auto AtlasLabelRewardGold = (TextAtlas*)GameMainLayer::seekNodeByName(_myLayer, "AtlasLabelRewardGold");
		auto ImgRewardTips = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "ImgRewardTips");

		if (!rewardGoldNum)
		{
			ImgRewardTips->setVisible(false);
		}
		else
		{
			AtlasLabelRewardGold->setString(StringUtils::format("%d", rewardGoldNum));
			ImgRewardTips->setOpacity(0);
			ImgRewardTips->setScale(5);
			ImgRewardTips->runAction(Sequence::create(
				DelayTime::create(0.5),
				CCFadeIn::create(0.01),
				CCScaleTo::create(0.1, 0.6),
				nullptr));
		}

		playStarAction();
		playWingAction();
		playTextFlash();

		NewAudio::getInstance()->playEffect(Music_GameWin);
	} while (0);


	int maxlevel = NewDataMgr::getInstance()->getCurMaxLevel();
	if (2 == maxlevel)
	{
		if ( 0 == NewDataMgr::getInstance()->mGameCardSystem->getCardConfigWithId(2)->islock)
		{
			NewDataMgr::getInstance()->mGameCardSystem->unLockCard2();
		}

		if(!NewDataMgr::getInstance()->getCardUseState())
		{
			deleyGuideCard();
		}
	}

}

void GameResultSuccess::deleyGuideCard(void)
{
	_eventDispatcher->setEnabled(true);
	CProtocolManager::getInstance()->sendNotificationMsg(MsgEventName_ToTaskBarLayer, EProtocolID_HelpFeqCard);
}

void GameResultSuccess::playStarAction(void)
{
	int curLevel = NewDataMgr::getInstance()->getCurLevel();
	int lvGrade = NewDataMgr::getInstance()->getLevelStarGrade(curLevel);

	float delayT = 2;
	for (int i = 1; i <= 3; i++)
	{
		auto ImgStar = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, StringUtils::format("ImgStar%d", i));

		if (i<=lvGrade)
		{
			playStarGradeAction(ImgStar, delayT);
			delayT += 0.4;
		}
		else
		{
			ImgStar->setVisible(false);
		}
	}
}

void GameResultSuccess::playStarGradeAction(Node* star, float delay)
{
	star->setOpacity(0);
	star->setScale(6);
	star->runAction(Sequence::create(
		DelayTime::create(delay* 0.5f + 0.5),
		Spawn::create(CCEaseSineIn::create(CCScaleTo::create(0.15f, 1.0)), FadeIn::create(0.15f), nullptr),
		CCCallFuncN::create(this, callfuncN_selector(GameResultSuccess::callFuncPlayStarAction)),
		nullptr));
}

void GameResultSuccess::playWingAction(void)
{
	auto NodeWing = (Node*)GameMainLayer::seekNodeByName(_myLayer, "NodeWing");
	NodeWing->setScale(1.2f);
	// 右流光效果
	auto imgLeftWing = Sprite::create("ccsRes/resResoutLayer/ImgWing.png");
	imgLeftWing->setAnchorPoint(Vec2(0, 0.5));

	Size clipSize = imgLeftWing->getContentSize();
	Sprite* spark = Sprite::create("FirstUIRes/Sptigeguang.png");
	spark->setScaleY(1.2f);
	//spark->setPosition(-clipSize.width, -50);

	ClippingNode* clippingLeftNode = ClippingNode::create();
	NodeWing->addChild(clippingLeftNode);

	clippingLeftNode->setAlphaThreshold(0.05f); //设置alpha闸值  
	clippingLeftNode->setContentSize(clipSize); //设置尺寸大小  

	clippingLeftNode->setStencil(imgLeftWing);   //设置模板stencil  
	clippingLeftNode->addChild(imgLeftWing, 1);  //先添加标题,会完全显示出来,因为跟模板一样大小  
	clippingLeftNode->addChild(spark, 2);       //会被裁减  

	MoveTo* moveAction = MoveTo::create(1.0f, Vec2(clipSize.width, 0));
	MoveTo* moveBackAction = MoveTo::create(0.01f, Vec2(-clipSize.width, 0));
	spark->runAction(RepeatForever::create(Sequence::create(CCDelayTime::create(1.5f), moveAction, moveBackAction, NULL)));

	// 左流光效果
	auto imgRightWing = Sprite::create("ccsRes/resResoutLayer/ImgWing.png");
	imgRightWing->setFlipX(true);
	imgRightWing->setAnchorPoint(Vec2(1, 0.5));

	clipSize = imgRightWing->getContentSize();
	Sprite* rightspark = Sprite::create("FirstUIRes/Sptigeguang.png");
	rightspark->setFlipX(true);
	rightspark->setScaleY(1.2f);
	//spark->setPosition(-clipSize.width, -50);

	ClippingNode* clippingRightNode = ClippingNode::create();
	NodeWing->addChild(clippingRightNode);

	clippingRightNode->setAlphaThreshold(0.05f); //设置alpha闸值  
	clippingRightNode->setContentSize(clipSize); //设置尺寸大小  

	clippingRightNode->setStencil(imgRightWing);   //设置模板stencil  
	clippingRightNode->addChild(imgRightWing, 1);  //先添加标题,会完全显示出来,因为跟模板一样大小  
	clippingRightNode->addChild(rightspark, 2);       //会被裁减  

	MoveTo* moveAction1 = MoveTo::create(1.0f, Vec2(-clipSize.width, 0));
	MoveTo* moveBackAction1 = MoveTo::create(0.01f, Vec2(clipSize.width, 0));
	rightspark->runAction(RepeatForever::create(Sequence::create(CCDelayTime::create(1.5f), moveAction1, moveBackAction1, NULL)));
}

void GameResultSuccess::callFuncPlayStarAction(Node* node)
{
	Sprite* star = (Sprite*)node;

	auto size = node->getContentSize();
	auto parcile = ParticleSystemQuad::create("particla/GameResoult/test1.plist");
	parcile->setScale(2.0f);
	parcile->setPosition(Vec2(size.width/2, size.height/2));
	node->addChild(parcile);
}

void GameResultSuccess::callFuncMainStarAction(Node* node)
{
	Sprite* star = (Sprite*)node;

	auto size = node->getContentSize();
	auto parcile = ParticleSystemQuad::create("particla/GameResoult/test2.plist");
	parcile->setScale(2.0f);
	parcile->setPosition(Vec2(size.width / 2, size.height / 2));
	node->addChild(parcile);
}

void GameResultSuccess::playTextFlash(void)
{
	return;
	auto StrSuc = (Sprite*)GameMainLayer::seekNodeByName(_myLayer, "StrSuc");
	auto strSucSize = StrSuc->getContentSize();

	auto imgFlash1 = Sprite::create("FirstUIRes/imgflash.png");
	auto imgFlash2 = Sprite::create("FirstUIRes/imgflash.png");
	auto imgFlash3 = Sprite::create("FirstUIRes/imgflash.png");
	auto imgFlash4 = Sprite::create("FirstUIRes/imgflash.png");
	imgFlash1->setScale(0.1f);
	imgFlash2->setScale(0.1f);
	imgFlash3->setScale(0.1f);
	imgFlash4->setScale(0.1f);

	imgFlash1->setBlendFunc(BlendFunc::ADDITIVE);
	imgFlash2->setBlendFunc(BlendFunc::ADDITIVE);
	imgFlash3->setBlendFunc(BlendFunc::ADDITIVE);
	imgFlash4->setBlendFunc(BlendFunc::ADDITIVE);
	imgFlash1->setPosition(Vec2(strSucSize.width*0.5, strSucSize.height*0.88));
	imgFlash2->setPosition(Vec2(strSucSize.width*0.05, strSucSize.height*0.3));
	imgFlash3->setPosition(Vec2(strSucSize.width*0.05, strSucSize.height*0.78));
	imgFlash4->setPosition(Vec2(strSucSize.width, strSucSize.height*0.85));
	StrSuc->addChild(imgFlash1);
	StrSuc->addChild(imgFlash2);
	StrSuc->addChild(imgFlash3);
	StrSuc->addChild(imgFlash4);

	auto action = Spawn::create(CCScaleTo::create(0.5, 1.0), CCFadeIn::create(0.5), nullptr);
	auto actionback = Spawn::create(CCScaleTo::create(0.5, 0.2), CCFadeOut::create(0.5), nullptr);
	imgFlash1->runAction(RepeatForever::create(Sequence::create(
		CCDelayTime::create(1.0f), Spawn::create(
		CCScaleTo::create(0.01, 0.2),
		CCFadeOut::create(0.01), nullptr),
		Spawn::create(CCScaleTo::create(0.5, 1.0), CCFadeIn::create(0.5), nullptr),
		Spawn::create(CCScaleTo::create(0.5, 0.2), CCFadeOut::create(0.5), nullptr),
		nullptr)));

	imgFlash2->runAction(RepeatForever::create(Sequence::create(
		CCDelayTime::create(1.2f), Spawn::create(
		CCScaleTo::create(0.01, 0.2),
		CCFadeOut::create(0.01), nullptr),
		Spawn::create(CCScaleTo::create(0.5, 0.3), CCFadeIn::create(0.5), nullptr),
		Spawn::create(CCScaleTo::create(0.5, 0.1), CCFadeOut::create(0.5), nullptr),
		nullptr)));

	imgFlash3->runAction(RepeatForever::create(Sequence::create(
		CCDelayTime::create(0.5f), Spawn::create(
		CCScaleTo::create(0.01, 0.1),
		CCFadeOut::create(0.01), nullptr),
		Spawn::create(CCScaleTo::create(0.5, 0.3), CCFadeIn::create(0.5), nullptr),
		Spawn::create(CCScaleTo::create(0.5, 0.1), CCFadeOut::create(0.5), nullptr),
		nullptr)));

	imgFlash4->runAction(RepeatForever::create(Sequence::create(
		CCDelayTime::create(1.8f), Spawn::create(
		CCScaleTo::create(0.01, 0.2),
		CCFadeOut::create(0.01), nullptr),
		Spawn::create(CCScaleTo::create(0.5, 0.3), CCFadeIn::create(0.5), nullptr),
		Spawn::create(CCScaleTo::create(0.5, 0.1), CCFadeOut::create(0.5), nullptr),
		nullptr)));
}

void GameResultSuccess::callFuncGuideCard(void)
{

}



/*******************************************
*	失败结算界面
*******************************************/
void GameResultFail::showLayout(Node* node, int tag)
{
	if (node && !node->getChildByName("GameResultFail"))
	{
		auto layout = GameResultFail::create();
		layout->setTag(tag);
		node->addChild(layout, LocalZOrde_500, "GameResultFail");
	}
}

void GameResultFail::onEnter()
{
	Layer::onEnter();

	int curLevel = NewDataMgr::getInstance()->getCurLevel() - 1;
	if (3 >= curLevel)
	{
		auto showGoodsInfoLayer = PackShowLayer::getPackShowLayer();
		showGoodsInfoLayer->setShowInfoType(PackShowLayer::PackType::InfoType_OverflowPack);
		GameInterfaceMgr::getInstance()->getParentNode()->addChild(showGoodsInfoLayer, LocalZOrde_1000);
	}
	else
	{
		auto showGoodsInfoLayer = PackShowLayer::getPackShowLayer();
		showGoodsInfoLayer->setShowInfoType(PackShowLayer::PackType::InfoType_TyrantPack);
		Director::getInstance()->getRunningScene()->addChild(showGoodsInfoLayer);
	}

	initInterface();
	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(GameResultFail::onMessageEvent), MsgEventName_ToFailResult, nullptr);
}

void GameResultFail::onExit()
{
	Layer::onExit();

	// 绉婚櫎瑙傚療鑰呮秷鎭�
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void GameResultFail::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	switch (protocolID)
	{
	default:
		break;
	}
}

void GameResultFail::setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback)
{
	do
	{
		CC_BREAK_IF(node == nullptr);

		node->setTag(tag);

		if (callback)
			node->addTouchEventListener(callback);
		else
			node->addTouchEventListener(CC_CALLBACK_2(GameResultFail::callFuncButtonEvent, this));

	} while (0);
}

void GameResultFail::callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType)) return;

	Button* btn = (Button*)ref;
	int nTag = btn->getTag();

	switch (nTag)
	{
	case Tag_Back:
		GameInterfaceMgr::getInstance()->closeAllInterface();
		break;

	case Tag_Continut:
		GameInterfaceMgr::getInstance()->closeCurInterface();
		break;

	case Tag_Again:
		GameInterfaceMgr::getInstance()->closeCurInterface();
		GameSceneMgr::getInstance()->changeScene(GameSceneType::Scene_Type_PlayInterface);
		break;

	default:
		break;
	}

}

void GameResultFail::initInterface()
{
	_myLayer = CSLoader::createNode(Ccs_ResoutFailLayer_FilePath);

	if (!_myLayer)
	{
		return;
	}

	addChild(_myLayer);

	do
	{
		// 返回
		auto ButtonBack = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonBack");
		setEventForNode(ButtonBack, Tag_Back, nullptr);

		// 鍏抽棴
		auto ButtonContinut = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonContinut");
		setEventForNode(ButtonContinut, Tag_Continut, nullptr);

		auto ButtonAgain = (Button*)GameMainLayer::seekNodeByName(_myLayer, "ButtonAgain");
		setEventForNode(ButtonAgain, Tag_Again, nullptr);

		auto AtlasLabelScore = (TextAtlas*)GameMainLayer::seekNodeByName(_myLayer, "AtlasLabelScore");
		auto AtlasLabelGold = (TextAtlas*)GameMainLayer::seekNodeByName(_myLayer, "AtlasLabelGold");

// 		AtlasLabelScore->setString(StringUtils::format("%d", GameData::getInstance()->getScoreNum()));
// 		AtlasLabelGold->setString(StringUtils::format("%d", GameData::getInstance()->getGoldNum()));

		int curLevel = NewDataMgr::getInstance()->getCurLevel();
		auto AtlasLabelLevel = (TextAtlas*)GameMainLayer::seekNodeByName(_myLayer, "AtlasLabelLevel");
		AtlasLabelLevel->setString(StringUtils::format(".%d/", curLevel));

		auto TextUpgradeTips = (Text*)GameMainLayer::seekNodeByName(_myLayer, "TextUpgradeTips");
		TextUpgradeTips->runAction(RepeatForever::create(Sequence::create(
			CCFadeOut::create(1.0f),
			DelayTime::create(0.7),
			CCFadeIn::create(0.5f),
			nullptr)));

		auto TextUpgradeTips1 = (Text*)GameMainLayer::seekNodeByName(_myLayer, "TextUpgradeTips1");
		TextUpgradeTips1->runAction(RepeatForever::create(Sequence::create(
			CCFadeOut::create(1.0f),
			DelayTime::create(0.7),
			CCFadeIn::create(0.5f),
			nullptr)));

	} while (0);
}

void GameResultFail::playStarGradeAction(Node* star, int delay)
{
	star->setOpacity(0);
	star->setScale(6);
	star->runAction(Sequence::create(
		DelayTime::create(delay* 0.5f + 0.5f),
		Spawn::create(CCEaseSineIn::create(CCScaleTo::create(0.15f, 1.0)), FadeIn::create(0.15f), nullptr),
		CCCallFuncN::create(this, callfuncN_selector(GameResultFail::callFuncPlayStarAction)),
		nullptr));
}

void GameResultFail::callFuncPlayStarAction(Node* node)
{
	Sprite* star = (Sprite*)node;

	auto size = node->getContentSize();
	auto parcile = ParticleSystemQuad::create("particla/GameResoult/test1.plist");
	parcile->setScale(2.0f);
	parcile->setPosition(Vec2(size.width / 2, size.height / 2));
	node->addChild(parcile);
}
