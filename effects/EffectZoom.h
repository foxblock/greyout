#ifndef EFFECTZOOM_H
#define EFFECTZOOM_H

#include "BaseEffect.h"

#include "Vector2df.h"

#include "Colour.h"
#include "Rectangle.h"
#include <SDL/SDL.h>

class EffectZoom : public BaseEffect
{
public:
    EffectZoom(CRint time, const Vector2df& pos, const Vector2df& newSize=Vector2df(1,1), const Colour& col=BLACK, CRbool inverted = false);
    virtual ~EffectZoom();

    // general
    virtual void update();

    virtual void render();

private:
    SDL_Surface* surf;
    Rectangle rect;
    Colour zoomCol;
    Vector2df position;
    Vector2df size;
    int timer;
    int zoomTime;
};

#endif // EFFECTZOOM_H


