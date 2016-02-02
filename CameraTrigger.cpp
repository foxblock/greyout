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

#include "CameraTrigger.h"

#include "Level.h"

CameraTrigger::CameraTrigger(Level* newParent) : BaseTrigger(newParent)
{
	stringToProp["destination"] = cpDestination;
	stringToProp["time"] = cpTime;
	time = 1000;
	dest = Vector2df(0,0);
	triggerCol = PURPLE;
}

CameraTrigger::~CameraTrigger()
{
	//
}

///---public---

bool CameraTrigger::processParameter(const PARAMETER_TYPE& value)
{
	bool parsed = true;

	switch (stringToProp[value.first])
	{
	case cpDestination:
	{
		vector<string> token;
		StringUtility::tokenize(value.second,token,DELIMIT_STRING);
		if (token.size() != 2)
		{
			parsed = false;
			break;
		}
		dest.x = StringUtility::stringToFloat(token[0]);
		dest.y = StringUtility::stringToFloat(token[1]);
		break;
	}
	case cpTime:
	{
		time = StringUtility::stringToInt(value.second);
		break;
	}
	default:
		parsed = false;
	}

	if (not parsed)
		return BaseTrigger::processParameter(value);

	return parsed;
}

void CameraTrigger::generateParameters()
{
	BaseTrigger::generateParameters();
	if (dest.x != 0 || dest.y != 0)
		parameters.push_back(make_pair("destination", StringUtility::vecToString(dest)));
	parameters.push_back(make_pair("time", StringUtility::intToString(time)));
}

#ifdef _DEBUG
string CameraTrigger::debugInfo()
{
	string result = BaseTrigger::debugInfo();
	result += StringUtility::vecToString(dest) + "," + StringUtility::intToString(time) + "\n";
	return result;
}
#endif

///---protected---

void CameraTrigger::doTrigger(const UnitCollisionEntry& entry)
{
	parent->cam.centerOnPos(dest,time);
	BaseTrigger::doTrigger(entry);
}

///---private---
