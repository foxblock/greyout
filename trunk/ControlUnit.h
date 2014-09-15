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

#ifndef CONTROLUNIT_H
#define CONTROLUNIT_H

#include "SimpleJoy.h"

#include "BaseUnit.h"

/**
Adding control pass-through to BaseUnit essentially making the unit able to
react to button presses
**/

class ControlUnit : public BaseUnit
{
public:
	ControlUnit(Level* newParent);
	~ControlUnit();

	virtual bool processParameter(const PARAMETER_TYPE& value);

	virtual void control(SimpleJoy* input);

	bool takesControl; // if true control will be called by level

protected:
	enum ControlProps
	{
		cpControl=BaseUnit::upEOL,
		cpEOL
	};
};

#endif
