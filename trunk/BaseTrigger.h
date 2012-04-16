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

    virtual void hitUnit(const UnitCollisionEntry& entry);
    virtual bool checkCollisionColour(const Colour& col) const;

    int getWidth() const {return width;}
    int getHeight() const {return height;}

    #ifdef _DEBUG
    virtual string debugInfo();
    #endif

    bool enabled;

protected:
    virtual void doTrigger(const UnitCollisionEntry& entry);

    int width;
    int height;
    Colour triggerCol;
    vector<BaseUnit*> targets;
    vector<BaseUnit*> activators;
    PARAMETER_TYPE targetParam;

    enum BaseTriggerProps
    {
        bpEnabled=BaseUnit::upEOL,
        bpAction,
        bpActivator,
        bpEOL
    };
private:

};


#endif // BASE_TRIGGER_H

