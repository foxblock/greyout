/*
	Greyout - a colourful platformer about love

	Greyout is Copyright (c)2011-2014 Janek Schäfer

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
	Janek Schäfer (foxblock), foxblock_at_gmail_dot_com
*/

#include "ParticleEmitter.h"

#include "NumberUtility.h"
#include "Random.h"
#include "Level.h"
#include "MyGame.h"

ParticleEmitter::ParticleEmitter( Level *newParent ) :
	BaseUnit(newParent),
	particleTimer(0),
	emitDir(0,0),
	angleScatter(0),
	emitPower(0,0),
	particleLifetime(0,0),
	nextParticleTime(0,0),
	size(16,16),
	centred(true),
	enabled(true)
{
	flags.addFlag(ufNoMapCollision);
	flags.addFlag(ufNoGravity);
	flags.addFlag(ufInvincible);
	#ifndef _DEBUG
	flags.addFlag(ufNoRender);
	#endif // _DEBUG
	unitCollisionMode = 0;

	stringToProp["direction"] = epDirection;
	stringToProp["power"] = epPower;
	stringToProp["lifetime"] = epLifetime;
	stringToProp["delay"] = epDelay;
	stringToProp["directionscatter"] = epDirectionScatter;
	stringToProp["powerscatter"] = epPowerScatter;
	stringToProp["lifetimescatter"] = epLifetimeScatter;
	stringToProp["delayscatter"] = epDelayScatter;
	stringToProp["enabled"] = epEnabled;
	stringToProp["multiplier"] = epMultiplier;
	stringToProp["centred"] = epCentred;

	Random::randSeed();
}

ParticleEmitter::~ParticleEmitter()
{
	//
}


///--- PUBLIC ------------------------------------------------------------------

void ParticleEmitter::reset()
{
	enabled = true;
	BaseUnit::reset();
}

void ParticleEmitter::update()
{
	BaseUnit::update();
	if (enabled)
	{
		if (particleTimer == 0)
		{
			int multi = multiplier.empty() ? 1 : multiplier[ENGINE->settings->getParticleDensity()];
			for (int I = 0; I < multi; ++I)
			{
				Vector2df tempDir = emitDir;
				Vector2df pos = position;
				if (angleScatter != 0)
				{
					float tempAngle = Random::nextFloat(-angleScatter, angleScatter);
					tempDir.x = emitDir.x * cos(tempAngle) - emitDir.y * sin(tempAngle);
					tempDir.y = emitDir.x * sin(tempAngle) + emitDir.y * cos(tempAngle);
				}
				tempDir *= Random::nextFloat(emitPower.x,emitPower.y);
				if (!centred)
				{
					pos.x = Random::nextFloat(pos.x, pos.x + size.x);
					pos.y = Random::nextFloat(pos.y, pos.y + size.y);
				}
				parent->addParticle(this,col,pos,tempDir,Random::nextInt(particleLifetime.x,particleLifetime.y));
			}
			particleTimer = max(Random::nextInt(nextParticleTime.x,nextParticleTime.y),1);
		}
		--particleTimer;
	}
}

void ParticleEmitter::render(SDL_Surface* surf)
{
#ifdef _DEBUG
	SDL_Rect temp;
	if (centred)
	{
		temp.x = position.x;
		temp.y = position.y - size.y / 2.0f;
		temp.w = 1;
		temp.h = size.y;
		SDL_FillRect(surf,&temp,col.getSDL_Uint32Colour(surf));
		temp.x = position.x - size.x / 2.0f;
		temp.y = position.y;
		temp.w = size.x;
		temp.h = 1;
		SDL_FillRect(surf,&temp,col.getSDL_Uint32Colour(surf));
	}
	else
	{
		temp.x = position.x;
		temp.y = position.y;
		temp.w = size.x;
		temp.h = 1;
		SDL_FillRect(surf,&temp,col.getSDL_Uint32Colour(surf));
		temp.y += size.y;
		SDL_FillRect(surf,&temp,col.getSDL_Uint32Colour(surf));
		temp.w = 1;
		temp.h = size.y;
		temp.y -= size.y;
		SDL_FillRect(surf,&temp,col.getSDL_Uint32Colour(surf));
		temp.x += size.x;
		SDL_FillRect(surf,&temp,col.getSDL_Uint32Colour(surf));
	}
#else
	// Don't render anything
#endif
}

bool ParticleEmitter::processParameter(const PARAMETER_TYPE& value)
{
	bool parsed = true;

	switch (stringToProp[value.first])
	{
	case BaseUnit::upSize:
	{
		size = StringUtility::stringToVec<Vector2di>(value.second);
		break;
	}
	case epEnabled:
	{
		enabled = StringUtility::stringToBool(value.second);
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
		multiplier.clear();
		vector<string> tokens;
		StringUtility::tokenize( value.second, tokens, DELIMIT_STRING );
		for (vector<string>::const_iterator I = tokens.begin(); I != tokens.end(); ++I)
		{
			multiplier.push_back(StringUtility::stringToInt(*I));
		}
		int temp = multiplier.size();
		for (int I = temp; I < Settings::pdEOL; ++I)
		{
			multiplier.push_back(multiplier[temp-1]);
		}
		break;
	}
	case epCentred:
	{
		centred = StringUtility::stringToBool(value.second);
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
