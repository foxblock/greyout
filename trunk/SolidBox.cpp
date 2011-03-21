#include "SolidBox.h"

SolidBox::SolidBox(Level* newParent) : PushableBox(newParent)
{
    flags.addFlag(ufNoGravity);
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoUnitCollision);
}

SolidBox::~SolidBox()
{
    //
}

void SolidBox::hitUnit(const UNIT_COLLISION_DATA_TYPE& collision, BaseUnit* const unit)
{
    // on collision: just sit there, don't even think about moving!
}

void SolidBox::move()
{
    BaseUnit::move();
}
