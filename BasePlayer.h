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

        virtual bool load(const PARAMETER_TYPE& params);

        virtual void update();

        virtual void hitMap(const Vector2df& correctionOverride);

        virtual void control(SimpleJoy* input);

    protected:
        bool canJump;
    private:

};


#endif
