#ifndef SWITCH_H
#define SWITCH_H

#include "BaseUnit.h"

class Switch : public BaseUnit
{
public:
    Switch(Level* newParent);
    virtual ~Switch();

    virtual bool load(list<PARAMETER_TYPE >& params);
    virtual bool processParameter(const PARAMETER_TYPE& value);
    virtual void reset();

    virtual void hitUnit(const UnitCollisionEntry& entry);

    virtual void update();
protected:

    typedef void (Switch::*FuncPtr)(BaseUnit* unit);
    FuncPtr switchOn;
    FuncPtr switchOff;
    PARAMETER_TYPE paramOn;
    PARAMETER_TYPE paramOff;
    void movementOn(BaseUnit* unit);
    void movementOff(BaseUnit* unit);
    void parameterOn(BaseUnit* unit);
    void parameterOff(BaseUnit* unit);

    int switchTimer;
    vector<BaseUnit*> targets;
    vector<std::string> targetIDs;

    enum SwitchProp
    {
        spFunction=BaseUnit::upEOL,
        bpEOL
    };

    enum SwitchFunction
    {
        sfMovement,
        sfParameter,
        sfParameterOn,
        sfParameterOff
    };
    static map<string,int> stringToFunc;
private:
};

#endif // SWITCH_H


