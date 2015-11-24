#include "CircleStageList.h"


CandidateRole::CandidateRole()
{

}
CandidateRole::~CandidateRole()
{

}
bool CandidateRole::init(const char* name)
{

	mPositionIndex=0;
	mbIsSelected=false;
	mDistance=0;
	mColorB=255;
	mColorG=255;
	mColorR=255;

	mpArmature = Sprite::create(name);
	this->addChild(mpArmature);

	return true;
}

CandidateRole* CandidateRole::create(const char* name)
{
	CandidateRole* pRet = new (std::nothrow) CandidateRole();
	if (pRet && pRet->init(name))
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}
	return pRet;
}

void CandidateRole::setPositionIndex(int index)
{
	mPositionIndex=index;
}

int CandidateRole::getPositionIndex()
{
	return mPositionIndex;

}

CircleStage::CircleStage()
	:mCountActionEndNum(0)
{

}

CircleStage::~CircleStage()
{

}

bool CircleStage::init()
{
	mCircleA=Director::getInstance()->getVisibleSize().width*0.4;
	mCircleB=70;
	mRoleNum=0;
	mpHead=NULL;
	mbOnAction=false;

	return true;

}
void CircleStage::updateColor()
{
	mpCurrentRole=mpSelectRole;
	for (int i=0;i<mRoleNum;i++)
	{
		mpCurrentRole->mColorR=255;
		mpCurrentRole->mColorG=255;
		mpCurrentRole->mColorB=255;
		for (int j=0;j<mpCurrentRole->mDistance;j++)
		{

			mpCurrentRole->mColorR*=0.4;
			mpCurrentRole->mColorG*=0.4;
			mpCurrentRole->mColorB*=0.4;
		}
		mpCurrentRole->mpArmature->setColor(ccc3(mpCurrentRole->mColorR,mpCurrentRole->mColorG,mpCurrentRole->mColorB));
		mpCurrentRole=mpCurrentRole->mpNextRole;
	}
}

void CircleStage::updateDistances()
{
	mpCurrentRole=mpSelectRole;
	int distance=0;
	for (int i=0;i<=mRoleNum/2;i++)
	{
		mpCurrentRole->mDistance=distance;
		++distance;
		mpCurrentRole=mpCurrentRole->mpNextRole;

	}
	mpCurrentRole=mpSelectRole;
	distance=0;
	for (int i=0;i<=mRoleNum/2;i++)
	{
		mpCurrentRole->mDistance=distance;
		++distance;
		mpCurrentRole=mpCurrentRole->mpForeRole;
	}

	mpSelectRole->mDistance=0;
}
void CircleStage::initAppearance()
{
	mpCurrentRole=mpSelectRole;

	for (int i=0;i<mRoleNum;i++)
	{
		double scale=1;

		for (int j=0;j<mpCurrentRole->mDistance;j++)
		{
			scale*=0.8;
			mpCurrentRole->mColorR*=0.6;
			mpCurrentRole->mColorG*=0.6;
			mpCurrentRole->mColorB*=0.6;
		}
		mpCurrentRole->setScale(scale);

		mpCurrentRole->mpArmature->setColor(ccc3(mpCurrentRole->mColorR,mpCurrentRole->mColorG,mpCurrentRole->mColorB));
		mpCurrentRole=mpCurrentRole->mpNextRole;
	}
	mpSelectRole->setScale(1.0f);

}

void CircleStage::updateZorders()
{
	mpSelectRole->setZOrder(mRoleNum);
	mpSelectRole->mDistance=0;
	mpCurrentRole=mpSelectRole;

	for (int i=0;i<=mRoleNum/2;i++)
	{

		mpCurrentRole->setZOrder(mRoleNum-mpCurrentRole->mDistance);
		mpCurrentRole=mpCurrentRole->mpNextRole;
	}
	for (int i=mRoleNum/2+1;i<mRoleNum;i++)
	{

		mpCurrentRole->setZOrder(abs(mRoleNum/2-mpCurrentRole->mDistance));
		mpCurrentRole=mpCurrentRole->mpNextRole;
	}

}




void CircleStage::addRole(CandidateRole *newRole)
{
	if (!mpHead)
	{
		mpHead=newRole;
		this->addChild(newRole);
		mpLast=mpHead;
		mpLast->mpNextRole=mpHead;
		mpHead->mpForeRole=mpHead;
		mpCurrentRole=mpHead;
		mRoleNum++;
		mpCurrentRole->setPositionIndex(mRoleNum);
		mpSelectRole=newRole;
	} 
	else
	{
		mpLast->mpNextRole=newRole;
		newRole->mpNextRole=mpHead;
		newRole->mpForeRole=mpLast;
		mpHead->mpForeRole=newRole;
		this->addChild(newRole);
		mpLast=newRole;
		mRoleNum++;
		newRole->setPositionIndex(mRoleNum);
	}
	int x,y;
	double t;
	while(1)
	{    
		t=360/mRoleNum*(mpCurrentRole->getPositionIndex()-1)-90;
		t=t*3.14/180;//convert t to radian
		x=mCircleA*cos(t);
		y=mCircleB*sin(t);
		mpCurrentRole->setPositionX(x);
		mpCurrentRole->setPositionY(y);
		mpCurrentRole=mpCurrentRole->mpNextRole;

		if (mpCurrentRole==mpHead)
		{
			break;
		}

	}

	updateDistances();
	updateZorders();
	initAppearance();
	updateColor();
}

void CircleStage::callbackSetCurrentRole1( void )
{
	changeToNext(0.01f);

}

void CircleStage::callbackSetCurrentRole2( void )
{
	setCurrentRole( mTargetIndex );
}

void CircleStage::setCurrentRole( int index )
{
	mTargetIndex = index;
	if ( mpCurrentRole->getPositionIndex() == index )
	{
		mpCurrentRole->stopAllActions();
	}
	else
	{
		mpCurrentRole->runAction( Sequence::create(
			CCCallFunc::create(this, callfunc_selector( CircleStage::callbackSetCurrentRole1)),
			DelayTime::create(0.03f),
			CCCallFunc::create(this, callfunc_selector( CircleStage::callbackSetCurrentRole2 )),nullptr) );
	}

}

void CircleStage::changeToNext( float t/* = 0.4f */) //从右向左
{
	if ( mbOnAction )
		return;

	mpSelectRole=mpSelectRole->mpNextRole;
	CCCallFunc* callFuncActionEnd = CCCallFunc::create(this,callfunc_selector(CircleStage::actionEnd));
	CCCallFunc* callUpdateZorder = CCCallFunc::create(this,callfunc_selector(CircleStage::updateZordersCallBack));
	if (mpCurrentRole->mpNextRole)
	{

		mpSelectRole->setZOrder(mRoleNum);
		for (int i=0;i<mRoleNum;i++)
		{

			mpCurrentRole->mNextPositionX=mpCurrentRole->mpForeRole->getPositionX();
			mpCurrentRole->mNextPositionY=mpCurrentRole->mpForeRole->getPositionY();
			mbOnAction=true;
			CCMoveTo *moveToNext=CCMoveTo::create(t,ccp(mpCurrentRole->mNextPositionX,mpCurrentRole->mNextPositionY));
			CCScaleTo *scaleToFore=CCScaleTo::create(t,mpCurrentRole->mpForeRole->getScale());
			CCFiniteTimeAction*  spawnAction = CCSpawn::create(moveToNext,scaleToFore,NULL);

			CCSequence* actions = CCSequence::create(spawnAction,callUpdateZorder,callFuncActionEnd,NULL);
			mpCurrentRole->runAction(actions);

			mpCurrentRole=mpCurrentRole->mpNextRole;

		}
		updateDistances();
		updateColor();

	}
}

void CircleStage::updateZordersCallBack()
{
	updateZorders();
}
void CircleStage::actionEnd()
{
	mbOnAction=false;

	++mCountActionEndNum;
	if ( mCountActionEndNum >= mRoleNum )
	{
		mCountActionEndNum = 0;
		if ( callFuncCircleEnd )
		{
			callFuncCircleEnd( mpCurrentRole->getPositionIndex() );
		}
	}

}

void CircleStage::changeToFore(float t/* = 0.4f */) //从左向右
{
	if ( mbOnAction )
		return;

	mpSelectRole=mpSelectRole->mpForeRole;
	CCCallFunc* callFuncActionEnd = CCCallFunc::create(this,callfunc_selector(CircleStage::actionEnd));

	if (mpCurrentRole->mpForeRole)
	{
		for (int i=0;i<mRoleNum;i++)
		{
			mpCurrentRole->mNextPositionX=mpCurrentRole->mpNextRole->getPositionX();
			mpCurrentRole->mNextPositionY=mpCurrentRole->mpNextRole->getPositionY();
			mbOnAction=true;

			CCMoveTo *moveToFore=CCMoveTo::create(t,ccp(mpCurrentRole->mNextPositionX,mpCurrentRole->mNextPositionY));
			CCScaleTo *scaleToFore=CCScaleTo::create(t,mpCurrentRole->mpNextRole->getScale());
			CCFiniteTimeAction*  spawnAction = CCSpawn::create(moveToFore,scaleToFore,NULL);
			CCSequence* actions = CCSequence::create(spawnAction,callFuncActionEnd,NULL);
			mpCurrentRole->runAction(actions);
			mpCurrentRole=mpCurrentRole->mpNextRole;

		}
		updateDistances();
		updateZorders();
		updateColor();

	}
}

bool CircleStage::isOnAction()
{
	return mbOnAction;
}
