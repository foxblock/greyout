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

bool SolidBox::hitUnitCheck(const BaseUnit* const caller) const
{
    return false;
}

void SolidBox::hitUnit(const UnitCollisionEntry& entry)
{
    // this moves a carried player unit when moving horizontally
    // this also only is called when the noUnitCollision flag is removed, use SolidPlatform for that purpose
    if (!flags.hasFlag(ufNoUpdate))
    {
        if (entry.overlap.x > entry.overlap.y && entry.unit->position.y < position.y && velocity.x != 0)
        {
            if (velocity.x != 0)
                entry.unit->collisionInfo.positionCorrection.x += velocity.x;
            if (velocity.y != 0)
                entry.unit->velocity.y -= entry.overlap.y - NumberUtility::sign(entry.overlap.y);
        }
    }
}

void SolidBox::move()
{
    BaseUnit::move();
}
