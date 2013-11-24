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
