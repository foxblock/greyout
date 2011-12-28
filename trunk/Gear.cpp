#include "Gear.h"

Gear::Gear(Level* newParent) : BaseUnit(newParent)
{
    stringToProp["speed"] = gpSpeed;
    stringToProp["rotation"] = gpRotation;

    stringToOrder["rotation"] = goRotation;

    speed = 1;
    angle = 0;
    screenPosition = Vector2df(0,0);
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoUnitCollision);
    flags.addFlag(ufNoGravity);
}

Gear::~Gear()
{
    //
}

///---public---

bool Gear::load(const list<PARAMETER_TYPE >& params)
{
    bool result = BaseUnit::load(params);

    if (imageOverwrite[0] == 0)
    {
        if (col == BLACK)
            imageOverwrite = "images/units/gear_black.png";
        else
            imageOverwrite = "images/units/gear_white.png";
    }
    img.loadImage(getSurface(imageOverwrite));
    img.setSurfaceSharing(true);
    img.setTransparentColour(Colour(MAGENTA));
    img.setDegreesCached(359);
    img.precacheRotations();

    return result;
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

bool Gear::processOrder(Order& next)
{
    bool parsed = true;

    vector<string> tokens;
    StringUtility::tokenize(next.value,tokens,DELIMIT_STRING);
    int ticks = 1;
    // This is kinda fucked up, because of the frame based movement
    if (tokens.size() > 0)
    {
        ticks = round(StringUtility::stringToFloat(tokens.front()) / 1000.0f * (float)FRAME_RATE);
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
            cout << "Error: Bad order parameter \"" << next.value << "\" on unit id \"" << id << "\"" << endl;
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
