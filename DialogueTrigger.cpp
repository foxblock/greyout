#include "DialogueTrigger.h"

#include "Dialogue.h"
#include "StringUtility.h"

DialogueTrigger::DialogueTrigger(Level* newParent) : BaseUnit(newParent)
{
    stringToProp["textkey"] = tpTextKey;
    stringToProp["size"] = tpSize;
    stringToProp["time"] = tpTime;
    width = 32;
    height = 32;
    textKey = "";
    time = 1000;
    triggered = false;
    collisionColours.push_back(BLACK);
    collisionColours.push_back(WHITE);
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);
}

DialogueTrigger::~DialogueTrigger()
{
    //
}

///---public---

void DialogueTrigger::render(SDL_Surface* surf)
{
    // don't render anything
}

void DialogueTrigger::hitUnit(const UNIT_COLLISION_DATA_TYPE& collision, BaseUnit* const unit)
{
    if (not triggered)
    {
        DIALOGUE->queueLine(textKey,this,time);
        triggered = true;
    }
}

///---protected---

bool DialogueTrigger::processParameter(const pair<string,string>& value)
{
    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case tpSize:
    {
        vector<string> token;
        StringUtility::tokenize(value.second,token,DELIMIT_STRING);
        if (token.size() != 2)
        {
            parsed = false;
            break;
        }
        width = StringUtility::stringToInt(token.at(0));
        height = StringUtility::stringToInt(token.at(1));
        break;
    }
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
        return BaseUnit::processParameter(value);

    return parsed;
}

///---private---
