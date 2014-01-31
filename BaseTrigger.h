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

    virtual void update();
    virtual void render(SDL_Surface* surf);

    virtual void hitUnit(const UnitCollisionEntry& entry);
    virtual bool checkCollisionColour(const Colour& col) const;

    int getWidth() const {return size.x;}
    int getHeight() const {return size.y;}

    #ifdef _DEBUG
    virtual string debugInfo();
    #endif

    bool enabled;

protected:
    virtual void doTrigger(const UnitCollisionEntry& entry);

    Vector2di size;
    Colour triggerCol;
    vector<BaseUnit*> targets;
    vector<string> targetIDs;
    vector<BaseUnit*> activators;
    vector<string> activatorIDs;
    PARAMETER_TYPE targetParam;

    bool autoReEnable;
    int enableTimer;
    bool actionHitTarget; // if true action can be performed on the hitUnit entry if targets is empty

    enum BaseTriggerProps
    {
        bpEnabled=BaseUnit::upEOL,
        bpAction,
        bpActivator,
        bpAutoReEnable,
        bpEOL
    };
private:

};


#endif // BASE_TRIGGER_H

