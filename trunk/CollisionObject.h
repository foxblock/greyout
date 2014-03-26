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

#ifndef COLLISIONOBJECT_H
#define COLLISIONOBJECT_H

#include <vector>

#include "SimpleDirection.h"
#include "Vector2df.h"
#include "Colour.h"

/**
Contains information about the last collision check, such as the collising pixels, correction, etc.
Also provides functions for analysing said data
**/

struct MapCollisionEntry
{
    SimpleDirection dir;
    Vector2df pos;
    Colour col;
    Vector2df correction; // currently unused
};

class BaseUnit;

struct UnitCollisionEntry
{
    SimpleDirection dir;
    Vector2df overlap;
    BaseUnit* unit;
};

class CollisionObject
{
    public:
        CollisionObject();
        virtual ~CollisionObject();

        virtual void clear();

        /// Analysing functions
        bool isBeingSquashed() const;
        bool isHealthy(const Vector2df& vel);

        // Contains the checked pixels
        vector<MapCollisionEntry> pixels;
        // Total velocity correction, calculated by Physics
        Vector2df correction;
        // Total position correction - this correction will only affect the next
        // cycle directly and is usually caused by vertical/slope correction to
        // avoid rocketing the player in the air
        Vector2df positionCorrection;

        // count the ticks in which the unit is being squashed here
        // if that value reaches a certain value the unit will explode
        int squashCounter;
        int squashThreshold;

        // Hold all units with which the unit collided
        vector<UnitCollisionEntry> units;
    protected:

    private:
};



#endif // COLLISIONOBJECT_H

