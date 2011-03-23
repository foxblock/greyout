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
        PushableBox(Level* newParent, CRint newWidth, CRint newHeight, const Colour& newCol);
        virtual ~PushableBox();

        virtual bool load(const PARAMETER_TYPE& params);

        virtual int getHeight() const;
        virtual int getWidth() const;

        virtual void updateScreenPosition(Vector2di offset);
        virtual void render(SDL_Surface* surf);

        virtual void hitUnit(const UNIT_COLLISION_DATA_TYPE& collision, BaseUnit* const unit);

        void setRectangle();

        int width;
        int height;
        Rectangle rect;
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
