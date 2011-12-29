#include "CameraTrigger.h"

#include "Level.h"

CameraTrigger::CameraTrigger(Level* newParent) : BaseTrigger(newParent)
{
    stringToProp["destination"] = cpDestination;
    stringToProp["time"] = cpTime;
    time = 1000;
    dest = Vector2df(0,0);
    triggerCol = PURPLE;
}

CameraTrigger::~CameraTrigger()
{
    //
}

///---public---

///---protected---

bool CameraTrigger::processParameter(const PARAMETER_TYPE& value)
{
    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case cpDestination:
    {
        vector<string> token;
        StringUtility::tokenize(value.second,token,DELIMIT_STRING);
        if (token.size() != 2)
        {
            parsed = false;
            break;
        }
        dest.x = StringUtility::stringToFloat(token[0]);
        dest.y = StringUtility::stringToFloat(token[1]);
        break;
    }
    case cpTime:
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

void CameraTrigger::doTrigger(const UnitCollisionEntry& entry)
{
    parent->cam.centerOnPos(dest,time);
    enabled = false;
}

///---private---
