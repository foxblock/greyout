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
        parent->swapControl();
        if (entry.unit->flags.hasFlag(ufMissionObjective))
            parent->winCounter--;
		enabled = false;
    }
}

///---private---
