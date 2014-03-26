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

#include "LevelTrigger.h"

#include "Level.h"

LevelTrigger::LevelTrigger(Level* newParent) : BaseTrigger(newParent)
{
    triggerCol = ORANGE;
    targetLvl = newParent;
	actionHitTarget = false;
}

LevelTrigger::~LevelTrigger()
{
    //
}

///---public---

bool LevelTrigger::processParameter(const PARAMETER_TYPE& value)
{
    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case BaseUnit::upTarget:
    {
        printf("WARNING: LevelTrigger does not take targets.\n");
        break;
    }
    default:
        parsed = false;
    }

    if (not parsed)
        return BaseTrigger::processParameter(value);

    return parsed;
}

///---protected---

void LevelTrigger::doTrigger(const UnitCollisionEntry& entry)
{
    targetLvl->processParameter(targetParam);
    targetLvl->tilingSetup(); // re-set variables set once after load-call, which might be invalid now
    BaseTrigger::doTrigger(entry);
}

///---private---
