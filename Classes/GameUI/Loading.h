/********************************************************************
创建于:		2014/11/27  12:28	
文件名: 	E:\Cocos3.3_Project\MyGame\Classes\Loading.h 
作  者:		alex(alex)
修改于：	2014/11/27  12:28	

目  的:		资源加载界面
*********************************************************************/
#ifndef _LOADING_H_
#define _LOADING_H_


#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

typedef struct ResStruct
{
	std::string plistFile;
	std::string pngFile;
	std::string configFile;
	ResStruct()
	{
		plistFile = "";
		pngFile = "";
		configFile = "";
	}
}ResStruct;

class LoadingLayer : public Layer
{
public:
	CREATE_FUNC(LoadingLayer);
	~LoadingLayer();
	static Scene* createScene();

	virtual bool init();

	void initProgress();
	void refreshProgress();

	//加载序列帧资源
	void initFile();

	void loadMusic();

	void LoadResources();

	void LoadResCallback(cocos2d::Texture2D* texture);

	void LoadArmatureCallback(float dt);

	//加载序列帧、打包的资源
	void addFrameRes(const char* plist, const char* pngfile);

	//加载骨骼动画
	void addArmatureRes(const std::string& imagePath, const std::string& plistPath, const std::string& configFilePath);

	//为了解决有些一次性大量渲染（首次）导致瞬间卡顿的现象，这里在背景层之下隐性渲染一次
	void HideDraw();

	//延迟加载场景
	void delayDrawScene();

private:
	//Label*					m_loadProgress;
	std::thread* _loadingMusicThread;

	int						m_curPreloadNum;
	int						m_totalNums;		//资源总数量

	std::vector<ResStruct>	m_FrameResVec;		//普通资源列表
	std::vector<ResStruct>	m_ArmatureResVec;	//骨骼动画资源列表

	ProgressTimer*			m_LoadingBar;

	Sprite*					m_CurProgress;
};


#endif