/*
	Greyout - a colourful platformer about love

	Greyout is Copyright (c)2011-2014 Janek Sch�fer

	This file is part of Greyout.

    Greyout is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

	Greyout is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Please direct any feedback, questions or comments to
	Janek Sch�fer (foxblock), foxblock_at_gmail_dot_com
*/

#include "ParticleEmitter.h"

#include "NumberUtility.h"
#include "Random.h"
#include "Level.h"

ParticleEmitter::ParticleEmitter( Level *newParent ) :
	BaseUnit(newParent),
	particleTimer(0),
	emitDir(0,0),
	angleScatter(0),
	emitPower(0,0),
	particleLifetime(0,0),
	nextParticleTime(0,0),
	multiplier(0),
	active(true)
{
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);
    flags.addFlag(ufInvincible);
    unitCollisionMode = 0;

    stringToProp["direction"] = epDirection;
    stringToProp["power"] = epPower;
    stringToProp["lifetime"] = epLifetime;
    stringToProp["delay"] = epDelay;
    stringToProp["directionscatter"] = epDirectionScatter;
    stringToProp["powerscatter"] = epPowerScatter;
    stringToProp["lifetimescatter"] = epLifetimeScatter;
    stringToProp["delayscatter"] = epDelayScatter;
    stringToProp["active"] = epActive;
    stringToProp["multiplier"] = epMultiplier;

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
		if (particleTimer == 0)
		{
			for (int I = 0; I < multiplier; ++I)
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
			}
			particleTimer = Random::nextInt(nextParticleTime.x,nextParticleTime.y);
		}
		--particleTimer;
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
        active = StringUtility::stringToBool(value.second);
        break;
    }
    case epDirection:
	{
		emitDir = StringUtility::stringToVec<Vector2df>(value.second);
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
		int temp = 0;
		pLoadTime( value.second, temp );
		particleLifetime.x = temp;
		particleLifetime.y = temp;
		break;
	}
    case epDelay:
	{
		int temp = 0;
		pLoadTime( value.second, temp );
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
	case epLifetimeScatter:
	{
		int temp = 0;
		pLoadTime( value.second, temp );
		particleLifetime.x -= temp;
		particleLifetime.y += temp;
		break;
	}
    case epDelayScatter:
	{
		int temp = 0;
		pLoadTime( value.second, temp );
		nextParticleTime.x -= temp;
		nextParticleTime.y += temp;
		break;
	}
	case epMultiplier:
	{
		multiplier = StringUtility::stringToInt(value.second);
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
