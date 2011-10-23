#include "PushableBox.h"

#include "fileTypeDefines.h"
#include "Level.h"
#include "MusicCache.h"

#define PUSHING_SPEED 1.0f

PushableBox::PushableBox(Level* newParent) : BaseUnit(newParent)
{
    rect.w = 32;
    rect.h = 32;
    col = BLACK;

    stringToProp["size"] = bpSize;
}

PushableBox::~PushableBox()
{
    //
}

/// ---public---

int PushableBox::getHeight() const
{
    return rect.h;
}

int PushableBox::getWidth() const
{
    return rect.w;
}

Vector2df PushableBox::getPixel(const SimpleDirection& dir) const
{
    switch (dir.value)
    {
    case diTOPLEFT:
        return position + Vector2df(1,0);
    case diTOPRIGHT:
        return Vector2df(position.x + getWidth() - 2, position.y);
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
    rect.x = position.x - offset.x;
    rect.y = position.y - offset.y;
}

void PushableBox::render(SDL_Surface* surf)
{
    // SDL_FillRect might sometimes change the passed rect which we don't want
    int w = rect.w;
    int h = rect.h;
    SDL_FillRect(surf,&rect,col.getSDL_Uint32Colour(surf));
    rect.w = w;
    rect.h = h;

    BaseUnit::render(surf);
}

void PushableBox::hitUnit(const UnitCollisionEntry& entry)
{
    if (velocity.y < 4 && entry.unit->isPlayer) // if not falling
    {
        if (entry.overlap.y > entry.unit->velocity.y && entry.overlap.y > entry.overlap.x)
        {
            // horizontal collision
            float diff = entry.unit->velocity.x;
            // limit speed of pushing unit
            float vel = PUSHING_SPEED * NumberUtility::sign(entry.unit->velocity.x);
            diff -= vel;
            if (abs(diff) <= abs(entry.overlap.x))
            {
                velocity.x += entry.overlap.x * entry.dir.xDirection() * -1 - diff;
                entry.unit->velocity.x = vel;
                if (entry.unit->direction > 0)
                    entry.unit->setSpriteState("pushRight");
                else
                    entry.unit->setSpriteState("pushLeft");
            }
        }
    }

    if (entry.overlap.x > entry.overlap.y && entry.unit->position.y < position.y)
    {
        if (collisionInfo.positionCorrection.x != 0)
            entry.unit->collisionInfo.positionCorrection.x += collisionInfo.positionCorrection.x;
        if (velocity.y != 0 || collisionInfo.positionCorrection.y != 0)
            entry.unit->collisionInfo.positionCorrection.y -= entry.overlap.y - NumberUtility::sign(entry.overlap.y);
    }
}

void PushableBox::explode()
{
    if (parent)
    {
        Vector2df vel(0,0);
        int time = 0;
        for (int X = 0; X < getWidth(); X+=2)
        {
            for (int Y = 0; Y < getHeight(); Y+=2)
            {
                vel.x = Random::nextFloat(-5,5);
                vel.y = Random::nextFloat(-8,-3);
                time = Random::nextInt(750,1250);
                parent->addParticle(this,col,position + Vector2df(X,Y),vel,time);
            }
        }
        MUSIC_CACHE->playSound("sounds/die.wav",parent->chapterPath);
    }
    toBeRemoved = true;
}

/// ---protected---

bool PushableBox::processParameter(const PARAMETER_TYPE& value)
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
        rect.w = StringUtility::stringToInt(token.at(0));
        rect.h = StringUtility::stringToInt(token.at(1));
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
