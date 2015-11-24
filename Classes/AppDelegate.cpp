#include "AppDelegate.h"
#include "AudioEngine.h"
#include "GameUI/GameSceneMgr.h"
#include "GameCommon/NewDataMgr.h"
#include "cocos2d.h"
#include "GameCommon/GlobalSchedule.h"
//#include "LuaHelp/LuaHelp.h"

USING_NS_CC;
using namespace experimental;


AppDelegate::AppDelegate() {
}
  
AppDelegate::~AppDelegate() 
{
	AudioEngine::stopAll();
//	GameSceneMgr::purgeInstance();
	//NewRoleDataMgr::purgeInstance();
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("My Game");
//		glview->setFrameSize(320, 480);
//		glview->setFrameSize(720, 960);
//		glview->setFrameSize(1920,1080);
//		glview->setFrameSize(540, 960);
//		glview->setFrameSize(1080, 1920);
//		glview->setFrameSize(1080, 1780);
		glview->setFrameSize(720, 1280);
//		glview->setFrameSize(1280, 1920);
#if WIN32
		glview->setFrameZoomFactor(0.6f);
#endif
        director->setOpenGLView(glview);
    }

	glview->setDesignResolutionSize(720,1280,ResolutionPolicy::EXACT_FIT);

    // turn on display FPS
#if WIN32
    director->setDisplayStats(true);
#else
	director->setDisplayStats(false);
#endif

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

#if 0
    // create a scene. it's an autorelease object
    auto scene = GameModel::createScene();

    // run
    director->runWithScene(scene);
#else

	//GameSceneMgr::getInstance()->changeScene(Scene_Type_FristInterface);
	GameSceneMgr::getInstance()->changeScene(Scene_Type_FristInterface);
#endif
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
	AudioEngine::pauseAll();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();

}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
	AudioEngine::resumeAll();
    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

