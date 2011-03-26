#ifndef EFFECTOVERLAY_H
#define EFFECTOVERLAY_H

#include "BaseEffect.h"

#include "Colour.h"
#include "Rectangle.h"

class EffectOverlay : public BaseEffect
{
public:
    EffectOverlay(const Colour& col=BLACK);
    virtual ~EffectOverlay();

    // general
    virtual void update();

    virtual void render();

private:
    Rectangle rect;
};

#endif // EFFECTOVERLAY_H
