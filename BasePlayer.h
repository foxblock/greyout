#ifndef BASEPLAYER_H
#define BASEPLAYER_H

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

        virtual bool load(list<PARAMETER_TYPE >& params);
        virtual void resetTemporary();

        virtual void update();

        virtual void hitMap(const Vector2df& correctionOverride);

        virtual void control(SimpleJoy* input);

        #ifdef _DEBUG
        virtual string debugInfo();
        #endif // _DEBUG

        // used in PushableBox::hitUnit for some hacky movement stuff (don't look!)
        bool activelyMoving;

    protected:
        AnimatedSprite* loadFrames(SDL_Surface* const surf, CRint skip, CRint num, CRbool loop, CRstring state);

        bool canJump;
        bool isJumping;
        int fallCounter;
        int standDelay;

    private:

};


#endif
