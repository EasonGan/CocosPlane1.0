/********************************************************************
创建于:		2014/11/27  11:10	
文件名: 	E:\Cocos3.3_Project\MyGame\Classes\EffectsManager.h 
作  者:		alex(alex)
修改于：	2014/11/27  11:10	

目  的:		特效管理界面层
*********************************************************************/
#ifndef _EFFECTS_MANAGER_H_
#define _EFFECTS_MANAGER_H_


#include "cocos2d.h"
#include "cocos-ext.h"
#include "../GamePlaying/PhysicsUnit.h"
#include "spine/extension.h"
#include "GameConfig.h"

USING_NS_CC;
USING_NS_CC_EXT;

#define REPEAT_FORVER 10000000
//子弹击中目标特效类型
enum class BulletEffectType
{
	TYPE1,
	TYPE2,
	TYPE3
};

class BasePlane;
class EffectsManager : public Layer
{
public:
	EffectsManager()
	{
		m_MeteorNum = 0;
	}
	CREATE_FUNC(EffectsManager);

	//子弹击中目标后播放的特效
	void displayEffectsHitTarget(const Vec2& displayPos);

	//范围爆炸特效
	void displayScopeAmmo(const Vec2& displayPos);

	//燃烧爆炸特效
	void displayBurningExplosion(const Vec2& pos);

	//电圈特效
	void displayEletricCircle(Sprite* node);

	//横向激光特效
	void displayHorizontalLaser();

	//瞬间放慢游戏，持续若干秒
	void displaySlowdown(float duration = 1.2f, float timescale = 0.1);

	//播放序列帧动画
	void displayAnimation(Node* parent,const char* plist, const char* animationName, bool isRepeatForever = false);

	//播放死亡爆炸动画
	void displayDeathExplosion(const Point& pos);

	//全屏减速特效
	void displaySlowDownEffect(const Vec2& displayPos);

	//曲线激光的序列帧
	void displayQuxianLaserAnimation(Node* node);

	//屏幕抖动
	static void ScreenShake(Node* node, float t = -0.1f);

	//精灵抖动
	static void NodeShake(Node* node, float t = -0.1f);

	//白猫
	void displayAniOfWhiteCat(Node* node);

	//播放闪电受击序列帧
	void displayLightingEff(Node* node);

public:
	//播放普通闪电序列帧
	void playLightAnimation(const Point& startPos, const Point& endPos);

	//播放大招闪电序列帧
	void playSpecialLightAnimation(const Point& startPos, const Point& endPos);

	//播放激光序列帧2
	void playLaserAnimation1(Sprite* sp); 

	//播放激光序列帧2
	void playLaserAnimation2(Sprite* sp); 

	//范围爆炸序列帧
	void playExplosionAnimation(Sprite* sp);

	//电圈序列帧
	void playElectricAnimation(Sprite* sp);

	//反弹墙前段序列帧
	void playBounceWallBeginAnimation(Sprite* sp);

	//反弹墙后段序列帧
	void playBounceWallRepeatAnimation(Sprite* sp);

	//回收挡板序列帧
	void playRecyclePadAnimation(Sprite* sp);

	//火鸟
	void playFireBirdAnimation(Sprite* sp);

	//风球与敌机碰撞效果
	void playWindEnemyContactAnimation(Node* node);

	//龙卷风
	void playTornadoAnimation(Node* node);

	//燃烧掉血效果
	void playBurningAnimation(Node* node);

	//火球正常模式序列帧动画
	void playFireBallNormalAnimation(Node* node);

	//火球能量满模式序列帧动画
	void playFireBallEnergyFullAnimation(Node* node);

	//电球普通状态序列帧动画
	void playElectricBallNormalAnimation(Node* node);

	//电球满能量状态序列帧动画
	void playElectricBallEnergyFullAnimation(Node* node);

	//电球与敌机碰撞序列帧动画
	void playElectricContactAnimation(Node* node);

	//主角1普通序列帧
	Animate* getMainPl1Animation_N();

	//主角1满级序列帧
	Animate* getMainPl1Animation_S();

	//主角2普通序列帧
	Animate* getMainPl2Animation_N();

	//主角2满级序列帧
	Animate* getMainPl2Animation_S();

	//主角3普通序列帧
	Animate* getMainPl3Animation_N();

	//主角3满级序列帧
	Animate* getMainPl3Animation_S();

	//弹珠出现去前的序列帧
	ActionInterval* playHoodleAnimation();

	static ParticleSystemQuad* addParticle(const std::string& file, Node* parent,const Vec2& pos = Vec2(0,0),bool autoRM = true, float delay = -1.0f, int Zorder = 10);

	//主角死亡
	void playActionOfMainPlDead(BasePlane* node, ActionInterval* func);

	//主角接到奖励时的特效
	void playEffGetRewards(int tp);

	//序列帧
	static Animate* getFramesAnimation( const std::string& qianzui, int nums, float dt, int start = 0, int loop = 1,bool iscache = true);
	static Animate* getFramesAnimation( const std::string& qianzui, float dt = 1.0/24, int loop = 1,bool iscache = true);

	//飞机禁止移动
	void PlaneCantTouchAble(float t);
	void PlaneResumeTouch();

private:
	//范围爆炸回调
	void removeScopeAmmo(Node* sender);

	//横向激光回调
	void removeHorizontalLaser(Node* sender);

	//放慢后回调，恢复正常速度
	void resumeScheduler();

	//序列帧动画删除
	void removeAnimation(Node* node);

	//序列帧播放时替换的精灵
	Sprite* AnimationSprite(Node* node);
private:
	int			m_MeteorNum;	//流星雨数量
};

//范围爆炸
class ScopeEffect : public PhysicsUnit
{
public:
	static ScopeEffect* create(const char* file,const Size& size = Size(90,90));
	virtual void initBody();
	void initSelfPro();
	virtual void FuncAfterEnter();

private:
	Size			m_expSize;
};

//血量数字减少显示
class BloodDecreaseTTF : public Node
{
public:
	static BloodDecreaseTTF* create(Node* targert, int bloodNum);

	void addTTF(Node* targert,const int& num);

	void removeTTF();
};

//半屏闪电
class BigLighting : public PhysicsUnit
{
public:
	static BigLighting* create();
	virtual void initBody();
	void initSelfPro();
	virtual void FuncAfterEnter();

	//立马删除自己
	void removeCallback();
};


//将半屏闪电写成类，加上特效
class LightingLayer : public Layer
{
public:
	CREATE_FUNC(LightingLayer);
	virtual void onEnter();

	void Lighting(const int& times);

private:
	//加上特效
	void addEffect();
	void effectCallback(Node* node);

	//闪电刚体
	void addLight();
	void lightCallback();

private:
	float		m_fadeIntime;
};

//陨石
class FallingStone : public PhysicsUnit
{
public:
	static FallingStone* create();

	virtual void initBody();
	virtual void FuncAfterEnter();
	void initSelfPro();

	//警示
	void warning();

	//掉落
	void falling();


};

//BOSS爆炸
class ExplosionSprite : public Node
{
public:
	CREATE_FUNC(ExplosionSprite);

	virtual void onEnter();

	//void addParticle();

	void displayParticle();

	void RemovePartical(Node* node);

	void PartEff1();
	void PartEff2();
	void PartEff3();
	void shake();

	//全屏清怪
	void clearAllEnemy();

private:
};


//BOSS出现警告
class WarningOfBoss : public Layer
{
public:
	CREATE_FUNC(WarningOfBoss);
	virtual void onEnter();

	//滚动竖条
	void RollSp(bool isUp);

	//中间文字
	void MidWenzi();

	//全屏变红
	void ScreenRed();
	void removeWarning();

};

//椭圆运动
class  EllipticFunc
{
public:
	// 定义一个结构来包含确定椭圆的参数
	typedef struct _EllipticConfig {
		Vec2 conterPos;
		//椭圆a长，三角斜边
		float aLength;
		//椭圆c长，三角底边
		float cLength;

		_EllipticConfig(const Vec2& pos, float aL, float cL)
		{
			conterPos = pos;
			aLength = aL;
			cLength = cL;
		}
	}EllipticConfig;


	static inline float tuoyuanXat( float a, float c, float t, bool isleft )//返回X坐标
	{
		//参数方程
		if (isleft)
		{
			return a*cos(2*PI*t)-a;
		}
		
		return -a*cos(2*PI*t)+a;
	}
	static inline float tuoyuanYat( float a, float c, float t )//返回Y坐标
	{
		float b = sqrt(powf(a, 2) - powf(c, 2));//因为之前定义的参数是焦距c而不是短半轴b，所以需要计算出b
		//参数方程
		return b*sin(2*PI*t);
	}
};


//流星层
class MeteorLayer : public Layer
{
public:
	CREATE_FUNC(MeteorLayer);
	virtual void onEnter();

	void StartMeteor();

	void RemoveMe();

	void InitMeteorData();

private:
	float		m_dur;	//持续时间
	float		m_Hurt;	//伤害
};

//符咒
class TalismanLayer : public Node
{
public:
	TalismanLayer()
	{
		m_isStop = false;
	}
	CREATE_FUNC(TalismanLayer);
	virtual void onEnter();
	virtual void onExit();

	//中间字符
	void MidText();

	void RemoveMe();

	void StopText();

	Node* createHalfEff();

	void RotateSurround();

	void StopTalisman(Ref* obj);

private:
	int			m_TextId;
	Size		m_ws;
	Sprite*		m_UpderNode;
	CC_SYNTHESIZE(float, m_dur, duration);
	bool		m_isStop;
};

//弓箭
class Bow : public PhysicsUnit
{
public:
	Bow()
	{
		m_shootTimes = 0;
	}
	static Bow* create();
	virtual void onEnter();

	void InitNode();
	void Start();
	void removeSelf();

	//聚气动作
	Animate* JuqiAnimat();

	//弓发光动作
	Animate* BowShine();

	void ShootOut();
	void Hurt();
	void setBody();

private:
	int					m_shootTimes;
};


//激光大招
class MainPlane;
class SpecialLaser : public Node
{
public:
	SpecialLaser()
	{
		m_BulletHurt = DataConfig::getDazhaoHurtOfBullet();
		m_LaserHurt = DataConfig::getDazhaoHurtOfLaser();
	}
	CREATE_FUNC(SpecialLaser);
	virtual void onEnter();
	void setPlane(MainPlane* plane);

	//先发射烟花子弹
	void ShootFirework();

	//聚齐
	void Juqi();

	//喷口
	Animate* PenKou1();
	Animate* PenKou2();
	Animate* PenKou3();

	//发射
	void Shoot();

	//消失
	void disapear();

	void RemoveMe();

private:
	Sprite*			m_JuqiNode;
	Sprite*			m_animatNode;
	MainPlane*		m_Plane;
	float			m_BulletHurt;
	float			m_LaserHurt;

};

//凤凰大招
class FengHuangSkill : public Layer
{
public:
	FengHuangSkill()
	{
		m_BulletHurt = DataConfig::getDazhaoHurtOfBullet();
		m_ExplHurt = DataConfig::getDazhaoHurtOfLaser();
		m_Plane = nullptr;
	}
	CREATE_FUNC(FengHuangSkill);
	void onEnter();
	void setMainPlane(MainPlane* pl);

	void ShootFirework();

	void FenghuangFly();

	void FenghuangBaozha();

	void removeMe();

private:
	int			m_BulletHurt;
	int			m_ExplHurt;
	MainPlane*		m_Plane;

};

//持续造成伤害的刚体
class AttackContinuous : public PhysicsUnit
{
public:
	static AttackContinuous* createCircle(float radius, float dur, int hurts, 
		int cateBit, int collisionBit, int contactBit);
	static AttackContinuous* createBox(const Size& size, float dur, int hurts, 
		int cateBit, int collisionBit, int contactBit);
	void UpdateBox(float dt);
	void UpdateCircle(float dt);
	void initBox(const Size& size);
	void initCircle(float radius);

	void RemoveWhenTimeout(float dur);
private:
	float		m_dur;
	float		m_radius;
	Size		m_Size;
	int			m_cateBit;
	int			m_collosionBit;
	int			m_contactBit;
	int			m_hurts;
};

//大招提示
class SpecialSkillTips : public Layer
{
public:
	static SpecialSkillTips* create(int lvl);

	void addClippingNode();
	void addTips();

	virtual void onEnter();
	Node* getClippingContent();
	void removeMe();
	static void StartSpecialSkill(int lvl = 0);

	virtual void update(float delta);

private:
	Sprite*			m_bg1;
	Sprite*			m_bg2;
	Size			m_ws;
	int				m_lvl;
};

//通关
class CompleteGame : public Layer
{
public:
	static CompleteGame* create(int lvl);
	void addClippingNode();
	Node* getClippingContent();
	void addBg();

	virtual void onEnter();

	//跳转到结果界面
	void TurnToResultLayer();

private:
	Size			m_ws;
	int				m_lvl;
};

//地图切换
class MapSwitchTips : public Layer
{
public:
	static MapSwitchTips* create(int lvl, float restTime, float delay = 0.5f);
	virtual bool init();
	virtual void onEnter();
	void setRestTime(float t);
	void addTips();
	void removeSelf();

private:
	int			m_lvl;
	Size		m_ws;
	float		m_delay;
};

//飞行距离
class FlyDistance : public Node
{
public:
	static FlyDistance* create();
	virtual void onEnter();
	void refreshDis(float dt);

private:
	int			m_dis;
};

//眼睛BOSS的眼睛
class EyeOfBoss : public Sprite
{
public:
	EyeOfBoss()
	{
		m_FuncTarget = nullptr;
		m_func = nullptr;
		m_OpenTimes = 0;
	}
	static EyeOfBoss* create(const std::string& eyeName, const std::string& eyelidName );

	void OpenEye();
	void OpenEyeWithoutFunc();
	void CloseEye();

	void OpenEyeWithFunc(float delay, float stayTime, Ref* target, SEL_CallFunc fun, int OpenTimes = 0);

private:
	void addEyelid(const std::string& eyelidName);
private:
	Sprite*				m_EyeLid;
	Ref*				m_FuncTarget;
	SEL_CallFunc		m_func;
	CC_SYNTHESIZE(float, m_WpAngle, WpAngle);
	std::string			m_eyelidName;
	int					m_OpenTimes;
};

//复活读秒
class PauseLayer : public LayerColor
{
public:

	enum ActivateTag
	{
		ActivateTag_None,			// 无
		ActivateTag_Fuhuo,			// 激活复活
		ActivateTag_DaZhao,			// 激活大招
		ActivateTag_HuDun,			// 激活护盾
	}; 

	PauseLayer()
	{
		m_Id = 3;
		m_isAddPlane = false;
		mActivateType = ActivateTag_None;
	}
	CREATE_FUNC(PauseLayer);
	virtual void onEnter();
	virtual bool onTouchBegan(Touch *touch, Event *unused_event); 

	void ResumeGame();

	//显示一个数字
	void addOneNumber();

	void setAddMainPlane();
	CC_SYNTHESIZE(ActivateTag, mActivateType, ActivateType);

private:
	int			m_Id;
	bool		m_isAddPlane;
};


//闪烁
ActionInterval* ShanshuoAction(const float& time); 

//椭圆运动
void RunEllipticAction(Node* node,const EllipticFunc::EllipticConfig& ec, float time, bool isleft);
cocos2d::Point MoveStep( const Point& Amipos, const Point& myPos,const float& maxPix );

#endif