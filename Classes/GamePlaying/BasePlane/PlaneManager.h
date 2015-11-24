/********************************************************************
创建于:		2014/12/10  14:49	
文件名: 	E:\Cocos3.3_Project\MyGame\Classes\PlaneManager.h 
作  者:		alex(alex)
修改于：	2014/12/10  14:49	

目  的:		各种飞机的生成管理
*********************************************************************/
#ifndef _PLANE_MANAGER_H_
#define _PLANE_MANAGER_H_


#include "cocos2d.h"
#include "cocos-ext.h"
#include "../../GameCommon/GameDefine.h"

USING_NS_CC;
USING_NS_CC_EXT;

//组合序号及掉落物品序号
class PhysicsUnit;
class MainPlane;
class BasePlane;
class PlaneManager : public Ref
{
public:
	PlaneManager();
	~PlaneManager();
	static PlaneManager* create();
	void purge();

	void InitLvlData(const int& lvl);

	void refreshEnemyPlane();
	void AddGiftPlaneGroup();
	void addNextNormalGroup();
	void AnalyseOneGroup(TMXObjectGroup* group);	//创建其中的某一组
	BasePlane* CreateWithPro(const Vec2& pos, int plId, int weaponId, int bulletRes, 
		int actionId, int AIid, int gameLvl, float userdata);

	int getNamePro(TMXObjectGroup* group,const int& idx);
	
	//加载下一关卡
	void AddNextLevel();

	//休息一段时间
	void Rest(float dur);
	void RestCallback();

	void AddOnePlane(PhysicsUnit* pl);
	void DelOnePlane(PhysicsUnit* pl);

	void resetData();
	
private:
	bool init();

	/************TMX处理***************/
	Point getPosByTmx(ValueMap& dict);							//敌机位置
	void AddDropsByTmx(ValueMap& dict,BasePlane* pl);			//敌机凋落物
	bool readGroupTmx(const int& gameLvl);						//读取TMX文件

	Vector<TMXObjectGroup*>				m_GruopVec;
	Vector<TMXObjectGroup*>::iterator   m_GruopIt;

	CC_SYNTHESIZE(cocos2d::experimental::TMXTiledMap*,m_GroupTmx,GroupTmx);	//敌机组合
	CC_SYNTHESIZE(bool, m_IsGroupOver, IsGroupOver);				//游戏是否结束
	CC_SYNTHESIZE(int, m_curLvl, CurLvl);						//当前关卡
	float								m_restTime;				//休息时间
	int									m_restCount;			//休息计数
	std::list<PhysicsUnit*>				m_PlaneGroup;
};

class GiftPlaneManager : public Node
{
public:
	CREATE_FUNC(GiftPlaneManager);
	virtual void onEnter();

	//随机创建运输机
	void addGiftPlaneGroup();
	void addNextGiftGroup();
	void addOneGiftPlane();

private:
	int				m_PlaneId;
	int				m_ActionId;
};


#endif