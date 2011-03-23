#include "PushableBox.h"

#include "LevelLoader.h"

PushableBox::PushableBox(Level* newParent) : BaseUnit(newParent)
{
    width = 24;
    height = 24;
    col = BLACK;
    setRectangle();

    stringToProp["size"] = bpSize;
}

PushableBox::PushableBox(Level* newParent, CRint newWidth, CRint newHeight, const Colour& newCol) : BaseUnit(newParent)
{
    width = newWidth;
    height = newHeight;
    col = newCol;
    setRectangle();
}

PushableBox::~PushableBox()
{
    //
}

/// ---public---

// Custom load function making sure the unit is properly initialized after loading
bool PushableBox::load(const PARAMETER_TYPE& params)
{
    if (BaseUnit::load(params))
    {
        setRectangle();
        return true;
    }
    return false;
}

int PushableBox::getHeight() const
{
    return height;
}

int PushableBox::getWidth() const
{
    return width;
}

void PushableBox::updateScreenPosition(Vector2di offset)
{
    rect.setPosition(position - offset);
}

void PushableBox::render(SDL_Surface* surf)
{
    rect.render(surf);

    BaseUnit::render(surf);
}

void PushableBox::hitUnit(const UNIT_COLLISION_DATA_TYPE& collision, BaseUnit* const unit)
{
    if (collision.second.y > unit->velocity.y && collision.second.y > collision.second.x)
    {
        float diff = unit->velocity.x;
        float vel = 4 * NumberUtility::sign(unit->velocity.x);
        diff -= vel;
        if (abs(diff) < abs(collision.second.x))
        {
            velocity.x += collision.second.x * collision.first.xDirection() * -1 - diff;
            unit->velocity.x = vel;
        }
    }
    else if (unit->position.y > position.y)
    {
        position.y -= collision.second.y;
        velocity.y = 0;
    }
}

void PushableBox::setRectangle()
{
    rect.setDimensions(width,height);
    rect.setPosition(position);
    rect.setColour(col);
}

/// ---protected---

bool PushableBox::processParameter(const pair<string,string>& value)
{
    // first ensure backwards compatibility by passing the value to BaseUnit
    // (also avoids having to copy that code)
    if (BaseUnit::processParameter(value))
        return true;

    // if the specific parameter is not caught by BaseUnit is has to be special
    // to this unit (or erroneous), so custom implementation here, following the
    // same structure as BaseUnit::processParameter
    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case bpSize:
    {
        vector<string> token;
        StringUtility::tokenize(value.second,token,DELIMIT_STRING);
        if (token.size() != 2)
        {
            parsed = false;
            break;
        }
        width = StringUtility::stringToInt(token.at(0));
        height = StringUtility::stringToInt(token.at(1));
        break;
    }
    default:
        parsed = false;
    }

    return parsed;
}

void PushableBox::move()
{
    BaseUnit::move();
    velocity.x = 0; // without an implementation of friction, this avoids sliding
}
