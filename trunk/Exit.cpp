#include "Exit.h"

#include "Level.h"

Exit::Exit(Level* newParent) : BaseUnit(newParent)
{
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);
    flags.addFlag(ufNoUnitCollision);

    col = Colour(50,217,54);
    collisionColours.insert(Colour(BLACK).getIntColour());
    collisionColours.insert(Colour(WHITE).getIntColour());
}

Exit::~Exit()
{
    //
}

///---public---

bool Exit::load(const list<PARAMETER_TYPE >& params)
{
    bool result = BaseUnit::load(params);

    if (imageOverwrite[0] == 0)
    {
        imageOverwrite = "images/units/exit.png";
    }
    AnimatedSprite* temp = new AnimatedSprite;
    temp->loadFrames(getSurface(imageOverwrite),2,1,0,1);
    temp->setTransparentColour(MAGENTA);
    states["open"] = temp;
    temp = new AnimatedSprite;
    temp->loadFrames(getSurface(imageOverwrite),2,1,1,1);
    temp->setTransparentColour(MAGENTA);
    states["close"] = temp;
    startingState = "open";

    return result;
}

bool Exit::hitUnitCheck(const BaseUnit* const caller) const
{
    return false;
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
            entry.unit->toBeRemoved = true;
            parent->swapControl();
            if (entry.unit->flags.hasFlag(ufMissionObjective))
                parent->winCounter--;
        }
    }
}

///---protected---

///---private---
