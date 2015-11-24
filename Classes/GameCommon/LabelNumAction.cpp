#include "LabelNumAction.h"
#include "../cocos2d/cocos/ui/UITextAtlas.h"


LabelNumAction* LabelNumAction::create( float d, int targetnum )
{
	LabelNumAction* action = new (std::nothrow) LabelNumAction();

	action->initWithDuration(d);
	action->autorelease();

	action->initWithTimeTarget(d,targetnum);


	return action;
}

void LabelNumAction::initWithTimeTarget( float t,int targetnum )
{
	_runTime = t;
	_targetNum = targetnum;

	_countT = 0;
	_curTime = 0;
}

void LabelNumAction::update( float time )
{
	int intervalTime = 2;

	static int nT = 0;
	nT++;
	if ( nT < intervalTime)
		return;

	nT = 0;

	_countT = _countT + _targetNum/intervalTime/_runTime;

	if ( time >= 1.0f || _countT >= _targetNum )
	{
		_countT = _targetNum;
	}

	char string[10] = {0};
	sprintf(string, "%d", (int)(_countT));

	auto tempNode = (TextAtlas*)_target;
	tempNode->setString( string );
}


