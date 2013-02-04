#include "BaseTrigger.h"

#include "StringUtility.h"
#include "Level.h"
#include "ControlUnit.h"

BaseTrigger::BaseTrigger(Level* newParent) : BaseUnit(newParent)
{
    stringToProp["size"] = BaseUnit::upSize;
    stringToProp["enabled"] = bpEnabled;
    stringToProp["action"] = bpAction;
    stringToProp["activator"] = bpActivator;
    width = 32;
    height = 32;
    collisionColours.insert(Colour(BLACK).getIntColour());
    collisionColours.insert(Colour(WHITE).getIntColour());
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);
    flags.addFlag(ufInvincible);
    unitCollisionMode = 0;
    triggerCol = YELLOW;
    enabled = true;
    targetParam.first = "";
    targetParam.second = "";
}

BaseTrigger::~BaseTrigger()
{
    targets.clear();
    activators.clear();
    targetIDs.clear();
    activatorIDs.clear();
}

///---public---

bool BaseTrigger::processParameter(const PARAMETER_TYPE& value)
{
    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case BaseUnit::upSize:
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
        break;
    }
    case BaseUnit::upTarget:
    {
        targetIDs.clear();
        vector<string> tokens;
        StringUtility::tokenize(value.second,tokens,DELIMIT_STRING);
        targetIDs.insert(targetIDs.begin(),tokens.begin(),tokens.end());
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
        break;
    }
    case bpActivator:
    {
        activatorIDs.clear();
        vector<string> tokens;
        StringUtility::tokenize(value.second,tokens,DELIMIT_STRING);
        activatorIDs.insert(activatorIDs.begin(),tokens.begin(),tokens.end());
        break;
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
    enabled = true;
    width = 32;
    height = 32;
    BaseUnit::reset();
}

void BaseTrigger::update()
{
    if (!targetIDs.empty())
    {
        targets.clear();
        parent->getUnitsByID(targetIDs,targets);
        targetIDs.clear();
    }
    if (!activatorIDs.empty())
    {
        activators.clear();
        parent->getUnitsByID(activatorIDs,activators);
        activatorIDs.clear();
    }
    BaseUnit::update();
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

void BaseTrigger::hitUnit(const UnitCollisionEntry& entry)
{
    if (enabled)
    {
        if (!activators.empty())
        {
            bool found = false;
            for (vector<BaseUnit*>::iterator I = activators.begin(); I != activators.end(); ++I)
            {
                if (*I == entry.unit)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                return; // not hit by activator unit, exit here
        }
        // activator units may be any explicitly named unit or any player (by default)
        if ((!activators.empty() || entry.unit->isPlayer))
        {
            doTrigger(entry);
            if (targetParam.first[0] != 0)
            {
                for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
                {
                    if (targetParam.first == "order")
                        (*I)->resetOrder(true); // clear order list
                    (*I)->processParameter(targetParam);
                    // orders need an additional kickstart to work
                    if (targetParam.first == "order")
                        (*I)->resetOrder(false);
                }
            }
        }
    }
}

bool BaseTrigger::checkCollisionColour(const Colour& col) const
{
    set<int>::const_iterator iter = collisionColours.find(col.getIntColour());
    if (iter != collisionColours.end())
        return true;
    return false;
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
    if (!activators.empty())
    {
        result += "U: ";
        for (vector<BaseUnit*>::iterator I = activators.begin(); I != activators.end(); ++I)
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
