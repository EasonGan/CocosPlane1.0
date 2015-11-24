#include "RollingBg.h"
#include "UIController.h"
#include "../../GameCommon/GameData.h"
#include "../../GameCommon/EffectsManager.h"



bool GameBg::init()
{
	if (!Layer::init())
	{
		return false;
	}

	m_size = Director::getInstance()->getWinSize();
	m_curSpeed = GameData::getInstance()->getUpdateSpeed();

    m_BgTexture = Director::getInstance()->getTextureCache()->addImage("Bg/bg1.jpg");
	m_curBgId = 1;
	return true;
}

void GameBg::onEnter()
{
	Layer::onEnter();
	addBgAndRolling();
	scheduleUpdate();
	schedule(schedule_selector(GameBg::refreshSpeed));
	NotificationCenter::getInstance()->addObserver(this,callfuncO_selector(GameBg::Shake),Msg_Shake,nullptr);
}

void GameBg::onExit()
{
	Layer::onExit();
	NotificationCenter::getInstance()->removeAllObservers(this);
}

float GameBg::SpeedRate()
{
	return 0.0f;
}

bool GameBg::isFlip()
{
	return false;
}

void GameBg::addBgAndRolling()
{
	m_size = Director::sharedDirector()->getWinSize();
	m_Bg1 = Sprite::createWithTexture(m_BgTexture);
	m_Bg1->setAnchorPoint(ccp(0.5,1));
	m_Bg1->setPosition(m_size.width*0.5, m_size.height*0.5);
	addChild(m_Bg1);

	m_Bg2 = Sprite::createWithTexture(m_BgTexture);
	m_Bg2->setPosition(m_size.width*0.5, m_Bg1->getContentSize().height*1.5);
	m_Bg2->setFlipY(isFlip());
	m_Bg2->setAnchorPoint(ccp(0.5,1));
	addChild(m_Bg2);

	//SwitchBg(NewDataMgr::getInstance()->getCurLevel());

	/*auto partc = EffectsManager::addParticle("particla/wu/wu.plist",this,Vec2(m_size.width * 0.5, m_size.height * 1.1),false);
	if (partc)
	{
		partc->setPositionType(ParticleSystem::PositionType::GROUPED);
	}*/
}

void GameBg::update( float delta )
{
	bool isPause = GameData::getInstance()->isGamePause();
	if (isPause)
	{
		return;
	}
	auto bg1Pos = m_Bg1->getPosition();
	auto bg2Pos = m_Bg2->getPosition();

	float offset = m_curSpeed + SpeedRate();

	bg1Pos.y -= offset;
	bg2Pos.y -= offset;

	m_Bg1->setPosition(bg1Pos.x,bg1Pos.y);
	m_Bg2->setPosition(bg2Pos.x,bg2Pos.y);

	if (bg1Pos.y <= 0.0f)
	{
		m_Bg1->setPosition(m_size.width*0.5, bg2Pos.y + m_Bg1->getContentSize().height);
	}
	if (bg2Pos.y <= 0.0f)
	{
		m_Bg2->setFlipY(isFlip());
		m_Bg2->setPosition(m_size.width*0.5, bg1Pos.y + m_Bg2->getContentSize().height);
	}

}

void GameBg::Shake( Ref* obj )
{
	UIController::getInstance()->getEffectsLayer()->ScreenShake(this);
}

void GameBg::refreshSpeed( float dt )
{
	SpeedUpEase();
}

void GameBg::SwitchBg( int curLevel )
{
	m_curBgId = curLevel;
    std::string fileName = "";
    int value = floor((curLevel - 1) / 4) + 1;
	switch (value)
	{
	case 1:
		{
            fileName = "Bg/bg1.jpg";
		}
		break;
	case 2:
		{
            fileName = "Bg/bg2.jpg";
		}
		break;
	case 3:
		{
            fileName = "Bg/bg3.jpg";
		}
		break;
    case 4:
        {
            fileName = "Bg/bg4.jpg";
        }
            break;

        default:
        {
            fileName = "Bg/bg3.jpg";
        }
		break;
	}

    m_BgTexture = Director::getInstance()->getTextureCache()->addImage(fileName);
    this->m_Bg1->setTexture(m_BgTexture);
    this->m_Bg2->setTexture(m_BgTexture);
}


//---------------------

float RollingBg::SpeedRate()
{
	return 0.0f;
}

bool RollingBg::isFlip()
{
	return false;
}

void RollingBg::SpeedUpEase()
{
	float newSpeed = GameData::getInstance()->getUpdateSpeed();
	if(this->m_curSpeed != newSpeed) {
		this->m_curSpeed = newSpeed;
	}
	/*if (m_curSpeed < newSpeed)
	{
	m_curSpeed += 0.1f;
	}
	else
	{
	m_curSpeed -= 0.3f;
	}*/
}

void RollingBg::onEnter()
{
	GameBg::onEnter();
	//auto cloudLayer = RollingClouds::create();
	//addChild(cloudLayer);
}

