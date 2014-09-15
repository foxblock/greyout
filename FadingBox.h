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

#ifndef FADING_BOX_H
#define FADING_BOX_H

#include "PushableBox.h"

class FadingBox : public PushableBox
{
public:
	FadingBox(Level* parent);
	virtual ~FadingBox();

	virtual bool processParameter(const PARAMETER_TYPE& value);

	virtual void update();

	virtual void hitUnit(const UnitCollisionEntry& entry);
	virtual bool checkCollisionColour(const Colour& col) const;

	#ifdef _DEBUG
	virtual string debugInfo();
	#endif
protected:
	void move();

	// first = player close, second = player distant colour
	pair<Colour,Colour> colours;
	// x = inner radius (full close colour), y = outer radius (full distant colour)
	Vector2df fadeRadius;
	int fadeSteps;

	enum FadingProp
	{
		fpFarColour=BaseUnit::upEOL,
		fpFadeRadius,
		fpFadeSteps,
		fpEOL
	};

private:

};


#endif // FADING_BOX_H

