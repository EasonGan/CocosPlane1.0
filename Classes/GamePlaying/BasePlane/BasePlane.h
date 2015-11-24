/********************************************************************
*********************************************************************/
#ifndef _BASEPLANE_H_
#define _BASEPLANE_H_


#include "cocos2d.h"
#include "../PhysicsUnit.h"
#include "../../GameCommon/NewAudio.h"
#include "../../../cocos2d/cocos/editor-support/cocostudio/CCArmature.h"
#include "../../GameCommon/GameDefine.h"
#include "../Weapon/WeaponFactory.h"
#include "PlaneSkin.h"
#include "../Weapon/weapon.h"
#include "../../Config/TrackConfig.h"

USING_NS_CC;
using namespace cocostudio;

#define CREATE_PLANE_FRAME(__TYPE__) \
static __TYPE__* createWithAnimation( Animate* animat ) \
{ \
	__TYPE__* plane = new (std::nothrow) __TYPE__; \
	if (plane && plane->initWithFile("weapon.png")) \
	{ \
		plane->InitSkinWithAimation(animat); \
		plane->initBody(); \
		plane->ShowHpBar(); \
		plane->autorelease(); \
		return plane; \
	} \
	else \
	{ \
		CC_SAFE_DELETE(plane); \
		return nullptr; \
	} \
} \

class BossType1AI;
class Weapon;
class BossType2AI;
class BossType3AI;
class PlaneAI;
class BasePlane;
class TrackManager;


typedef struct PlanePro
{
	CC_SYNTHESIZE(UnitId,	killer, Killer);
	CC_SYNTHESIZE(int,		score,	Score);
	CC_SYNTHESIZE(int,		gold,	Gold);
	std::vector<DropReward>		DropVec;	

	PlanePro()
	{
		killer = UnitId::DefaultIdx;
		score = 0;
		gold = 0;
		DropVec.clear();
	}

}PlanePro;

class HpBar : public Sprite
{
public:
	HpBar();
	static HpBar* create(BasePlane* plane);
	virtual void onEnter();

	void RefreshHpBar(float percent);

	void setHpBarOffset(const Vec2& offset);

	void SetHpBarVisible(bool visible);

	void setHpBarLengthScale(float scale);

private:
	void ShowHpBar();

private:
	ProgressTimer*		m_HpBar;
	BasePlane*			m_Plane;
	Color3B				m_BgColor;
	Color3B				m_BarColor;
};

class BasePlane : public PhysicsUnit 
{
public:
	BasePlane();
	virtual void onExit();

	virtual void FuncWhenHpChanged();
	virtual void DeathImpactOfDrops();
	virtual void DeathImpactOfDropReward();

	void setBitmask(const int& cate,const int& collision,const int& test);

	void setBltHurts(int hurts);

	void setDefense(int defense);

	void setDeadMusic(const std::string& file);

	void AddOneWp(Weapon* weapon,WeaponType wt);

	void AddWpVec(std::vector<Weapon*> wpVec,WeaponType wt);

	void RemoveOneWeapon(Weapon* weapon);

	void resumeNorWp();

	void clearWeapon();

	void setShootAble(bool b);
	void setNeedFlyToMainPl(bool b);
	virtual void AnimationWhenDead();
	virtual void OtherDealWhenContact(const BasePro& rs);
	virtual void PlayMusicWhenDead();
	virtual void DoSomethingBeforeDead();

	void FlyToMainPlaneWhenSpeedUp();
	void setShootMusicAble(bool b);

	virtual Vec2 getArmatPosOffset();
	virtual Size getSkinSize();

	void addRewards(float HpPct, int wardNum, DropGoodsType dgt);

	void setPlanePro(const PlanePro& pro){m_PlanePro = pro;}
	PlanePro& getPlanePro(){return m_PlanePro;}
	virtual void StartFire(){}
	void setDestroyCallFun(const std::function<void(BasePlane*)> &func);
	std::function<void(BasePlane*)> func;
	void updateHpBarHeight(float height);

protected:

	void ShowHpBar();
	void StartShoot();

	void WudiOver(float dt);

	virtual void initBody();

	virtual void FuncAfterEnter();

	void removePartical(Node* node);

	void FlyToMainPlane(float dt);
    void initDropVecFromDropArr(std::string dropArray);
    void parseOneItem(std::string dropStr);
    void addDropItemToVec(int itemId,int num);
protected:
	PlanePro	m_PlanePro;

	std::vector<Weapon*>			m_SpecialWpVec;
	std::vector<Weapon*>			m_NorWpVec;
	std::vector<Weapon*>			m_TempWpVec;

	CC_SYNTHESIZE_PASS_BY_REF(HpEff, m_HpEff, HpEff);
	std::string						m_Musicfile;
	bool							m_ShootMusicAble;
	bool							m_isNeedFlyToMainPl;
	CC_SYNTHESIZE(bool, m_IsShootAble, IsShootAble);			
	CC_SYNTHESIZE_PASS_BY_REF(Size, m_SkinSize, SkinSize);
	CC_SYNTHESIZE(HpBar*, m_HpBar, HpBar);
    CC_SYNTHESIZE(std::string, barrageArray, BarrageArray);
    CC_SYNTHESIZE(std::string, dropArray, DropArray);
	CC_SYNTHESIZE(int ,m_PlaneId, PlaneId);
	CC_SYNTHESIZE(bool,m_FeignDeath, FeignDeath);
};

class PlaneProtect;
class MainPlane : public BasePlane,public BuffManager
{
public:
	MainPlane();
	static MainPlane* create( PlaneType pt );
	
	void InitSkin();
	virtual bool onTouchBegan( Touch *touch, Event *unused_event );
	virtual void onTouchMoved(Touch *touch, Event *unused_event);
	virtual void onTouchEnded(Touch *touch, Event *unused_event);
	void checkIsMoveing(float dt);
	void initMoveEdge();
	virtual void onEnter();
	virtual void onExit();

	virtual void DoSomethingBeforeDead();
	virtual void AnimationWhenDead();
	virtual void OtherDealWhenContact(const BasePro& rs);
	virtual void ImpactOfHp(const BasePro& pro);
	virtual void initBody();
	void initMoveInParticle();
	void setNeedTrackParticle(bool isNeed);
	void runReadyAction(float dur);
	void addSpeedUpEffect(float dur = -1.0f);
	void delayAddParticle(float time,std::string particlePath,Node* node);
	void toAddParticle(std::string particlePath,Node* node);
	void delayAddNode(float time,float endTime);
	void addTheNode(float endTime);
	void removeSpeedUpEffect();

	void FlyUp();
	void FlyUpCallback();

	void Revive();
	void receiveRewards();
	void receiveRewardsCB();

	void WudiOver(Node* node);

	void AddProtect();
    void startProtect();

	void ShootMusic();
	void setShootMusic(const std::string& musicfile);
	void PlayShootMusic();
	void ShootMusicCB(Node* node);

	void TintWhenHurt(float dur = 1.0f);
	void resumeColor();

	void addMagnet();
	void removeMagnet();

	void playNorAnimation();
	void AnimatFinished();

	ActionInterval* getMoveAnimationL(bool isHead = true);
	ActionInterval* getMoveAnimationR(bool isHead = true);
	ActionInterval* getNorAnimation();

	void WeaponUpgrade();
	void WeaponDowngrade();

	void setSkinOffset(const Vec2& offset);
	void setBody(float t);
	void gameEnd();

	//大招
	void PlayDaZhao();
	void addDaZhao();
	void addFengHuang();

	//清屏大招
	void ClearScreenDazhao();

	void PlaneFadeOut(Node* node);
	void PlaneFadeIn(Node* node);
	void ResumePlaneState();
	void CanbeTouch();

protected:
	Node*			m_SpeedUpNode;
	Node*			m_RechargeNode;
	Node*			m_SpeedUpTail;
	std::string		m_shootMusic;
	Sprite*			m_NorAnimationNode;
	PlaneProtect*	m_Protect;
	CC_SYNTHESIZE(bool, m_IsHasMagnet, IsHasMagnet);
	CC_SYNTHESIZE(bool, m_isTouchEnabled, TouchAble);
	CC_SYNTHESIZE(PlaneType,m_PlaneType, PlaneType);

	bool			m_isMoveAniPlayingL;
	bool			m_isMoveAniPlayingR;
	bool			m_isFinished;
	bool            m_isNeedTrackParticle;
	float			m_TempPosX;
	Vec2			m_PrePos;
	Vec2			m_CurPos;
	int				m_WpLvl;

	float			m_MoveEdgeLeft;
	float			m_MoveEdgeRight;
	float			m_MoveEdgeUp;
	float			m_MoveEdgeDown;

	int				m_bitMaskCate;
	int				m_bitMaskColl;
	int				m_bitMaskCont;
};

//BOSS
class CalculateRating;
class BossTimeOut;
class BossHpBar;
class BaseBoss : public BasePlane
{
public:
	BaseBoss()
	{
		m_BgId = 0;
		m_Armat = nullptr;
		m_BossTimeOut = nullptr;
	}
	void InitSkinWithArmat( const std::string& name );
	Vec2 getMidPos();

	virtual void FuncAfterEnter();

	void BossDelayIn();
	virtual void ImpactOfHp( const BasePro& pro );
	virtual void FuncHpEmpty(const BasePro& pro);
	virtual void DoSomethingBeforeDead();
	virtual void OtherDealWhenContact(const BasePro& rs);

	void BossExplosion();
	void displayBossExplosion();
	void shake();
	void resumeColor();
	void RealDead();
	virtual void addAi(){}
	void addRating();
	void addTimeOut(float maxTime);
	void RemoveTimeOut();

	virtual float BeHitEffRadius();
	virtual Vec2 BeHitEffOffset();

	void SwitchBgWhenDead(int BgId);

	virtual void FuncWhenHpChanged();

	void ShowBossHpBar(int HpNum, int maxHp);
	BossHpBar* getBossHpBar();

	void Angry();
	void AngryCB();

protected:
	CalculateRating*	m_ratingNode;
	PlaneAI*			m_Ai;
	CC_SYNTHESIZE(int, m_BossLvl, BossLvl);
	int					m_BgId;
	CC_SYNTHESIZE(Armature*, m_Armat, Armature);
	CC_SYNTHESIZE(BossTimeOut*, m_BossTimeOut, BossTimeOut);
};

class IronManBoss : public BaseBoss
{
public:
	static IronManBoss* create(const std::string& artFile, int lvl); 

	virtual void addAi();
	virtual void initBody();

	virtual void OtherDealWhenContact(const BasePro& rs);
	virtual Vec2 getArmatPosOffset();

};



class BoneWarriorBoss : public BaseBoss
{
public:
	static BoneWarriorBoss* create(const std::string& artFile, int lvl); 
	virtual void onEnter();

	virtual void addAi();
	virtual void initBody();
	virtual void OtherDealWhenContact(const BasePro& rs);

};

class RenzheBoss : public BaseBoss
{
public:
	static RenzheBoss* create(const std::string& artFile, int lvl);

	virtual void addAi();
	virtual void initBody();
};


class PoseidonBoss : public BaseBoss
{
public:
	static PoseidonBoss* create(const std::string& artFile, int vl);
	virtual void initBody();

	virtual void addAi();
	virtual Vec2 BeHitEffOffset();

};

class FemaleGunBoss : public BaseBoss
{
public:
	static FemaleGunBoss* create(const std::string& artFile, int lvl);
	virtual void initBody();
	virtual void addAi();
};


class EyeBoss : public BaseBoss
{
public:
	static EyeBoss* create(const std::string& artName, int lvl);
	virtual void initBody();
	virtual void addAi();

};


class YizhiBoss : public BaseBoss
{
public:
	static YizhiBoss* create(const std::string& artName, int lvl);
	virtual void initBody();
	virtual void addAi();
};


class DoubleGunBoss : public BaseBoss
{
public:
	static DoubleGunBoss* create(const std::string& artName, int lvl);
	virtual void initBody();
	virtual void addAi();
};


class EnemyPlane : public BasePlane
{
public:
	EnemyPlane()
	{
		m_Picture = nullptr;
		m_Armat = nullptr;
		m_isNeedFlipX = false;
		m_trackMgr = nullptr;
        m_isAutoFire = false;
		m_haveCallBack = false;
        m_isMoveIn = false;
		m_FeignDeath = true;
	}
	~EnemyPlane();
	static EnemyPlane* createWithPicture(const std::string& file, const std::string& TrackId = "0");

	static EnemyPlane* createWithArmature(const std::string& name, const std::string& TrackId = "0");
	virtual void ImpactOfHp(const BasePro& pro);
	virtual void OtherDealWhenContact(const BasePro& rs);
	virtual void FuncHpEmpty(const BasePro& pro);
	virtual void DoSomethingBeforeDead();

	void initTrack(const std::string& TrackId);
	void InitSkinWithPicture( const std::string& file );
	void InitSkinWithArmature( const std::string& name );
	virtual void onEnter();
	void addWeapon();
	void setFlipXonEnter();
	void startTrack();
	virtual void StartFire();
	void addConfigWeapon(bool isOneTime = false);

	//BOSS?ï¿½ï¿½??????????ï¿??
	void ListenBossAngryCB(Ref* sender);

	//BOSS?ï¿½ï¿½??ï¿????????????ï¿??
	void ListenBossAngryEndCB(Ref* sender);

	//自检删除
	void CheckToRemove(float t);

    bool m_isAutoFire;
	bool m_haveCallBack;
private:
	CC_SYNTHESIZE(Sprite*, m_Picture, Picture);
	CC_SYNTHESIZE(PlaneArmature*, m_Armat, PlaneArmature);
	CC_SYNTHESIZE_PASS_BY_REF(WeaponPro, m_WeaponPro, WeaponPro);
	CC_SYNTHESIZE(bool, m_isMoveIn, IsMoveIn);
	TrackManager*		m_trackMgr;
	bool				m_isNeedFlipX;
};

class CalculateRating : public Node
{
public:
	CalculateRating()
	{
		m_SurvivlaTime = 0.0f;
		m_BossType = 1001;
		m_isShow = false;
	}
	static CalculateRating* create();
	void setBossType(int tp);
	void startRating();

	void updateTime(float dt);

	BossRating getRating();
	float getSurvivlaTime(){ return m_SurvivlaTime; };

	void SurvivalTimeRecord();

	void showRating();
	void removeRatingSp(Node* node);

protected:
	float		m_SurvivlaTime;
	int			m_BossType;
	bool		m_isShow;
};

#endif
