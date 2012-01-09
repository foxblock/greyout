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

    virtual bool processParameter(const PARAMETER_TYPE& value);

    virtual void control(SimpleJoy* input);

    bool takesControl; // if true control will be called by level

protected:
    enum ControlProps
    {
        cpControl=BaseUnit::upEOL,
        cpEOL
    };
};

#endif
