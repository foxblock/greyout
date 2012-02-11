#include "FadingBox.h"

#include "ControlUnit.h"
#include "Level.h"

FadingBox::FadingBox(Level* newParent) : PushableBox(newParent)
{
    flags.addFlag(ufNoGravity);
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoUnitCollision);
    flags.addFlag(ufInvincible);

    stringToProp["farcolour"] = fpFarColour;
    stringToProp["faderadius"] = fpFadeRadius;

    colours.first = WHITE;
    colours.second = BLACK;
    fadeRadius = Vector2df(32,96);
}

FadingBox::~FadingBox()
{
    //
}

///---public---

bool FadingBox::processParameter(const PARAMETER_TYPE& value)
{
    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case upColour:
    {
        int val = StringUtility::stringToInt(value.second);
        if (val > 0 || value.second == "0") // passed parameter is a numeric colour code
            colours.first = Colour(val);
        else // string colour code
            colours.first = Colour(value.second);
        col = colours.first;
        break;
    }
    case fpFarColour:
    {
        int val = StringUtility::stringToInt(value.second);
        if (val > 0 || value.second == "0") // passed parameter is a numeric colour code
            colours.second = Colour(val);
        else // string colour code
            colours.second = Colour(value.second);
        break;
    }
    case fpFadeRadius:
    {
        if (value.second.find(DELIMIT_STRING) != string::npos) // Inner and outer radius defined
        {
            fadeRadius = StringUtility::stringToVec(value.second);
        }
        else // only outer radius defined
        {
            fadeRadius.y = StringUtility::stringToInt(value.second);
        }
        if (fadeRadius.x < 0 || fadeRadius.y < 0 || fadeRadius.y < fadeRadius.x)
        {
            parsed = false;
            fadeRadius = Vector2df(32,96);
        }
        break;
    }
    default:
        parsed = false;
    }

    if (parsed == false)
        return PushableBox::processParameter(value);
}

void FadingBox::update()
{
    BaseUnit::update();

    float distance = fadeRadius.y + 1;

    for (vector<ControlUnit*>::const_iterator unit = parent->players.begin();
         unit != parent->players.end(); ++unit)
    {
        float dist = ((*unit)->getPixel(diMIDDLE) - getPixel(diMIDDLE)).length();
        if (dist < fadeRadius.y && checkCollisionColour((*unit)->col))
        {
            distance = min(distance,dist);
        }
    }
    if (distance >= fadeRadius.y)
    {
        col = colours.second;
    }
    else if (distance <= fadeRadius.x)
    {
        col = colours.first;
    }
    else
    {
        distance -= fadeRadius.x;
        col.red = colours.first.red + (float)(colours.second.red - colours.first.red) / (fadeRadius.y - fadeRadius.x) * distance;
        col.green = colours.first.green + (float)(colours.second.green - colours.first.green) / (fadeRadius.y - fadeRadius.x) * distance;
        col.blue = colours.first.blue + (float)(colours.second.blue - colours.first.blue) / (fadeRadius.y - fadeRadius.x) * distance;
    }
}

bool FadingBox::hitUnitCheck(const BaseUnit* const caller) const
{
    return false;
}

void FadingBox::hitUnit(const UnitCollisionEntry& entry)
{
    //
}

bool FadingBox::checkCollisionColour(const Colour& col) const
{
    set<int>::const_iterator iter = collisionColours.find(col.getIntColour());
    if (iter != collisionColours.end())
        return true;
    return false;
}

#ifdef _DEBUG
string FadingBox::debugInfo()
{
    string result = PushableBox::debugInfo();

    result += StringUtility::vecToString(fadeRadius) + "\n";

    return result;
}
#endif

///---protected---

void FadingBox::move()
{
    BaseUnit::move();
}

///---private---
