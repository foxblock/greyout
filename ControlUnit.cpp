#include "ControlUnit.h"

ControlUnit::ControlUnit(Level* newParent) : BaseUnit(newParent)
{
    takesControl = true;
    isPlayer = true;

    stringToProp["control"] = cpControl;
}

ControlUnit::~ControlUnit()
{
    //
}

/// ---public---

bool ControlUnit::processParameter(const PARAMETER_TYPE& value)
{
    if (BaseUnit::processParameter(value))
        return true;

    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case cpControl:
    {
        takesControl = StringUtility::stringToBool(value.second);
        break;
    }
    default:
        parsed = false;
    }

    return parsed;
}

void ControlUnit::control(SimpleJoy* input)
{
    if (input->isLeft())
    {
        acceleration[0].x = -2;
        acceleration[1].x = -16;
    }
    else if (input->isRight())
    {
        acceleration[0].x = 2;
        acceleration[1].x = 16;
    }
    else
    {
        acceleration[0].x = 0;
        acceleration[1].x = 0;
        velocity.x = 0;
    }
    if (input->isUp())
    {
        acceleration[0].y = -2;
        acceleration[1].y = -16;
    }
    else if (input->isDown())
    {
        acceleration[0].y = 2;
        acceleration[1].y = 16;
    }
    else
    {
        acceleration[0].y = 0;
        acceleration[1].y = 0;
        velocity.y = 0;
    }
}

/// ---protected---
