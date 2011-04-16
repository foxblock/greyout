#include "SolidBox.h"

SolidBox::SolidBox(Level* newParent) : PushableBox(newParent)
{
    flags.addFlag(ufNoGravity);
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoUnitCollision);
    flags.addFlag(ufInvincible);
}

SolidBox::~SolidBox()
{
    //
}

void SolidBox::hitUnit(const UNIT_COLLISION_DATA_TYPE& collision, BaseUnit* const unit)
{
    // this moves a carried player unit when moving horizontally
    // this also only is called when the noUnitCollision flag is removed, use SolidPlatform for that purpose
    if (collision.second.x > collision.second.y && unit->position.y < position.y && velocity.x != 0)
    {
        unit->position.x += velocity.x;
    }
    if (collision.second.x > collision.second.y && velocity.y != 0)
    {
        unit->velocity.y -= collision.second.y;
    }
}

void SolidBox::move()
{
    BaseUnit::move();
}
