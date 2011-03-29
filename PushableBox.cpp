#include "PushableBox.h"

#include "fileTypeDefines.h"
#include "Level.h"

#define PUSHING_SPEED 3.0f

PushableBox::PushableBox(Level* newParent) : BaseUnit(newParent)
{
    width = 32;
    height = 32;
    col = BLACK;

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

Vector2df PushableBox::getPixel(const SimpleDirection& dir) const
{
    switch (dir.value)
    {
    case diBOTTOMLEFT:
        return Vector2df(position.x+1, position.y + getHeight() - 1);
    case diBOTTOMRIGHT:
        return position + getSize() - Vector2df(2,1);
    default:
        return BaseUnit::getPixel(dir);
    }
}


void PushableBox::updateScreenPosition(const Vector2di& offset)
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
    if (velocity.y < 4) // if not falling
    {
        if (collision.second.y > unit->velocity.y && collision.second.y > collision.second.x)
        {
            // horizontal collision
            float diff = unit->velocity.x;
            // limit speed of pushing unit
            float vel = PUSHING_SPEED * NumberUtility::sign(unit->velocity.x);
            diff -= vel;
            if (abs(diff) <= abs(collision.second.x))
            {
                velocity.x += collision.second.x * collision.first.xDirection() * -1 - diff;
                unit->velocity.x = vel;
                if (unit->direction > 0)
                    unit->setSpriteState("pushRight");
                else
                    unit->setSpriteState("pushLeft");
            }
        }
        else if (unit->position.y > position.y)
        {
            // vertical collision
            position.y -= collision.second.y;
            velocity.y = 0;
        }
    }
}

void PushableBox::explode()
{
    if (parent)
    {
        Vector2df vel(0,0);
        for (int X = 0; X < getWidth(); X+=2)
        {
            for (int Y = 0; Y < getHeight(); Y+=2)
            {
                vel.x = Random::nextFloat(-10,10);
                vel.y = Random::nextFloat(-15,-5);
                parent->addParticle(this,col,position + Vector2df(X,Y),vel,750);
            }
        }
    }
    toBeRemoved = true;
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
