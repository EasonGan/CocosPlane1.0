#include "RapidParticle.h"


RapidParticle* RapidParticle::create( const std::string& file )
{
	RapidParticle *ret = new (std::nothrow) RapidParticle();
	if (ret && ret->initWithFile(file))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return ret;
}

RapidParticle* RapidParticle::createWithExsitParticle( RapidParticle* particle )
{
	RapidParticle *ret = new (std::nothrow) RapidParticle();
	if (ret && ret->initParticlePro(particle))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return ret;
}

bool RapidParticle::initParticlePro( RapidParticle* particle  )
{
	bool ret = false;
	unsigned char *buffer = nullptr;
	unsigned char *deflated = nullptr;

	do 
	{
		int maxParticles = particle->getTotalParticles();;
		// self, not super
		if(this->initWithTotalParticles(maxParticles))
		{
			// angle
			_angle = particle->getAngle();
			_angleVar = particle->getAngleVar();

			// duration
			_duration = particle->getDuration();

			_blendFunc.src = particle->getBlendFunc().src;
			_blendFunc.dst = particle->getBlendFunc().dst;

			// color
			_startColor.r = particle->getStartColor().r;
			_startColor.g = particle->getStartColor().g;
			_startColor.b = particle->getStartColor().b;
			_startColor.a = particle->getStartColor().a;

			_startColorVar.r = particle->getStartColorVar().r;
			_startColorVar.g = particle->getStartColorVar().g;
			_startColorVar.b = particle->getStartColorVar().b;
			_startColorVar.a = particle->getStartColorVar().a;

			_endColor.r = particle->getEndColor().r;
			_endColor.g = particle->getEndColor().g;
			_endColor.b = particle->getEndColor().b;
			_endColor.a = particle->getEndColor().a;

			_endColorVar.r = particle->getEndColorVar().r;
			_endColorVar.g = particle->getEndColorVar().g;
			_endColorVar.b = particle->getEndColorVar().b;
			_endColorVar.a = particle->getEndColorVar().a;

			// particle size
			_startSize = particle->getStartSize();
			_startSizeVar = particle->getStartSizeVar();
			_endSize = particle->getEndSize();
			_endSizeVar = particle->getEndSizeVar();

			// position
			float x = particle->getSourcePosition().x;
			float y = particle->getSourcePosition().y;
			this->setPosition(x,y);            
			_posVar.x = particle->getPosVar().x;
			_posVar.y = particle->getPosVar().y;

			// Spinning
			_startSpin = particle->getStartSpin(); 
			_startSpinVar = particle->getStartSpinVar(); 
			_endSpin= particle->getEndSpin(); 
			_endSpinVar= particle->getEndSpinVar(); 

			_emitterMode = particle->getEmitterMode(); 

			// Mode A: Gravity + tangential accel + radial accel
			if (_emitterMode == Mode::GRAVITY)
			{
				// gravity
				modeA.gravity.x = particle->getGravity().x; 
				modeA.gravity.y = particle->getGravity().y; 

				// speed
				modeA.speed = particle->getSpeed(); 
				modeA.speedVar = particle->getSpeedVar(); 

				// radial acceleration
				modeA.radialAccel = particle->getRadialAccel(); 
				modeA.radialAccelVar = particle->getRadialAccelVar(); 

				// tangential acceleration
				modeA.tangentialAccel = particle->getTangentialAccel();
				modeA.tangentialAccelVar = particle->getTangentialAccelVar();

				// rotation is dir
				modeA.rotationIsDir = particle->getRotationIsDir();
			}

			// or Mode B: radius movement
			else if (_emitterMode == Mode::RADIUS)
			{
				modeB.startRadius = particle->getStartRadius();
				modeB.startRadiusVar = particle->getStartRadiusVar();

				modeB.endRadius = particle->getEndRadius();
				modeB.endRadiusVar = particle->getEndRadiusVar();

				modeB.rotatePerSecond = particle->getRotatePerSecond();
				modeB.rotatePerSecondVar = particle->getRotatePerSecondVar();

			} else {
				CCASSERT( false, "Invalid emitterType in config file");
				CC_BREAK_IF(true);
			}

			// life span
			_life = particle->getLife();
			_lifeVar = particle->getLifeVar();

			// emission Rate
			_emissionRate = _totalParticles / _life;

			//don't get the internal texture if a batchNode is used
			if (!_batchNode)
			{
				// Set a compatible default for the alpha transfer
				_opacityModifyRGB = false;

				Texture2D* tex = particle->getTexture();			//ÌùÍ¼

				if (tex)
				{
					setTexture(tex);
				}
				else
				{                        
					CCASSERT(tex != nullptr, "particle texture is null");
				}

				if( !this->_texture)
					CCLOGWARN("cocos2d: Warning: ParticleSystemQuad system without a texture");
			}
			ret = true;
		}
	} while (0);
	free(buffer);
	free(deflated);
	return ret;

}
