#ifndef GEAR_H
#define GEAR_H

#include "BaseUnit.h"

class Gear : public BaseUnit
{
public:
    Gear(Level* newParent);
    virtual ~Gear();

    virtual bool load(const list<PARAMETER_TYPE >& params);

    virtual inline int getHeight() const;
    virtual inline int getWidth() const;

    virtual void update();
    virtual void updateScreenPosition(const Vector2di& offset);
    virtual void render(SDL_Surface* surf);
protected:
    virtual bool processParameter(const PARAMETER_TYPE& value);
    virtual bool processOrder(Order& next);

    enum GearProp
    {
        gpSpeed=BaseUnit::upEOL,
        gpRotation,
        gpEOL
    };
    enum GearOder
    {
        goRotation=BaseUnit::okEOL,
        goEOL
    };

    float speed;
    float angle;
    Image img;
    Vector2df screenPosition;
private:

};


#endif // GEAR_H

