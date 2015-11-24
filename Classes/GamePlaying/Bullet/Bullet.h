/********************************************************************

*********************************************************************/

#ifndef _BULLET_H_ 
#define _BULLET_H_



#include "cocos2d.h"
#include "../PhysicsUnit.h"
#include "../Weapon/WeaponDelegate.h"
#include "BulletFactory.h"

USING_NS_CC;

#define CREATE_BULLET(__TYPE__) \
static __TYPE__* create( WeaponDelegate* wpDlgt ) \
{ \
	__TYPE__* bullet = new (std::nothrow) __TYPE__; \
	std::string fileName = BulletFactory::getResByTag( wpDlgt->getBltResTag() ); \
	if (bullet && bullet->initWithSpriteFrameName(fileName)) \
	{ \
		bullet->initBullet(); \
		bullet->autorelease(); \
		bullet->setWeaponDelegate(wpDlgt); \
		bullet->initBody(); \
		return bullet; \
	} \
	CC_SAFE_DELETE(bullet); \
	return nullptr; \
}

class FixDirect;
class Weapon;
class Bullet : public PhysicsUnit
{
public:
	
	static Bullet* create(const std::string& frameName);
	static Bullet* createWithPicture(const std::string& file);
	CREATE_BULLET(Bullet);

	virtual void initBullet();
	virtual void onEnter();

	virtual void ImpactOfHp(const BasePro& pro);	//子弹被碰到就死，除非无敌

	void FixAngleWithWeapon(Weapon* weapon);

	void setBitmask(const int& cate,const int& collision,const int& test);

	void resetHurt(int hurts);

	void ListeningStopWeapon();
	void ListeningBossDead();			//?àìyBOSS?àí?
	void ListeningProtectRemove();		//?àìy?¤?ü??á?

	void setWeaponDelegate(WeaponDelegate* wpDlgt);

	void setBltFlySpeed(int speed);
	int getBltFlySpeed();
	virtual void initBody();

protected:
	void setBodyPro();
	virtual void BossDead(Ref* obj);
	void StopBullet(Ref* obj);
	void ProtectRemove(Ref* obj);
	CC_SYNTHESIZE(Weapon*, m_weapon, Weapon);
	int			m_FlySpeed;
	Vec2		m_OriginVelocity;
	WeaponDelegate*			m_WeaponDelegate;
    bool        couldChange;

	int			m_curBitmaskCate;
	int			m_curBitmaskColl;
	int			m_curBitmaskCont;
};

class MissileForDis : public Bullet
{
public:
	MissileForDis()
	{
		m_particle1 = nullptr;
		m_particle2 = nullptr;
	}

	static MissileForDis* create(const std::string& frameName);
	static MissileForDis* create();

	virtual void onEnter();

	void FlyToTarget(float dt);

	virtual void initBody();
	virtual void OtherDealWhenContact(const BasePro& rs);
	virtual void DoSomethingBeforeDead();
	void addParticle();
	void Explosion();

	void removeExpSp(Node* node);

	void Dying();

	void setBody(float t);
private:
	ParticleSystemQuad*		m_particle1;
	ParticleSystemQuad*		m_particle2;

	Vec2					m_targetPos;
};

class MissileForBloodlimit : public Bullet
{
public:
	MissileForBloodlimit();
	static MissileForBloodlimit* create(const std::string& frameName, const float& AngleOffet);
	virtual void onEnter();
	void FlyToTarget(float dt);

	//DT?y·￠é?ê±μ????è
	void FixDirect();

	virtual void OtherDealWhenContact(const BasePro& rs);

	virtual void addParticle();

	//???¨ê±???ú×??ˉ±??¨??í?
	virtual void Explosion();

protected:
	float				m_AngleOffset;
};

//????Dí×óμˉ
class FallingBullet : public Bullet
{
public:
	static FallingBullet* create(const std::string& frameName);
	virtual void onEnter();

	void newFalling(float dt);
};

//?????????????
class FallingBulletEX : public Bullet
{
public:
	static FallingBulletEX* create(const std::string& frameName);
	virtual void onEnter();

	//??????
	void QuickShoot();
	void shootCallback();

	void newFalling(float dt);
	virtual void initBody();
};

//???????
class CurveBullet : public Bullet
{
public:
	static CurveBullet* create(const std::string& frameName);
	virtual void onEnter();

	void newCurve(float dt);
	virtual void initBullet();

};

//???????2
class CurveBullet2 : public Bullet
{
public:
	typedef enum BullDirection
	{
		Dirct_Left,
		Dirct_Right
	}BullDirection;

	static CurveBullet2* create(const std::string& frameName,const BullDirection& dir);
	virtual void onEnter();

	void newCurve(float dt);
	virtual void initBullet();

private:
	BullDirection _dir;

};

// ???????
class ReboundBullect : public Bullet
{
public:
	ReboundBullect()
	:_collisionTimes(1)
	,m_MaxTimes(2)
	{

	};

	static ReboundBullect* create(const std::string& frameName );
	virtual void onEnter();
	virtual void initBody();
	virtual void ImpactOfHp(const BasePro& rs);

private:
	int			m_MaxTimes;		//????????????
	int			_collisionTimes;
};

// ??
class GrenadeBullect : public Bullet
{
public:
	
	static GrenadeBullect* create(const std::string& frameName);
	virtual void onEnter();
	virtual void initBullet();
	void RandPos();

	virtual void OtherDealWhenContact(const BasePro& rs);

private:

};

//???????
class HelixBullet : public Bullet
{
public:
	
	static HelixBullet* create(const std::string& frameName);
	virtual void onEnter();

	void Helix(float dt);
	virtual void initBullet();
private:
	Vec2	m_centerPos;
	Vec2	m_beginPos;
	float	m_randNun;
};

//????+???
class HelixBulletEx : public HelixBullet
{
public:
	static HelixBulletEx* create(const std::string& frameName);
	virtual void onEnter();
	virtual void initBody();

	//?????
	void StopHelix();

	//???????
	void MoveToPl();

};

//???????
class DartBullet : public Bullet
{
public:
	
	static DartBullet* create(const std::string& frameName);
	virtual void onEnter();
	virtual void initBullet();
	virtual void initBody();

	void PlayAction();

	void addParticle();

	void addForce();
	
};

//С???????,?????????????????
class DartBulletRebounce : public Bullet
{
public:
	static DartBulletRebounce* create(const std::string& frameName, int direct);
	virtual void onEnter();
	virtual void initBody();

	//????λ???????????????????2??????????????????
	void PlayAction();
	void fixAngle();

	virtual void OtherDealWhenContact(const BasePro& rs);

private:
	int			m_direct;
};

//С????
class DartBulletSmall : public Bullet
{
public:
	static DartBulletSmall* create(const std::string& framename);
	virtual void initBody();
	virtual void onEnter();
};

//???????
class DartBulletTrace : public Bullet
{
public:
	static DartBulletTrace* create(const std::string& framename);
	virtual void initBody();
	virtual void onEnter();

	//????????
	void UpdatePos(float dt);
	void fixPos();

	//????????body,???????body??????????
	void Hurt(float dt); 
	void addParticle();
};

//???????
class LaserBullet : public Bullet
{
public:
	static LaserBullet* create(int lvl = 1);
	virtual void initBody();
	virtual void initBullet();
	virtual void onEnter();
	virtual void onExit();

	void setBody(float dt);

	//?????о
	void addLaserCore();

	//???????
	void addShootPoint();

	ActionInterval* getCommonAction(float scaleX = 1.0f, float scaleY = 1.0f);

	void removeLaser();
	void removeLaserO(Ref* obj);

	//??????????????
	void addExtraLight();

	//?????????
	void ExtraCircle();
	void addExtraCircle();

	virtual Vec2 getLaserCoreOffset();
	virtual Vec2 getShootPointOffset();

protected:
	Sprite*			m_ShootSp;
	float			m_timeIn;
	float			m_TimeToScale;
	float			m_TimeToBack;
	float			m_dur;
	float			m_scaleY;
	float			m_scaleX;
	int				m_level;
};

//??Χ????
class OutsideLaser : public LaserBullet
{
public:
	OutsideLaser();
	static OutsideLaser* create(const Vec2& pos, int zorder);
	virtual void onEnter();

	//???????????spriteEaseCircleInOut????
	void MoveCircle();

	void addDipan();
	virtual Vec2 getLaserCoreOffset();
	virtual Vec2 getShootPointOffset();
private:
	Vec2			m_pos;
	int				m_zorder;
};

//è
class CatBullet : public Bullet
{
public:
	enum CatType
	{
		cat,
		wolf,
		tiger
	};
	
	static CatBullet* create(bool isleft, CatType ct = cat);
	virtual void initBullet();
	virtual void initBody();
	virtual void onEnter();
	void initCat();
	void FirstJump();		//?????

	//?????
	void JumpState();

	//?????
	void StayState();

	//?????????λ??
	void NextPos();

	//???
	void TurnDirect();

	//????è????
	void AdjustCatAngle();

	//???????
	void CatRun();

	virtual void OtherDealWhenContact(const BasePro& rs);

	//?????????????
	bool isTargetInScreen(const Vec2& pos);

	//è??????????
	void CatLeave();
	void LeaveCB();

	//??????????(б45??)
	void FixJumpPos();

private:
	//?????л???è?
	void addClawL();
	void addClawR();

private:
	CatType		m_catType;	
	Vec2		m_contactPos;
	Vec2		m_NextRunPos;		//???????????
	Vec2		m_JumpVec;			//???????

	Sprite*		m_catStay;			//?????è
	Sprite*		m_catJump;			//?????è

	int			m_direct;			//????-1??+1???
	bool		m_timeToLeave;		//??
	float		m_ExDis;			//??BOSS????????????

};

//è?
class CatClaw : public Bullet
{
public:
	static CatClaw* create(const Vec2& direc);
	virtual void onEnter();
	
	virtual void initBody();

	//??ж???????С??????????

	void Catch();

private:
	Vec2			m_MvDirect;
};

//BOSS????
class BossDart : public Bullet
{
public:
	static BossDart* create(const std::string& frameName);
	virtual void initBody();
	virtual void initBullet();
	
	void addParticle();
};

//????
class FlowerBullet : public Bullet
{
public:
	static FlowerBullet* create();
	virtual void initBody();
	virtual void initBullet();
	virtual void onEnter();
	void initFLower();
	//????????body?γ???????
	void setBody(float dt);
	void start();

	//??ж???????????????????????
	void FLowerAction();
	void Eff1Action();
	void Eff2Action();

	//??????
	void Explosion();

	//????????
	void DropFlower();

private:
	Sprite*		m_Flower;
	Sprite*		m_eff1;
	Sprite*		m_eff2;
	float		m_radius;
};

//???
class Flaming : public PhysicsUnit
{
public:
	enum FireType
	{
		eSmall,
		eMiddle,
		eBig,
	};

	static Flaming* create(FireType ft = eBig);
	virtual void initBody();
	virtual void onEnter();
	void setBody(float dt);

	//???????
	void DelayShowSparks();
	void DelayShowFireCircle();

	//??
	void Sparks();

	//????
	void FlamCircle();
private:
	ParticleSystemQuad* m_particle;
	FireType			m_fireType;
	Size				m_FireSize;
};

//?????
class FireDragon : public Bullet
{
public:
	FireDragon()
	{
		m_node = nullptr;
	}
	static FireDragon* create(Flaming::FireType ft);
	virtual void initBody();
	virtual void onEnter();

	//???????????
	void MoveUp();

	//???
	void ShootFire();

	void addArmature();
	
private:
	Node*				m_node;
	Flaming::FireType	m_fireType;
};

//????????????
class FireWorks : public Bullet
{
public:
	static FireWorks* create(const std::string& frameName);

	virtual void initBody();

	virtual void onEnter();

	//??N???????????????
	void resetLinearDamping();
};

//???????
class EneRebounceBullet : public Bullet
{
public:
	static EneRebounceBullet* create(const std::string& frameName);
	virtual void initBody();
	virtual void OtherDealWhenContact( const BasePro& rs );
};

//????з????????
class BulletToTarget : public Bullet
{
public:
	static BulletToTarget* create(const std::string& frameName, bool isSelf = true);
	virtual void initBody();

	virtual void onEnter();

	/****************?з?**************/
	void FixAngleToMainplane();		//?????????

	/****************????**************/
	//??????
	void FixAngleOfSelf();

	//?????
	void FindAmiOfSelf(int direct);

private:
	bool			m_isSelf;
};

//Χ???????????
class BulletSurroundMainPl : public Bullet
{
public:
	BulletSurroundMainPl()
	{
		m_dur = 8.0f;
	}
	static BulletSurroundMainPl* create(int lvl);
	virtual void onEnter();
	virtual void initBody();

	void addSurroundBullet(float angle, bool isflyout = true);

	void setBody(float dt);
	float getDur();

	void removeMeO(Ref* obj);
private:
	int			m_Level;		//??????
	float		m_radius;		//?????
	float		m_curBodyRadius;//?????
	float		m_dur;			//???????
};

class SurroundBullet : public Sprite
{
public:
	static SurroundBullet* create(const std::string& file, float dur, bool isflyout = false);
	virtual void onEnter();
	virtual void onExit();

	//????
	void FlyOut();

	//??????
	void FixRotate();
	void setOffset(const Vec2& pos);

	void removeMe();
	void setDur(float t);
private:
	bool			m_isFlyOut;
	Vec2			m_offset;
	float			m_dur;

};

//?????????
class FlyOutBullet : public Bullet
{
public:
	static FlyOutBullet* create();
	virtual void initBody();
};

//?????????????????????????????????????????
class ThreePetalsBullet : public Bullet
{
public:
	static ThreePetalsBullet* create(const std::string& file);
	virtual void initBody();

	virtual void onEnter();
	void addParticle();
	virtual void DoSomethingBeforeDead();
};

//??????
class AirazorBullet :  public Bullet
{
public:
	AirazorBullet()
	{
		m_tail = nullptr;
	}
	static AirazorBullet* create(const std::string& file, float r, bool isPierce);
	virtual void onEnter();
	virtual void initBody();

	//?????????????
	void appear();

	//???????????????
	void FixAngle();

	//????
	void FlyOut();

	//??????β
	void AddTail();

	virtual void DoSomethingBeforeDead();

private:
	float			m_rotate;		//??????
	bool			m_isPierce;		//????
	Vec2			m_flyDirect;	//???з???
	MotionStreak*	m_tail;
};

//???絥??????
class DrawLighting : public Node
{
public:
	DrawLighting();
	static DrawLighting* create(const Vec2& startPos,const Vec2& endPos);
	virtual void onEnter();

	//?????????
	void RefreshPicture();
	void removeMe();
private:
	Vec2			m_startPos;
	Vec2			m_endPos;
	Sprite*			m_Sp;
};

//?????
class NewLightingLayer : public Layer
{
public:
	NewLightingLayer();
	static NewLightingLayer* create(int num, int hurt);
	virtual void onEnter();
	virtual void onExit();
	void setHurt(int hurt);

	//???????????
	void FindAllTargets();
	bool FindTarget(Vec2& startPos);	//??????

	bool isFound(PhysicsUnit* target);

	bool isInScreen(const Vec2& pos);

	//?????????
	void DrawLight();

	//??????磬???????
	void StartLighting();

	void removeMeO(Ref* obj);

private:
	std::vector<PhysicsUnit*>	m_targetList;	//???????????б?
	std::vector<Vec2>			m_posList;		//???????б?
	int							m_lightingNums;	//????????
	float						m_EdgeL;
	float						m_EdgeR;
	float						m_EdgeD;
	float						m_EdgeU;
	int							m_hurt;
};

//???繥????
class LightingAttackPoint : public Bullet
{
public:
	static LightingAttackPoint* create();
	virtual void initBody();

	virtual void onEnter();
};

//?????
class ExplosionBullet : public Bullet
{
public:
	static ExplosionBullet* create( float angle);
	virtual void initBody();
	virtual void onEnter();

	//?????????
	void FixBulletAngle();

	//????????????????????????8??????
	void Explosion();

	//??????????????????????????????????
	void ExplAction();

private:
	float			m_angle;
};

//Boss3?????
class DartOfBoss3 : public PhysicsUnit
{
public:
	static DartOfBoss3* create();
	virtual void initBody();
	virtual void onEnter();

	void setBody(float dt);
	void addParticle();
};

//Boss4?????1,???????????????????????
class Bullet1OfBoss4 : public Bullet
{
public:
	Bullet1OfBoss4()
	{
		m_StayTime = 2.0;
	}
	static Bullet1OfBoss4* create(ResTag res,float stayTime);
	virtual void initBody();
	virtual void onEnter();

	static void BreathAction(Node* node);

	//???????
	void FloatUpDown();

	void Helix(float dt);

	//?????
	void StopHelix();

	//???????
	void MoveToPl();

	//????????
	void Start();

private:
	float		m_StayTime;
};

//Boss4?????2?????????????????
class Bullet2OfBoss4 : public Bullet
{
public:
	static Bullet2OfBoss4* create();
	virtual void initBody();
	virtual void onEnter();

	//??????????
	void MoveBezier();

	//??????
	void StartFloat();
	void newFalling(float dt);
};

//Boss4?????3??????????????????????????????λ??????????????????????????????
class Bullet3OfBoss4 : public Bullet
{
public:
	Bullet3OfBoss4()
	{
		m_MaxChance = 2;
	}
	static Bullet3OfBoss4* create();
	virtual void initBody();
	virtual void onEnter();
	void expolsion();

	//???????
	static void addWp(Node* node, int hurt);
};

//Boss4?????4?????????3???????????λ???????????????????
class Bullet4OfBoss5: public Bullet
{
public:
	Bullet4OfBoss5()
	{
		m_dur = 0.0f;
	}
	static Bullet4OfBoss5* create(float dur);
	virtual void initBody();
	virtual void onEnter();
	void expolsion();

private:
	float			m_dur;
};

//????
class BulletMeteor : public Bullet
{
public:
	static BulletMeteor* create(const std::string& file, float v);
	virtual void initBody();
	virtual void onEnter();
	void addParticle();
	void randRemove();
	void explosion();
	virtual void DoSomethingBeforeDead();

private:
	float			m_velocity;
};

//????
class BulletConvolution  : public Bullet
{
public:
	BulletConvolution()
	{
		m_CurCount = 0;
		m_ConvolutionDur = 0.2f;
		m_ConvolutionTimes = 2;
	}
	CREATE_BULLET(BulletConvolution);
	virtual void onEnter();

	void BulletReverse();

	CC_SYNTHESIZE(float,	m_ConvolutionDur,		ConvolutionDur);
	CC_SYNTHESIZE(int,		m_ConvolutionTimes,		ConvolutionTimes);

private:
	int				m_CurCount;
};

//???????
class CrossBullet : public Bullet
{
public:
	static CrossBullet* create(const std::string& file, Weapon* weapon);
	virtual void initBody();

	virtual void onEnter();

	void cross(float dt);

};


//?з????????
class DirectBullet : public Bullet
{
public:
	DirectBullet()
	{
		m_FlyTime = 0.3f;
	}
	static DirectBullet* create(const std::string& file, float angleOffset, bool isLeft, bool isFlipX = false);

	virtual void initBody();

	virtual void onEnter();
	virtual void DoSomethingBeforeDead();

	void Start();

	void refreshAngle(float t);

	void runBezier();

	void GoBack();

	void ResumeRotate();

	//???
	void ShuaGuang();

private:
	Vec2			m_prePos;
	Vec2			m_curPos;
	float			m_FixAngle;
	float			m_curAngle;

	float			m_AngleOffset;
	bool			m_isFlipX;
	bool			m_isLeft;

	float			m_OriginAngle;
	CC_SYNTHESIZE(float,	m_FlyTime, FlyTime);
};

//???BOSS?????????????+???
class FallBulletOfEyeBoss : public Bullet
{
public:
	static FallBulletOfEyeBoss* create(const std::string& file);
	void initBody();

	virtual void onEnter();
	void FlyToTarget();
};

//???BOSS?????????????????
class BezierBulletOfEyeBoss : public Bullet
{
public:
	static BezierBulletOfEyeBoss* create(const std::string& file);
	void initBody();

	virtual void onEnter();
	void runBezier();
};

//???BOSS????????????????
class DanxiangLaserOfEyeBoss : public Bullet
{
public:
	DanxiangLaserOfEyeBoss();
	static DanxiangLaserOfEyeBoss* create(float t = -1);
	virtual void onEnter();
	virtual void onExit();
	void initBody();

	void ShootPos();

	void ShootLaser();

	void FadeInAndRemove();

	void Hurts(float t);

	void setLaserLength(float length);

	void LaserSaoshe(float t, float beginAngle, float angle);

	void UpdateLaserRotate(float t);

	void AddWarning(float t);

	void LaserBegin();

	virtual void BossDead(Ref* obj);

private:
	Bullet*			m_laser;
	Sprite*			m_LaserPoint;		//????????
	float			m_dur;
	float			m_laserLength;
	float			m_saosheAngle;
	float			m_saosheTime;
	float			m_beginAngle;
	float			m_warningTime;
	CC_SYNTHESIZE(float,	m_laserWidth,  LaserWidth);
};

//???BOSS?????????????????
class TelescopicLaserOfEnyBoss : public Bullet
{
public:
	TelescopicLaserOfEnyBoss()
	{
		m_time = 0;
		m_TextureId = 0;
	}
	static TelescopicLaserOfEnyBoss* create(const std::string& file);
	void initBody();
	virtual void onEnter();

	//????
	void MoveDown();

	//??????????
	void addEyes();
	void updateEyeTelescopicLaser(float t);
	void MoveLR();
private:
	std::string			m_FileName;
	Sprite*				m_Eye1;
	Sprite*				m_Eye2;
	Sprite*				m_laser;
	float				m_time;
	int					m_TextureId;
};

//???BOSS????????????????
class DrawShapeOfEyeBoss : public Layer
{
public:
	DrawShapeOfEyeBoss()
	{
		m_LaserCount = 0;
	}
	~DrawShapeOfEyeBoss()
	{
		for (int i = 0; i < m_EpVec.size(); i++)
		{
			EyePro* ep = m_EpVec.at(i);
			delete ep;
		}
	}
	struct EyePro
	{
		int beginId;
		int endId;
		EyePro(int id1, int id2)
		{
			beginId = id1;
			endId = id2;
		}
	};
	CREATE_FUNC(DrawShapeOfEyeBoss);
	virtual void onEnter();
	virtual void onExit();
	
	void initEyesPos();

	void addEyeOneByOne();

	void addOneLaser();

	void addOneLaserCB(Node* node, void* data);

	void RemoveMe(Ref* obj);

private:
	std::vector<Vec2>		m_EyePosVec;
	std::vector<EyePro*>	m_EpVec;
	std::vector<EyePro*>::iterator	m_EpVecIt;
	int						m_LaserCount;
};

//???BOSS??????????????
class AutoExplosionOfYizhiBoss : public Bullet
{
public:
	AutoExplosionOfYizhiBoss()
	{
		m_ExplTime = 1.0f;
		m_IsExpl = false;
	}
	static AutoExplosionOfYizhiBoss* create(const std::string& name);
	virtual void initBody();

	void autoExplosion();

	void setExplosionTime(float t);
	virtual void OtherDealWhenContact(const BasePro& rs);

	virtual void onEnter();
private:
	float			m_ExplTime;
	bool			m_IsExpl;
};

//???BOSS??????????????
class SpiralMissile : public Bullet
{
public:
	static SpiralMissile* create(const std::string& name);
	virtual void onEnter();
	virtual void initBody();

	void FlyToTarget( float dt );

	//???????????λ??
	void RandChangeTargetPos();

	//???????λ??
	void LockTargetPos(float t);

	Vec2 getRandPos();

private:
	Vec2						m_targetPos;
	ParticleSystemQuad*			m_particle;
};

//???BOSS?????????????????
class SpeedUpBulletOfYizhiBoss : public Bullet
{
public:
	static SpeedUpBulletOfYizhiBoss* create(const std::string& name);

	virtual void initBody();
	virtual void onEnter();

	void start();
	void SpeedUp(float t);
};

//???BOSS????????????+????
class ExplosionCyclotronOfYizhiBoss : public Bullet
{
public:
	static ExplosionCyclotronOfYizhiBoss* create(const std::string& name);

	virtual void initBody();
	void autoExplosion();

	virtual void onEnter();
};

//???BOSS?????????????+????
class SpeedDownAndUniform : public Bullet
{
public:
	static SpeedDownAndUniform* create(const std::string& name);
	virtual void initBody();
	virtual void onEnter();
	void SlowDown();
};

//烟花
class FireworkBullet : public Bullet
{
public:
	static FireworkBullet* create(int hurt);
	virtual void initBody();
	virtual void onEnter();
	virtual void DoSomethingBeforeDead();

	void FlyToTarget( float dt );

	void StartBaidong();
	void baidong(float t);

	void Dying(Node* node);

private:
	ParticleSystemQuad* m_particle1;
	Vec2				m_targetPos;
	int					m_direct;
	bool				m_Baidong;
	int					m_hurt;
};


class FixDirect
{
public:
	void setFixTarget(Sprite* sp);

	void BeginFix();
private:
	Sprite*		m_FixTarget;
	Vec2		m_CurPos;
	Vec2		m_PrePos;
};

#endif
