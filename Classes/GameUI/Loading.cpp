#include "Loading.h"
#include "../editor-support/cocostudio/CCArmatureDataManager.h"
#include "GameSceneMgr.h"
#include "../GameCommon/GameData.h"



using namespace cocostudio;

Scene* LoadingLayer::createScene()
{
	auto scene = Scene::create();
	
	//loading层
	auto layer = LoadingLayer::create();
	scene->addChild(layer);

	return scene;
}

LoadingLayer::~LoadingLayer()
{
	if (_loadingMusicThread)
	{
		CC_SAFE_DELETE(_loadingMusicThread);
	}
}
 
bool LoadingLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	_loadingMusicThread = nullptr;
	m_curPreloadNum = 0;
	m_totalNums = 0;

	auto ws =Director::getInstance()->getWinSize();

	auto imgGuang = Sprite::create("loadRes/imgRoleGuang.png");
	imgGuang->setPosition(Vec2(ws.width*0.5 - 5+ 120 ,ws.height*0.6-5 ));
	imgGuang->setScale( 2.3f );
	addChild(imgGuang);

	auto role = Sprite::create("ccsRes/resMainSceneTask/roleImg3.png");
	role->setPosition(Vec2(ws.width*0.5+ 120,ws.height*0.6 ));
	role->setScale( 0.8f );
	addChild(role);

 	initProgress();
 	initFile();

	return true;
}

void LoadingLayer::initFile()
{
	//敌方小怪子弹
	addFrameRes("ammo/EnemyNorBullet.plist",
		"ammo/EnemyNorBullet.png");

	//小怪死亡爆炸
	addFrameRes("animation/PlaneDeadExpl/PlaneDeadExpl.plist",
		"animation/PlaneDeadExpl/PlaneDeadExpl.png");

	//主角子弹
	addFrameRes("ammo/SelfBullet.plist",
		"ammo/SelfBullet.png");
	
	//BOSS子弹
	addFrameRes("ammo/BossBullet.plist",
		"ammo/BossBullet.png");

	//卡牌子弹
	addFrameRes("ammo/CardBullet.plist",
		"ammo/CardBullet.png");
	
	//爆裂弹前段
	addFrameRes("animation/ExplosionBullet/ExplBult_1.plist",
		"animation/ExplosionBullet/ExplBult_1.png");

	//爆裂弹后段
	addFrameRes("animation/ExplosionBullet/ExplBult_2.plist",
		"animation/ExplosionBullet/ExplBult_2.png");

	//掉落物
	addFrameRes("Drops/DropResList.plist",
		"Drops/DropResList.png");

	//敌机资源
	addFrameRes("plane/enemy/enemyPlane.plist",
		"plane/enemy/enemyPlane.png");

	//主角1普通
	addFrameRes("animation/MainPl1N/MainPl1.plist",
		"animation/MainPl1N/MainPl1.png");

	//主角2普通
	addFrameRes("animation/MainPl2N/MainPl2.plist",
		"animation/MainPl2N/MainPl2.png");

	//主角3普通
	addFrameRes("animation/MainPl3N/MainPl3.plist",
		"animation/MainPl3N/MainPl3.png");

	//闪电受击
	addFrameRes("animation/LightingEff/LightingEff.plist",
		"animation/LightingEff/LightingEff.png");

	//护盾
	addFrameRes("animation/PlaneProtect/PlaneProtect.plist",
		"animation/PlaneProtect/PlaneProtect.png");

	//火龙喷火火花
	addFrameRes("animation/FireSurround/FireExpl.plist",
		"animation/FireSurround/FireExpl.png");

	//BOSS暴怒
	addFrameRes("animation/BossAngry/BossAngry.plist",
		"animation/BossAngry/BossAngry.png");

	//BOSS1
	addArmatureRes("animation/boss1/kaijia0.png",
		"animation/boss1/kaijia0.plist",
		"animation/boss1/kaijia.ExportJson");

	//BOSS2
	addArmatureRes("animation/BoneWarrior/BoneWarrior0.png",
		"animation/BoneWarrior/BoneWarrior0.plist",
		"animation/BoneWarrior/BoneWarrior.ExportJson");

	//BOSS眼睛
	addArmatureRes("animation/boss-siyan/boss-siyan0.png",
		"animation/boss-siyan/boss-siyan0.plist",
		"animation/boss-siyan/boss-siyan.ExportJson");

	//BOSS义肢
	addArmatureRes("animation/boss-yizhi/boss-yizhi0.png",
		"animation/boss-yizhi/boss-yizhi0.plist",
		"animation/boss-yizhi/boss-yizhi.ExportJson");

	
	//火龙3级
	addArmatureRes("plane/armat/fireDragon/julong/julong0.png",
		"plane/armat/fireDragon/julong/julong0.plist",
		"plane/armat/fireDragon/julong/julong.ExportJson");

	//主角形式1
	//addArmatureRes("","","animation/Role1/shijiu.ExportJson");

	//主角形式2
	//addArmatureRes("","","animation/Role2/shuanglong.ExportJson");

	//主角形式3
	//addArmatureRes("","","animation/Role3/huolong.ExportJson");


	LoadResources();
	_loadingMusicThread = new std::thread(&LoadingLayer::loadMusic, this);
}

void LoadingLayer::loadMusic()
{
	NewAudio::getInstance()->preloadAllFile();
};

void LoadingLayer::LoadResources()
{
	for (auto it = m_FrameResVec.begin(); it != m_FrameResVec.end(); it++)
	{
		std::string pngFile = (*it).pngFile;
		Director::getInstance()->getTextureCache()->addImageAsync(pngFile.c_str(),
			CC_CALLBACK_1(LoadingLayer::LoadResCallback,this));
	}
	
}

void LoadingLayer::LoadResCallback( cocos2d::Texture2D* texture )
{
	std::string plistFile = m_FrameResVec.at(m_curPreloadNum).plistFile;
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistFile.c_str(), texture); 
	m_curPreloadNum++;

	refreshProgress();

	if (m_curPreloadNum >= m_FrameResVec.size())
	{
		if (m_ArmatureResVec.empty())
		{
			HideDraw();
			return;
		}
		//加载完了普通资源
		for (auto it = m_ArmatureResVec.begin(); it != m_ArmatureResVec.end(); it++)
		{
			ResStruct rs = (*it);
			if (rs.plistFile.empty())
			{
				ArmatureDataManager::getInstance()->
					addArmatureFileInfoAsync(rs.configFile,
					this,schedule_selector(LoadingLayer::LoadArmatureCallback)
					);
			}
			else
			{
				ArmatureDataManager::getInstance()->
					addArmatureFileInfoAsync(rs.pngFile,rs.plistFile,rs.configFile,
					this,schedule_selector(LoadingLayer::LoadArmatureCallback)
					);
			}
		}
	}
}

void LoadingLayer::addFrameRes( const char* plist, const char* pngfile)
{
	ResStruct rs;
	rs.plistFile = plist;
	rs.pngFile = pngfile;
	m_FrameResVec.push_back(rs);
}

void LoadingLayer::initProgress()
{
	Size ws = Director::getInstance()->getWinSize();
	//m_loadProgress = Label::create("0%","",50);
	//m_loadProgress->setPosition(ws.width*0.5, ws.height*0.3);
	//addChild(m_loadProgress,1);

	auto load = Sprite::create("loadRes/imgLoading.png");
	load->setPosition(ws.width*0.5, ws.height*0.25);
	addChild(load);

	Sprite* barBg = Sprite::create("loadRes/imgProgressBg.png");
	barBg->setPosition(ws.width*0.5, ws.height*0.3);
	addChild(barBg);

	Sprite* sp = Sprite::create("loadRes/imgProgress.png");
	m_LoadingBar = ProgressTimer::create(sp);
	m_LoadingBar->setType(kCCProgressTimerTypeBar);  
	m_LoadingBar->setBarChangeRate(Vec2(1,0));
	m_LoadingBar->setPercentage(0);
	m_LoadingBar->setMidpoint(ccp(0,0.5));
	m_LoadingBar->setPosition(ccp(barBg->getContentSize().width*0.5,barBg->getContentSize().height*0.5));
	barBg->addChild(m_LoadingBar);
	barBg->setScaleY(0.9);

	m_CurProgress = Sprite::create("loadRes/imgCurPr.png");
	m_CurProgress->setPosition(ccp(20,barBg->getContentSize().height*0.45));
	barBg->addChild(m_CurProgress);

	auto parcile = ParticleSystemQuad::create("particla/LoadBar/LoadBar.plist");
	parcile->setPosition( Vec2(9,15) );
	m_CurProgress->addChild( parcile );

	ActionInterval* tint = TintBy::create(0.3f,0,0,-255);
	ActionInterval* ease = EaseCircleActionInOut::create(tint);
	ActionInterval* back = ease->reverse();
	ActionInterval* seq = Sequence::create(tint, back, nullptr);
	ActionInterval* rep = RepeatForever::create(seq);
	//m_BloodBar->runAction(rep);

}

void LoadingLayer::refreshProgress()
{
	int totalnum = m_FrameResVec.size() + m_ArmatureResVec.size();

	float percent = m_curPreloadNum*1.0 /totalnum;
	std::string progress = String::createWithFormat("%3.0f%%",percent * 100)->getCString();
	//m_loadProgress->setString(progress);
	m_LoadingBar->setPercentage(percent*100);

	float barY = m_LoadingBar->getContentSize().width*percent;
	if ( barY < m_LoadingBar->getContentSize().width*0.95 )
		m_CurProgress->setPosition( Vec2(barY,m_CurProgress->getPosition().y) );
	else
		m_CurProgress->setPosition( Vec2(m_LoadingBar->getContentSize().width*0.95,m_CurProgress->getPosition().y) );
}

void LoadingLayer::HideDraw()
{
	//加载完成，进入游戏
	//GameSceneMgr::getInstance()->changeScene(Scene_Type_FristInterface);
	if (_loadingMusicThread)
		_loadingMusicThread->join();

	GameSceneMgr::getInstance()->changeScene(Scene_Type_RoleInterface);
}

void LoadingLayer::addArmatureRes( const std::string& imagePath, const std::string& plistPath, const std::string& configFilePath )
{
	ResStruct rs;
	rs.plistFile = plistPath;
	rs.pngFile = imagePath;
	rs.configFile = configFilePath;
	m_ArmatureResVec.push_back(rs);
}

void LoadingLayer::LoadArmatureCallback( float dt )
{
	m_curPreloadNum++;
	refreshProgress();

	int totalnum = m_FrameResVec.size() + m_ArmatureResVec.size();
	if (m_curPreloadNum >= totalnum)
	{
		ActionInterval* delay = DelayTime::create(1.0f);
		CallFunc* func = CallFunc::create(this,callfunc_selector(LoadingLayer::delayDrawScene));
		ActionInterval* seq = Sequence::create(delay, func, nullptr);
		runAction(seq);
	}

}

void LoadingLayer::delayDrawScene()
{
	// 引发断点
	NewDataMgr::getInstance()->initGameData();
	HideDraw();
}

