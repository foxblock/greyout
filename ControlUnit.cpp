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

#include "ControlUnit.h"

ControlUnit::ControlUnit(Level* newParent) : BaseUnit(newParent)
{
	takesControl = true;
	isPlayer = true;

	stringToProp["control"] = cpControl;
}

ControlUnit::~ControlUnit()
{
	//
}

/// ---public---

bool ControlUnit::processParameter(const PARAMETER_TYPE& value)
{
	switch (stringToProp[value.first])
	{
	case cpControl:
	{
		takesControl = StringUtility::stringToBool(value.second);
		return true;
	}
	default:
		return BaseUnit::processParameter(value);
	}
}

void ControlUnit::generateParameters()
{
	BaseUnit::generateParameters();
	parameters.push_back(make_pair("control", StringUtility::boolToString(takesControl)));
}

void ControlUnit::control(SimpleJoy* input)
{
	if (!input)
	{
		acceleration[0].x = 0;
		acceleration[1].x = 0;
		velocity.x = 0;
		return;
	}

	if (input->isLeft())
	{
		acceleration[0].x = -2;
		acceleration[1].x = -16;
	}
	else if (input->isRight())
	{
		acceleration[0].x = 2;
		acceleration[1].x = 16;
	}
	else
	{
		acceleration[0].x = 0;
		acceleration[1].x = 0;
		velocity.x = 0;
	}
	if (input->isUp())
	{
		acceleration[0].y = -2;
		acceleration[1].y = -16;
	}
	else if (input->isDown())
	{
		acceleration[0].y = 2;
		acceleration[1].y = 16;
	}
	else
	{
		acceleration[0].y = 0;
		acceleration[1].y = 0;
		velocity.y = 0;
	}
}

/// ---protected---
