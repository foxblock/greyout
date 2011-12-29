#ifndef BASE_TRIGGER_H
#define BASE_TRIGGER_H

#include "BaseUnit.h"

class BaseTrigger : public BaseUnit
{
public:
    BaseTrigger(Level* newParent);
    virtual ~BaseTrigger();

    virtual void reset();

    virtual void render(SDL_Surface* surf);

    virtual bool hitUnitCheck(const BaseUnit* const caller) const;
    virtual void hitUnit(const UnitCollisionEntry& entry);

    int getWidth() const {return width;}
    int getHeight() const {return height;}

    bool enabled;
    bool startingEnabled;

protected:
    virtual void doTrigger(const UnitCollisionEntry& entry);

    virtual bool processParameter(const PARAMETER_TYPE& value);

    int width;
    int height;
    Colour triggerCol;
    vector<BaseTrigger*> targets;

    enum BaseTriggerProps
    {
        bpSize=BaseUnit::upEOL,
        bpEnabled,
        bpTarget,
        bpEOL
    };
private:

};


#endif // BASE_TRIGGER_H

