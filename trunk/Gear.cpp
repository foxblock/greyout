#include "Gear.h"

Gear::Gear(Level* newParent) : BaseUnit(newParent)
{
    stringToProp["speed"] = gpSpeed;
    stringToProp["rotation"] = gpRotation;

    stringToOrder["rotation"] = goRotation;

    speed = 0;
    angle = 0;
    screenPosition = Vector2df(0,0);
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);
    unitCollisionMode = 0;
}

Gear::~Gear()
{
    //
}

///---public---

bool Gear::load(list<PARAMETER_TYPE >& params)
{
    bool result = BaseUnit::load(params);

    if (imageOverwrite[0] == 0)
    {
        if (col == BLACK)
            imageOverwrite = "images/units/gear_black.png";
        else
            imageOverwrite = "images/units/gear_white.png";
    }
    else // clear sprites loaded by BaseUnit
    {
        for (map<string,AnimatedSprite*>::iterator I = states.begin(); I != states.end(); ++I)
        {
            delete I->second;
        }
        states.clear();
    }
    img.loadImage(getSurface(imageOverwrite));
    img.setSurfaceSharing(true);
    img.setTransparentColour(Colour(MAGENTA));
    img.setDegreesCached(359);
    img.precacheRotations();

    return result;
}

bool Gear::processParameter(const PARAMETER_TYPE& value)
{
    if (BaseUnit::processParameter(value))
        return true;

    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case gpSpeed:
    {
        speed = StringUtility::stringToFloat(value.second);
        break;
    }
    case gpRotation:
    {
        angle = StringUtility::stringToFloat(value.second);
        break;
    }
    default:
        parsed = false;
    }

    return parsed;
}

int Gear::getHeight() const
{
    return img.getHeight();
}

int Gear::getWidth() const
{
    return img.getWidth();
}

void Gear::update()
{
    angle += speed;
    if (abs(angle) > 360)
        angle = (int)angle % 360 + angle - (int)angle;
    img.setRotation(angle);
    BaseUnit::update();
}

void Gear::updateScreenPosition(const Vector2di& offset)
{
    screenPosition = position - offset;
}

void Gear::render(SDL_Surface* surf)
{
    img.renderImage(surf,screenPosition);
}

///---protected---

bool Gear::processOrder(Order& next)
{
    bool parsed = true;

    vector<string> tokens;
    StringUtility::tokenize(next.value,tokens,DELIMIT_STRING);
    int ticks = 1;
    if (!tokens.empty())
    {
    	string time = tokens.front();
    	if (time[time.length()-1] == 'f')
			ticks = StringUtility::stringToInt(time.substr(0,time.length()-1));
		else // This is kinda fucked up, because of the frame based movement
			ticks = round(StringUtility::stringToFloat(time) / 1000.0f * (float)FRAME_RATE);
    }

    switch (next.key)
    {
    case okIdle:
    {
        velocity = Vector2df(0,0);
        acceleration[0] = Vector2df(0,0);
        acceleration[1] = Vector2df(0,0);
        speed = 0;
        break;
    }
    case goRotation:
    {
        if (tokens.size() < 2)
        {
            printf("ERROR: Bad order parameter \"%s\" on unit id \"%s\"\n",next.value.c_str(),id.c_str());
            orderList.erase(orderList.begin() + currentOrder);
            orderTimer = 1; // process next order in next cycle
            return false;
        }
        speed = (StringUtility::stringToFloat(tokens[1]) - angle) / ticks;
        break;
    }
    default:
        parsed = false;
    }

    if (parsed == false)
        return BaseUnit::processOrder(next);
    else
    {
        orderTimer = ticks;
        orderRunning = true;
    }
    return parsed;
}

///---private---
