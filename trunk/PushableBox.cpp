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
    sizeTimer.x = 0;
    sizeTimer.y = 0;
    dynSize.x = 0;
    dynSize.y = 0;

    stringToOrder["size"] = boSize;
}

PushableBox::~PushableBox()
{
    //
}

/// ---public---

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
    case BaseUnit::upSize:
    {
        vector<string> token;
        StringUtility::tokenize(value.second,token,DELIMIT_STRING);
        if (token.size() != 2)
        {
            parsed = false;
            break;
        }
        rect.w = StringUtility::stringToInt(token[0]);
        rect.h = StringUtility::stringToInt(token[1]);
        break;
    }
    default:
        parsed = false;
    }

    return parsed;
}

void PushableBox::reset()
{
    rect.w = 32;
    rect.h = 32;
    BaseUnit::reset();
}

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
        if (entry.overlap.y > entry.unit->velocity.y && entry.overlap.y > entry.overlap.x
			 && entry.unit->velocity.x != 0.0f )
        {
        	velocity.x = PUSHING_SPEED * NumberUtility::sign( entry.unit->velocity.x );
        	//entry.unit->velocity.x = entry.overlap.x + velocity.x;
			if (entry.unit->direction > 0)
				entry.unit->setSpriteState("pushright");
			else
				entry.unit->setSpriteState("pushleft");
        }
    }

    if (entry.overlap.x > entry.overlap.y && entry.unit->position.y < position.y)
    {
        if (collisionInfo.positionCorrection.x != 0)
            entry.unit->collisionInfo.positionCorrection.x += collisionInfo.positionCorrection.x;
        if (velocity.y != 0.0f || collisionInfo.positionCorrection.y != 0.0f)
            entry.unit->collisionInfo.positionCorrection.y -= entry.overlap.y - NumberUtility::sign(entry.overlap.y);
    }
}

void PushableBox::explode()
{
    if (parent)
    {
        Vector2df vel(0,0);
        int time = 0;
        int inc = round(max((float)(getWidth() + getHeight()) / 64.0f,2.0f));
        for (int X = 0; X < getWidth(); X+=inc)
        {
            for (int Y = 0; Y < getHeight(); Y+=inc)
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

bool PushableBox::processOrder(Order& next)
{
    bool parsed = true;

    vector<string> tokens;
    StringUtility::tokenize(next.value,tokens,DELIMIT_STRING);
    int ticks = 1;
    // This is kinda fucked up, because of the frame based movement
    if (!tokens.empty())
    {
        ticks = round(StringUtility::stringToFloat(tokens.front()) / 1000.0f * (float)FRAME_RATE);
    }

    switch (next.key)
    {
    case boSize:
    {
        if (tokens.size() < 3)
        {
            printf("ERROR: Bad order parameter \"%s\" on unit id \"%s\"\n",next.value.c_str(),id.c_str());
            orderList.erase(orderList.begin() + currentOrder);
            orderTimer = 1; // process next order in next cycle
            return false;
        }
        Vector2di destSize;
        destSize.x = StringUtility::stringToInt(tokens[1]);
        destSize.y = StringUtility::stringToInt(tokens[2]);
        sizeTimer.x =  (float)(destSize.x - rect.w) / (float)ticks;
        sizeTimer.y = (float)(destSize.y - rect.h) / (float)ticks;
        dynSize.x = rect.w;
        dynSize.y = rect.h;
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

bool PushableBox::updateOrder(const Order& curr)
{
    if (BaseUnit::updateOrder(curr))
        return true;

    bool parsed = true;

    switch (curr.key)
    {
    case boSize:
    {
        dynSize += sizeTimer;
        rect.w = round(dynSize.x);
        rect.h = round(dynSize.y);
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
