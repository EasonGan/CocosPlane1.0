#include "BossHpBar.h"
#include "../../GameCommon/GameDefine.h"
#include "BasePlane.h"

BossHpBar* BossHpBar::create(BaseBoss* boss)
{
	BossHpBar* bar = new (std::nothrow) BossHpBar;
	if (bar && bar->initWithFile("Image/BossHpBar/Bg.png"))
	{
		bar->m_boss = boss;
		bar->autorelease();
		bar->initBarTexture();
		return bar;
	}
	CC_SAFE_DELETE(bar);
	return nullptr;
}

void BossHpBar::onEnter()
{
	Sprite::onEnter();
	initBar();
	SwitchBar();

	//监听BOSS死亡
	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(BossHpBar::Remove),Msg_BossDead, nullptr);

}

void BossHpBar::onExit()
{
	Sprite::onExit();
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void BossHpBar::Remove( Ref* obj )
{
	this->removeFromParentAndCleanup(true);
}

void BossHpBar::setHpNumbers( int hpNums )
{
	m_HpNums = hpNums;
	m_LeftNum = hpNums;
	m_EachHp = m_MaxHp/m_HpNums;
}

void BossHpBar::setHeadByName( const std::string& name )
{

}

void BossHpBar::setBossName( const std::string& name )
{

}

void BossHpBar::setMaxHp( int maxHp )
{
	m_MaxHp = maxHp;
	m_CurHp = maxHp;
	m_EachHp = m_MaxHp/m_HpNums;
}

void BossHpBar::UpdateHp( int addHp )
{
	m_CurHp += addHp;

	int hp = m_CurHp % m_EachHp;

	//求百分比
	float p = hp * 1.0 / m_EachHp;

	m_HpBar->setPercentage(p * 100);

	SwitchBar();
}

void BossHpBar::initBarTexture()
{
	m_barTextureName1 = "Image/BossHpBar/bar1.png";
	m_barTextureName2 = "Image/BossHpBar/bar2.png";
	TextureCache::getInstance()->addImage(m_barTextureName1);
	TextureCache::getInstance()->addImage(m_barTextureName2);
}

void BossHpBar::initBar()
{
	auto s = getContentSize();
	if (m_HpNums > 1)
	{
		m_BarHp = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey(m_barTextureName1));
	}
	else
	{
		m_BarHp = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey(m_barTextureName2));
	}
	
	m_HpBar = ProgressTimer::create(m_BarHp);
	m_HpBar->setType(kCCProgressTimerTypeBar);  
	m_HpBar->setBarChangeRate(Vec2(1,0));
	m_HpBar->setPercentage(100.0f);
	m_HpBar->setPosition(ccp(s.width*0.5 + 6,s.height*0.4));
	m_HpBar->setMidpoint(Vec2(0,0.5));
	addChild(m_HpBar, 2);

	if (m_HpNums > 1)
	{
		m_BarBg = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey(m_barTextureName2));
	}
	else
	{
		m_BarBg = Sprite::createWithTexture(TextureCache::getInstance()->getTextureForKey(m_barTextureName1));
	}
	m_BarBg->setPosition(ccp(s.width*0.5 + 6,s.height*0.4));
	addChild(m_BarBg, 1);

	//头像
	auto head = Sprite::create("Image/BossHpBar/head.png");
	head->setPosition(8,s.height*0.5 + 11);
	addChild(head, 3);
}

void BossHpBar::SwitchBar()
{
	int num = m_CurHp / m_EachHp;
	if (num == m_LeftNum)
	{
		return;
	}
	m_LeftNum = num;
	if (m_LeftNum >= 1)
	{
		//大于一管血
		m_BarBg->setVisible(true);
		m_BarBg->setTexture(m_barTextureName2);
		m_BarHp->setTexture(m_barTextureName1);
	}
	else
	{
		if (m_HpNums > 1)
		{
			//当血管大于1的boss减少至最后一管时，触发暴怒
			m_boss->Angry();
			//此时所有敌机出现都暂停
			EnemyPlaneController::getInstance()->pauseSendGiftPlane();
			NotificationCenter::getInstance()->postNotification(Msg_BossAngry,nullptr);
		}
	
		m_BarBg->setVisible(false);
		m_BarHp->setTexture(m_barTextureName2);
	}
}

int BossHpBar::getMaxHpNum()
{
	return m_HpNums;
}

int BossHpBar::getCurHpNum()
{
	return m_LeftNum;
}

