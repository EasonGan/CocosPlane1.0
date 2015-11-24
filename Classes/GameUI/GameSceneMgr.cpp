#include "GameSceneMgr.h"
#include "Loading.h"
#include "GameFristScene.h"
#include "GameRoleScene.h"
#include "../GameCommon/GameData.h"
#include "../GamePlaying/Layer/PhysicsLayer.h"
#include "../GameCommon/NewDataMgr.h"




GameSceneMgr* GameSceneMgr::_gameSceneMgr = nullptr;

GameSceneMgr* GameSceneMgr::getInstance( void )
{
	if ( !GameSceneMgr::_gameSceneMgr )
	{
		GameSceneMgr::_gameSceneMgr = new (std::nothrow) GameSceneMgr();
	}
	return GameSceneMgr::_gameSceneMgr;
}

void GameSceneMgr::purgeInstance( void )
{
	if ( GameSceneMgr::_gameSceneMgr )
	{
		CC_SAFE_DELETE( GameSceneMgr::_gameSceneMgr );
	}
}


void GameSceneMgr::changeScene( GameSceneType type , bool isfrist /*= false*/ )
{
	//Director::getInstance()->getOpenGLView()->setDesignResolutionSize(720,1280,ResolutionPolicy::FIXED_HEIGHT);


	Director::getInstance()->getScheduler()->setTimeScale(1.0f);
	switch ( type )
	{
	case Scene_Type_FristInterface:
		{			  
			auto scene = GameFristScene::createScene();
			Director::getInstance()->runWithScene(scene);
		}
		break;

	case Scene_Type_LevelInterface:
		{
		}
		break;

	case Scene_Type_RoleInterface:
		{
			auto scene = GameRoleScene::createScene();
			Director::getInstance()->replaceScene(getTransitionAction(0.50f, scene, GameSceneMgr::CCTransitionFade));
		}
		break;

	case Scene_Type_PlayInterface:
		{
			//Director::getInstance()->getOpenGLView()->setDesignResolutionSize(720,1280,ResolutionPolicy::FIXED_HEIGHT);
			Scene* scene = PhysicsLayer::createScene();
			Director::getInstance()->replaceScene( scene );

			NewDataMgr::getInstance()->setFirstEntryState(false);
	}
		break;

	case Scene_Type_PlayILoading:
		{
			GameInterfaceMgr::getInstance()->closeAllInterface();

			auto scene = LoadingLayer::createScene();
			Director::getInstance()->replaceScene(scene);

		}
		break;
	default:
		break;
	}
	setCurScen(type);
}

Scene* GameSceneMgr::getTransitionAction( float t, Scene* s, TransitionType type )
{
	Scene* scene = nullptr;
	switch (type)
	{
	case GameSceneMgr::CCTransitionJumpZoom:
		scene = TransitionJumpZoom::create( t, s );
		break;
	case GameSceneMgr::CCTransitionProgressRadialCCW:
		scene = TransitionProgressRadialCCW::create( t, s );
		break;
	case GameSceneMgr::CCTransitionProgressRadialCW:
		scene = TransitionProgressRadialCW::create( t, s );
		break;
	case GameSceneMgr::CCTransitionProgressHorizontal:
		scene = TransitionProgressHorizontal::create( t, s );
		break;
	case GameSceneMgr::CCTransitionProgressVertical:
		scene = TransitionProgressVertical::create( t, s );
		break;
	case GameSceneMgr::CCTransitionProgressInOut:
		scene = TransitionProgressInOut::create( t, s );
		break;
	case GameSceneMgr::CCTransitionProgressOutIn:
		scene = TransitionProgressOutIn::create( t, s );
		break;
	case GameSceneMgr::CCTransitionCrossFade:
		scene = TransitionCrossFade::create( t, s );
		break;
	case GameSceneMgr::CCTransitionPageTurn:
		scene = TransitionPageTurn::create( t, s, false );
		break;
	case GameSceneMgr::CCTransitionFadeTR:
		scene = TransitionFadeTR::create( t, s );
		break;
	case GameSceneMgr::CCTransitionFadeBL:
		scene = TransitionFadeBL::create( t, s );
		break;
	case GameSceneMgr::CCTransitionFadeUp:
		scene = TransitionFadeUp::create( t, s );
		break;
	case GameSceneMgr::CCTransitionFadeDown:
		scene = TransitionFadeDown::create( t, s );
		break;
	case GameSceneMgr::CCTransitionTurnOffTiles:
		scene = TransitionTurnOffTiles::create( t, s );
		break;
	case GameSceneMgr::CCTransitionSplitRows:
		scene = TransitionSplitRows::create( t, s );
		break;
	case GameSceneMgr::CCTransitionSplitCols:
		scene = TransitionSplitCols::create( t, s );
		break;
	case GameSceneMgr::CCTransitionFade:
		scene = TransitionFade::create( t, s );
		break;
	case GameSceneMgr::CCTransitionFlipX:
		scene = TransitionFlipX::create( t, s );
		break;
	case GameSceneMgr::CCTransitionFlipY:
		scene = TransitionFlipY::create( t, s );
		break;
	case GameSceneMgr::CCTransitionFlipAngular:
		scene = TransitionFlipAngular::create( t, s );
		break;
	case GameSceneMgr::CCTransitionZoomFlipX:
		scene = TransitionZoomFlipX::create( t, s );
		break;
	case GameSceneMgr::CCTransitionZoomFlipY:
		scene = TransitionZoomFlipY::create( t, s );
		break;
	case GameSceneMgr::CCTransitionZoomFlipAngular:
		scene = TransitionZoomFlipAngular::create( t, s );
		break;
	case GameSceneMgr::CCTransitionShrinkGrow:
		scene = TransitionShrinkGrow::create( t, s );
		break;
	case GameSceneMgr::CCTransitionRotoZoom:
		scene = TransitionRotoZoom::create( t, s );
		break;
	case GameSceneMgr::CCTransitionMoveInL:
		scene = TransitionMoveInL::create( t, s );
		break;
	case GameSceneMgr::CCTransitionMoveInR:
		scene = TransitionMoveInR::create( t, s );
		break;
	case GameSceneMgr::CCTransitionMoveInB:
		scene = TransitionMoveInB::create( t, s );
		break;
	case GameSceneMgr::CCTransitionMoveInT:
		scene = TransitionMoveInT::create( t, s );
		break;
	case GameSceneMgr::CCTransitionSlideInL:
		scene = TransitionSlideInL::create( t, s );
		break;
	case GameSceneMgr::CCTransitionSlideInR:
		scene = TransitionSlideInR::create( t, s );
		break;
	case GameSceneMgr::CCTransitionSlideInT:
		scene = TransitionSlideInT::create( t, s );
		break;
	case GameSceneMgr::CCTransitionSlideInB:
		scene = TransitionSlideInB::create( t, s );
		break;
	default:
		scene = s;
		break;
	}

	return scene;
}

Scene* GameSceneMgr::getRandTransitionAction( float t, Scene* s )
{
	int type = CCRANDOM_0_1()*(TransitionType_Max);

	return getTransitionAction(t, s, (TransitionType)type );
}

