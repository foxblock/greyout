#include "BaseTrigger.h"

#include "StringUtility.h"
#include "Level.h"

BaseTrigger::BaseTrigger(Level* newParent) : BaseUnit(newParent)
{
    stringToProp["size"] = bpSize;
    stringToProp["enabled"] = bpEnabled;
    stringToProp["target"] = bpTarget;
    stringToProp["action"] = bpAction;
    width = 32;
    height = 32;
    collisionColours.insert(Colour(BLACK).getIntColour());
    collisionColours.insert(Colour(WHITE).getIntColour());
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);
    flags.addFlag(ufNoUnitCollision);
    triggerCol = YELLOW;
    enabled = true;
    startingEnabled = true;
    targetParam.first = "";
    targetParam.second = "";
}

BaseTrigger::~BaseTrigger()
{
    //
}

///---public---

bool BaseTrigger::processParameter(const PARAMETER_TYPE& value)
{
    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case bpSize:
    {
        vector<string> token;
        StringUtility::tokenize(value.second,token,DELIMIT_STRING);
        if (token.size() != 2)
        {
            parsed = false;
            break;
        }
        width = StringUtility::stringToInt(token[0]);
        height = StringUtility::stringToInt(token[1]);
        break;
    }
    case bpEnabled:
    {
        enabled = StringUtility::stringToBool(value.second);
        startingEnabled = enabled;
        break;
    }
    case bpTarget:
    {
        vector<string> tokens;
        StringUtility::tokenize(value.second,tokens,DELIMIT_STRING);
        for (vector<BaseUnit*>::iterator I = parent->units.begin(); I != parent->units.end(); ++I)
        {
            for (vector<string>::iterator str = tokens.begin(); str != tokens.end(); ++str)
            {
                if ((*I)->id == (*str))
                    targets.push_back(*I);
            }
        }
        break;
    }
    case bpAction:
    {
        vector<string> tokens;
        StringUtility::tokenize(value.second,tokens,VALUE_STRING,2);
        if (tokens.size() < 2)
        {
            parsed = false;
            break;
        }
        targetParam.first = tokens.front();
        targetParam.second = tokens.back();
    }
    default:
        parsed = false;
    }

    if (not parsed)
        return BaseUnit::processParameter(value);

    return parsed;
}

void BaseTrigger::reset()
{
    enabled = startingEnabled;
    BaseUnit::reset();
}

void BaseTrigger::render(SDL_Surface* surf)
{
#ifdef _DEBUG
    SDL_Rect temp;
    temp.x = position.x;
    temp.y = position.y;
    temp.w = width;
    temp.h = 1;
    SDL_FillRect(surf,&temp,triggerCol.getSDL_Uint32Colour(surf));
    temp.y += height;
    SDL_FillRect(surf,&temp,triggerCol.getSDL_Uint32Colour(surf));
    temp.w = 1;
    temp.h = height;
    temp.y -= height;
    SDL_FillRect(surf,&temp,triggerCol.getSDL_Uint32Colour(surf));
    temp.x += width;
    SDL_FillRect(surf,&temp,triggerCol.getSDL_Uint32Colour(surf));
#else
    // Don't render anything
#endif
}

bool BaseTrigger::hitUnitCheck(const BaseUnit* const caller) const
{
    return false;
}

void BaseTrigger::hitUnit(const UnitCollisionEntry& entry)
{
    if (entry.unit->isPlayer && enabled)
    {
        doTrigger(entry);
        for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
        {
            (*I)->processParameter(targetParam);
            // orders need an additional kickstart to work
            if (targetParam.first == "order" && !(*I)->orderRunning)
                (*I)->resetOrder();
        }
    }
}


#ifdef _DEBUG
string BaseTrigger::debugInfo()
{
    string result = BaseUnit::debugInfo();
    if (enabled)
        result += "enabled\n";
    else
        result += "disabled\n";
    if (!targets.empty())
    {
        result += "T: ";
        for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
            result += (*I)->id + ", ";
        result += "\n";
    }
    if (targetParam.first[0] != 0)
        result += "A: " + targetParam.first + "=" + targetParam.second + "\n";
    return result;
}
#endif

///---protected---

void BaseTrigger::doTrigger(const UnitCollisionEntry& entry)
{
    enabled = false;
}

///---private---
