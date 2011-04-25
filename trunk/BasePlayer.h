#ifndef BASEPLAYER_H
#define BASEPLAYER_H

#include "CountDown.h"

#include "ControlUnit.h"

/**
The base class for player units in Greyout
Basically just an implementation of ControlUnit customized to load player
graphics and react on the controls like the player unit should do
**/

class BasePlayer : public ControlUnit
{
    public:
        BasePlayer(Level* newParent);
        ~BasePlayer();

        virtual bool load(const PARAMETER_TYPE& params);
        virtual void resetTemporary();

        virtual void update();

        virtual void hitMap(const Vector2df& correctionOverride);

        virtual void control(SimpleJoy* input);

    protected:
        AnimatedSprite* loadFrames(SDL_Surface* const surf, CRint skip, CRint num, CRbool loop, CRstring state);

        bool canJump;
        CountDown fallCounter;

    private:

};


#endif
