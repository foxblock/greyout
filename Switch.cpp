#include "Switch.h"

#include "Level.h"
#include "ControlUnit.h"

#define SWITCH_TIMEOUT 15

map<string,int> Switch::stringToFunc;

Switch::Switch(Level* newParent) : BaseUnit(newParent)
{
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);
    flags.addFlag(ufNoUnitCollision);

    col = Colour(50,217,54);
    collisionColours.insert(Colour(BLACK).getIntColour());
    collisionColours.insert(Colour(WHITE).getIntColour());
    switchTimer = 0;
    switchOn = NULL;
    switchOff = NULL;

    stringToProp["function"] = spFunction;

    stringToFunc["movement"] = sfMovement;
    stringToFunc["parameter"] = sfParameter;
    stringToFunc["parameteron"] = sfParameterOn;
    stringToFunc["parameteroff"] = sfParameterOff;
}

Switch::~Switch()
{
    targets.clear();
}

///---public---

bool Switch::load(list<PARAMETER_TYPE >& params)
{
    bool result = BaseUnit::load(params);

    if (imageOverwrite[0] == 0)
    {
        imageOverwrite = "images/units/switch.png";
    }
    else // clear sprites loaded by BaseUnit
    {
        for (map<string,AnimatedSprite*>::iterator I = states.begin(); I != states.end(); ++I)
        {
            delete I->second;
        }
        states.clear();
    }
    AnimatedSprite* temp = new AnimatedSprite;
    temp->loadFrames(getSurface(imageOverwrite),2,1,0,1);
    temp->setTransparentColour(MAGENTA);
    states["off"] = temp;
    temp = new AnimatedSprite;
    temp->loadFrames(getSurface(imageOverwrite),2,1,1,1);
    temp->setTransparentColour(MAGENTA);
    states["on"] = temp;

    if (startingState[0] == 0 || startingState == "default")
        startingState = "off";
    setSpriteState(startingState,true);

    if (!switchOn || targets.empty())
        result = false;

    return result;
}

bool Switch::processParameter(const PARAMETER_TYPE& value)
{
    if (BaseUnit::processParameter(value))
        return true;

    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case spFunction:
    {
        vector<string> tokens;
        StringUtility::tokenize(value.second,tokens,DELIMIT_STRING,2);
        switch (stringToFunc[tokens.front()])
        {
        case sfMovement:
            switchOn = &Switch::movementOn;
            switchOff = &Switch::movementOff;
            break;
        case sfParameter:
        {
            switchOn = &Switch::parameterOn;
            switchOff = &Switch::parameterOff;
            vector<string> temp;
            StringUtility::tokenize(tokens.back(),temp,VALUE_STRING,2);
            if (tokens.size() < 2 || temp.size() < 2)
            {
                parsed = false;
                break;
            }
            paramOn.first = temp.front();
            paramOn.second = temp.back();
            paramOff.first = temp.front();
            paramOff.second = temp.back();
            break;
        }
        case sfParameterOn:
        {
            switchOn = &Switch::parameterOn;
            vector<string> temp;
            StringUtility::tokenize(tokens.back(),temp,VALUE_STRING,2);
            if (tokens.size() < 2 || temp.size() < 2)
            {
                parsed = false;
                break;
            }
            paramOn.first = temp.front();
            paramOn.second = temp.back();
            break;
        }
        case sfParameterOff:
        {
            switchOff = &Switch::parameterOff;
            vector<string> temp;
            StringUtility::tokenize(tokens.back(),temp,VALUE_STRING,2);
            if (tokens.size() < 2 || temp.size() < 2)
            {
                parsed = false;
                break;
            }
            paramOff.first = temp.front();
            paramOff.second = temp.back();
            break;
        }
        default:
            cout << "Unknown function parameter for switch \"" << id << "\"" << endl;
        }
        break;
    }
    case BaseUnit::upTarget:
    {
        targets.clear();
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

void Switch::reset()
{
    if (startingState == "off")
        for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
            (this->*switchOff)(*I);
    else
        for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
            (this->*switchOn)(*I);
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
        if (((ControlUnit*)entry.unit)->takesControl && parent->getInput()->isUp() && switchTimer == 0)
        {
            if (currentState == "off")
            {
                setSpriteState("on",true);
                if (switchOn)
                    for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
                        (this->*switchOn)(*I);
            }
            else
            {
                setSpriteState("off",true);
                if (switchOff)
                    for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
                        (this->*switchOff)(*I);
            }
            switchTimer = SWITCH_TIMEOUT;
        }
    }
}

///---protected---

void Switch::movementOn(BaseUnit* unit)
{
    unit->flags.removeFlag(BaseUnit::ufNoUpdate);
}

void Switch::movementOff(BaseUnit* unit)
{
    unit->flags.addFlag(BaseUnit::ufNoUpdate);
}

void Switch::parameterOn(BaseUnit* unit)
{
    if (paramOn.first == "order")
        unit->resetOrder(true);
    unit->processParameter(paramOn);
}

void Switch::parameterOff(BaseUnit* unit)
{
    if (paramOn.first == "order")
        unit->resetOrder(true);
    unit->processParameter(paramOff);
}

///---private---
