/********************************************************************
	created:	2015/08/28
	created:	28:8:2015   10:40
	filename: 	E:\cocos3.4_projects\PlaneGame_Danji\Classes\GamePlaying\BasePlane\BossHpBar.h
	file path:	E:\cocos3.4_projects\PlaneGame_Danji\Classes\GamePlaying\BasePlane
	file base:	BossHpBar
	file ext:	h
	author:		alex
	
	purpose:	BOSS的血条。在BOSS出现的时候，血条始终在屏幕上方显示
*********************************************************************/
#ifndef BossHpBar_h__
#define BossHpBar_h__
#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class BaseBoss;
//BOSS的血条
class BossHpBar : public Sprite
{
public:
	BossHpBar()
	{
		m_HpNums = 1;
		m_MaxHp = 1000;		//给1000初始值能很快判断是否忘了设置
		m_CurHp = m_MaxHp;
		m_EachHp = m_MaxHp;
	}
	static BossHpBar* create(BaseBoss* boss);
	virtual void onEnter();
	virtual void onExit();

	//血条管数
	void setHpNumbers(int hpNums);
	int getMaxHpNum();
	int getCurHpNum();

	//boss头像
	void setHeadByName(const std::string& name);

	//BOSS昵称
	void setBossName(const std::string& name);

	//血量值
	void setMaxHp(int maxHp);

	//血量更新
	void UpdateHp(int addHp);

private:
	void initBarTexture();
	void initBar();
	void SwitchBar();
	void Remove(Ref* obj);

private:
	int			m_HpNums;
	int			m_MaxHp;
	int			m_CurHp;
	int			m_EachHp;
	int			m_LeftNum;
	std::string	m_barTextureName1;
	std::string	m_barTextureName2;
	ProgressTimer* m_HpBar;
	Sprite*		m_BarBg;
	Sprite*		m_BarHp;
	BaseBoss*	m_boss;
};















#endif // BossHpBar_h__
