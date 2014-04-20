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

#ifndef _PARTICLE_EMITTER_H
#define _PARTICLE_EMITTER_H

#include "BaseUnit.h"

class ParticleEmitter : public BaseUnit
{
public:
    ParticleEmitter(Level* newParent);
	virtual ~ParticleEmitter();

	int getWidth() const {return size.x;}
	int getHeight() const {return size.y;}

    virtual void reset();

	virtual void update();
	virtual void render(SDL_Surface* surf);

	virtual bool processParameter(const PARAMETER_TYPE& value);

protected:
	int particleTimer;
	Vector2df emitDir;
	float angleScatter;
	Vector2df emitPower;
	Vector2di particleLifetime;
	Vector2di nextParticleTime;
	int multiplier;
	Vector2di size;
	bool centred;

	bool active;

	enum EmitterProps {
		epDirection=BaseUnit::upEOL,
		epPower,
		epLifetime,
		epDelay,
		epDirectionScatter,
		epPowerScatter,
		epLifetimeScatter,
		epDelayScatter,
		epActive,
		epMultiplier,
		epCentred,
		epEOL
	};
private:

};


#endif // _PARTICLE_EMITTER_H

