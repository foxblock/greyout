#ifndef FADING_BOX_H
#define FADING_BOX_H

#include "PushableBox.h"

class FadingBox : public PushableBox
{
public:
    FadingBox(Level* parent);
    virtual ~FadingBox();

    virtual bool processParameter(const PARAMETER_TYPE& value);

    virtual void update();

    virtual void hitUnit(const UnitCollisionEntry& entry);
    virtual bool checkCollisionColour(const Colour& col) const;

    #ifdef _DEBUG
    virtual string debugInfo();
    #endif
protected:
    void move();

    // first = player close, second = player distant colour
    pair<Colour,Colour> colours;
    // x = inner radius (full close colour), y = outer radius (full distant colour)
    Vector2df fadeRadius;
    int fadeSteps;

    enum FadingProp
    {
        fpFarColour=BaseUnit::upEOL,
        fpFadeRadius,
        fpFadeSteps,
        fpEOL
    };

private:

};


#endif // FADING_BOX_H

