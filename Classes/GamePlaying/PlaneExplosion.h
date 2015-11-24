/********************************************************************
	created:	2015/09/22
	created:	22:9:2015   15:27
	filename: 	E:\cocos3.4_projects\PlaneGame_Danji\Classes\GamePlaying\PlaneExplosion.h
	file path:	E:\cocos3.4_projects\PlaneGame_Danji\Classes\GamePlaying
	file base:	PlaneExplosion
	file ext:	h
	author:		
	
	purpose:	飞机死亡的爆炸效果
*********************************************************************/
#ifndef PlaneExplosion_h__
#define PlaneExplosion_h__
#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

struct GroupExpl
{
	Vec2	pos;
	float	delayTime;
};

class SingleExpl : public Sprite
{
public:
	static SingleExpl* create(float delayTime, bool isParticle = false);
	virtual void onEnter();

	void Explosion();

	void RemoveMe();
private:
	float			m_delayTime;
	bool			m_isParticle;
};

class BasePlane;
class BaseExplosion : public Node
{
public:
	BaseExplosion();
	CREATE_FUNC(BaseExplosion);
	virtual void onEnter();
	void setPlane(BasePlane* pl);
	virtual void StartExplosion();

	void RemovePlaneAfterTime(float t);

	std::vector<GroupExpl> getGroupExpl(int num = 4);

	SingleExpl* CommonExpl(const Vec2& pos, float t, bool isParticle = false, bool isTail = false);
	void OneExplosionCircle( float radius, float delay, float scale, const Vec2& offset = Vec2::ZERO );

	void PlayParticle1(float time, float scale, int Zorder = 0);
	void PlayParticle2(float time, float scale, int Zorder = 0);
	void PlayParticle3(float time, float scale, int Zorder = 0);
	void PlayParticle4(float time, float scale, int Zorder = 0);
	void PlayParticle5(float time, float scale, int Zorder = 0);
	void PlayParticle6(float time, float scale, int Zorder = 0);
	void PlayParticle7(float time, float scale, int Zorder = 0);
	void PlayParticle8(float time, float scale, int Zorder = 0);
	void PlayParticle9(float time, float scale, int Zorder = 0);

	void addParticle1(Node* node);
	void addParticle2(Node* node);
	void addParticle3(Node* node);
	void addParticle4(Node* node);
	void addParticle5(Node* node);
	void addParticle6(Node* node);
	void addParticle7(Node* node);
	void addParticle8(Node* node);
	void addParticle9(Node* node);

protected:
	void PlaneDeath();
	BasePlane*		m_BasePlane;
};


//普通小怪爆炸:1.原地爆炸；2.移动中爆炸
class NorEnemyPlaneExpl : public BaseExplosion
{
public:
	CREATE_FUNC(NorEnemyPlaneExpl);
	virtual void StartExplosion();

	//原地爆炸
	void ExplosionNoMove();

	//随机移动中爆炸
	void ExplosionRandMove();

	//掉落中爆炸
	void ExplosionFalling();

	void SingleExplRandMove();

	void SingleExplFalling();

	//原地爆炸音效
	void MusicNoMove();


private:

};

//精英怪的爆炸
class SpecialEnemyPlaneExpl : public BaseExplosion
{
public:
	CREATE_FUNC(SpecialEnemyPlaneExpl);
	virtual void StartExplosion();

	//先一连串爆炸
	void ExplosionGroups();

	//再随机点爆炸
	void ExplosionRand();

	//最后爆圈
	void ExplosionCircle();

	//粒子阶段
	void Particle1(float t);

	void MidExpl();

	void HidePlane();

};

//BOSS爆炸
class BossExplosion : public BaseExplosion
{
public:
	CREATE_FUNC(BossExplosion);
	virtual void StartExplosion();

	//1.范围爆炸
	void ExplosionRandDelay(float t);
	void ExplosionRand();

	//2.随机点爆炸
	void ExplosionRandPosDelay(float t);
	void ExplosionRandPos();

	//3.最后一爆
	void LastExplosionDelay(float t);
	void LastExplosion();

	void HideBoss();
};






#endif // PlaneExplosion_h__
