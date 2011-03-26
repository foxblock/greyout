#include "EffectOverlay.h"

EffectOverlay::EffectOverlay(const Colour& col) : BaseEffect()
{
    rect.setPosition(0,0);
    rect.setDimensions(GFX::getXResolution(),GFX::getYResolution());
    rect.setColour(col);

    type = etOverlay;
    limit = 1;
}

EffectOverlay::~EffectOverlay()
{

}

void EffectOverlay::update()
{
    //
}

void EffectOverlay::render()
{
    rect.render();
}
