#ifndef BASEEFFECT_H
#define BASEEFFECT_H

#include "GFX.h"

#include "PenjinTypes.h"

enum EFFECT_TYPE
{
    etBase,
    etCustom,
    etFade,
    etZoom,
    etOverlay,
    etWipe
};

class BaseEffect
{
public:
    BaseEffect(){finished = false; type = etBase; limit = 0;};
    virtual ~BaseEffect(){};

    bool hasFinished() const {return finished;};

    EFFECT_TYPE getType() const {return type;};
    int getLimit() const {return limit;};

    // general
    virtual void update()=0;

    virtual void render()=0;

protected:
    bool finished;
    EFFECT_TYPE type;
    uint limit;
};

#endif // BASEEFFECT_H

