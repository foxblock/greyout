#include "Switch.h"

#include "Level.h"

#define SWITCH_TIMEOUT 15

Switch::Switch(Level* newParent) : BaseUnit(newParent)
{
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);
    flags.addFlag(ufNoUnitCollision);

    col = Colour(50,217,54);
    collisionColours.insert(Colour(BLACK).getIntColour());
    collisionColours.insert(Colour(WHITE).getIntColour());
    switchTimer = 0;

    stringToProp["function"] = spFunction;
    stringToProp["target"] = spTarget;
    stringToFunc["movement"] = sfMovement;
}

Switch::~Switch()
{
    targets.clear();
}

///---public---

bool Switch::load(const list<PARAMETER_TYPE >& params)
{
    bool result = BaseUnit::load(params);

    if (imageOverwrite[0] == 0)
    {
        imageOverwrite = "images/units/switch.png";
    }
    AnimatedSprite* temp = new AnimatedSprite;
    temp->loadFrames(getSurface(imageOverwrite),2,1,1,1);
    temp->setTransparentColour(MAGENTA);
    states["off"] = temp;
    temp = new AnimatedSprite;
    temp->loadFrames(getSurface(imageOverwrite),2,1,0,1);
    temp->setTransparentColour(MAGENTA);
    states["on"] = temp;

    if (startingState[0] == 0)
        startingState = "off";

    if (!switchOn || targets.empty())
        result = false;

    return result;
}

void Switch::reset()
{
    if (startingState == "off")
        (this->*switchOff)();
    else
        (this->*switchOn)();
    BaseUnit::reset();
}

void Switch::update()
{
    if (switchTimer > 0)
        --switchTimer;
}

bool Switch::hitUnitCheck(const BaseUnit* const caller) const
{
    return false;
}

void Switch::hitUnit(const UnitCollisionEntry& entry)
{
    // standing still on the ground
    if (entry.unit->isPlayer && (int)entry.unit->velocity.x == 0 && abs(entry.unit->velocity.y) < 4 && entry.overlap.x > 10)
    {
        if (parent->getInput()->isUp() && switchTimer == 0)
        {
            if (currentState == "off")
            {
                setSpriteState("on",true);
                (this->*switchOn)();
            }
            else
            {
                setSpriteState("off",true);
                (this->*switchOff)();
            }
            switchTimer = SWITCH_TIMEOUT;
        }
    }
}

///---protected---

bool Switch::processParameter(const PARAMETER_TYPE& value)
{
    if (BaseUnit::processParameter(value))
        return true;

    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case spFunction:
    {
        switch (stringToFunc[value.second])
        {
        case sfMovement:
            switchOn = &Switch::movementOn;
            switchOff = &Switch::movementOff;
            break;
        default:
            cout << "Unknown function parameter for switch \"" << id << "\"" << endl;
        }
        break;
    }
    case spTarget:
    {
        vector<string> tokens;
        StringUtility::tokenize(value.second,tokens,DELIMIT_STRING);
        for (vector<BaseUnit*>::iterator I = parent->units.begin(); I != parent->units.end(); ++I)
        {
            for (vector<string>::iterator str = tokens.begin(); str != tokens.end(); ++str)
            {
                if ((*I)->id == (*str))
                    targets.push_back(*I);
            }
        }
        break;
    }
    default:
        parsed = false;
    }

    return parsed;
}

void Switch::movementOn()
{
    for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
        (*I)->flags.removeFlag(BaseUnit::ufNoUpdate);
}

void Switch::movementOff()
{
    for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
        (*I)->flags.addFlag(BaseUnit::ufNoUpdate);
}

///---private---
