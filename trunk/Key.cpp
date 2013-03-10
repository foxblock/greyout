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
    targetIDs.clear();
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

    if (targetIDs.empty())
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
        targetIDs.clear();
        vector<string> token;
        StringUtility::tokenize(value.second,token,DELIMIT_STRING);
        targetIDs.insert(targetIDs.begin(),token.begin(),token.end());
        break;
    }
    default:
        parsed = false;
    }

    return parsed;
}

void Key::update()
{
    if (!targetIDs.empty())
    {
        targets.clear();
        parent->getUnitsByID(targetIDs,targets);
        targetIDs.clear();
		for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
			(*I)->setSpriteState("closed");
    }
    BaseUnit::update();
}

void Key::reset()
{
    BaseUnit::reset();
    for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
        (*I)->setSpriteState("closed");
}

void Key::hitUnit(const UnitCollisionEntry& entry)
{
    if (entry.unit->isPlayer)
    {
        for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
            (*I)->setSpriteState("open");
        toBeRemoved = true;
    }
}

///---protected---

///---private---
