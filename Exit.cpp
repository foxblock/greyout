#include "Exit.h"

#include "Level.h"

Exit::Exit(Level* newParent) : BaseUnit(newParent)
{
    stringToProp["link"] = epLink;

    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);
    unitCollisionMode = 0;

    col = Colour(50,217,54);
    collisionColours.insert(Colour(BLACK).getIntColour());
    collisionColours.insert(Colour(WHITE).getIntColour());

    isExiting = false;
    allExited = false;
}

Exit::~Exit()
{
    targets.clear();
    targetIDs.clear();
}

///---public---

bool Exit::load(list<PARAMETER_TYPE >& params)
{
    bool result = BaseUnit::load(params);

    if (imageOverwrite[0] == 0)
    {
        imageOverwrite = "images/units/exit.png";
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
    temp->loadFrames(getSurface(imageOverwrite),2,1,0,1);
    temp->setTransparentColour(MAGENTA);
    states["open"] = temp;
    temp = new AnimatedSprite;
    temp->loadFrames(getSurface(imageOverwrite),2,1,1,1);
    temp->setTransparentColour(MAGENTA);
    states["closed"] = temp;

    if (startingState[0] == 0 || startingState == "default")
        startingState = "open";
    setSpriteState(startingState,true);

    return result;
}

void Exit::hitUnit(const UnitCollisionEntry& entry)
{
    if (currentState == "open")
    {
        // standing still on the ground
        if (entry.unit->isPlayer && (int)entry.unit->velocity.x == 0 &&
            abs(entry.unit->velocity.y) < 3 && entry.unit->collisionInfo.correction.y != 0 &&
            entry.overlap.x > 10 && entry.overlap.y > 20)
        {
        	isExiting = true;
        	if (allExited || targets.empty())
			{
				entry.unit->toBeRemoved = true;
				parent->swapControl();
				if (entry.unit->flags.hasFlag(ufMissionObjective))
					parent->winCounter--;
			}
			if (!targets.empty() && checkAllExited())
			{
				for	(vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
					if ((*I)->tag == "EXIT")
						((Exit*)(*I))->allExited = true;
				allExited = true;
			}
        }
    }
}

bool Exit::processParameter(const PARAMETER_TYPE& value)
{
    if (BaseUnit::processParameter(value))
        return true;

    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case epLink:
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

void Exit::update()
{
    if (!targetIDs.empty())
    {
        targets.clear();
        parent->getUnitsByID(targetIDs,targets);
        targetIDs.clear();
    }
    isExiting = false;
    BaseUnit::update();
}

///---protected---

bool Exit::checkAllExited() const
{
	for	(vector<BaseUnit*>::const_iterator I = targets.begin(); I != targets.end(); ++I)
	{
		if ((*I)->tag == "EXIT")
			if (!((Exit*)(*I))->isExiting)
				return false;
	}
	return true;
}

///---private---
