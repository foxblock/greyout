#include "Hollywood.h"
#include "GFX.h"

Hollywood* Hollywood::m_self = NULL;

Hollywood::Hollywood()
{

}

Hollywood::~Hollywood()
{
    clear();
}

Hollywood* Hollywood::GetSingleton()
{
    if (not m_self)
        m_self = new Hollywood();
    return m_self;
}

/// general

void Hollywood::update()
{
    vector<BaseEffect*>::iterator I;
    for (I = effects.begin(); I < effects.end();)
    {
        if ((*I)->hasFinished())
        {
            delete *I;
            I = effects.erase(I);
        }
        else
        {
            (*I)->update();
            ++I;
        }
    }
}


void Hollywood::render()
{
    vector<BaseEffect*>::iterator I;
    for (I = effects.begin(); I < effects.end(); ++I)
    {
        (*I)->render();
    }
}

bool Hollywood::hasFinished(CRint index)
{
    if (index < 0)
    {
        vector<BaseEffect*>::iterator I;
        for (I = effects.begin(); I < effects.end();++I)
        {
            if (not (*I)->hasFinished())
                return false;
        }
        return true;
    }
    else if (index < effects.size())
        return effects[index]->hasFinished();
    else
        return false;
}

/// fading

void Hollywood::fadeIn(CRint time, const Colour& col)
{
    checkEffectLimit(etFade);
    EffectFade* temp = new EffectFade(time,col);
    effects.push_back(temp);
}

void Hollywood::fadeOut(CRint time, const Colour& col)
{
    checkEffectLimit(etFade);
    EffectFade* temp = new EffectFade(-time,col);
    effects.push_back(temp);
}

/// zooming

void Hollywood::zoomIn(CRint time, const Vector2df& pos, const Vector2df& size, const Colour& col, CRbool inverted)
{
    checkEffectLimit(etZoom);
    EffectZoom* temp = new EffectZoom(time,pos,size,col,inverted);
    effects.push_back(temp);
}

void Hollywood::zoomOut(CRint time, const Vector2df& pos, const Vector2df& size, const Colour& col, CRbool inverted)
{
    checkEffectLimit(etZoom);
    EffectZoom* temp = new EffectZoom(time * -1,pos,size,col,inverted);
    effects.push_back(temp);
}

/// overlay

void Hollywood::colourOverlay(const Colour& col)
{
    checkEffectLimit(etOverlay);
    EffectOverlay* temp = new EffectOverlay(col);
    effects.push_back(temp);
}

void Hollywood::removeOverlay()
{
    vector<BaseEffect*>::iterator I;
    for (I = effects.begin(); I < effects.end(); ++I)
    {
        if ((*I)->getType() == etOverlay)
        {
            delete *I;
            effects.erase(I);
            return;
        }
    }
}

/// wiping

void Hollywood::wipe(CRint time, const SimpleDirection& startingDirection, const Colour& wipeColour, CRbool inverted)
{
    checkEffectLimit(etWipe);
    EffectWipe* temp = new EffectWipe(time,startingDirection,wipeColour,inverted);
    effects.push_back(temp);
}

/// custom

void Hollywood::addCustomEffect(BaseEffect* eff)
{
    effects.push_back(eff);
}

/*
/// rain
void Hollywood::startRain(CRchar density, const Vector2df direction, CRbool fade)
{
    EffectRain* temp = new EffectRain(density, direction, fade);
    effects.push_back(temp);
}*/

void Hollywood::clear()
{
    vector<BaseEffect*>::iterator I;
    for (I = effects.begin(); I < effects.end(); ++I)
    {
        delete *I;
    }
    effects.clear();
}

/// private

void Hollywood::checkEffectLimit(EFFECT_TYPE checkType)
{
    vector<BaseEffect*>::iterator I;
    vector<BaseEffect*>::iterator first = effects.end();
    int count = 1;
    for (I = effects.begin(); I < effects.end(); ++I)
    {
        if ((*I)->getType() == checkType)
        {
            if ((*I)->getLimit() == 0) // no limit
                return;

            if (count == 1)
                first = I;
            if (count >= (*first)->getLimit())
            {
                delete *first;
                effects.erase(first);
                return;
            }
            ++count;
        }
    }
}
