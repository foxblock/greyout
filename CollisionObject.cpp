#include "CollisionObject.h"

#include <map>

#define DEFAULT_THRESHOLD 0

CollisionObject::CollisionObject()
{
    correction = Vector2df(0.0f,0.0f);
    positionCorrection = Vector2df(0.0f,0.0f);
    squashCounter = 0;
    squashThreshold = DEFAULT_THRESHOLD;
}

CollisionObject::~CollisionObject()
{
    clear();
}

bool CollisionObject::isBeingSquashed() const
{
    map<int,bool> table;
    for (vector<CollisionEntry>::const_iterator iter = entries.begin(); iter != entries.end(); ++iter)
    {
        table[iter->dir.value] = true;
    }
    if (table[diLEFT] && table[diRIGHT])
        return true;
    if (table[diTOP] && table[diBOTTOM])
        return true;
    if (table[diTOPLEFT] && table[diTOPRIGHT] && not table[diTOP])
        return true;
    return false;
}

bool CollisionObject::isHealthy(const Vector2df& vel)
{
    // red = death
    for (vector<CollisionEntry>::const_iterator iter = entries.begin(); iter != entries.end(); ++iter)
    {
        if (iter->col == Colour(RED))
            return false;
    }

    if (isBeingSquashed())
        ++squashCounter;
    else
        squashCounter = 0;
    if (abs(vel.x) <= 16)
    {
        if (abs(correction.x) > 16)
            ++squashCounter;
    }
    else if (abs(correction.x) > abs(vel.x))
        ++squashCounter;

    if (squashCounter > squashThreshold)
        return false;

    return true;
}
