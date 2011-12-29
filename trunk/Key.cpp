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

    target = NULL;

    stringToProp["target"] = kpTarget;
}

Key::~Key()
{
    //
}

///---public---

bool Key::load(const list<PARAMETER_TYPE >& params)
{
    bool result = BaseUnit::load(params);

    if (imageOverwrite[0] == 0)
    {
        imageOverwrite = "images/units/key.png";
    }
    AnimatedSprite* temp = new AnimatedSprite;
    temp->loadFrames(getSurface(imageOverwrite),1,1,0,1);
    temp->setTransparentColour(MAGENTA);
    states["key"] = temp;
    startingState = "key";

    if (target == NULL)
        result = false;

    return result;
}

void Key::reset()
{
    BaseUnit::reset();
    target->setSpriteState("close");
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
        target->setSpriteState("open");
        toBeRemoved = true;
    }
}

///---protected---

bool Key::processParameter(const PARAMETER_TYPE& value)
{
    if (BaseUnit::processParameter(value))
        return true;

    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case kpTarget:
    {
        for (vector<BaseUnit*>::iterator I = parent->units.begin(); I != parent->units.end(); ++I)
        {
            if ((*I)->id == value.second)
                target = *I;
        }
        break;
    }
    default:
        parsed = false;
    }

    return parsed;
}

///---private---
