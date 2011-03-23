#ifndef BASEUNITCONTAINER_H
#define BASEUNITCONTAINER_H

/// TODO: Re-think this class to solely use it to link two objects together so
/// that move the one will also move the other (share collision and input info)

#include "BaseUnit.h"

class BaseUnitContainer
{
    public:
        BaseUnitContainer();
        virtual ~BaseUnitContainer();

        virtual void update();
        virtual void updateScreenPosition(const Vector2df& offset);
        virtual void render() {render(GFX::getVideoSurface());}
        virtual void render(SDL_Surface* surf);

        virtual void handleMapCollision(SDL_Surface* const level, const Vector2df& mapOffset = Vector2df(0,0));
        virtual void handleUnitCollision();

        vector<BaseUnit*> units;
    protected:

    private:

};


#endif // BASEUNITCONTAINER_H

