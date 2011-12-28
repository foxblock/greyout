#include "DialogueTrigger.h"

#include "Dialogue.h"

DialogueTrigger::DialogueTrigger(Level* newParent) : BaseTrigger(newParent)
{
    stringToProp["textkey"] = tpTextKey;
    stringToProp["time"] = tpTime;
    textKey = "";
    time = 1000;
    triggered = false;
    triggerCol = LIGHT_GREEN;
}

DialogueTrigger::~DialogueTrigger()
{
    //
}

///---public---

///---protected---

bool DialogueTrigger::processParameter(const PARAMETER_TYPE& value)
{
    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case tpTextKey:
    {
        textKey = value.second;
        break;
    }
    case tpTime:
    {
        time = StringUtility::stringToInt(value.second);
        break;
    }
    default:
        parsed = false;
    }

    if (not parsed)
        return BaseTrigger::processParameter(value);

    return parsed;
}

void DialogueTrigger::doTrigger(const UnitCollisionEntry& entry)
{
    if (not triggered)
    {
        DIALOGUE->queueLine(textKey,this,time);
        triggered = true;
    }
}

///---private---
