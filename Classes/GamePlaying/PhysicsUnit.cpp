#include "PhysicsUnit.h"
#include "DropGoods.h"
#include "Layer/UIController.h"
#include "../GameCommon/GameData.h"
#include "../GameCommon/EffectsManager.h"
#include "ContactManager.h"


using namespace experimental;

//#define UP_LINE			1.08
//#define BOTTOM_LINE		-0.08
//#define LEFT_LINE		-0.12
//#define RIGHT_LINE		1.12

#define UP_LINE			1.5
#define BOTTOM_LINE		0.0
#define LEFT_LINE		-2.0
#define RIGHT_LINE		2.0

const GLchar* colorRotationShaderBody();
void xRotateMat(float mat[3][3], float rs, float rc);
void yRotateMat(float mat[3][3], float rs, float rc);
void zRotateMat(float mat[3][3], float rs, float rc);
void matrixMult(float a[3][3], float b[3][3], float c[3][3]);
void hueMatrix(GLfloat mat[3][3], float angle);
void premultiplyAlpha(GLfloat mat[3][3], float alpha);

PhysicsUnit::PhysicsUnit()
{
	m_WinSize = Director::getInstance()->getWinSize();
	m_DeadType = DeadType::Dead_default;
	m_CurChance = 0;
	m_MaxChance = -1;
	m_delayRemoveTime = 0.01;
	m_isDead = false;
	m_isRemoving = false;
}

PhysicsUnit* PhysicsUnit::createWithFrameName( const std::string name )
{
	PhysicsUnit* pu = new (std::nothrow) PhysicsUnit;
	if (pu && pu->initWithSpriteFrameName(name))
	{
		pu->autorelease();
		pu->initBody();
		return pu;
	}
	CC_SAFE_DELETE(pu);
	return nullptr;
}

PhysicsUnit* PhysicsUnit::createWithFile( const std::string file )
{
	PhysicsUnit* pu = new (std::nothrow) PhysicsUnit;
	if (pu && pu->initWithFile(file))
	{
		pu->autorelease();
		pu->initBody();
		return pu;
	}
	CC_SAFE_DELETE(pu);
	return nullptr;
}

PhysicsUnit* PhysicsUnit::create()
{
	PhysicsUnit* pu = new (std::nothrow) PhysicsUnit;
	if (pu && pu->init())
	{
		pu->autorelease();
		pu->initBody();
		return pu;
	}
	CC_SAFE_DELETE(pu);
	return nullptr;
}

void PhysicsUnit::onEnter()
{
	Sprite::onEnter();
	autoCheckChance();
	//PhysicsUnitManager::getInstance()->AddUnit(this);

	FuncAfterEnter();
}

void PhysicsUnit::onExit()
{
	Sprite::onExit();
	//PhysicsUnitManager::getInstance()->DelUnit(this);
	NotificationCenter::getInstance()->removeAllObservers(this);
}

void PhysicsUnit::RemoveMyself()
{
	if (!this->m_isDead)
	{
		return;
	}
	if (m_isRemoving)
	{
		return;
	}
	if (m_delayRemoveTime > 0)
	{
		auto delay = DelayTime::create(m_delayRemoveTime);
		auto rm = CallFunc::create(this, callfunc_selector(PhysicsUnit::RemoveMyself));
		auto seq = Sequence::create(delay, rm, nullptr);
		auto node = Node::create();
		node->setTag(RemoveTag);
		addChild(node);
		node->runAction(seq);

		m_delayRemoveTime = -1;
		return;
	}
	m_isRemoving = true;
	DoSomethingBeforeDead();
	this->removeFromParentAndCleanup(true);
}

void PhysicsUnit::setMaxHp(int max )
{
	m_BasePro.setMaxHp(max);
	m_BasePro.setCurHp(max);
}

void PhysicsUnit::ImpactOfHp(const BasePro& pro)
{
	if (m_isDead)
	{
		return;
	}
	if (isScreenEdge(pro))
	{
		m_CurChance++;
		if (m_CurChance >= m_MaxChance)
		{
			if (!m_BasePro.getIsIgnoreScreenEdge())
			{
				setDeadType(DeadType::Dead_Nature);
				m_isDead = true;
				return;
			}
		}
	}
	else
	{
		if (m_BasePro.getIsInvincible())
		{
			return;
		}
		int hurt = 0;
		if (pro.getHurts() > 0)
		{
			hurt = pro.getHurts();
		}
		else
		{
			hurt = pro.getHurts() + m_BasePro.getDefense();
		}
		m_BasePro.setCurHp(m_BasePro.getCurHp() + hurt);
		FuncWhenHpChanged();
		if (m_BasePro.getCurHp() <= 0)
		{
			FuncHpEmpty(pro);
			DeathImpactOfDropReward();
			DeathImpactOfDrops();
			PlayMusicWhenDead();			//
			AnimationWhenDead();			//
			setDeadType(DeadType::Dead_Kill);
			m_isDead = true;
		}
	}
}

float PhysicsUnit::getHpPercent()
{
	return m_BasePro.getCurHp() * 1.0f / m_BasePro.getMaxHp();
}

void PhysicsUnit::setInvincible(int invincible )
{
	m_BasePro.setIsInvincible( invincible );
}

void PhysicsUnit::ChangeProperty( const BasePro& pro)
{
	if (m_BasePro.getIsInvincibleAbsolutely())
	{
		//绝对无敌
		return;
	}
	if (pro.getMyId() == UnitId::eScreenEdgeBig)
	{
		//死亡线,立马删除,不触发任何死亡效果
		if (m_BasePro.getMyId() == UnitId::eMainPlane)
		{
			GameData::getInstance()->setMainPlane(nullptr);
		}
		RemoveMyself();
		return;
	}
	if (pro.getMyId() == UnitId::eScreenEdgeSmall)
	{
		//如果是边缘线，那么延迟删除
		if (m_delayRemoveTime < 0)
		{
			m_delayRemoveTime = 0.5;
		}
	}

	PlayMusicWhenContact();

	this->OtherDealWhenContact(pro);
	this->ImpactOfHp(pro);		//һ��Ҫ���
	this->RemoveMyself();
}

std::vector<DropReward> PhysicsUnit::getDropVec(const std::vector<DropReward>& vec)
{
	float curblood = getHpPercent();
	//log("----- cur curHP = %f ", curblood);
	std::vector<DropReward> drVec = vec;
	std::vector<DropReward>::iterator it = drVec.begin();

	int rewardsNums = 0;
	for (int i = 0; i < drVec.size(); i++)
	{
		DropReward dr = drVec.at(i);
//		if (dr.HpPercent >= curblood)
//		{
			rewardsNums++;
//		}
	}

	for (int i = 0; i < rewardsNums; i++)
	{
		DropReward dr = (*it);
		DropGoodsType tp = dr.DropTp;
		int num = dr.nums;

		Point DropPos = convertToWorldSpaceAR(Point::ZERO);
		if(!UserDefault::getInstance()->getBoolForKey("noGui") && int(tp) <= 3 && EnemyPlaneController::getInstance()->isFirstDrop && EnemyPlaneController::getInstance()->isGuideLevel()) {
			EnemyPlaneController::getInstance()->startGuide();
		}
		GameData::getInstance()->getDropsFactory()->AddDrops(DropPos,tp, num);

		it = drVec.erase(it);
	}
	return drVec;
}

void PhysicsUnit::setUnitId( const UnitId& idx )
{
	m_BasePro.setMyId(idx);
}

void PhysicsUnit::setBelongTo( const UnitId& idx )
{
	m_BasePro.setBelongTo(idx);
}

void PhysicsUnit::setCantContact()
{
	_physicsBody->setCollisionBitmask(0);
	_physicsBody->setContactTestBitmask(0);
}

void PhysicsUnit::initBody()
{
	auto body = PhysicsBody::createBox(getContentSize());
	setPhysicsBody(body);
}

void PhysicsUnit::DeathForce()
{
	if (m_DeadType == DeadType::Dead_default)
	{
		setDeadType(DeadType::Dead_Force);
	}
	m_isDead = true;
	RemoveMyself();
}

void PhysicsUnit::FuncHpEmpty(const BasePro& pro)
{
}

void PhysicsUnit::setInvincibleAtEdge(int b)
{
	m_BasePro.setIsIgnoreScreenEdge(b);
}

bool PhysicsUnit::isScreenEdge( const BasePro& pro )
{
	if (pro.getMyId() == UnitId::eScreenEdgeSmall)
	{
		return true;
	}
	return false;
}

void PhysicsUnit::autoCheckChance()
{
	if (m_MaxChance > 0)
	{
		return;
	}
	auto pos = convertToWorldSpaceAR(Vec2::ZERO);
	if (pos.x < m_WinSize.width + 50 && pos.x > -50
		&& pos.y > -50 && pos.y < m_WinSize.height + 50)
	{
		m_MaxChance = 1;
	}
	else
	{
		m_MaxChance = 2;
	}
}

void PhysicsUnit::setInvincibleAbsolutely( int b /*= 1*/ )
{
	m_BasePro.setIsInvincibleAbsolutely(b);
}


//*****************����ɫ��*********************/

//bool PhysicsUnit::initWithTexture(cocos2d::Texture2D *texture, const cocos2d::Rect &rect, bool rotated)
//{
//	bool ret = Sprite::initWithTexture(texture, rect, rotated);
//	if(ret)
//	{
//		setupDefaultSettings();
//		initShader();
//	}
//	return ret;
//}
//
//bool PhysicsUnit::initWithTexture(cocos2d::Texture2D *texture)
//{
//	CCASSERT(texture != nullptr, "Invalid texture for sprite");
//
//	cocos2d::Rect rect = cocos2d::Rect::ZERO;
//	rect.size = texture->getContentSize();
//
//	return initWithTexture(texture, rect);
//}
//
//bool PhysicsUnit::initWithTexture(cocos2d::Texture2D *texture, const cocos2d::Rect& rect)
//{
//	return initWithTexture(texture, rect, false);
//}
//
//bool PhysicsUnit::initWithSpriteFrame(cocos2d::SpriteFrame *spriteFrame)
//{
//	CCASSERT(spriteFrame != nullptr, "");
//
//	bool bRet = initWithTexture(spriteFrame->getTexture(), spriteFrame->getRect());
//	setSpriteFrame(spriteFrame);
//
//	return bRet;
//}
//
//void PhysicsUnit::setupDefaultSettings()
//{
//	_hue = 0.0f;
//}
//
//void PhysicsUnit::initShader()
//{
//	auto glprogram = cocos2d::GLProgramCache::getInstance()->getGLProgram("hue_program");
//	if(!glprogram)
//	{
//		glprogram = cocos2d::GLProgram::createWithByteArrays(cocos2d::ccPositionTextureColor_noMVP_vert, shaderBody());
//		cocos2d::GLProgramCache::getInstance()->addGLProgram(glprogram, "hue_program");
//	}
//	auto glprogramstate = cocos2d::GLProgramState::create(glprogram);
//	setGLProgramState(glprogramstate);
//	updateColor();
//}
//
//const GLchar* PhysicsUnit::shaderBody()
//{
//	return colorRotationShaderBody();
//}
//
//void PhysicsUnit::updateColor()
//{
//	Sprite::updateColor();
//	updateColorMatrix();
//	updateAlpha();
//}
//
//void PhysicsUnit::hueUniformCallback(cocos2d::GLProgram *p, cocos2d::Uniform *u)
//{
//	glUniformMatrix3fv(u->location, 1, GL_FALSE, (GLfloat*)&_mat);
//}
//
//void PhysicsUnit::updateColorMatrix()
//{
//	hueMatrix(_mat, _hue);
//	premultiplyAlpha(_mat, getAlpha());
//
//	getGLProgramState()->setUniformCallback("u_hue", CC_CALLBACK_2(PhysicsUnit::hueUniformCallback, this));
//}
//
//void PhysicsUnit::updateAlpha()
//{
//	getGLProgramState()->setUniformFloat("u_alpha", getAlpha());
//}
//
//GLfloat PhysicsUnit::getAlpha()
//{
//	return _displayedOpacity/255.0f;
//}
//
//float PhysicsUnit::getHue()
//{
//	return _hue;
//}
//
//void PhysicsUnit::setHue(float hue)
//{
//	_hue = hue;
//	updateColorMatrix();
//}



const GLchar * colorRotationShaderBody()
{
	return
		"                                                               \n\
		#ifdef GL_ES                                                    \n\
		precision mediump float;                                        \n\
		#endif                                                          \n\
		\n\
		varying vec2 v_texCoord;                                        \n\
		uniform mat3 u_hue;                                             \n\
		uniform float u_alpha;                                          \n\
		\n\
		void main()                                                     \n\
		{                                                               \n\
		vec4 pixColor = texture2D(CC_Texture0, v_texCoord);             \n\
		vec3 rgbColor = u_hue * pixColor.rgb;                           \n\
		gl_FragColor = vec4(rgbColor, pixColor.a * u_alpha);            \n\
		}                                                               \n\
		";
}

void xRotateMat(float mat[3][3], float rs, float rc)
{
	mat[0][0] = 1.0;
	mat[0][1] = 0.0;
	mat[0][2] = 0.0;

	mat[1][0] = 0.0;
	mat[1][1] = rc;
	mat[1][2] = rs;

	mat[2][0] = 0.0;
	mat[2][1] = -rs;
	mat[2][2] = rc;
}

void yRotateMat(float mat[3][3], float rs, float rc)
{
	mat[0][0] = rc;
	mat[0][1] = 0.0;
	mat[0][2] = -rs;

	mat[1][0] = 0.0;
	mat[1][1] = 1.0;
	mat[1][2] = 0.0;

	mat[2][0] = rs;
	mat[2][1] = 0.0;
	mat[2][2] = rc;
}

void zRotateMat(float mat[3][3], float rs, float rc)
{
	mat[0][0] = rc;
	mat[0][1] = rs;
	mat[0][2] = 0.0;

	mat[1][0] = -rs;
	mat[1][1] = rc;
	mat[1][2] = 0.0;

	mat[2][0] = 0.0;
	mat[2][1] = 0.0;
	mat[2][2] = 1.0;
}

void matrixMult(float a[3][3], float b[3][3], float c[3][3])
{
	int x, y;
	float temp[3][3];

	for(y=0; y<3; y++) {
		for(x=0; x<3; x++) {
			temp[y][x] = b[y][0] * a[0][x] + b[y][1] * a[1][x] + b[y][2] * a[2][x];
		}
	}
	for(y=0; y<3; y++) {
		for(x=0; x<3; x++) {
			c[y][x] = temp[y][x];
		}
	}
}

void hueMatrix(GLfloat mat[3][3], float angle)
{
#define SQRT_2      sqrt(2.0)
#define SQRT_3      sqrt(3.0)

	float mag, rot[3][3];
	float xrs, xrc;
	float yrs, yrc;
	float zrs, zrc;

	// Rotate the grey vector into positive Z
	mag = SQRT_2;
	xrs = 1.0/mag;
	xrc = 1.0/mag;
	xRotateMat(mat, xrs, xrc);
	mag = SQRT_3;
	yrs = -1.0/mag;
	yrc = SQRT_2/mag;
	yRotateMat(rot, yrs, yrc);
	matrixMult(rot, mat, mat);

	// Rotate the hue
	zrs = sin(angle);
	zrc = cos(angle);
	zRotateMat(rot, zrs, zrc);
	matrixMult(rot, mat, mat);

	// Rotate the grey vector back into place
	yRotateMat(rot, -yrs, yrc);
	matrixMult(rot,  mat, mat);
	xRotateMat(rot, -xrs, xrc);
	matrixMult(rot,  mat, mat);
}

void premultiplyAlpha(GLfloat mat[3][3], float alpha)
{
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			mat[i][j] *= alpha;
		}
	}
}





