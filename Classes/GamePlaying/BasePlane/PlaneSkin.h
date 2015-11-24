#ifndef PlaneSkin_h__
#define PlaneSkin_h__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "../../../cocos2d/cocos/editor-support/cocostudio/CCArmature.h"


USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocostudio;

class EnemyPlane;

//飞机皮肤
class PlaneSkin
{
public:
	//皮肤有三种形式
	enum class SkinType
	{
		Skin_Picture,		//图片
		Skin_Animation,		//序列帧
		Skin_Armature,		//骨骼动画
	};

	Sprite* createSkinWithPicture(const std::string &name);
	//Animate* createSkinWithAnimate();
	Armature* createSkinWithArmatureById(int id);
};

//飞机骨骼动画
class PlaneArmature : public Armature
{
public:
	PlaneArmature()
	{
		m_Target = nullptr;
	}
	static PlaneArmature* create(const std::string& name, EnemyPlane* target);
	
	void playRestAnimation(int idx = 0);		//休息动作，默认为0
	void playShootAnimation(int idx = 1);		//射击动作，默认为1
	void playHackAnimation(int idx = 2);		//砍杀动作，默认为2
	
	//加上武器
	void StartShoot();

private:
	void setPlaneTarget(EnemyPlane* target);

	void onFrameEventOfShoot( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex );
	void onFrameEventOfHack( Bone *bone, const std::string& evt, int originFrameIndex, int currentFrameIndex );

	EnemyPlane*					m_Target;	
};


#endif // PlaneSkin_h__