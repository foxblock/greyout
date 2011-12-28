#include "BaseTrigger.h"

#include "StringUtility.h"

BaseTrigger::BaseTrigger(Level* newParent) : BaseUnit(newParent)
{
    stringToProp["size"] = bpSize;
    width = 32;
    height = 32;
    collisionColours.insert(Colour(BLACK).getIntColour());
    collisionColours.insert(Colour(WHITE).getIntColour());
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);
    flags.addFlag(ufNoUnitCollision);
    triggerCol = YELLOW;
}

BaseTrigger::~BaseTrigger()
{
    //
}

///---public---

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
    if (entry.unit->isPlayer)
    {
        doTrigger(entry);
    }
}

///---protected---

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
        width = StringUtility::stringToInt(token.at(0));
        height = StringUtility::stringToInt(token.at(1));
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
