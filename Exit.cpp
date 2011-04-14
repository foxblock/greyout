#include "Exit.h"

#include "Level.h"

Exit::Exit(Level* newParent) : BaseUnit(newParent)
{
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);

    col = Colour(50,217,54);
    collisionColours.push_back(WHITE);
    collisionColours.push_back(BLACK);
}

Exit::~Exit()
{
    //
}

///---public---

bool Exit::load(const PARAMETER_TYPE& params)
{
    bool result = BaseUnit::load(params);

    if (imageOverwrite[0] == 0)
    {
        imageOverwrite = "images/units/exit.png";
    }
    AnimatedSprite* temp = new AnimatedSprite;
    temp->loadFrames(getSurface(imageOverwrite),2,1,0,1);
    temp->setTransparentColour(MAGENTA);
    currentSprite = temp;
    states["open"] = temp;
    temp = new AnimatedSprite;
    temp->loadFrames(getSurface(imageOverwrite),2,1,1,1);
    temp->setTransparentColour(MAGENTA);
    states["close"] = temp;

    return result;
}

void Exit::hitUnit(const UNIT_COLLISION_DATA_TYPE& collision, BaseUnit* const unit)
{
    // standing still on the ground
    if ((int)unit->velocity.x == 0 && (int)(unit->velocity.y + unit->collisionInfo.correction.y) == 0
        && unit->collisionInfo.correction.y < 0 && collision.second.x > 10)
    {
        unit->toBeRemoved = true;
        parent->swapControl();
        parent->winCounter--;
    }
}

///---protected---

///---private---
