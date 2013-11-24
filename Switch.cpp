#include "Switch.h"

#include "Level.h"
#include "ControlUnit.h"

#define SWITCH_TIMEOUT 30

map<string,int> Switch::stringToFunc;

Switch::Switch(Level* newParent) : BaseUnit(newParent)
{
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);
    unitCollisionMode = 0;

    col = Colour(50,217,54);
    collisionColours.insert(Colour(BLACK).getIntColour());
    collisionColours.insert(Colour(WHITE).getIntColour());
    switchTimer = 0;
    switchOn = NULL;
    switchOff = NULL;

    linkTimer = 0;

    stringToProp["function"] = spFunction;

    stringToFunc["movement"] = sfMovement;
    stringToFunc["parameter"] = sfParameter;
    stringToFunc["parameteron"] = sfParameterOn;
    stringToFunc["parameteroff"] = sfParameterOff;
}

Switch::~Switch()
{
    targets.clear();
    targetIDs.clear();
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

    if (!switchOn || !switchOff)
    {
    	if (!switchOn && !switchOff)
			result = true;
        printf("WARNING: No Function specified for switch with ID \"%s\"\n",id.c_str());
    }
    if (targetIDs.empty())
    {
        result = false;
        printf("ERROR: No targets specified for switch with ID \"%s\"\n",id.c_str());
    }

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
            printf("Unknown function parameter for switch \"%s\"\n",id.c_str());
        }
        break;
    }
    case BaseUnit::upTarget:
    {
        targetIDs.clear();
        vector<string> tokens;
        StringUtility::tokenize(value.second,tokens,DELIMIT_STRING);
        targetIDs.insert(targetIDs.begin(),tokens.begin(),tokens.end());
        break;
    }
    default:
        parsed = false;
    }

    return parsed;
}

void Switch::reset()
{
    if (startingState == "off" && switchOff)
        for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
            (this->*switchOff)(*I);
    else if (switchOn)
        for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
            (this->*switchOn)(*I);
	switchTimer = 0;
	linkTimer = 0;
    BaseUnit::reset();
}

void Switch::update()
{
    if (!targetIDs.empty())
    {
        targets.clear();
        parent->getUnitsByID(targetIDs,targets);

        if (startingState == "off" && switchOff)
            for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
                (this->*switchOff)(*I);
        else if (switchOn)
            for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
                (this->*switchOn)(*I);

        targetIDs.clear();
    }

    if (switchTimer > 0)
        --switchTimer;
	if (linkTimer > 0)
	{
		--linkTimer;
		if (linkTimer == 0)
			parent->removeLink(this);
	}
    BaseUnit::update();
}

void Switch::hitUnit(const UnitCollisionEntry& entry)
{
    // standing still on the ground
    if (entry.unit->isPlayer && (int)entry.unit->velocity.x == 0 && abs(entry.unit->velocity.y) < 4 &&
		entry.overlap.x > 10 && ((ControlUnit*)entry.unit)->takesControl)
    {
        if (parent->getInput()->isUp() && switchTimer == 0)
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
        if (linkTimer == 0) // no links shown, player needs to stand still once to activate them
		{
			for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
				parent->addLink(this,*I);
			linkTimer = 3;
		}
    }
    if (linkTimer > 0 && entry.unit->isPlayer && ((ControlUnit*)entry.unit)->takesControl) // once activated, show until player leaves switch area
		linkTimer = 3;
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
    if (paramOn.first == "state")
    {
        unit->setSpriteState(paramOn.second,true);
        return;
    }
    if (paramOn.first == "order")
        unit->resetOrder(true);
    unit->processParameter(paramOn);
    if (paramOn.first == "order")
        unit->resetOrder(false);
}

void Switch::parameterOff(BaseUnit* unit)
{
    if (paramOff.first == "state")
    {
        unit->setSpriteState(paramOff.second,true);
        return;
    }
    if (paramOff.first == "order")
        unit->resetOrder(true);
    unit->processParameter(paramOff);
    if (paramOff.first == "order")
        unit->resetOrder(false);
}

///---private---
