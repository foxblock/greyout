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

#ifndef PUSHABLEBOX_H
#define PUSHABLEBOX_H

#include "Sound.h"
#include "Rectangle.h"

#include "BaseUnit.h"

/**
A pushable (by any player) box
Overrides some of BaseUnit's functionality as it uses a Penjin::Rectangle to
draw itself instead of a sprite
Adds a size property and shows how to extend loading in the processParameter and
load functions
**/

class PushableBox : public BaseUnit
{
	public:
		PushableBox(Level* newParent);
		virtual ~PushableBox();

		virtual bool processParameter(const PARAMETER_TYPE& value);
		virtual void generateParameters();

		virtual void reset();

		virtual int getHeight() const;
		virtual int getWidth() const;
		virtual Vector2df getPixel(const SimpleDirection& dir) const;

		virtual void update();
		virtual void updateScreenPosition(const Vector2di& offset);
		virtual void render(SDL_Surface* surf);

		// Also sets the unit's/player's velocity in this case (to slow it down and
		// create the illusion of weight)
		virtual void hitUnit(const UnitCollisionEntry& entry);

		virtual void explode();

		SDL_Rect rect;
	protected:
		virtual bool processOrder(Order& next);
		virtual bool updateOrder(const Order& curr);

		virtual string generateParameterOrders(const Order &o);

		virtual void move();

		Vector2df sizeTimer; // incremental step for size orders
		Vector2df dynSize; // floating point size for size orders (to preserve accuracy)
		bool isBeingPushed; // only used for playing push sound
		Sound pushSound;

		enum BoxOrder
		{
			boSize=BaseUnit::okEOL,
			boEOL
		};
	private:
};


#endif // PUSHABLEBOX_H
