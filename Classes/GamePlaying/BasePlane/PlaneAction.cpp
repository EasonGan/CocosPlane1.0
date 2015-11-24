#include "PlaneAction.h"
#include "BasePlane.h"

//轨迹点数组(屏幕宽高的百分比)
float LinePos[][2]=
{
	{0,0},
	{0.15, 0.05},
	{0.28, 0.10},
	{0.45, 0.15},
	{0.67, 0.22},
};

EnterAction* EnterAction::create()
{
	EnterAction* pa = new (std::nothrow) EnterAction;
	if (pa && pa->init())
	{
		pa->retain();
		pa->autorelease();
		return pa;
	}
	else
	{
		CC_SAFE_DELETE(pa);
		return nullptr;
	}
}

void EnterAction::purge()
{
	this->release();
}

bool EnterAction::init()
{
	do 
	{
		m_winSize = Director::getInstance()->getWinSize();

	} while (0);

	return true;
}

ActionInterval* EnterAction::AddEnemyActionByIdx(int ActionId,BasePlane* pl )
{
	ActionInterval* action = nullptr;
	switch (ActionId)
	{
	case 1:
		action = CreateMoveToUpEdge(pl);
		break;
	case 2:
		action = CreateMoveToScreen(pl);
		break;
	case 3:
		action = CreateHorizonMoveToScreen(pl);
		break;
	case 4:
		action = CreateMoveToFasteningPos(pl);
		break;
	case 5:
		action = CreateCardinalSplineL2U(pl);
		break;
	case 6:
		action = CreateCardinalSplineL2B(pl);
		break;
	case 7:
		action = CreateCardinalSplineR2U(pl);
		break;
	case 8:
		action = CreateCardinalSplineR2B(pl);
		break;

	case 9:
		action = CreateBezierFromUpToBackAtRight(pl);
		break;
	case 10:
		action = CreateBezierFromBottomToBackAtRight(pl);
		break;
	case 11:
		action = CreateBezierFromUpToBackAtLeft(pl);
		break;
	case 12:
		action = CreateBezierFromBottomToBackAtLeft(pl);
		break;
	case 13:
		action = CreateBezierRight2Left(pl);
		break;
	case 14:
		action = CreateBezierLeft2Right(pl);
		break;
	case 15:
		action = CreateBezierFromLP2RP(pl);
		break;
	case 16:
		action = CreateBezierFromRP2LP(pl);
		break;
	case 17:
		action = CreateBezierWaveAtRight(pl);
		break;
	case 18:
		action = CreateBezierWaveAtLeft(pl);
		break;
	}
	return action;
}

ActionInterval* EnterAction::CreateCardinalSplineL2U(BasePlane* pl)
{
	//左上轨迹
	/*auto array = PointArray::create(20);
	array->addControlPoint(Vec2(0, 0));
	array->addControlPoint(Vec2(-LinePos[1][0] * s.width, LinePos[1][1] * s.height));
	array->addControlPoint(Vec2(-LinePos[2][0] * s.width, LinePos[2][1] * s.height));
	array->addControlPoint(Vec2(-LinePos[3][0] * s.width, LinePos[3][1] * s.height));
	array->addControlPoint(Vec2(-LinePos[4][0] * s.width, LinePos[4][1] * s.height));

	ActionInterval* actionLine = CardinalSplineBy::create(1.0, array, 0);*/

	ActionInterval* moveby = MoveBy::create(4.5f,Vec2(-m_winSize.width * 2.0,m_winSize.height*0.6));
	ActionInterval* ease = EaseSineInOut::create(moveby);

	return ease;
}

ActionInterval* EnterAction::CreateCardinalSplineL2B(BasePlane* pl)
{
	ActionInterval* moveby = MoveBy::create(4.5,Vec2(-m_winSize.width*2.0,-m_winSize.height*0.4));
	ActionInterval* ease = EaseSineInOut::create(moveby);

	return ease;
}

ActionInterval* EnterAction::CreateCardinalSplineR2U(BasePlane* pl)
{
	////右上轨迹
	//auto array = PointArray::create(20);
	//array->addControlPoint(Vec2(0, 0));
	//array->addControlPoint(Vec2(LinePos[1][0] * s.width, LinePos[1][1] * s.height));
	//array->addControlPoint(Vec2(LinePos[2][0] * s.width, LinePos[2][1] * s.height));
	//array->addControlPoint(Vec2(LinePos[3][0] * s.width, LinePos[3][1] * s.height));
	//array->addControlPoint(Vec2(LinePos[4][0] * s.width, LinePos[4][1] * s.height));

	//ActionInterval* actionLine = CardinalSplineBy::create(2, array, 0);

	ActionInterval* moveby = MoveBy::create(4.5,Vec2(m_winSize.width*2.0,m_winSize.height*0.6));
	ActionInterval* ease = EaseSineInOut::create(moveby);

	return ease;
}

ActionInterval* EnterAction::CreateCardinalSplineR2B(BasePlane* pl)
{
	ActionInterval* moveby = MoveBy::create(4.5,Vec2(m_winSize.width*2.0,-m_winSize.height*0.4));
	ActionInterval* ease = EaseSineInOut::create(moveby);
	return ease;
}

ActionInterval* EnterAction::CreateMoveToUpEdge(BasePlane* pl)
{
	Vec2 pos = pl->convertToWorldSpaceAR(Vec2::ZERO);

	ActionInterval* moveDown = MoveBy::create(1.0f,Vec2(0, -(pos.y - m_winSize.height )* 2 - 50));
	ActionInterval* ease = EaseSineInOut::create(moveDown);
	return ease;
}

ActionInterval* EnterAction::CreateMoveToScreen(BasePlane* pl)
{
	Vec2 mypos = pl->convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 endPos = Vec2(mypos.x,m_winSize.height * 0.1f * cocos2d::random(7,9));

	ActionInterval* moveDown = MoveTo::create(1.0f,endPos);
	ActionInterval* ease = EaseSineInOut::create(moveDown);
	return ease;
}

ActionInterval* EnterAction::CreateHorizonMoveToScreen( BasePlane* pl )
{
	Vec2 mypos = pl->convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 endPos = Vec2(m_winSize.width * 0.2f * cocos2d::random(1,4),mypos.y);

	ActionInterval* moveDown = MoveTo::create(1.0f,endPos);
	ActionInterval* ease = EaseSineInOut::create(moveDown);
	return ease;
}

ActionInterval* EnterAction::CreateMoveToFasteningPos( BasePlane* pl )
{
	Vec2 mypos = pl->convertToWorldSpaceAR(Vec2::ZERO);
	Vec2 endPos = Vec2(m_winSize.width * 0.2f * cocos2d::random(1,4),mypos.y);

	if (mypos.x < 0)
	{
		endPos = Vec2(m_winSize.width * 0.25f,mypos.y);
	}
	else
	{
		endPos = Vec2(m_winSize.width * 0.75f,mypos.y);
	}

	ActionInterval* moveDown = MoveTo::create(1.0f,endPos);
	ActionInterval* ease = EaseSineInOut::create(moveDown);
	return ease;
}

ActionInterval* EnterAction::CreateBezierFromUpToBackAtRight( BasePlane* pl )
{
	pl->setPosition(m_winSize.width + 100, m_winSize.height*0.95);

	ccBezierConfig config;
	config.controlPoint_1 = Vec2(-m_winSize.width, -m_winSize.height * 0.2);
	config.controlPoint_2 = Vec2(-m_winSize.width * 0.6, -m_winSize.height * 0.6);
	config.endPosition = Vec2(0, -m_winSize.height * 0.4);

	auto mv = BezierBy::create(5.0f, config);
	return mv;
}

ActionInterval* EnterAction::CreateBezierFromBottomToBackAtRight( BasePlane* pl )
{
	pl->setPosition(m_winSize.width + 100, m_winSize.height*0.4);

	ccBezierConfig config;
	config.controlPoint_1 = Vec2(-m_winSize.width, m_winSize.height * 0.2);
	config.controlPoint_2 = Vec2(-m_winSize.width * 0.6, m_winSize.height * 0.6);
	config.endPosition = Vec2(0, m_winSize.height * 0.4);

	auto mv = BezierBy::create(5.0f, config);
	return mv;
}

ActionInterval* EnterAction::CreateBezierFromUpToBackAtLeft( BasePlane* pl )
{
	pl->setPosition( -100, m_winSize.height*0.9);

	ccBezierConfig config;
	config.controlPoint_1 = Vec2(m_winSize.width, -m_winSize.height * 0.2);
	config.controlPoint_2 = Vec2(m_winSize.width * 0.6, -m_winSize.height * 0.6);
	config.endPosition = Vec2(0, -m_winSize.height * 0.4);

	auto mv = BezierBy::create(5.0f, config);
	return mv;
}

ActionInterval* EnterAction::CreateBezierFromBottomToBackAtLeft( BasePlane* pl )
{
	pl->setPosition(- 100, m_winSize.height*0.4);

	ccBezierConfig config;
	config.controlPoint_1 = Vec2(m_winSize.width, m_winSize.height * 0.2);
	config.controlPoint_2 = Vec2(m_winSize.width * 0.6, m_winSize.height * 0.6);
	config.endPosition = Vec2(0, m_winSize.height * 0.4);

	auto mv = BezierBy::create(5.0f, config);
	return mv;
}

ActionInterval* EnterAction::CreateBezierRight2Left( BasePlane* pl )
{
	auto pos = Vec2(m_winSize.width + 100, m_winSize.height*0.8);
	pl->setPosition(pos);

	ccBezierConfig config;
	config.controlPoint_1 = Vec2(-m_winSize.width * 0.5 - 150, -m_winSize.height * 0.5);
	config.controlPoint_2 = Vec2(-m_winSize.width - 200, 0);
	config.endPosition = Vec2(-m_winSize.width - 500, 0);

	auto mv = BezierBy::create(5.0f, config);
	return mv;
}

ActionInterval* EnterAction::CreateBezierLeft2Right( BasePlane* pl )
{
	auto pos = Vec2(-100, m_winSize.height*0.8);
	pl->setPosition(pos);

	ccBezierConfig config;
	config.controlPoint_1 = Vec2(m_winSize.width * 0.5 + 150, -m_winSize.height * 0.5);
	config.controlPoint_2 = Vec2(m_winSize.width + 200, 0);
	config.endPosition = Vec2(m_winSize.width + 500, 0);

	auto mv = BezierBy::create(5.0f, config);
	return mv;
}

ActionInterval* EnterAction::CreateBezierFromLP2RP( BasePlane* pl )
{
	auto pos = Vec2(100, m_winSize.height + 100);
	pl->setPosition(pos);

	ccBezierConfig config;
	config.controlPoint_1 = Vec2(m_winSize.width * 0.5, -m_winSize.height * 1.3);
	config.controlPoint_2 = Vec2(m_winSize.width - 200, 0);
	config.endPosition = Vec2(m_winSize.width - 200, 500);

	auto mv = BezierBy::create(6.0f, config);
	return mv;
}

ActionInterval* EnterAction::CreateBezierFromRP2LP( BasePlane* pl )
{
	auto pos = Vec2(m_winSize.width - 100, m_winSize.height + 100);
	pl->setPosition(pos);

	ccBezierConfig config;
	config.controlPoint_1 = Vec2(-m_winSize.width * 0.5, -m_winSize.height * 1.3);
	config.controlPoint_2 = Vec2(-m_winSize.width + 200, 0);
	config.endPosition = Vec2(-m_winSize.width + 200, 500);

	auto mv = BezierBy::create(6.0f, config);
	return mv;
}

ActionInterval* EnterAction::CreateBezierWaveAtRight( BasePlane* pl )
{
	auto pos = Vec2(m_winSize.width + 100, m_winSize.height * 0.5);
	pl->setPosition(pos);

	auto mv = MoveBy::create(5.0f, Vec2(- pos.x - 100, 0));
	auto jump = JumpBy::create(5.0f, Vec2(0,-50), 100, 3);
	auto spw = Spawn::create(mv, jump, nullptr);

	return spw;
}

ActionInterval* EnterAction::CreateBezierWaveAtLeft( BasePlane* pl )
{
	auto pos = Vec2(-100, m_winSize.height * 0.5);
	pl->setPosition(pos);

	auto mv = MoveBy::create(5.0f, Vec2(m_winSize.width + 200, 0));
	auto jump = JumpBy::create(5.0f, Vec2(0,-50), 100, 3);
	auto spw = Spawn::create(mv, jump, nullptr);

	return spw;

}
