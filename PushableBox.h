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

        virtual inline int getHeight() const;
        virtual inline int getWidth() const;
        virtual Vector2df getPixel(const SimpleDirection& dir) const;

        virtual void updateScreenPosition(const Vector2di& offset);
        virtual void render(SDL_Surface* surf);

        virtual void hitUnit(const UnitCollisionEntry& entry);

        virtual void explode();

        SDL_Rect rect;
    protected:
        virtual bool processParameter(const pair<string,string>& value);

        virtual void move();

        enum BoxProp
        {
            bpSize=BaseUnit::upEOL,
            bpEOL
        };
    private:
};


#endif // PUSHABLEBOX_H
