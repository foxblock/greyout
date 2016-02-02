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

#include "FadingBox.h"

#include "ControlUnit.h"
#include "Level.h"

FadingBox::FadingBox(Level* newParent) : PushableBox(newParent)
{
	flags.addFlag(ufNoGravity);
	flags.addFlag(ufNoMapCollision);
	flags.addFlag(ufInvincible);
	unitCollisionMode = 0;

	stringToProp["farcolour"] = fpFarColour;
	stringToProp["farcolor"] = fpFarColour;
	stringToProp["faderadius"] = fpFadeRadius;
	stringToProp["fadesteps"] = fpFadeSteps;

	colours.first = WHITE;
	colours.second = BLACK;
	fadeRadius = Vector2df(32,96);
	fadeSteps = 0;
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
		parsed = pLoadColour( value.second, colours.first );
		col = colours.first;
		break;
	}
	case fpFarColour:
	{
		parsed = pLoadColour( value.second, colours.second );
		break;
	}
	case fpFadeRadius:
	{
		if (value.second.find(DELIMIT_STRING) != string::npos) // Inner and outer radius defined
		{
			fadeRadius = StringUtility::stringToVec<Vector2df>(value.second);
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
	case fpFadeSteps:
	{
		fadeSteps = StringUtility::stringToInt(value.second);
		break;
	}
	default:
		parsed = false;
	}

	if (parsed == false)
		return PushableBox::processParameter(value);

	return parsed;
}

void FadingBox::generateParameters()
{
	PushableBox::generateParameters();
	parameters.push_back(make_pair("farcolour", colourToString(col)));
	parameters.push_back(make_pair("faderadius", StringUtility::vecToString(fadeRadius)));
	if (fadeSteps != 0)
		parameters.push_back(make_pair("fadesteps", StringUtility::intToString(fadeSteps)));
}

void FadingBox::update()
{
	BaseUnit::update();

	float distance = fadeRadius.y;

	for (vector<ControlUnit*>::const_iterator unit = parent->players.begin();
		 unit != parent->players.end(); ++unit)
	{
		if ( checkCollisionColour((*unit)->col) )
		{
			float temp = ((*unit)->getPixel(diMIDDLE) - getPixel(diMIDDLE)).length();
			if ( temp < distance )
			{
				distance = temp;
			}
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
		float factor =  (distance - fadeRadius.x) / (fadeRadius.y - fadeRadius.x); // 0..1
		if ( fadeSteps > 0 )
		{
			factor = round( factor * fadeSteps ) / fadeSteps;
		}
		col.red = colours.first.red + (float)(colours.second.red - colours.first.red) * factor;
		col.green = colours.first.green + (float)(colours.second.green - colours.first.green) * factor;
		col.blue = colours.first.blue + (float)(colours.second.blue - colours.first.blue) * factor;
	}
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

	result += "FR: " + StringUtility::vecToString(fadeRadius) + "\n";

	return result;
}
#endif

///---protected---

void FadingBox::move()
{
	BaseUnit::move();
}

///---private---
