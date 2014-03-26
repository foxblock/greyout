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

#include "SolidBox.h"

SolidBox::SolidBox(Level* newParent) : PushableBox(newParent)
{
    flags.addFlag(ufNoGravity);
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufInvincible);
    unitCollisionMode = 0;
}

SolidBox::~SolidBox()
{
    //
}

void SolidBox::hitUnit(const UnitCollisionEntry& entry)
{
    // this moves a carried player unit when moving horizontally
    if (!flags.hasFlag(ufNoUpdate))
    {
        if (entry.overlap.x > entry.overlap.y && entry.unit->position.y < position.y && velocity.x != 0)
        {
            if (velocity.x != 0)
                entry.unit->collisionInfo.positionCorrection.x += velocity.x;
            if (velocity.y != 0)
                entry.unit->velocity.y -= entry.overlap.y - NumberUtility::sign(entry.overlap.y);
        }
    }
}

void SolidBox::move()
{
    BaseUnit::move();
}
