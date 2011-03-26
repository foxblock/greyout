#ifndef EFFECTWIPE_H
#define EFFECTWIPE_H

#include "BaseEffect.h"

#include "Colour.h"
#include "Rectangle.h"
#include "SimpleDirection.h"
#include "CountDown.h"

class EffectWipe : public BaseEffect
{
public:
    EffectWipe(CRint time, const SimpleDirection& startingDirection, const Colour& wipeColour=BLACK, CRbool inverted = false);
    virtual ~EffectWipe();

    // general
    virtual void update();

    virtual void render();

private:
    Rectangle rect;
    SimpleDirection direction;
    CountDown timer;
    bool isInverted;
};

#endif // EFFECTWIPE_H


