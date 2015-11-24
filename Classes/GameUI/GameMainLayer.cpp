#include "GameMainLayer.h"
#include "../GameCommon/GameData.h"
#include "../GameCommon/GameDefine.h"
#include "../GameCommon/EffectsManager.h"
#include "../GamePlaying/BasePlane/BasePlane.h"
#include "../GameCommon/NewDataMgr.h"
#include "../GamePlaying/Layer/UIController.h"
#include "../GamePlaying/Layer/PhysicsLayer.h"
#include "../JniHelp/CppCallJava.h"
#include "../GameCommon/CommonRes.h"
#include "GameSceneMgr.h"
#include "../GamePlaying/Layer/RollingBg.h"
#include "../GameUtil/StatisticsHelp.h"


bool GameMainLayer::init()
{
	bool ret = false;
	if ( Layer::init() )
	{
		ret = true;
	}

	return ret;
}

void GameMainLayer::onEnter()
{
	Layer::onEnter();

	int lifeNum = NewDataMgr::getInstance()->getGoodsNumWithID(GoodsID_Life);

	// 进入游戏至少2条生命
	if ( 2 > lifeNum )
	{
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Life, 2 - lifeNum);
	}

	initInterface();

	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(GameMainLayer::onMessageEvent), MsgEventName_ToGameMainLayer, nullptr);
}

void GameMainLayer::onExit()
{
	Layer::onExit();

	NotificationCenter::getInstance()->removeAllObservers(this);
}

void GameMainLayer::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	updateGoodsNum();

	GameData::getInstance()->GamePause();
	switch (protocolID)
	{
	case EProtocolID_DaHudun:
	{
		PauseLayer* layer = PauseLayer::create();
		layer->setActivateType(PauseLayer::ActivateTag_HuDun);
		Director::getInstance()->getRunningScene()->addChild(layer, 10000);
		this->m_fighting_shield_num += 3;
		this->updateGoodsNum();
	}
		break;
	case EProtocolID_TyrantPackBuy:
    {

		NewDataMgr::getInstance()->setTyrantPackBuyState(true);

		PauseLayer* layer = PauseLayer::create();
		layer->setActivateType(PauseLayer::ActivateTag_HuDun);
		Director::getInstance()->getRunningScene()->addChild(layer, 10000);

		this->m_fighting_shield_num += 12;
		this->m_fighting_skill_num += 12;
		this->updateGoodsNum();
    }
		break;
	case EProtocolID_PropPackBuy:
	{
		NewDataMgr::getInstance()->setTyrantPackBuyState(true);

		PauseLayer* layer = PauseLayer::create();
		layer->setActivateType(PauseLayer::ActivateTag_HuDun);
		Director::getInstance()->getRunningScene()->addChild(layer, 10000);

		this->m_fighting_shield_num += 3;
		this->m_fighting_skill_num += 3;
		this->updateGoodsNum();
	}
		break;

	case EProtocolID_DaZhaoBuy:
	{
		PauseLayer* layer = PauseLayer::create();
		layer->setActivateType(PauseLayer::ActivateTag_DaZhao);
		Director::getInstance()->getRunningScene()->addChild(layer, 10000);
		this->m_fighting_skill_num += 3;
		this->updateGoodsNum();
	}
		break;

	default:
		break;
	}
}

void GameMainLayer::initInterface( void )
{
	_mainLayerRootNode = CSLoader::createNode( Ccs_GameMainLayer_FilePath );

	if ( !_mainLayerRootNode )
	{
		return;
	}
	addChild( _mainLayerRootNode );
	
	Button* BtnPause = (Button*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "ButtonMenu" );
	BtnPause->addClickEventListener(CC_CALLBACK_1(GameMainLayer::callbackPause, this));

	Button* ButtonDaZhao = (Button*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "ButtonDaZhao" );
	ButtonDaZhao->addClickEventListener(CC_CALLBACK_1(GameMainLayer::callbackDazhao, this));

	Button* ButtonPack = (Button*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "ButtonPack");
	ButtonPack->addClickEventListener(CC_CALLBACK_1(GameMainLayer::callbackButtonPack, this));

	Button* ButtonHuDun = (Button*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "ButtonHuDun");
	ButtonHuDun->addClickEventListener(CC_CALLBACK_1(GameMainLayer::callbackButtonHudun, this));

	if ( 0 == NewDataMgr::getInstance()->getCurLevel())
	{
		ButtonDaZhao->setTouchEnabled(false);
		ButtonHuDun->setTouchEnabled(false);
		ButtonDaZhao->setColor(Color3B(47, 47, 47));
		ButtonHuDun->setColor(Color3B(47, 47, 47));

	}

    this->m_fighting_shield_num = NewDataMgr::getInstance()->getGoodsNumWithID(GoodsID_HuDun);
    this->m_fighting_skill_num = NewDataMgr::getInstance()->getGoodsNumWithID(GoodsID_DaZhao);
	updateGoodsNum();

	updataGamedistance(0);

	updateUIGold( 0 );

	auto ImgBtnGiftFlash = (Sprite*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "ImgBtnGiftFlash"); 
	ImgBtnGiftFlash->runAction(RepeatForever::create(Sequence::create(
		CCFadeOut::create(0.5f),
		CCFadeIn::create(0.5f),
		nullptr)));

	//schedule(schedule_selector(GameMainLayer::HpDecrease),0.1);
}

void GameMainLayer::updateGoodsNum(void)
{
//	int dazhaoNum = NewDataMgr::getInstance()->getGoodsNumWithID(GoodsID_DaZhao);
//	int huDunNum = NewDataMgr::getInstance()->getGoodsNumWithID(GoodsID_HuDun);
    int dazhaoNum = this->m_fighting_skill_num;
    int huDunNum = this->m_fighting_shield_num;
	int lifeNum = NewDataMgr::getInstance()->getGoodsNumWithID(GoodsID_Life);
	auto AtlasLabelLifeNum = (TextAtlas*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "AtlasLabelLifeNum");
	auto AtlasLabelDazhaoNum = (TextAtlas*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "AtlasLabelDazhaoNum");
	auto AtlasLabelHudunNum = (TextAtlas*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "AtlasLabelHudunNum");

	
	AtlasLabelLifeNum->setString(StringUtils::format(".%d", lifeNum));
	AtlasLabelDazhaoNum->setString(StringUtils::format("%d", dazhaoNum));
	AtlasLabelHudunNum->setString(StringUtils::format("%d", huDunNum));

	auto ImgDaZhaoAdd = (Sprite*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "ImgDaZhaoAdd");
	ImgDaZhaoAdd->setVisible((dazhaoNum) ? false : true);

	auto ImgHudunAdd = (Sprite*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "ImgHudunAdd");
	ImgHudunAdd->setVisible((huDunNum) ? false : true);

	auto ImgDaZhaoTipsFlash = (Sprite*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "ImgDaZhaoTipsFlash"); ImgDaZhaoTipsFlash->setVisible(false);
	auto ImgHudunTipsFlash = (Sprite*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "ImgHudunTipsFlash"); ImgHudunTipsFlash->setVisible(false);
	if (!dazhaoNum)
	{
		ImgDaZhaoTipsFlash->setVisible(true);
		ImgDaZhaoTipsFlash->runAction(RepeatForever::create(Sequence::create(
			CCFadeOut::create(0.5f),
			CCFadeIn::create(0.5f),
			nullptr)));
	}
	if (!huDunNum)
	{
		ImgHudunTipsFlash->setVisible(true);
		ImgHudunTipsFlash->runAction(RepeatForever::create(Sequence::create(
			CCFadeOut::create(0.5f),
			CCFadeIn::create(0.5f),
			nullptr)));
	}

	// 生命显示处理
	AtlasLabelLifeNum->setVisible((4 < lifeNum)?true:false);

	for (int i = 1; i <= 4; i++ )
	{
		auto imgLifeIco = (Sprite*)GameMainLayer::seekNodeByName(_mainLayerRootNode, StringUtils::format("ImgLife%d", i));

		if ( 4 < lifeNum )
		{
			imgLifeIco->setVisible((1 == i)?true:false);
		}
		else
		{
			imgLifeIco->setVisible((lifeNum >= i) ? true : false);
		}
	}

	
}


void GameMainLayer::updateUIGold( int goldnum )
{
	int curScore = GameData::getInstance()->getGoldNum();

	// 更新成绩
	auto atlasLabelScore = (TextAtlas*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "AtlasLabelGold" );
	atlasLabelScore->setString( __String::createWithFormat("%d", curScore)->getCString() );
}


void GameMainLayer::updateUIScore( int score )
{
	// 更新成绩
	auto atlasLabelScore = (TextAtlas*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "AtlasLabelScore" );
	atlasLabelScore->setString( __String::createWithFormat("%d", score)->getCString() );
}

Node* GameMainLayer::createEnergyFullAnim( Vec2 ps )
{
	Vector<SpriteFrame*> frame_array;

	for (int i = 0 ; i <=4; ++i)
	{
		std::string pic = CCString::createWithFormat("%s%d.png","Electric_",i)->getCString();
		CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pic.c_str());

		frame_array.pushBack(frame);
	}
	auto loadingBarEnergy			= (LoadingBar*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "LoadingBarEnergy" );

	auto p_animation = Animation::createWithSpriteFrames(frame_array,0.1f);
	p_animation->setLoops(1);

	auto animate =  Animate::create(p_animation) ;

	Vec2 pos = loadingBarEnergy->convertToWorldSpace( loadingBarEnergy->getPosition());

	auto upgradeAnimNode = Sprite::create("weapon.png");
	upgradeAnimNode->setPosition( Vec2(pos.x,pos.y+CCRANDOM_0_1()*600) );// 在Y值高高为600内随机位置
	addChild( upgradeAnimNode );

	upgradeAnimNode->runAction(Sequence::create( 
		animate, 
		CallFuncN::create(CC_CALLBACK_1(GameMainLayer::removeEnergyAnim, this)),
		nullptr ));

	return nullptr;
}

void GameMainLayer::removeEnergyAnim( Node* node )
{
	node->removeFromParentAndCleanup(true);
}

void GameMainLayer::callFuncRandAnim( float t )
{
	if ( !_decayIng )
	{
		// 创建闪电
		createEnergyFullAnim(Vec2(0,0));

		// 随机一个间隔进行创建闪电
		schedule(CC_SCHEDULE_SELECTOR(GameMainLayer::callFuncRandAnim), CCRANDOM_0_1()*5*0.1 );
	}
	else
	{
		unschedule( CC_SCHEDULE_SELECTOR(GameMainLayer::callFuncRandAnim) );
	}
}

void GameMainLayer::playLifeBarWarning( void )
{
	auto loadingBarEnergy = (Sprite*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "imgloadbg" );

	loadingBarEnergy->runAction( RepeatForever::create(
		Sequence::create(TintTo::create(1.0f, 255, 75,75),TintTo::create(1.0f, 255, 255,255),nullptr)));
}

void GameMainLayer::AddHp( float num )
{
	return;

	//每次被撞前，先判断HP是否为0：
	//@ 如果为0，那么此次撞击造成伤害的话，如果有道具，则加上HP；否则主飞机死亡；
	//@ 如果还有HP，那么正常计算HP的加减，HP值减到0，飞机死亡
	if (m_isDead)
	{
		return;
	}
	if (!m_isCanBeHurt)
	{
		return;		//受到伤害期间不再受伤害
	}
	auto mianpl = GameData::getInstance()->getMainPlane();

	if (num < 0)
	{
		mianpl->WeaponDowngrade(); //武器降级

		//受到伤害时，飞机闪烁，屏幕闪烁
		CppCallJava::getInstance()->vibrate(350);
		UIController::getInstance()->getPhysicsLayer()->BeHurtWarning();
		NotificationCenter::getInstance()->postNotification(Msg_Shake,nullptr);
		WudiSometime(2.0f);
		mianpl->TintWhenHurt(2.0f);
	}
	bool isDead = false;
	m_CurHp += num;

	if (m_CurHp <= 0)	//HP是否为0
	{
		if (num < 0 )	//是否有伤害值
		{
			isDead = true;
		}
	}

	if (m_CurHp > m_HpLimit)
	{
		m_CurHp = m_HpLimit;
	}
	else if(m_CurHp < 0.0f)
	{
		m_CurHp = 0.0;
	}

	float p = m_CurHp/m_HpLimit * 100;
	auto hpBar = (LoadingBar*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "LoadingBarEnergy" );
	hpBar->setPercent(p);

	auto barbg = (Sprite*)GameMainLayer::seekNodeByName( _mainLayerRootNode,"imgloadbg" );
	barbg->stopActionByTag(20150328);
	barbg->runAction( TintTo::create(0.1f, 255,255,255));

	//飞机死亡操作
	if (isDead)
	{
		int curRole = NewDataMgr::getInstance()->mRoleConfig.roleIndex;
		switch (curRole)
		{
		case 1:
			NewAudio::getInstance()->playEffect(Music_MainPlDeadVoice1);
			break;
		case 2:
			NewAudio::getInstance()->playEffect(Music_MainPlDeadVoice2);
			break;
		case 3:
			NewAudio::getInstance()->playEffect(Music_MainPlDeadVoice3);
			break;

		default:
			break;
		}

		if (mianpl)
		{
			mianpl->setShootAble(false);
			mianpl->clearWeapon();
			mianpl->setTouchAble(false);
			mianpl->setCantContact();
		}

		CallFunc* func = CallFunc::create(this,callfunc_selector(GameMainLayer::GameFailed));

		//直接被打死,弹出游戏失败界面
		UIController::getInstance()->getEffectsLayer()->playActionOfMainPlDead(mianpl,(ActionInterval*)func);
	}
}

void GameMainLayer::GameFailed()
{
	GameData::getInstance()->getMainPlane()->DeathForce();
	GameData::getInstance()->GameOver(false);
}

void GameMainLayer::showBuyFuHuoWidget( void )
{
	BuyFuHuo::showLayout( this );
}

void GameMainLayer::showSettingWidget( void )
{
	SettingLayer::showLayout( this );
}

void GameMainLayer::buyFuhuoBtnEvent( CommonWidget::EventKey key )
{
	switch (key)
	{
	case CommonWidget::EventKey::EventKey_Exit:
		break;
	case CommonWidget::EventKey::EventKey_Btn1:
		break;
	case CommonWidget::EventKey::EventKey_Btn2:
		break;
	default:
		break;
	}
}

void GameMainLayer::buyFuhuoCancel( void )
{
	GameData::getInstance()->GameResume();

	// 取消购买 失败
	UIController::getInstance()->ShowResultLayer(false);
}

void GameMainLayer::BuyFuHuoConfirm( void )
{
	GameData::getInstance()->GameResume();

	//游戏状态
	GameData::getInstance()->setisGamePlaying(true);

	//死亡状态
	UIController::getInstance()->getMainUILayer()->SetRealDead(false);

	//血量回满
	UIController::getInstance()->getMainUILayer()->AddHp(10000);
	UIController::getInstance()->getPhysicsLayer()->addPlaneWithProtect();
}

void GameMainLayer::callbackPause( Ref* ref )
{
	UIController::getInstance()->getPhysicsLayer()->doPause();
}

void GameMainLayer::callbackDazhao( Ref* ref )
{
	// 大招道具处理
//	if (0 >= NewDataMgr::getInstance()->getGoodsNumWithID(GoodsID_DaZhao))
	if(!this->skillIsEnough())
	{
		GameData::getInstance()->GamePause();

		auto showGoodsInfoLayer = PackShowLayer::getPackShowLayer();
		showGoodsInfoLayer->setShowInfoType(PackShowLayer::PackType::InfoType_PropPack);
		showGoodsInfoLayer->setTag(1);// 这个标志对应游戏内处理
		addChild(showGoodsInfoLayer);

		return;
	}

	startDaZhao();
}

Node * GameMainLayer::seekNodeByName(Node* widget, const std::string &name ) 
{ 
	if(!widget || widget->getName() == name) 
		return widget; 

	auto vecChildren = &widget->getChildren(); 
	std::vector<decltype(vecChildren)>vec; 
	vec.push_back(vecChildren); 
	size_t index = 0; 
	do  
	{ 
		vecChildren = vec[index]; 
		for(auto node : *vecChildren) 
		{ 
			if(node->getName() == name) 
			{ 
				return node; 
			} 
			vec.push_back(&node->getChildren()); 
		} 
		++index; 
	} while (index != vec.size()); 

	return nullptr; 
}

void GameMainLayer::GamePause()
{
	GameData::getInstance()->GamePause();
	UIController::getInstance()->getMainUILayer()->showSettingWidget();
}

void GameMainLayer::DelayPuse(Node* node)
{
	ActionInterval* delay = DelayTime::create(0.15f);
	CallFunc* func = CallFunc::create(nullptr,callfunc_selector(GameMainLayer::GamePause));
	ActionInterval* seq = Sequence::create(delay, func, nullptr);
	node->runAction(seq);
}

void GameMainLayer::WudiSometime(float dur)
{
	m_isCanBeHurt = false;
	auto node = getChildByTag(4251206);
	if (node)
	{
		node->removeFromParentAndCleanup(true);
	}
	node = Node::create();
	addChild(node,0,4251206);

	ActionInterval* delay = DelayTime::create(dur);
	CallFuncN* funN = CallFuncN::create(this,callfuncN_selector(GameMainLayer::WudiOver));
	ActionInterval* seq = Sequence::create(delay, funN, nullptr);
	node->runAction(seq);
}

void GameMainLayer::WudiOver(Node* node)
{
	m_isCanBeHurt = true;
	node->removeFromParentAndCleanup(true);
}

void GameMainLayer::SetRealDead(bool b)
{
	m_isDead = b;
}

void GameMainLayer::playPassSucceed( void )
{
	auto ws = Director::getInstance()->getWinSize();

	auto nodePassSuccee = Node::create();
	nodePassSuccee->setPosition( Vec2(ws.width*0.55, ws.height * 0.82) );
	addChild(nodePassSuccee,1000);

	auto spriteYuan = Sprite::create( "Effect/passSuccee/zhandou4.png" );
	auto spriteZhen = Sprite::create( "Effect/passSuccee/zhandou3.png" );
	auto spriteChen = Sprite::create( "Effect/passSuccee/zhandou1.png" );
	auto spriteGong = Sprite::create( "Effect/passSuccee/zhandou2.png" );

	auto spriteTopGuang = Sprite::create( "Effect/passSuccee/guang2.png" );
	auto spriteDownGuang = Sprite::create( "Effect/passSuccee/guang2.png" );

	spriteYuan->setPosition( Vec2(-250, 0) );spriteYuan->setOpacity( 0 );spriteYuan->setScale( 1.4f );
	nodePassSuccee->addChild( spriteYuan, 0, "yuanNode" );
	spriteZhen->setPosition( Vec2(-100, 0) );spriteZhen->setOpacity( 0 );spriteZhen->setScale( 1.4f );
	nodePassSuccee->addChild( spriteZhen , 0, "zhenNode");
	spriteChen->setPosition( Vec2(50, 0) );spriteChen->setOpacity( 0 );spriteChen->setScale( 1.4f );
	nodePassSuccee->addChild( spriteChen  , 0, "chenNode");
	spriteGong->setPosition( Vec2(200, 0) );spriteGong->setOpacity( 0 );spriteGong->setScale( 1.4f );
	nodePassSuccee->addChild( spriteGong  , 0, "gongNode");



	float fadeInT = 0.2f;
	float delayT = 0.1f;


	spriteYuan->runAction( Sequence::create( CCDelayTime::create(delayT*1)	
		,CCSpawn::create(
		CCCallFuncN::create(this, callfuncN_selector(GameMainLayer::callFuncGuang1)),CCFadeIn::create(fadeInT), CCScaleTo::create( fadeInT ,1.0f ), nullptr)
		,nullptr ) );


	spriteZhen->runAction( Sequence::create( CCDelayTime::create(delayT*2)	
		,CCSpawn::create(
		CCCallFuncN::create(this, callfuncN_selector(GameMainLayer::callFuncGuang1)),CCFadeIn::create(fadeInT), CCScaleTo::create( fadeInT ,1.0f ), nullptr)
		,nullptr ) );


	spriteChen->runAction( Sequence::create( CCDelayTime::create(delayT*3)	
		,CCSpawn::create(
		CCCallFuncN::create(this, callfuncN_selector(GameMainLayer::callFuncGuang1)),CCFadeIn::create(fadeInT), CCScaleTo::create( fadeInT ,1.0f ), nullptr)
		,nullptr ) );


	spriteGong->runAction( Sequence::create( CCDelayTime::create(delayT*4)	
		,CCSpawn::create(
		CCCallFuncN::create(this, callfuncN_selector(GameMainLayer::callFuncGuang1)),CCFadeIn::create(fadeInT), CCScaleTo::create( fadeInT ,1.0f ), nullptr)
		,nullptr ) );


	spriteTopGuang->setPosition( Vec2(-300, 55 ) );spriteTopGuang->setOpacity( 0 );spriteTopGuang->setScaleX( 1.5f );
	nodePassSuccee->addChild( spriteTopGuang );

	spriteDownGuang->setPosition(Vec2(250, -55 )  );spriteDownGuang->setOpacity( 0 );spriteDownGuang->setScaleX( 1.5f );
	nodePassSuccee->addChild( spriteDownGuang );

	spriteTopGuang->runAction( Sequence::create(
		CCFadeIn::create(delayT*6)
		,CCMoveTo::create(0.5, spriteTopGuang->getPosition() + Vec2(600, 0) )
		,CCFadeOut::create(delayT*2)
		,nullptr ) );

	spriteDownGuang->runAction( Sequence::create(
		CCFadeIn::create(delayT*6)
		,CCMoveTo::create(0.5, spriteDownGuang->getPosition() + Vec2(-600, 0) )
		,CCFadeOut::create(delayT*2)
		,DelayTime::create( 0.5f)
		,CCCallFuncN::create(this, callfuncN_selector(GameMainLayer::callFuncNodeMove))
		,nullptr ) );

}

void GameMainLayer::callFuncGuang1( Node* node )
{
	auto spriteYuan = Sprite::create( "Effect/passSuccee/glow_0.png" );
	spriteYuan->setPosition( node->getContentSize()/2);
	node->addChild( spriteYuan );

	spriteYuan->setScale( 1.5f);
	spriteYuan->runAction( Sequence::create(
		CCSpawn::create(CCFadeOut::create(0.3f), CCScaleTo::create( 0.3f ,0.7f ), nullptr)
		,CCScaleTo::create( 0.01f ,1.5f )
		,DelayTime::create(0.3f)
		,CCFadeIn::create(0.01f)
		,DelayTime::create(0.1f)
		,CCFadeOut::create(0.02f)
		,nullptr ) );
}

void GameMainLayer::callFuncNodeMove( Node* node )
{
	auto nodePassSuccee = (Node*)GameMainLayer::seekNodeByName( _mainLayerRootNode, "NodePassSuccee" );
	nodePassSuccee->runAction( Spawn::create( MoveTo::create( 0.8f,  nodePassSuccee->getPosition() + Vec2(600, 0)), FadeOut::create( 0.7f ), nullptr));
}

void GameMainLayer::setDaZhaoState( bool state )
{
	Button* ButtonDaZhao = (Button*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "ButtonDaZhao" );

	if (!state || 0 == NewDataMgr::getInstance()->mGameBuySystem->getBuyGoodsDataWithID(BuyKey_BuyDaZhao)->number)
	{
		ButtonDaZhao->setTouchEnabled( false );
		//ButtonDaZhao->setColor( Color3B(74,74,74) );
	}
	else
	{
		ButtonDaZhao->setTouchEnabled( true );
		//ButtonDaZhao->setColor( Color3B(255,255,255) );
	}
}

void GameMainLayer::updataGamedistance( int km )
{
	auto SpriteKmIco = (Sprite*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "SpriteKmIco" );
	// 更新成绩
	auto AtlasLabelDistance = (TextAtlas*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "AtlasLabelDistance" );
	AtlasLabelDistance->setString( StringUtils::format("%d", km) );

	// 偏移KM 
	SpriteKmIco->setPosition( Vec2(AtlasLabelDistance->getContentSize().width,SpriteKmIco->getPosition().y) );
}

void GameMainLayer::playDaZhaoCD( void )
{
	Button* ButtonDaZhao = (Button*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "ButtonDaZhao" );

	auto s=CCSprite::create("ccsRes/resTaskLayer/renwu5.png");
	s->setColor(Color3B(47, 47, 47));
	s->setOpacity( 150 );
	CCProgressTimer *pt=CCProgressTimer::create(s);
	pt->setScale(0.91);
	pt->setReverseProgress(true);
	pt->setPosition( ButtonDaZhao->getPosition() );
	pt->setType(cocos2d::CCProgressTimerType(kCCProgressTimerTypeRadial));
	ButtonDaZhao->getParent()->addChild( pt );
	auto to1 = Sequence::createWithTwoActions(ProgressFromTo::create(0, 0, 0), ProgressFromTo::create(5, 100, 0));
	pt->runAction( Sequence::create(
		to1, 
		CCCallFuncN::create( this, callfuncN_selector(GameMainLayer::callFuncDaZhaoCD) ),
		nullptr));
}

void GameMainLayer::callFuncDaZhaoCD( Node* node )
{
	node->removeFromParentAndCleanup( true );

	setDaZhaoState( true );
}

void GameMainLayer::startDaZhaoSD( void )
{
	playDaZhaoCD();
}

void GameMainLayer::callFuncWidgetBuyDz( void )
{
	int goodsprice = NewDataMgr::getInstance()->mGameBuySystem->getBuyGoodsDataWithID(BuyKey_BuyDaZhao)->Gold * 2;

	if (NewDataMgr::getInstance()->getGoodsNumWithID(GoodsID_Gold) < goodsprice)
	{
		auto widget = CommonWidget::create();
		widget->setContent("金币不足!");
		widget->addBtn1ClickEventListener( CC_CALLBACK_0(GameMainLayer::callWidgetGoldTips, this) );
		widget->addBtnExitClickEventListener( CC_CALLBACK_0(GameMainLayer::callWidgetGoldTips, this) );
		addChild( widget );
		return;
	}
	// 减金币
	NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Gold, -goodsprice);

	setDaZhaoState( false );
	GameData::getInstance()->GameResume();

	int curRole = NewDataMgr::getInstance()->mRoleConfig.roleIndex;

	SpecialSkillTips::StartSpecialSkill();
}

void GameMainLayer::callFuncWidgetBuyDzExit( void )
{
	GameData::getInstance()->GameResume();
}

void GameMainLayer::callWidgetGoldTips( void )
{
	GameData::getInstance()->GameResume();
}

void GameMainLayer::callbackButtonHudun(Ref* ref)
{
	bool isValid = GameData::getInstance()->getIsRewardValid(); 
	if (!isValid)
	{
		return;
	}
//	if (0 >= NewDataMgr::getInstance()->getGoodsNumWithID(GoodsID_HuDun))
	if(!this->shieldIsEnough())
	{
		GameData::getInstance()->GamePause();

		auto showGoodsInfoLayer = PackShowLayer::getPackShowLayer();
		showGoodsInfoLayer->setShowInfoType(PackShowLayer::PackType::InfoType_PropPack);
		showGoodsInfoLayer->setTag(1);// 这个标志对应游戏内处理
		addChild(showGoodsInfoLayer);
		return;
	}

	startHuDun();
}

void GameMainLayer::callbackButtonPack(Ref* ref)
{
	GameData::getInstance()->GamePause();

	auto showGoodsInfoLayer = PackShowLayer::getPackShowLayer();
	showGoodsInfoLayer->setShowInfoType(PackShowLayer::PackType::InfoType_TyrantPack);
	showGoodsInfoLayer->setTag(1);// 这个标志对应游戏内处理
	addChild(showGoodsInfoLayer);
}

void GameMainLayer::consumeLife(void)
{
    this->changeLifeNum(-1);
}

void GameMainLayer::consumeHudun(void)
{
    this->m_fighting_shield_used++;
    this->changeHudunNUm(-1);
}

void GameMainLayer::consumeDazhao(void)
{
    this->m_fighting_skill_used++;
    this->changeDazhaoNum(-1);
}


void GameMainLayer::changeLifeNum(int lifeNum)
{
	NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID_Life, lifeNum, true);
    updateGoodsNum();
}

void GameMainLayer::changeHudunNUm(int hudunNum)
{
	this->m_fighting_shield_num += hudunNum;
    updateGoodsNum();
}

void GameMainLayer::changeDazhaoNum(int dazhaoNum)
{
    this->m_fighting_skill_num += dazhaoNum;
    updateGoodsNum();
}

void GameMainLayer::startDaZhao()
{
	StatisticsHelp::use("dazhao",1,1);
	auto mainPl = GameData::getInstance()->getMainPlane();
	if (!mainPl)
	{
		return;
	}
	mainPl->setInvincible(1);

	consumeDazhao();
	setDaZhaoState(false);

	int curRole = NewDataMgr::getInstance()->mRoleConfig.roleIndex;

	SpecialSkillTips::StartSpecialSkill();
}

void GameMainLayer::startHuDun()
{
	auto mainPlane = GameData::getInstance()->getMainPlane();
	if (mainPlane)
	{
		mainPlane->startProtect();
	}
}


void GameMainLayer::addHuDun()
{
	auto mainPlane = GameData::getInstance()->getMainPlane();
	if (mainPlane)
	{
		mainPlane->AddProtect();
	}
}

void GameMainLayer::helpFeqCard(void)
{
	NewDataMgr::getInstance()->setGuideCard(false);
	GameData::getInstance()->GamePause();
	HelpFeqLayer::showHelpFeq(HelpFeqLayer::HelpFeqType_CardTips, nullptr);
}

void GameMainLayer::helpFeqHuDun(void)
{
	Button* ButtonHuDun = (Button*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "ButtonHuDun");
	ButtonHuDun->setTouchEnabled(true);
	ButtonHuDun->setColor(Color3B(255, 255, 255));

	NewDataMgr::getInstance()->setGuideHunDun(false);
	GameData::getInstance()->GamePause();
	HelpFeqLayer::showHelpFeq(HelpFeqLayer::HelpFeqType_UseHudun, (Button*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "ButtonHuDun"));
}

void GameMainLayer::helpFeqDaZhao(void)
{
	Button* ButtonDaZhao = (Button*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "ButtonDaZhao");
	ButtonDaZhao->setTouchEnabled(true);
	ButtonDaZhao->setColor(Color3B(255, 255, 255));

	NewDataMgr::getInstance()->setGuideDaZhao(false);
	GameData::getInstance()->GamePause();
	HelpFeqLayer::showHelpFeq(HelpFeqLayer::HelpFeqType_UseDazhao, (Button*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "ButtonDaZhao"));
}

void GameMainLayer::showAddShieldEffect()
{
	Button* ButtonHuDun = (Button*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "ButtonHuDun");
	this->showAddEffect(ButtonHuDun);
}

void GameMainLayer::showAddSkillEffect()
{
	Button* ButtonDaZhao = (Button*)GameMainLayer::seekNodeByName(_mainLayerRootNode, "ButtonDaZhao");
	this->showAddEffect(ButtonDaZhao);
}

void GameMainLayer::showAddEffect(Widget* node)
{
	Sprite* bg = Sprite::create("Image/add_bg.png");
	bg->setPosition(Vec2(node->getContentSize().width * 0.5,node->getContentSize().height * 0.5));
	bg->setScale(1.1);
	node->addChild(bg);

	Sprite* addOne = Sprite::create("Image/add_one.png");
	addOne->setScale(1.4);
	addOne->setPosition(Vec2(bg->getContentSize().width * 0.5,bg->getContentSize().height * 0.5));
	bg->addChild(addOne);

	bg->setOpacity(0);
	bg->runAction(Sequence::create(FadeIn::create(0.2),DelayTime::create(0.2),FadeOut::create(0.4),DelayTime::create(0.4),RemoveSelf::create(true), nullptr));
	addOne->runAction(Spawn::create(MoveBy::create(1,Vec2(0,145)),FadeOut::create(1), nullptr));
}


bool GameMainLayer::shieldIsEnough()
{
	return this->m_fighting_shield_num > 0;
}

bool GameMainLayer::skillIsEnough()
{
	return this->m_fighting_skill_num > 0;
}

void GameMainLayer::setUserItemNumWhenGameEnd(bool isWin)
{
	int shieldNum = NewDataMgr::getInstance()->getGoodsNumWithID(GoodsID::GoodsID_HuDun);
	int skillNum = NewDataMgr::getInstance()->getGoodsNumWithID(GoodsID::GoodsID_DaZhao);
	if(isWin) {
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID::GoodsID_HuDun,this->m_fighting_shield_num - shieldNum);
		NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID::GoodsID_DaZhao,this->m_fighting_skill_num - skillNum);
	} else {

		if(this->m_fighting_shield_used > 0) {
			int updateShieldNum = 0;
			if((shieldNum - (this->m_fighting_shield_used)) < 0) {
				updateShieldNum = -shieldNum;
			} else {
				updateShieldNum = -(this->m_fighting_shield_used);
			}
			NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID::GoodsID_HuDun,updateShieldNum);
		}
		if(this->m_fighting_skill_used > 0) {
			int updateSkillNum = 0;
			if((skillNum - (this->m_fighting_skill_used)) < 0) {
				updateSkillNum = -skillNum;
			} else {
				updateSkillNum = -(this->m_fighting_skill_used);
			}
			NewDataMgr::getInstance()->addGoodsNumWithID(GoodsID::GoodsID_DaZhao,updateSkillNum);
		}
	}

}

// 复活
void BuyFuHuo::showLayout( Node* node )
{
	GameData::getInstance()->setisDisabledKey(true);
	if (node && !node->getChildByName("BuyFuHuo"))
	{
		auto layout = BuyFuHuo::create();
		node->addChild( layout, 1, "BuyFuHuo" );
	}
}

void BuyFuHuo::onMessageEvent(Ref* sender)
{
	int protocolID = (intptr_t)sender;

	switch (protocolID)
	{
	case EProtocolID_FuHuoBuy:
	{
		removeFromParentAndCleanup(true);
		BuyFuHuoConfirm();
	}
		break;
	default:
		break;
	}
}

void BuyFuHuo::onEnter()
{
	Layer::onEnter();
	initInterface();

	auto stone = UIController::getInstance()->getPhysicsLayer()->getChildByTag(20150924);
	if(stone) {
		stone->resume();
	}

	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(BuyFuHuo::onMessageEvent), MsgEventName_ToGameFuHuo, nullptr);
	int curRole = NewDataMgr::getInstance()->mRoleConfig.roleIndex;
	switch (curRole) {
		case 1:
			NewAudio::getInstance()->playEffect(Music_MainPlDeadVoice1);
			break;
		case 2:
			NewAudio::getInstance()->playEffect(Music_MainPlDeadVoice2);
			break;
		case 3:
			NewAudio::getInstance()->playEffect(Music_MainPlDeadVoice3);
			break;
	}
}
void BuyFuHuo::onExit()
{
	Layer::onExit();
	NotificationCenter::getInstance()->removeAllObservers(this);
	GameData::getInstance()->setAllEnemyPlaneInvincible(false);	//取消所有敌机无敌
}
void BuyFuHuo::initInterface()
{
	_fuHuoLayout = CSLoader::createNode( Ccs_FuHuoLayer_FilePath );

	if ( !_fuHuoLayout )
	{
		return;
	}

	addChild( _fuHuoLayout );

	do 
	{
		auto ButtonFuHuo = (Button*)GameMainLayer::seekNodeByName(_fuHuoLayout, "ButtonFuHuo" );
		setEventForNode( ButtonFuHuo, Tag_BuyFuhuo );


		auto Buttonclose = (Button*)GameMainLayer::seekNodeByName(_fuHuoLayout, "Buttonclose" );
		setEventForNode( Buttonclose, Tag_Close );

		auto ImgFuHuoFlash = (Sprite*)GameMainLayer::seekNodeByName(_fuHuoLayout, "ImgFuHuoFlash");
		ImgFuHuoFlash->runAction(CCRepeatForever::create(Sequence::create(
			CCFadeOut::create(0.6),
			CCFadeIn::create(0.6),
			nullptr)));

		auto ImgEge = (Sprite*)GameMainLayer::seekNodeByName(_fuHuoLayout, "ImgEge");
		ImgEge->setVisible(false);
		ImgEge->runAction(CCRepeatForever::create(Sequence::create(
			DelayTime::create(2.0),
			CCCallFuncN::create(CC_CALLBACK_1(BuyFuHuo::callFuncEge, this)),
			DelayTime::create(0.1),
			CCCallFuncN::create(CC_CALLBACK_1(BuyFuHuo::callFuncEge, this)),
			DelayTime::create(0.1),
			CCCallFuncN::create(CC_CALLBACK_1(BuyFuHuo::callFuncEge, this)),
			DelayTime::create(0.1),
			CCCallFuncN::create(CC_CALLBACK_1(BuyFuHuo::callFuncEge, this)),
			nullptr)));

		//地图进度
// 		float mapPer = UIController::getInstance()->getGameBgMgr()->getCurBgId();
// 		auto LoadingBarGame = (LoadingBar*)GameMainLayer::seekNodeByName(_fuHuoLayout, "LoadingBarYuanzhen");
// 		LoadingBarGame->setPercent(((mapPer / 3) * 100) );
// 
// 		auto AtlasLabelYuanZhenPer = (TextAtlas*)GameMainLayer::seekNodeByName(_fuHuoLayout, "AtlasLabelYuanZhenPer");
// 		AtlasLabelYuanZhenPer->setString(StringUtils::format("%d/", int((mapPer / 3) * 100)));

	}while(0);

}

void BuyFuHuo::setEventForNode( Button* node, ButtonTag tag )
{
	do 
	{
		CC_BREAK_IF( node == nullptr );

		node->setTag( tag );
		node->addClickEventListener(CC_CALLBACK_1(BuyFuHuo::callFuncButtonEvent, this));


	} while (0);
}

void BuyFuHuo::callFuncButtonEvent( Ref* ref )
{
	Button* btn = (Button*)ref;
	int nTag = btn->getTag();


	switch ( nTag )
	{

	case Tag_BuyFuhuo:
		{
			GameData::getInstance()->setisDisabledKey(false);
			NewDataMgr::getInstance()->mGameBuySystem->buyGoodsWithKey(BuyKey_BuyFuHuo);
		}
		break;	

	case Tag_Close:
		{
			GameData::getInstance()->setisDisabledKey(false);
			removeFromParentAndCleanup(true);
			buyFuhuoCancel();
		}
		break;	

		default:
			break;
	}
}

void BuyFuHuo::buyFuhuoCancel( void )
{
	GameData::getInstance()->GameResume();
	UIController::getInstance()->getMainUILayer()->setUserItemNumWhenGameEnd(false);
	// 取消购买 失败
	UIController::getInstance()->ShowResultLayer(false);
}

void BuyFuHuo::BuyFuHuoConfirm( void )
{
//	游戏状态
	GameData::getInstance()->setisGamePlaying(true);

	//死亡状态
	UIController::getInstance()->getMainUILayer()->SetRealDead(false);

	//血量回满
	UIController::getInstance()->getMainUILayer()->AddHp(10000);

	PauseLayer* layer = PauseLayer::create();
	layer->setActivateType(PauseLayer::ActivateTag_Fuhuo);
	Director::getInstance()->getRunningScene()->addChild(layer,10000);
}

void BuyFuHuo::callFuncAction1( Node* node )
{
	auto AtlasLabelFHTime = (TextAtlas*)node;

	int time = atoi( AtlasLabelFHTime->getString().c_str() );

	time -= 1;
	if ( -1 == time )
	{
		AtlasLabelFHTime->stopAllActions();
		buyFuhuoCancel();
	}
	else
	{
		AtlasLabelFHTime->setString( __String::createWithFormat("%d", time )->getCString() );
	}

}

void BuyFuHuo::callFuncEge(Node* node)
{
	node->setVisible(node->isVisible() ? false : true);
}

// 设置

void SettingLayer::showLayout( Node* node )
{
	GameData::getInstance()->setisDisabledKey(true);
	if (node && !node->getChildByName("SettingLayer"))
	{
		auto layout = SettingLayer::create();
		node->addChild( layout, 1, "SettingLayer" );
	}
}

void SettingLayer::onEnter()
{
	Layer::onEnter();
	initInterface();
}

void SettingLayer::initInterface()
{
	_settingayout = CSLoader::createNode( Ccs_SettingLayer_FilePath );

	if ( !_settingayout )
	{
		return;
	}

	addChild( _settingayout );

	do 
	{
		auto ButtonExitGame = (Button*)GameMainLayer::seekNodeByName(_settingayout, "ButtonExitGame" );
		setEventForNode( ButtonExitGame, Tag_ExitGame ,nullptr);

		auto ButtonClose = (Button*)GameMainLayer::seekNodeByName(_settingayout, "ButtonClose");
		setEventForNode(ButtonClose, Tag_ContinueGame, nullptr);

		auto ButtonContinue = (Button*)GameMainLayer::seekNodeByName(_settingayout, "ButtonContinue" );
		setEventForNode(ButtonContinue, Tag_ContinueGame, nullptr);

		// 音效
		auto CheckBoxMuice = (CheckBox*)GameMainLayer::seekNodeByName(_settingayout, "CheckBoxMuice" );
		CC_BREAK_IF( CheckBoxMuice == nullptr );
		CheckBoxMuice->addEventListener(CC_CALLBACK_2(SettingLayer::selectedMusicEvent, this));
		CheckBoxMuice->setSelected( NewAudio::getInstance()->getMusicState() );

		// 音乐
		auto CheckBoxSound = (CheckBox*)GameMainLayer::seekNodeByName(_settingayout, "CheckBoxSound" );
		CC_BREAK_IF( CheckBoxSound == nullptr );
		CheckBoxSound->addEventListener(CC_CALLBACK_2(SettingLayer::selectedSoundEvent, this));
		CheckBoxSound->setSelected( NewAudio::getInstance()->getSoundEffState() );


	}while(0);
}

void SettingLayer::setEventForNode(Button* node, int tag, const Widget::ccWidgetTouchCallback& callback)
{
	do 
	{
		CC_BREAK_IF( node == nullptr );

		node->setTag( tag );
		node->addTouchEventListener(CC_CALLBACK_2(SettingLayer::callFuncButtonEvent, this));


	} while (0);
}

void SettingLayer::callFuncButtonEvent(Ref* ref, Widget::TouchEventType touchType)
{
	if (!setButtonAction(ref, touchType)) return;

	Button* btn = (Button*)ref;
	int nTag = btn->getTag();
	btn->setEnabled(false);

	switch ( nTag )
	{
	case Tag_ExitGame:
		{
            //EnemyPlaneController::destroyInstance();
			UIController::getInstance()->getMainUILayer()->setUserItemNumWhenGameEnd(false);
			GameSceneMgr::getInstance()->changeScene( Scene_Type_RoleInterface );
			GameData::getInstance()->GameResume();
			auto mainPlane =GameData::getInstance()->getMainPlane();
			if(mainPlane) {
				mainPlane->gameEnd();
			}
		}
		break;	

	case Tag_ContinueGame:
		{
			removeFromParentAndCleanup( true );
			PauseLayer* layer = PauseLayer::create();
			Director::getInstance()->getRunningScene()->addChild(layer,10000);
		}
		break;	
	default:
		break;
	}

}

void SettingLayer::selectedMusicEvent( Ref* pSender,CheckBox::EventType type )
{
	switch (type)
	{
	case CheckBox::EventType::UNSELECTED:
		NewAudio::getInstance()->setMusicState(false);
		break;

	case CheckBox::EventType::SELECTED:
		NewAudio::getInstance()->setMusicState(true);
		break;

	default:
		break;
	}
}

void SettingLayer::selectedSoundEvent( Ref* pSender,CheckBox::EventType type )
{
	switch (type)
	{
	case CheckBox::EventType::UNSELECTED:
		NewAudio::getInstance()->setSoundEffState(false);
		break;

	case CheckBox::EventType::SELECTED:
		NewAudio::getInstance()->setSoundEffState(true);
		break;

	default:
		break;
	}
}
