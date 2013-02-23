#ifndef _PARTICLE_EMITTER_H
#define _PARTICLE_EMITTER_H

#include "BaseUnit.h"

#include "CountDown.h"

class ParticleEmitter : public BaseUnit
{
public:
    ParticleEmitter(Level* newParent);
	virtual ~ParticleEmitter();

	int getWidth() const {return 16;}
	int getHeight() const {return 16;}

    virtual void reset();

	virtual void update();
	virtual void render(SDL_Surface* surf);

	virtual bool processParameter(const PARAMETER_TYPE& value);

protected:
	CountDown particleTimer;
	Vector2df emitDir;
	float angleScatter;
	Vector2df emitPower;
	Vector2di particleLifetime;
	Vector2di nextParticleTime;

	bool active;

	enum EmitterProps {
		epDirection=BaseUnit::upEOL,
		epPower,
		epLifetime,
		epFrequency,
		epDirectionScatter,
		epPowerScatter,
		epLifetimeScatter,
		epFrequencyScatter,
		epActive
	};
private:

};


#endif // _PARTICLE_EMITTER_H

