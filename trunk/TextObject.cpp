#include "TextObject.h"

#include "StringUtility.h"

TextObject::TextObject(Level* newParent) : BaseUnit(newParent)
{
    stringToProp["font"] = tpFont;
    stringToProp["size"] = tpSize;
    stringToProp["text"] = tpText;

    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);
    flags.addFlag(ufNoUnitCollision);

    col = BLACK;
    line = "";
}

TextObject::~TextObject()
{
    //
}

///---public---

bool TextObject::load(const list<PARAMETER_TYPE >& params)
{
    bool result = BaseUnit::load(params);

    if (line[0] == 0)
    {
        cout << "ERROR: No text specified for TextObject: " << id << endl;
        return false;
    }
    currentText.setColour(col);

    return result;
}

void TextObject::updateScreenPosition(const Vector2di& offset)
{
    currentText.setPosition(position - offset);
}

void TextObject::render(SDL_Surface* screen)
{
    currentText.setColour(col);
    currentText.print(screen,line);
}

bool TextObject::hitUnitCheck(const BaseUnit* const caller) const
{
    return false;
}

///---protected---

bool TextObject::processParameter(const PARAMETER_TYPE& value)
{
    if (BaseUnit::processParameter(value))
        return true;

    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case tpFont:
    {
        currentText.loadFont(value.second);
        break;
    }
    case tpSize:
    {
        int val = StringUtility::stringToInt(value.second);
        if (val > 0)
            currentText.setFontSize(val);
        else
            parsed = false;
        break;
    }
    case tpText:
    {
        line = StringUtility::upper(value.second);
        break;
    }
    default:
        parsed = false;
    }

    return parsed;
}

///---private---
