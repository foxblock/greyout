#include "ExitTrigger.h"

#include "Level.h"

ExitTrigger::ExitTrigger(Level* newParent) : BaseTrigger(newParent)
{
    triggerCol = LIGHT_RED;
    startingState = "open";
    states["open"] = NULL;
    states["closed"] = NULL;
}

ExitTrigger::~ExitTrigger()
{
    //
}

///---public---

///---protected---

void ExitTrigger::doTrigger(const UnitCollisionEntry& entry)
{
    if (currentState == "open")
    {
        entry.unit->toBeRemoved = true;
        parent->swapControl();
        if (entry.unit->flags.hasFlag(ufMissionObjective))
            parent->winCounter--;
    }
}

///---private---
