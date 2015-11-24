#include "GameHelp.h"
#include "../GameCommon/GameData.h"

GameHelp* GameHelp::create()
{
	GameHelp* gh = new (std::nothrow) GameHelp;
	if (gh && gh->initWithFile("result/resultBg.png"))
	{
		gh->autorelease();
		return gh;
	}
	else
	{
		CC_SAFE_DELETE(gh);
		return nullptr;
	}
}

void GameHelp::confirmCallback( Ref* sender )
{
	GameData::getInstance()->GameResume();

	removeFromParentAndCleanup(true);
}

void GameHelp::addMenu()
{
	Size s = getContentSize();
	MenuItemImage* item = MenuItemImage::create("result/close_N.png","result/close_P.png",this,menu_selector(GameHelp::confirmCallback));
	item->setPosition(ccp(s.width*0.5,s.height*0.35));

	Menu* menu = Menu::create(item,nullptr);
	menu->setPosition(Point::ZERO);
	addChild(menu);
}
