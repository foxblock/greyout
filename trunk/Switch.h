#ifndef SWITCH_H
#define SWITCH_H

#include "BaseUnit.h"

class Switch : public BaseUnit
{
public:
    Switch(Level* newParent);
    virtual ~Switch();

    virtual bool load(const list<PARAMETER_TYPE >& params);
    virtual void reset();

    virtual bool hitUnitCheck(const BaseUnit* const caller) const;
    virtual void hitUnit(const UnitCollisionEntry& entry);

    virtual void update();
protected:
    virtual bool processParameter(const PARAMETER_TYPE& value);

    typedef void (Switch::*FuncPtr)();
    FuncPtr switchOn;
    FuncPtr switchOff;
    void movementOn();
    void movementOff();

    int switchTimer;
    vector<BaseUnit*> targets;

    enum SwitchProp
    {
        spFunction=BaseUnit::upEOL,
        spTarget,
        bpEOL
    };

    enum SwitchFunction
    {
        sfMovement
    };
    map<string,int> stringToFunc;
private:
};

#endif // SWITCH_H


