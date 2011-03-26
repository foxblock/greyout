#ifndef EFFECTFADE_H
#define EFFECTFADE_H

#include "BaseEffect.h"

#include "Colour.h"
#include "Rectangle.h"
#include "CountDown.h"

class EffectFade : public BaseEffect
{
public:
    EffectFade(CRint duration, const Colour& col=BLACK);
    virtual ~EffectFade();

    // general
    virtual void update();

    virtual void render();

private:
    CountDown timer;
    Rectangle rect;
    int fadeTime;
};

#endif // EFFECTFADE_H


