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
        CollisionObject();
        virtual ~CollisionObject();

        virtual void clear() {entries.clear();}

        /// Analysing functions
        bool isBeingSquashed() const;

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

