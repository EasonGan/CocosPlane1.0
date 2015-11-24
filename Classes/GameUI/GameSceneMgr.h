#ifndef __GAME_SCENE_MGR_H__
#define __GAME_SCENE_MGR_H__
#include "cocos2d.h"

USING_NS_CC;


/************************************************************************/
/* 场景管理中心                                                         */
/************************************************************************/

typedef enum GameSceneType
{
	Scene_Type_FristInterface,		// Game entry first interface
	Scene_Type_LevelInterface,		// Game level select interface
	Scene_Type_RoleInterface,		// Game show role or role select interface
	Scene_Type_PlayInterface,		// Game play entry...
	Scene_Type_PlayILoading,			// Game play loading
}GameSceneType_E;



class GameSceneMgr
{

public:

	typedef enum TransitionType
	{
		CCTransitionJumpZoom,				// 创建一个跳动的过渡动画
		CCTransitionProgressRadialCCW,		// 创建一个扇形条形式的过渡动画， 逆时针方向
		CCTransitionProgressRadialCW,		// 创建一个扇形条形式的过渡动画， 顺时针方向
		CCTransitionProgressHorizontal,		// 创建一个水平条形式的过渡动画
		CCTransitionProgressVertical,		// 创建一个垂直条形式的过渡动画
		CCTransitionProgressInOut,			// 创建一个由里向外扩展的过渡动画
		CCTransitionProgressOutIn,			// 创建一个由外向里扩展的过渡动画
		CCTransitionCrossFade,				// 创建一个逐渐透明的过渡动画
		CCTransitionPageTurn,				// 创建一个翻页的过渡动画
		CCTransitionFadeTR,					// 创建一个部落格过渡动画， 从左下到右上
		CCTransitionFadeBL,					// 创建一个部落格过渡动画， 从右上到左下
		CCTransitionFadeUp,					// 创建一个从下到上，条形折叠的过渡动画
		CCTransitionFadeDown,				// 创建一个从上到下，条形折叠的过渡动画
		CCTransitionTurnOffTiles,			// 创建一个随机方格消失的过渡动画
		CCTransitionSplitRows,				// 创建一个分行划分切换的过渡动画
		CCTransitionSplitCols,				// 创建一个分列划分切换的过渡动画
		CCTransitionFade,					// 创建一个逐渐过渡到目标颜色的切换动画
		CCTransitionFlipX,					// 创建一个x轴反转的切换动画
		CCTransitionFlipY,					// 过渡动作的持续时间
		CCTransitionFlipAngular,			// 创建一个带有反转角切换动画
		CCTransitionZoomFlipX,				// 创建一个带有缩放的x轴反转切换的动画
		CCTransitionZoomFlipY,				// 创建一个带有缩放的Y轴反转切换的动画
		CCTransitionZoomFlipAngular,		// 创建一个带有缩放 ，反转角切换的动画
		CCTransitionShrinkGrow,				// 创建一个放缩交替的过渡动画
		CCTransitionRotoZoom,				// 创建一个旋转放缩交替的过渡动画
		CCTransitionMoveInL,				// 创建一个从左边推入覆盖的过渡动画
		CCTransitionMoveInR,				// 创建一个从右边推入覆盖的过渡动画
		CCTransitionMoveInB,				// 创建一个从下边推入覆盖的过渡动画
		CCTransitionMoveInT,				// 创建一个从上边推入覆盖的过渡动画
		CCTransitionSlideInL,				// 创建一个从左侧推入并顶出旧场景的过渡动画
		CCTransitionSlideInR,				// 创建一个从右侧推入并顶出旧场景的过渡动画
		CCTransitionSlideInT,				// 创建一个从顶部推入并顶出旧场景的过渡动画
		CCTransitionSlideInB,				// 创建一个从下部推入并顶出旧场景的过渡动画
											   
		TransitionType_Max
	}TransitionType;


	GameSceneMgr()
		:_isShowLoading(true)
	{

	}
	~GameSceneMgr()
	{

	}
	// 创建与销毁实例
	static GameSceneMgr* getInstance(void);
	static void purgeInstance(void);

	// 切换场景
	void changeScene( GameSceneType type , bool isfrist = false );

	Scene* getTransitionAction( float t, Scene* s , TransitionType type);
	Scene* getRandTransitionAction( float t, Scene* s );

	CC_SYNTHESIZE(GameSceneType, m_curScene, CurScen);

private:
	static GameSceneMgr* _gameSceneMgr;

	bool _isShowLoading;


};





#endif	//!__GAME_SCENE_MGR_H__