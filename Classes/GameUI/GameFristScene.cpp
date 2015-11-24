#include "GameFristScene.h"
#include "AudioEngine.h"
#include "../GameCommon/NewAudio.h"
#include "GameSceneMgr.h"
#include "AppDelegate.h"
#include "../GameCommon/NewDataMgr.h"
#include "../GamePlaying/RapidParticle/ParticleManager.h"
USING_NS_CC;

Scene* GameFristScene::createScene()
{
	auto scene = Scene::create();

	auto layer = GameFristScene::create();
	scene->addChild(layer);

	return scene;
}

void GameFristScene::onEnter()
{
	Layer::onEnter();
	
	/*Director::getInstance()->getTextureCache()->addImage("animation/FristInterfaceAnim/AnimMaoF1.png");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("animation/FristInterfaceAnim/AnimMaoF1.plist");

	Director::getInstance()->getTextureCache()->addImage("animation/FristInterfaceAnim/AnimMaoF2.png");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("animation/FristInterfaceAnim/AnimMaoF2.plist");

	Director::getInstance()->getTextureCache()->addImage("animation/FristInterfaceAnim/AnimNongm.png");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("animation/FristInterfaceAnim/AnimNongm.plist");

	Director::getInstance()->getTextureCache()->addImage("animation/FristInterfaceAnim/AnimXiong.png");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("animation/FristInterfaceAnim/AnimXiong.plist");*/

	initInterface();
	// 闊充箰
	//NewAudio::getInstance()->playBgMusicForce(Music_Bg);
	//NewAudio::getInstance()->playBgMusicForce(Music_Bg);

}

bool GameFristScene::onTouchBegan(Touch *touch, Event *unusedEvent)
{
	return true;
}

void GameFristScene::onTouchMoved(Touch *touch, Event *unusedEvent)
{
}

void GameFristScene::onTouchEnded( Touch *touch, Event *unusedEvent )
{
	if (_listener)
	{
		_eventDispatcher->removeEventListener(_listener);
	}
	GameSceneMgr::getInstance()->changeScene(Scene_Type_PlayILoading);
}

void GameFristScene::initInterface( void )
{
	auto backgroudSpr = Sprite::create("FirstUIRes/firstBg.jpg");
	backgroudSpr->setAnchorPoint( Vec2(0.0f, 0.0f) );
	addChild( backgroudSpr );

	// Register Touch Event
	_listener = EventListenerTouchOneByOne::create();
	_listener->onTouchBegan = CC_CALLBACK_2(GameFristScene::onTouchBegan, this);
	_listener->onTouchMoved = CC_CALLBACK_2(GameFristScene::onTouchMoved, this);
	_listener->onTouchEnded = CC_CALLBACK_2(GameFristScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, backgroudSpr);

	// 触摸继续提示
	auto tips = Sprite::create("FirstUIRes/tips.png");
	tips->setPosition(Vec2(backgroudSpr->getContentSize().width*0.5,
		backgroudSpr->getContentSize().height*0.25));
	addChild(tips,10);
	ActionInterval* fadein = FadeIn::create(0.5f);
	ActionInterval* fadeout = FadeOut::create(0.5f);
	ActionInterval* seq = Sequence::create(fadein, fadeout, nullptr);
	ActionInterval* rep = RepeatForever::create(seq);
	tips->runAction(rep);

#if 0

	auto imgLogoBg = Sprite::create("FirstUIRes/spriteWidget1.png");
	imgLogoBg->setPosition(Vec2(backgroudSpr->getContentSize().width*0.5, backgroudSpr->getContentSize().height*0.88+17));
	this->addChild(imgLogoBg, 1);
	imgLogoBg->setScale(5.0f);
	//imgLogoBg->setOpacity(0);
	imgLogoBg->runAction(CCSequence::create(
		//DelayTime::create(1.0f),
		//CCFadeIn::create(0.01f),
		CCEaseBackOut::create(CCScaleTo::create(0.2, 1.0)),
		CCDelayTime::create(0.8),
		nullptr));

	auto gameLogoFream = Sprite::create("FirstUIRes/ImgLogo.png");
	auto gameLogo = Sprite::create("FirstUIRes/spriteWidget1.png");
	gameLogo->setPosition(Vec2(0,17));


	//gameLogo->setScale(1.1);

	// 字体流光效果
	Size clipSize = gameLogo->getContentSize();
	Sprite* spark = Sprite::create("FirstUIRes/Sptigeguang.png");
	spark->setPosition(-clipSize.width, -50);

	ClippingNode* clippingNode = ClippingNode::create();
	clippingNode->setPosition(Vec2(backgroudSpr->getContentSize().width*0.5, backgroudSpr->getContentSize().height*0.88));
	this->addChild(clippingNode, 5);

	clippingNode->setScale(5.0f);
	//imgLogoBg->setOpacity(0);
	clippingNode->runAction(CCSequence::create(
		//DelayTime::create(1.0f),
		//CCFadeIn::create(0.01f),
		CCEaseBackOut::create(CCScaleTo::create(0.2, 1.0)),
		CCDelayTime::create(0.8),
		nullptr));

	clippingNode->setAlphaThreshold(0.05f); //设置alpha闸值  
	clippingNode->setContentSize(clipSize); //设置尺寸大小  

	clippingNode->setStencil(gameLogoFream);   //设置模板stencil  
	clippingNode->addChild(gameLogoFream, 1);  //先添加标题,会完全显示出来,因为跟模板一样大小  
	clippingNode->addChild(gameLogo, 4);  //先添加标题,会完全显示出来,因为跟模板一样大小  
	clippingNode->addChild(spark, 5);       //会被裁减  

	MoveTo* moveAction = MoveTo::create(2.0f, Vec2(clipSize.width,0));
	MoveTo* moveBackAction = MoveTo::create(0.01f, Vec2(-clipSize.width, 0));
	spark->runAction(RepeatForever::create(Sequence::create(CCDelayTime::create(1.5f),moveAction, moveBackAction, NULL)));


	// 动画部分
	auto nongNode = Node::create();
	addChild(nongNode,1);

	auto imgNong = Sprite::create("FirstUIRes/imgNong.png");
	auto ImgNongL = Sprite::create("FirstUIRes/ImgNongL.png");
	auto ImgNongR = Sprite::create("FirstUIRes/ImgNongR.png");

	imgNong->setPosition(Vec2(360,640));
	nongNode->addChild(imgNong, 2);
	nongNode->runAction(CCRepeatForever::create(Sequence::create(CCMoveTo::create(1.2, Vec2(0, -8)), CCMoveTo::create(1.5, Vec2(0, 0)), nullptr)));
	
	ImgNongL->setPosition(Vec2(474, 207));
	ImgNongL->setAnchorPoint(Vec2(0.2,0.1));
	nongNode->addChild(ImgNongL);
	ImgNongL->runAction(CCRepeatForever::create(Sequence::create(CCRotateTo::create(2.0, 4), CCRotateTo::create(1.0, 0), nullptr)));

	ImgNongR->setPosition(Vec2(192, 183));
	ImgNongR->setAnchorPoint(Vec2(0.8,0.1));
	nongNode->addChild(ImgNongR);
	ImgNongR->runAction(CCRepeatForever::create(Sequence::create(CCRotateTo::create(2.0, -4), CCRotateTo::create(1.0, 0), nullptr)));

	addAnimNode("AnimMF",	15,0.1,Vec2(326,564), nongNode);
	addAnimNode("Anim1Mf",	15,0.1,Vec2(350,532), nongNode);
	addAnimNode("AnimX",	15,0.1,Vec2(384,442), nongNode);
	addAnimNode("AnimNm",	15,0.1,Vec2(460,277), nongNode);


	auto huoL = ParticleSystemQuad::create("particla/FristParticla/huo.plist");
	huoL->setPosition(Vec2(666, 254));
	huoL->setRotation(-35);
	addChild(huoL,0);

	auto huoR = ParticleSystemQuad::create("particla/FristParticla/huo.plist");
	huoR->setPosition(Vec2(45, 230));
	huoR->setRotation(35);
	addChild(huoR, 0);

	auto test2yanjingyou = ParticleSystemQuad::create("particla/FristParticla/test2yanjingyou.plist");
	test2yanjingyou->setPosition(Vec2(400, 999));
	addChild(test2yanjingyou, 0);

	auto test2yanjingzuo = ParticleSystemQuad::create("particla/FristParticla/test2yanjingzuo.plist");
	test2yanjingzuo->setPosition(Vec2(355, 999));
	addChild(test2yanjingzuo, 0);


	auto testlizi = ParticleSystemQuad::create("particla/FristParticla/testlizi01.plist");
	testlizi->setPosition(Vec2(762, 284));
	addChild(testlizi, 2);

	auto testlizi01 = ParticleSystemQuad::create("particla/FristParticla/testlizi01.plist");
	testlizi01->setPosition(Vec2(734, 32));
	addChild(testlizi01, 2);
	auto testlizi02 = ParticleSystemQuad::create("particla/FristParticla/testlizi02.plist");
	testlizi02->setPosition(Vec2(138, -21));
	addChild(testlizi02, 2);
	auto testlizi03 = ParticleSystemQuad::create("particla/FristParticla/testlizi03.plist");
	testlizi03->setPosition(Vec2(580, -33));
	addChild(testlizi03, 2);
	auto testlizi04 = ParticleSystemQuad::create("particla/FristParticla/testlizi04.plist");
	testlizi04->setPosition(Vec2(328, -31));
	addChild(testlizi04, 2);


#endif
}

void GameFristScene::onExit( void )
{
	Layer::onExit();


}

void GameFristScene::callFuncPlayEffect(Node* node)
{
	for (int i = 1; i <= 4; i++)
	{
		auto parcile = ParticleSystemQuad::create(StringUtils::format("particla/firstUI/test%d.plist", i));
		parcile->setPosition(node->getParent()->convertToWorldSpaceAR(node->getPosition()));
		parcile->setScale(2.5f);
		parcile->setBlendFunc(BlendFunc::ADDITIVE);
		addChild(parcile);
	}
}

void GameFristScene::addAnimNode(char* name, int animoNum, float delay, Vec2 position, Node* parent)
{
	Vector<SpriteFrame*> frame_array;

	for (int i = 1; i <= animoNum; ++i)
	{
		std::string pic = CCString::createWithFormat("%s%d.png", name, i)->getCString();
		CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pic.c_str());

		frame_array.pushBack(frame);
	}

	auto p_animation = Animation::createWithSpriteFrames(frame_array, 0.1f);
	p_animation->setLoops(-1);

	auto animate = Animate::create(p_animation);

	auto AnimNode = Sprite::create("weapon.png");
	AnimNode->setPosition(position);
	parent->addChild(AnimNode, 50);

	AnimNode->runAction(Sequence::create(
		animate,
		nullptr));
}


