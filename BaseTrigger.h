#ifndef BASE_TRIGGER_H
#define BASE_TRIGGER_H

#include "BaseUnit.h"

class BaseTrigger : public BaseUnit
{
public:
    BaseTrigger(Level* newParent);
    virtual ~BaseTrigger();

    virtual bool processParameter(const PARAMETER_TYPE& value);
    virtual void reset();

    virtual void render(SDL_Surface* surf);

    virtual bool hitUnitCheck(const BaseUnit* const caller) const;
    virtual void hitUnit(const UnitCollisionEntry& entry);

    int getWidth() const {return width;}
    int getHeight() const {return height;}

    #ifdef _DEBUG
    virtual string debugInfo();
    #endif

    bool enabled;
    bool startingEnabled;

protected:
    virtual void doTrigger(const UnitCollisionEntry& entry);

    int width;
    int height;
    Colour triggerCol;
    vector<BaseUnit*> targets;
    PARAMETER_TYPE targetParam;

    enum BaseTriggerProps
    {
        bpSize=BaseUnit::upEOL,
        bpEnabled,
        bpTarget,
        bpAction,
        bpEOL
    };
private:

};


#endif // BASE_TRIGGER_H

