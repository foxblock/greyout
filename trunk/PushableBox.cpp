/*
	Greyout - a colourful platformer about love

	Greyout is Copyright (c)2011-2014 Janek Schäfer

	This file is part of Greyout.

    Greyout is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

	Greyout is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Please direct any feedback, questions or comments to
	Janek Schäfer (foxblock), foxblock_at_gmail_dot_com
*/

#include "PushableBox.h"

#include "fileTypeDefines.h"
#include "Level.h"
#include "MusicCache.h"
#include "BasePlayer.h"
#include "MyGame.h"

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
        	// set fixed speed to avoid jittery animations (yup, this is very ugly and I am very lazy, but it works, so fuck it all)
        	if (((BasePlayer*)entry.unit)->activelyMoving)
				entry.unit->velocity.x = PUSHING_SPEED * NumberUtility::sign( entry.unit->velocity.x );
			// only move by full pixels (else the collision detection fucks up, due
			// to differences in hitUnit and hitMap... maybe I should fix that...)
			// TODO: Fix that in Physics (see above)
			if (entry.unit->velocity.x >= 0)
				velocity.x = ceil(entry.unit->velocity.x);
			else
				velocity.x = floor(entry.unit->velocity.x);
        	// snap unit to box
        	if ( velocity.x > 0.0f )
				entry.unit->collisionInfo.positionCorrection.x += position.x - entry.unit->position.x - entry.unit->getWidth();
			else
				entry.unit->collisionInfo.positionCorrection.x -= entry.unit->position.x - position.x - getWidth();
			if (entry.unit->direction > 0)
				entry.unit->setSpriteState("pushright");
			else
				entry.unit->setSpriteState("pushleft");
        }
    }

	// I have no real clue what this code does (years after writing and not documenting it)
	// might have to do with slopes (which aren't in the main game really and not
	// tested very much), too lazy to find out
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
		int inc;
        switch (ENGINE->settings->getParticleDensity())
        {
		case Settings::pdOff:
			MUSIC_CACHE->playSound("sounds/die.wav",parent->chapterPath);
			toBeRemoved = true;
			return;
		case Settings::pdFew:
			inc = round(max((float)(getWidth() + getHeight()) / 32.0f,4.0f));
			break;
		case Settings::pdMany:
			inc = round(max((float)(getWidth() + getHeight()) / 64.0f,2.0f));
			break;
		case Settings::pdTooMany:
			inc = 1;
			break;
		default:
			inc = round(max((float)(getWidth() + getHeight()) / 64.0f,2.0f));
			break;
        }
        for (int X = getWidth()-1; X >= 0; X-=inc)
        {
            for (int Y = getHeight()-1; Y >= 0; Y-=inc)
            {
                vel.x = Random::nextFloat(-5,5);
                vel.y = Random::nextFloat(-8,-3);
                time = Random::nextInt(45,75);
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

    switch (next.key)
    {
    case boSize:
    {
        if (next.params.size() < 3)
        {
        	string temp = StringUtility::combine(next.params, DELIMIT_STRING);
            printf("ERROR: Bad order parameter \"%s\"in order #%i on unit id \"%s\"\n", temp.c_str(), currentOrder, id.c_str());
            orderList.erase(orderList.begin() + currentOrder);
            orderTimer = 1; // process next order in next cycle
            return false;
        }
        Vector2di destSize;
        destSize.x = StringUtility::stringToInt(next.params[1]);
        destSize.y = StringUtility::stringToInt(next.params[2]);
        sizeTimer.x =  (float)(destSize.x - rect.w) / (float)next.ticks;
        sizeTimer.y = (float)(destSize.y - rect.h) / (float)next.ticks;
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
        orderTimer = next.ticks;
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
