#include "ParticleEmitter.h"

#include "NumberUtility.h"
#include "Random.h"
#include "Level.h"

ParticleEmitter::ParticleEmitter( Level *newParent ) :
	BaseUnit(newParent),
	emitDir(0,0),
	angleScatter(0),
	emitPower(0,0),
	particleLifetime(0,0),
	nextParticleTime(0,0),
	active(true)
{
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);
    flags.addFlag(ufInvincible);
    unitCollisionMode = 0;

    stringToProp["direction"] = epDirection;
    stringToProp["power"] = epPower;
    stringToProp["lifetime"] = epLifetime;
    stringToProp["frequency"] = epFrequency;
    stringToProp["directionscatter"] = epDirectionScatter;
    stringToProp["powerscatter"] = epPowerScatter;
    stringToProp["lifetimescatter"] = epLifetimeScatter;
    stringToProp["frequencyscatter"] = epFrequencyScatter;
    stringToProp["active"] = epActive;

    Random::randSeed();
}

ParticleEmitter::~ParticleEmitter()
{
	//
}


///--- PUBLIC ------------------------------------------------------------------

void ParticleEmitter::reset()
{
	active = true;
	BaseUnit::reset();
}

void ParticleEmitter::update()
{
	BaseUnit::update();
	if (active)
	{
		particleTimer.update();
		if (particleTimer.hasFinished() || !particleTimer.isStarted())
		{
			Vector2df tempDir = emitDir;
			if (angleScatter != 0)
			{
				float tempAngle = Random::nextFloat(-angleScatter,angleScatter);
				tempDir.x = emitDir.x * cos(tempAngle) - emitDir.y * sin(tempAngle);
				tempDir.y = emitDir.x * sin(tempAngle) + emitDir.y * cos(tempAngle);
			}
			tempDir *= Random::nextFloat(emitPower.x,emitPower.y);
			parent->addParticle(this,col,position,tempDir,Random::nextInt(particleLifetime.x,particleLifetime.y));
			particleTimer.start(Random::nextInt(nextParticleTime.x,nextParticleTime.y));
		}
	}
}

void ParticleEmitter::render(SDL_Surface* surf)
{
#ifdef _DEBUG
    SDL_Rect temp;
    temp.x = position.x - 8;
    temp.y = position.y - 8;
    temp.w = 16;
    temp.h = 1;
    SDL_FillRect(surf,&temp,col.getSDL_Uint32Colour(surf));
    temp.y += 16;
    SDL_FillRect(surf,&temp,col.getSDL_Uint32Colour(surf));
    temp.w = 1;
    temp.h = 16;
    temp.y -= 16;
    SDL_FillRect(surf,&temp,col.getSDL_Uint32Colour(surf));
    temp.x += 16;
    SDL_FillRect(surf,&temp,col.getSDL_Uint32Colour(surf));
#else
    // Don't render anything
#endif
}

bool ParticleEmitter::processParameter(const PARAMETER_TYPE& value)
{
	bool parsed = true;
	static float frequency = 1;

    switch (stringToProp[value.first])
    {
//    case BaseUnit::upSize:
//    {
//        vector<string> token;
//        StringUtility::tokenize(value.second,token,DELIMIT_STRING);
//        if (token.size() != 2)
//        {
//            parsed = false;
//            break;
//        }
//        width = StringUtility::stringToInt(token[0]);
//        height = StringUtility::stringToInt(token[1]);
//        break;
//    }
    case epActive:
    {
        active = StringUtility::stringToVec(value.second);
        break;
    }
    case epDirection:
	{
		emitDir = StringUtility::stringToVec(value.second);
		emitDir.normalise();
		break;
	}
    case epPower:
	{
		float temp = StringUtility::stringToFloat(value.second);
		emitPower.x = temp;
		emitPower.y = temp;
		break;
	}
    case epLifetime:
	{
		int temp = StringUtility::stringToInt(value.second);
		particleLifetime.x = temp;
		particleLifetime.y = temp;
		break;
	}
    case epFrequency:
	{
		frequency = StringUtility::stringToFloat(value.second);
		if (frequency == 0)
		{
			parsed = false;
			break;
		}
		float temp = 1000.f * 60.f / frequency;
		nextParticleTime.x = temp;
		nextParticleTime.y = temp;
		break;
	}
    case epDirectionScatter:
	{
		angleScatter = NumberUtility::degToRad(StringUtility::stringToFloat(value.second));
		break;
	}
    case epPowerScatter:
	{
		float temp = StringUtility::stringToFloat(value.second);
		emitPower.x -= temp;
		emitPower.y += temp;
		break;
	}
    case epFrequencyScatter:
	{
		float temp = StringUtility::stringToFloat(value.second);
		nextParticleTime.x = 1000.f * 60.f / (frequency + temp);
		nextParticleTime.y = 1000.f * 60.f / (frequency - temp);
		break;
	}
    default:
        parsed = false;
    }

    if (not parsed)
        return BaseUnit::processParameter(value);

    return parsed;
}


///--- PROTECTED ---------------------------------------------------------------

///--- PRIVATE -----------------------------------------------------------------
