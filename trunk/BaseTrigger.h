#ifndef BASE_TRIGGER_H
#define BASE_TRIGGER_H

#include "BaseUnit.h"

class BaseTrigger : public BaseUnit
{
public:
    BaseTrigger(Level* newParent);
    virtual ~BaseTrigger();

    virtual void render(SDL_Surface* surf);

    virtual bool hitUnitCheck(const BaseUnit* const caller) const;
    virtual void hitUnit(const UnitCollisionEntry& entry);

    int getWidth() const {return width;}
    int getHeight() const {return height;}

protected:
    virtual void doTrigger(const UnitCollisionEntry& entry)=0;

    virtual bool processParameter(const PARAMETER_TYPE& value);

    int width;
    int height;
    Colour triggerCol;

    enum BaseTriggerProps
    {
        bpSize=BaseUnit::upEOL,
        bpEOL
    };
private:

};


#endif // BASE_TRIGGER_H

