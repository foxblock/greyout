#include "ExitTrigger.h"

#include "Level.h"

ExitTrigger::ExitTrigger(Level* newParent) : BaseTrigger(newParent)
{
    triggerCol = LIGHT_RED;
}

ExitTrigger::~ExitTrigger()
{
    //
}

///---public---

///---protected---

void ExitTrigger::doTrigger(const UnitCollisionEntry& entry)
{
    entry.unit->toBeRemoved = true;
    parent->swapControl();
    if (entry.unit->flags.hasFlag(ufMissionObjective))
        parent->winCounter--;
}

///---private---
