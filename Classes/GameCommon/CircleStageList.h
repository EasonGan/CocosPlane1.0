#ifndef __CIRCLE_STAGE_LIST_H__
#define __CIRCLE_STAGE_LIST_H__

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;

using namespace cocostudio;


//链表节点——候选角色
class CandidateRole :public CCNode{
public:
	CandidateRole();
	~CandidateRole();
	virtual bool init(const char* name);
	static CandidateRole* create(const char* name);
	CandidateRole* mpNextRole; //节点右面角色的指针
	CandidateRole* mpForeRole;  //..左..........     

	void setPositionIndex(int );
	int getPositionIndex();   

	int mDistance;     //到椭圆最低点的距离
	int mPositionIndex;     //节点在链表中的序号
	int mNextPositionX;    
	int mNextPositionY;
	int mColorR;
	int mColorG;
	int mColorB;
	Sprite* mpArmature;
	bool mbIsSelected;   // 是否被选中

};


//由候选角色组成的链表，在屏幕上成椭圆分布
class CircleStage :public CCNode
{

public:
	typedef std::function<void(int)> onCircleEndCallBack;

	CircleStage();
	~CircleStage();
	virtual bool init();

	CREATE_FUNC(CircleStage);
	void addRole(CandidateRole *);    //添加一个角色到链表
	void changeToNext( float t = 0.4f );    //顺时针旋转
	void changeToFore( float t = 0.4f );    //逆时针旋转

	void callbackSetCurrentRole1( void );
	void callbackSetCurrentRole2( void );
	void setCurrentRole( int index );
	bool isOnAction();
	void actionEnd();

public:
	onCircleEndCallBack callFuncCircleEnd;	// end


private:
	int mCountActionEndNum;
	bool mbOnAction;
	int mRoleNum;
	CandidateRole* mpCurrentRole;     //操作链表用的指针
	CandidateRole* mpHead;
	CandidateRole* mpLast;
	CandidateRole* mpSelectRole;    //被选中的角色，在椭圆最低点
	int mCircleA;    //椭圆的a
	int mCircleB;    //椭圆的b
	void updateColor();    //更新每个节点的明暗，越远越暗
	void updateZorders();    //更新遮盖关系
	void updateZordersCallBack();
	void updateDistances();    //更新每个节点到中央节点的距离
	void initAppearance();    //初始化颜色和缩放

	int mTargetIndex;

};




#endif
