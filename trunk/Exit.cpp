#include "Exit.h"

#include "Level.h"

Exit::Exit(Level* newParent) : BaseUnit(newParent)
{
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);
    flags.addFlag(ufNoCollisionDraw);

    col = Colour(50,217,54);
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
    unit->toBeRemoved = true;
    parent->swapControl();
    parent->winCounter--;
}

///---protected---

///---private---
