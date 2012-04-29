#ifndef CAMERA_H
#define CAMERA_H

#include "PenjinTypes.h"
#include "CountDown.h"
#include "Vector2df.h"

class BaseUnit;
class Level;

/**
Provides functions to move the rendered area around
**/

class Camera
{
    public:
        Camera();
        virtual ~Camera();

        void update();
        void reset();

        // If time is set to 0 the move will be instant
        void centerOnUnit(const BaseUnit* const unit, CRint time=0);
        void centerOnPos(const Vector2df& pos, CRint time=0);

        Vector2df getSpeed() const {return speed;}
        Vector2df getDest() const {return dest;}

        Level* parent;
        // If false level boundaries will be taken into account when moving
        // the "camera" so space outside the level is never shown
        bool disregardBoundaries;
    protected:
        Vector2df speed;
        Vector2df dest;
    private:

};


#endif // CAMERA_H

