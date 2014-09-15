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

#include "CollisionObject.h"

#include <map>

#define DEFAULT_THRESHOLD 0

CollisionObject::CollisionObject()
{
	correction = Vector2df(0.0f,0.0f);
	positionCorrection = Vector2df(0.0f,0.0f);
	squashCounter = 0;
	squashThreshold = DEFAULT_THRESHOLD;
	pixels.reserve(4);
	units.reserve(4);
}

CollisionObject::~CollisionObject()
{
	clear();
}

void CollisionObject::clear()
{
	pixels.clear();
	positionCorrection = Vector2df(0.0f,0.0f);
	units.clear();
}

bool CollisionObject::isBeingSquashed() const
{
	map<int,bool> table;
	for (vector<MapCollisionEntry>::const_iterator iter = pixels.begin(); iter != pixels.end(); ++iter)
	{
		table[iter->dir.value] = true;
	}
	if (table[diLEFT] && table[diRIGHT])
		return true;
	if (table[diTOP] && table[diBOTTOM])
		return true;
	if (table[diTOPLEFT] && table[diTOPRIGHT] && not table[diTOP])
		return true;
	return false;
}

bool CollisionObject::isHealthy(const Vector2df& vel)
{
	int temp = squashCounter;

	// red = death
	for (vector<MapCollisionEntry>::const_iterator iter = pixels.begin(); iter != pixels.end(); ++iter)
	{
		if (iter->col == Colour(RED))
			return false;
	}

	if (correction != Vector2df(0,0) && isBeingSquashed())
		++squashCounter;
	else
		squashCounter = 0;
	if (abs(vel.x) <= 16)
	{
		if (abs(correction.x) > 16)
			++squashCounter;
	}
	else if (abs(correction.x) > abs(vel.x))
		++squashCounter;

	if (squashCounter > squashThreshold)
		return false;

	if (squashCounter == temp) // has not changed
		max(--squashCounter,0);

	return true;
}
