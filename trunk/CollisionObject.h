#ifndef COLLISIONOBJECT_H
#define COLLISIONOBJECT_H

#include <vector>

#include "SimpleDirection.h"
#include "Vector2df.h"
#include "Colour.h"

/**
Contains information about the last collision check, such as the collising pixels, correction, etc.
Also provides functions for analysing said data
**/

struct MapCollisionEntry
{
    SimpleDirection dir;
    Vector2df pos;
    Colour col;
    Vector2df correction; // currently unused
};

class BaseUnit;

struct UnitCollisionEntry
{
    SimpleDirection dir;
    Vector2df overlap;
    BaseUnit* unit;
};

class CollisionObject
{
    public:
        CollisionObject();
        virtual ~CollisionObject();

        virtual void clear();

        /// Analysing functions
        bool isBeingSquashed() const;
        bool isHealthy(const Vector2df& vel);

        // Contains the checked pixels
        vector<MapCollisionEntry> pixels;
        // Total velocity correction, calculated by Physics
        Vector2df correction;
        // Total position correction - this correction will only affect the next
        // cycle directly and is usually caused by vertical/slope correction to
        // avoid rocketing the player in the air
        Vector2df positionCorrection;

        // count the ticks in which the unit is being squashed here
        // if that value reaches a certain value the unit will explode
        int squashCounter;
        int squashThreshold;

        // Hold all units with which the unit collided
        vector<UnitCollisionEntry> units;
    protected:

    private:
};



#endif // COLLISIONOBJECT_H

