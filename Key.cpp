#include "Key.h"

#include "Level.h"

Key::Key(Level* newParent) : BaseUnit(newParent)
{
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);
    unitCollisionMode = 0;

    col = Colour(50,217,54);
    collisionColours.insert(Colour(BLACK).getIntColour());
    collisionColours.insert(Colour(WHITE).getIntColour());

}

Key::~Key()
{
    targets.clear();
}

///---public---

bool Key::load(list<PARAMETER_TYPE >& params)
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

    if (startingState[0] == 0 || startingState == "default")
        startingState = "key";
    setSpriteState(startingState,true);

    if (targets.empty())
    {
        printf("ERROR: Key \"%s\" without a target exit!\n",id.c_str());
        result = false;
    }

    return result;
}

bool Key::processParameter(const PARAMETER_TYPE& value)
{
    if (BaseUnit::processParameter(value))
        return true;

    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case BaseUnit::upTarget:
    {
        targets.clear();
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
