#ifndef CONTROLUNIT_H
#define CONTROLUNIT_H

#include "SimpleJoy.h"

#include "BaseUnit.h"

/**
Adding control pass-through to BaseUnit essentially making the unit able to
react to button presses
**/

class ControlUnit : public BaseUnit
{
public:
    ControlUnit(Level* newParent);
    ~ControlUnit();

    virtual void control(SimpleJoy* input);

    bool takesControl; // if true control will be called by level

protected:
    virtual bool processParameter(const pair<string,string>& value);

    enum ControlProps
    {
        cpControl=BaseUnit::upEOL,
        cpEOL
    };
};

#endif
