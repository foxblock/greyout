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

struct CollisionEntry
{
    SimpleDirection dir;
    Vector2df pixel;
    Colour col;
    Vector2df correction; // currently unused
};

class CollisionObject
{
    public:
        CollisionObject() {correction = Vector2df(0.0f,0.0f); positionCorrection = Vector2df(0.0f,0.0f);}
        virtual ~CollisionObject() {clear();}

        virtual void clear() {entries.clear();}

        /// Analysing functions
        /// TODO: Implement this, check for colliding pixels on opposite directions
        bool isBeingSquashed() const {};

        // Contains the checked pixels
        vector<CollisionEntry> entries;
        // Total velocity correction, calculated by Physics
        Vector2df correction;
        // Total position correction - this correction will only affect the next
        // cycle directly and is usually caused by vertical/slope correction to
        // avoid rocketing the player in the air
        Vector2df positionCorrection;
    protected:

    private:
};



#endif // COLLISIONOBJECT_H

