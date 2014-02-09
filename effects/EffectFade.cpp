#include "EffectFade.h"

EffectFade::EffectFade(CRint duration, const Colour& col) : BaseEffect()
{
    timer = abs(duration);
    rect.setPosition(0,0);
    rect.setDimensions(GFX::getXResolution(),GFX::getYResolution());
    rect.setColour(col);
    if (duration < 0) // fade out to colour
        rect.setAlpha(0);
    fadeTime = duration;
    type = etFade;
    limit = 1;
}

EffectFade::~EffectFade()
{
    //
}

void EffectFade::update()
{
    if (timer > 0)
    {
        if (fadeTime < 0) // fade out
        {
            rect.setAlpha(255 + round((float)timer / (float)fadeTime * 255.0f));
        }
        else if (fadeTime > 0) // fade in
        {
            rect.setAlpha(round((float)timer / (float)fadeTime * 255.0f));
        }
		--timer;
    }
    else
        finished = true;
}

void EffectFade::render()
{
    rect.render();
}
