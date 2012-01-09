#ifndef PUSHABLEBOX_H
#define PUSHABLEBOX_H

#include "Rectangle.h"

#include "BaseUnit.h"

/**
A pushable (by any player) box
Overrides some of BaseUnit's functionality as it uses a Penjin::Rectangle to
draw itself instead of a sprite
Adds a size property and shows how to extend loading in the processParameter and
load functions
**/

class PushableBox : public BaseUnit
{
    public:
        PushableBox(Level* newParent);
        virtual ~PushableBox();

        virtual bool processParameter(const PARAMETER_TYPE& value);
        virtual void reset();

        virtual int getHeight() const;
        virtual int getWidth() const;
        virtual Vector2df getPixel(const SimpleDirection& dir) const;

        virtual void updateScreenPosition(const Vector2di& offset);
        virtual void render(SDL_Surface* surf);

        virtual void hitUnit(const UnitCollisionEntry& entry);

        virtual void explode();

        Vector2di startingSize;

        SDL_Rect rect;
    protected:
        virtual bool processOrder(Order& next);
        virtual bool updateOrder(const Order& curr);

        virtual void move();

        Vector2di sizeTimer;

        enum BoxProp
        {
            bpSize=BaseUnit::upEOL,
            bpEOL
        };
        enum BoxOrder
        {
            boSize=BaseUnit::okEOL,
            boEOL
        };
    private:
};


#endif // PUSHABLEBOX_H
