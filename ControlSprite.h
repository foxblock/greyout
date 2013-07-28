#ifndef CONTROL_SPRITE_H
#define CONTROL_SPRITE_H

#include "BaseUnit.h"

class ControlSprite : public BaseUnit
{
public:
    ControlSprite(Level* newParent);
    virtual ~ControlSprite();

    virtual bool load(list<PARAMETER_TYPE >& params);

protected:

private:
	AnimatedSprite* loadFrames(SDL_Surface* const surf, CRint skip, CRint num, CRbool loop, CRstring state);

};


#endif // CONTROL_SPRITE_H

