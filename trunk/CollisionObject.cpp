#include "CollisionObject.h"

CollisionObject::CollisionObject()
{
    correction = Vector2df(0.0f,0.0f);
    positionCorrection = Vector2df(0.0f,0.0f);
}

CollisionObject::~CollisionObject()
{
    clear();
}

bool CollisionObject::isBeingSquashed() const
{
    int xDir = 0;
    int yDir = 0;
    for (vector<CollisionEntry>::const_iterator iter = entries.begin(); iter != entries.end(); ++iter)
    {
        switch (iter->dir.value)
        {
        case diLEFT:
            ++xDir;
            break;
        case diTOP:
            ++yDir;
            break;
        case diRIGHT:
            ++xDir;
            break;
        case diBOTTOM:
            ++yDir;
            break;
        default:
            break;
        }
    }
    if (xDir >= 2)
        return true;
    if (yDir >= 2)
        return true;
    return false;
}
