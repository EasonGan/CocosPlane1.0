
/********************************************************************
创建于:		2015/01/22  14:41
文件名: 	E:\Cocos3.3_Project\PlaneGame\Classes\UserInfo\DropGoods.h
作  者:		alex(alex)
修改于：	2015/01/22  14:41

目  的:		凋落物类
*********************************************************************/
#ifndef __DropGoods_H__
#define __DropGoods_H__


#include "cocos2d.h"
#include "cocos-ext.h"
#include "PhysicsUnit.h"
#include "../GameCommon/GameDefine.h"
#include "../GameUtil/DrawSpriteUtil.h"
#include "AudioEngine.h"

USING_NS_CC;
USING_NS_CC_EXT;


/************************************************************************/
/* 
击毁飞机后得到的奖励
*/
/************************************************************************/
#define CREATE_DROP(_ClassName_) \
static _ClassName_* create( const std::string& frameName, float UpSpeed = 200 ) \
{ \
	_ClassName_* s = new (std::nothrow) _ClassName_; \
	if (s && s->initWithSpriteFrameName(frameName)) \
	{ \
		s->autorelease(); \
		s->setUpSpeed(UpSpeed); \
		return s; \
	} \
	else \
	{ \
		CC_SAFE_DELETE(s); \
		return nullptr; \
	} \
} \

#define SUCTION_DIS_MOD_PLACE 400;
#define SUCTION_DIS_MOD_DROP 800;
#define SPEED_MOD_PLACE -1400;
//掉落模式
enum DropMod
{
    Place_Mod,		//摆放模式
    Drop_Mod		//掉落模式
};

typedef struct DropPro
{
	CC_SYNTHESIZE(int,		score,	Score);
	CC_SYNTHESIZE(int,		gold,	Gold);
	CC_SYNTHESIZE(BuffStruct, buff,	Buff);

	DropPro()
	{
		score = 0;
		gold = 0;
		buff = BuffStruct(BuffId::Buff_default,0);
	}
}DropPro;

class DropItem : public Ref
{
public:
    ~DropItem()
    {
        drop_item_resource->release();
    }
    const static unsigned int TYPE_GOLD = 1;
    const static unsigned int TYPE_SHIELD = 2;
    const static unsigned int TYPE_SKILL = 3;
    int drop_item_type;
    int drop_item_num;
    int drop_item_score;
    GameResource* drop_item_resource;
    std::string drop_item_name;
    static DropItem* create(int type,int addNum,int score,GameResource* resource,std::string name);
};

//掉落物
class DropGold : public PhysicsUnit
{
public:
    DropGold()
		:m_DropMod(DropMod::Drop_Mod)
		,m_MagnetDis(150)
		,m_UpSpeed(400.0f)
        ,waitTime(0)
		,m_isFlyToPlane(true)
    {
		//m_MaxChance = 1;
    }
    //CREATE_DROP(DropGold);
//    static DropGold* create(const Vec2& pos, ResTag res = ResTag::Drops_Golds1,  int glod  = 0, int score = 0);

    CREATE_FUNC(DropGold);
    virtual void onEnter();
    virtual void initBody();
    virtual void DoSomethingBeforeDead();
    virtual void PlayMusicWhenDead();			//死亡音效
    void setUpSpeed(float speed);
	void startCheck();
	void startCheckO(Ref* obj);
	virtual void DeathImpactOfDrops();
	virtual void OtherDealWhenContact(const BasePro& rs);
	void ImpactOfBuff();

    virtual void InitSpeed();
    //死亡后对得分、奖励的影响
    virtual void Rewards(){}
    void updateRewards(const int& rewardtype);

    void ActionCallback(Node* node);

    //统计接到的数量
    void StatictisNum();

    //飞向主飞机
    void MoveToMainPl(float dt);
    void setRewardsInfo(int gold = 0, int score = 0);

	void setDropPro(const DropPro& pro){m_DropPro = pro;}
	DropPro& getDropPro(){return m_DropPro;}

	void ListenMainplFuhuo();	//开启监听主飞机复活事件
	virtual void initMagnetDis();

    void playEffectGetUserItem(int userItemType);
    //接到护盾道具
    void playerEffectGetShield();
    //接到必杀道具
    void playerEffectGetSkill();
    void effectCallBack2(Node* node,std::string str);
    void effectCallBack(Node* node);
protected:
	CC_SYNTHESIZE_PASS_BY_REF(DropMod,m_DropMod,DropMod);
	DropPro			m_DropPro;
    float			m_UpSpeed;
    Vec2			m_StartPos;
    bool            isChange;
	CC_SYNTHESIZE(bool,m_isFlyToPlane,IsFlyToPlane);
	CC_SYNTHESIZE(float,m_MagnetDis,MagnetDis);
    float waitTime;

};

class WpDrops : public DropGold
{
public:
	virtual void onEnter();
    virtual void Rewards();
    virtual void delayGive();		//延迟发射接到的武器
    virtual void PlayMusicWhenDead();			//死亡音效
	virtual void initMagnetDis();

    bool couldAddWp();

	//花类型
    void addFlower(int lvl,int hurts);

    //飞镖类型
    void addFeibiao(int lvl, int hurts);

    //火圈类型
    void addFireCircle(int lvl,int hurts);

    //导弹类型
    void addMissile(int lvl,int hurts);

    //猫类型
    void addCat(int lvl, int hurts);

    //冰锥类型
    void addIcePiton(int lvl,int hurts);

    //龙类型
    void addDragon(int lvl,int hurts);

    //激光类型
    void addLaser(int lvl,int hurts);

    //闪电类型
    void addLighting(int lvl,int hurts);
};


//红色武器掉落物
class DropRedWp : public WpDrops
{
public:
    CREATE_DROP(DropRedWp);
    virtual void onEnter();

    //延迟发射武器
    virtual void delayGive();
    virtual void OtherDealWhenContact(const BasePro& pro);
};

//黄色武器掉落物
class DropYellowWp : public WpDrops
{
public:
    CREATE_DROP(DropYellowWp);

    virtual void onEnter();

    virtual void delayGive();
    virtual void OtherDealWhenContact(const BasePro& pro);

};

//蓝色武器掉落物
class DropBlueWp : public WpDrops
{
public:
    CREATE_DROP(DropBlueWp);
    virtual void onEnter();

    virtual void delayGive();
    virtual void OtherDealWhenContact(const BasePro& pro);
};

class ShieldItem : public DropGold
{
public:
    CREATE_FUNC(ShieldItem);
    virtual void DoSomethingBeforeDead();
    virtual void OtherDealWhenContact(const BasePro& pro);
};

class SkillItem : public DropGold
{
public:
    CREATE_FUNC(SkillItem);
    virtual void DoSomethingBeforeDead();
    virtual void OtherDealWhenContact(const BasePro& pro);
};

//掉落物工厂
class DropsFactory : public Ref
{
public:

    ~DropsFactory()
    {
        this->recovery_vector.clear();
    }
    Vector<DropGold*> recovery_vector;
    DropGold* getDropGoldFromRecoveryVector();
    static DropsFactory* create();
    static DropGoodsType getGoodsTypeFromItemId(int itemId);
    void purge();
    bool init();

    Map<int,DropItem*> drop_item_config;
    void addDropItemRandom(const Point& pos, int dropItemId, unsigned int num = 1);
    DropGold* createDropItem(const Point& pos, int dropItemId,DropMod dropMod,bool isChange = false, bool isFlyToPl = true);
    void addDropItemsToLayer(const Point& pos,DropGold* dropGold);

    void AddDrops(const Point& pos, DropGoodsType tp, int nums = 1);

    //金币1
    void DropGoldsType1(const Point& pos);

    //金币2
    void DropGoldsType2(const Point& pos);

    //金币3
    void DropGoldsType3(const Point& pos);
    //超级武器
    void DropWithSuperWp(const Point& pos);

    //红色武器
    void DropWithRedWp(const Point& pos);

    //蓝色武器
    void DropWithBlueWp(const Point& pos);

    //黄色武器
    void DropWithYellowWp(const Point& pos);

    //武器BUFF1
    void DropWithWpBuff1(const Vec2& pos);
    //武器BUFF2
    void DropWithWpBuff2(const Vec2& pos);
    //武器BUFF3
    void DropWithWpBuff3(const Vec2& pos);

    //回复血量100
    void AddHp100(const Vec2& pos);

    //回复血量500
    void AddHp500(const Vec2& pos);

    //磁铁
    void DropMagnet(const Vec2& pos);

    static std::string getResByTag(ResTag tag);

    Vec2 getRandPos(const Vec2& centerPos);

private:
    Vector<SpriteBatchNode*>	m_BatchNodeVec;

};



#endif
