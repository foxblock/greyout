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

#ifndef CAMERA_H
#define CAMERA_H

#include "PenjinTypes.h"
#include "Vector2df.h"

class BaseUnit;
class Level;

/**
Provides functions to move the rendered area around
**/

class Camera
{
	public:
		Camera();
		virtual ~Camera();

		void update();
		void reset();

		// If time is set to 0 the move will be instant
		void centerOnUnit(const BaseUnit* const unit, CRint time=0);
		void centerOnPos(const Vector2df& pos, CRint time=0);

		Vector2df getSpeed() const {return speed;}
		Vector2df getDest() const {return dest;}

		Level* parent;
		// If false level boundaries will be taken into account when moving
		// the "camera" so space outside the level is never shown
		bool disregardBoundaries;
	protected:
		Vector2df speed;
		Vector2df dest;
	private:

};


#endif // CAMERA_H

