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

#include "ExitTrigger.h"

#include "Level.h"

ExitTrigger::ExitTrigger(Level* newParent) : BaseTrigger(newParent)
{
    triggerCol = LIGHT_GREEN;
}

ExitTrigger::~ExitTrigger()
{
    //
}

///---public---

bool ExitTrigger::load(list<PARAMETER_TYPE >& params)
{
    bool result = BaseTrigger::load(params);

    states["open"] = NULL;
    states["closed"] = NULL;
	if (startingState[0] == 0 || startingState == "default")
		startingState = "open";
    setSpriteState(startingState);

    return result;
}

///---protected---

void ExitTrigger::doTrigger(const UnitCollisionEntry& entry)
{
    if (currentState == "open")
    {
        entry.unit->toBeRemoved = true;
        if (entry.unit->isPlayer)
			parent->swapControl();
        if (entry.unit->flags.hasFlag(ufMissionObjective))
            parent->winCounter--;
		BaseTrigger::doTrigger(entry);
    }
}

///---private---
