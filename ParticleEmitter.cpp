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
	screenPosition(0,0),
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

void ParticleEmitter::updateScreenPosition(const Vector2di& offset)
{
	screenPosition.x = position.x - offset.x;
	screenPosition.y = position.y - offset.y;
}

void ParticleEmitter::render(SDL_Surface* surf)
{
#ifdef _DEBUG
	SDL_Rect temp;
	if (centred)
	{
		temp.x = screenPosition.x;
		temp.y = screenPosition.y - size.y / 2.0f;
		temp.w = 1;
		temp.h = size.y;
		SDL_FillRect(surf,&temp,col.getSDL_Uint32Colour(surf));
		temp.x = screenPosition.x - size.x / 2.0f;
		temp.y = screenPosition.y;
		temp.w = size.x;
		temp.h = 1;
		SDL_FillRect(surf,&temp,col.getSDL_Uint32Colour(surf));
	}
	else
	{
		temp.x = screenPosition.x;
		temp.y = screenPosition.y;
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
	switch (stringToProp[value.first])
	{
	case BaseUnit::upSize:
	{
		size = StringUtility::stringToVec<Vector2di>(value.second);
		return true;
	}
	case epEnabled:
	{
		enabled = StringUtility::stringToBool(value.second);
		return true;
	}
	case epDirection:
	{
		emitDir = StringUtility::stringToVec<Vector2df>(value.second);
		emitDir.normalise();
		return true;
	}
	case epPower:
	{
		float temp = StringUtility::stringToFloat(value.second);
		emitPower.x = temp;
		emitPower.y = temp;
		return true;
	}
	case epLifetime:
	{
		int temp = 0;
		if (!pLoadTime( value.second, temp ))
		{
			printf("ERROR loading time value!\n");
			return false;
		}
		particleLifetime.x = temp;
		particleLifetime.y = temp;
		return true;
	}
	case epDelay:
	{
		int temp = 0;
		if (!pLoadTime( value.second, temp ))
		{
			printf("ERROR loading time value!\n");
			return false;
		}
		nextParticleTime.x = temp;
		nextParticleTime.y = temp;
		return true;
	}
	case epDirectionScatter:
	{
		angleScatter = NumberUtility::degToRad(StringUtility::stringToFloat(value.second));
		return true;
	}
	case epPowerScatter:
	{
		float temp = StringUtility::stringToFloat(value.second);
		emitPower.x -= temp;
		emitPower.y += temp;
		return true;
	}
	case epLifetimeScatter:
	{
		int temp = 0;
		if (!pLoadTime( value.second, temp ))
		{
			printf("ERROR loading time value!\n");
			return false;
		}
		particleLifetime.x -= temp;
		particleLifetime.y += temp;
		return true;
	}
	case epDelayScatter:
	{
		int temp = 0;
		if (!pLoadTime( value.second, temp ))
		{
			printf("ERROR loading time value!\n");
			return false;
		}
		nextParticleTime.x -= temp;
		nextParticleTime.y += temp;
		return true;
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
		return true;
	}
	case epCentred:
	{
		centred = StringUtility::stringToBool(value.second);
		return true;
	}
	default:
		return BaseUnit::processParameter(value);
	}
}

void ParticleEmitter::generateParameters()
{
	BaseUnit::generateParameters();
	parameters.push_back(make_pair("size", StringUtility::vecToString(size)));
	parameters.push_back(make_pair("direction", StringUtility::vecToString(emitDir)));
	parameters.push_back(make_pair("power", StringUtility::floatToString(emitPower.x + (emitPower.y - emitPower.x) / 2.0f)));
	parameters.push_back(make_pair("lifetime", StringUtility::intToString(particleLifetime.x + (particleLifetime.y - particleLifetime.x) / 2.0f) + "f"));
	parameters.push_back(make_pair("delay", StringUtility::intToString(nextParticleTime.x + (nextParticleTime.y - nextParticleTime.x) / 2.0f) + "f"));
	if (angleScatter != 0)
		parameters.push_back(make_pair("directionscatter", StringUtility::floatToString(angleScatter)));
	if (emitPower.x != emitPower.y)
		parameters.push_back(make_pair("powerscatter", StringUtility::floatToString((emitPower.y - emitPower.x) / 2.0f)));
	parameters.push_back(make_pair("lifetimescatter", StringUtility::intToString((particleLifetime.y - particleLifetime.x) / 2.0f)));
	parameters.push_back(make_pair("delayscatter", StringUtility::intToString((nextParticleTime.y - nextParticleTime.x) / 2.0f)));
	if (!enabled)
		parameters.push_back(make_pair("enabled", StringUtility::boolToString(enabled)));
	if (!multiplier.empty())
	{
		string temp = "";
		for (vector<int>::iterator I = multiplier.begin(); I != multiplier.end(); ++I)
			temp += StringUtility::intToString(*I) + DELIMIT_STRING;
		temp.erase(temp.length()-1);
		parameters.push_back(make_pair("multiplier", temp));
	}
	if (!centred)
		parameters.push_back(make_pair("centred", StringUtility::boolToString(centred)));

}

///--- PROTECTED ---------------------------------------------------------------

///--- PRIVATE -----------------------------------------------------------------
