#include "Key.h"

#include "Level.h"

Key::Key(Level* newParent) : BaseUnit(newParent)
{
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);
    flags.addFlag(ufNoUnitCollision);

    col = Colour(50,217,54);
    startingColour = col;
    collisionColours.insert(Colour(BLACK).getIntColour());
    collisionColours.insert(Colour(WHITE).getIntColour());

    stringToProp["target"] = kpTarget;
}

Key::~Key()
{
    targets.clear();
}

///---public---

bool Key::load(const list<PARAMETER_TYPE >& params)
{
    bool result = BaseUnit::load(params);

    if (imageOverwrite[0] == 0)
    {
        imageOverwrite = "images/units/key.png";
    }
    else // clear sprites loaded by BaseUnit
    {
        for (map<string,AnimatedSprite*>::iterator I = states.begin(); I != states.end(); ++I)
        {
            delete I->second;
        }
        states.clear();
    }
    AnimatedSprite* temp = new AnimatedSprite;
    temp->loadFrames(getSurface(imageOverwrite),1,1,0,1);
    temp->setTransparentColour(MAGENTA);
    states["key"] = temp;
    startingState = "key";

    if (targets.empty())
        result = false;

    return result;
}

bool Key::processParameter(const PARAMETER_TYPE& value)
{
    if (BaseUnit::processParameter(value))
        return true;

    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case kpTarget:
    {
        vector<string> tokens;
        StringUtility::tokenize(value.second,tokens,DELIMIT_STRING);
        for (vector<BaseUnit*>::iterator I = parent->units.begin(); I != parent->units.end(); ++I)
        {
            for (vector<string>::iterator str = tokens.begin(); str != tokens.end(); ++str)
            {
                if ((*I)->id == (*str))
                {
                    targets.push_back(*I);
                    (*I)->setSpriteState("closed");
                }
            }
        }
        break;
    }
    default:
        parsed = false;
    }

    return parsed;
}

void Key::reset()
{
    BaseUnit::reset();
    for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
        (*I)->setSpriteState("closed");
}

bool Key::hitUnitCheck(const BaseUnit* const caller) const
{
    return false;
}

void Key::hitUnit(const UnitCollisionEntry& entry)
{
    // standing still on the ground
    if (entry.unit->isPlayer)
    {
        for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
            (*I)->setSpriteState("open");
        toBeRemoved = true;
    }
}

///---protected---

///---private---
