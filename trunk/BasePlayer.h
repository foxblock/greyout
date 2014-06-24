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
        bool canJump;
        bool isJumping;
        int fallCounter;
        int lastYCorrection; // used to find out when walking off a cliff and start falling animation

    private:

};


#endif
