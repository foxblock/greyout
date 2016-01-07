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

#include "Gear.h"

Gear::Gear(Level* newParent) : BaseUnit(newParent)
{
	stringToProp["speed"] = gpSpeed;
	stringToProp["angle"] = gpAngle;

	stringToOrder["rotateto"] = goRotateTo;
	stringToOrder["rotateby"] = goRotateBy;

	orderToString[goRotateTo] = "rotateto";
	orderToString[goRotateBy] = "rotateby";

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
	case gpAngle:
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
	case goRotateTo:
	{
		if (next.params.size() < 2)
		{
			string temp = StringUtility::combine(next.params, DELIMIT_STRING);
			printf("ERROR: Bad order parameter \"%s\"in order #%i on unit id \"%s\"\n", temp.c_str(), currentOrder, id.c_str());
			orderList.erase(orderList.begin() + currentOrder);
			orderTimer = 1; // process next order in next cycle
			return false;
		}
		speed = (StringUtility::stringToFloat(next.params[1]) - angle) / next.ticks;
		break;
	}
	case goRotateBy:
	{
		if (next.params.size() < 2)
		{
			string temp = StringUtility::combine(next.params, DELIMIT_STRING);
			printf("ERROR: Bad order parameter \"%s\"in order #%i on unit id \"%s\"\n", temp.c_str(), currentOrder, id.c_str());
			orderList.erase(orderList.begin() + currentOrder);
			orderTimer = 1; // process next order in next cycle
			return false;
		}
		speed = (StringUtility::stringToFloat(next.params[1])) / next.ticks;
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

string Gear::generateParameterOrders(Order o)
{
	if (o.key != goRotateBy && o.key != goRotateTo)
		return BaseUnit::generateParameterOrders(o);
	// Key
	string result = orderToString[o.key];
	// Time
	if (o.randomTicks > 0)
		result += StringUtility::intToString(o.randomTicks) + "r";
	else
		result += StringUtility::intToString(o.ticks) + "f";
	// Parameters
	for (vector<string>::iterator I = o.params.begin(); I != o.params.end(); ++I)
		result += "," + *I;
	return result;
}

///---private---
