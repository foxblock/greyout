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

#include "Key.h"

#include "Level.h"
#include "Exit.h"

Key::Key(Level* newParent) : BaseUnit(newParent)
{
	flags.addFlag(ufNoMapCollision);
	flags.addFlag(ufNoGravity);
	unitCollisionMode = 0;

	col = Colour(50,217,54);
	collisionColours.insert(Colour(BLACK).getIntColour());
	collisionColours.insert(Colour(WHITE).getIntColour());

}

Key::~Key()
{
	targets.clear();
	targetIDs.clear();
}

///---public---

bool Key::load(list<PARAMETER_TYPE >& params)
{
	bool result = BaseUnit::load(params);

	if (imageOverwrite[0] == 0)
	{
		imageOverwrite = "images/units/key.png";
	}
	else // clear sprites loaded by BaseUnit
	{
		for (map<string,AnimatedSprite*>::iterator I = states.begin(); I != states.end(); ++I)
		{
			delete I->second;
		}
		states.clear();
	}
	AnimatedSprite* temp = new AnimatedSprite;
	temp->loadFrames(getSurface(imageOverwrite),1,1,0,1);
	temp->setTransparentColour(MAGENTA);
	states["key"] = temp;

	if (startingState[0] == 0 || startingState == "default")
		startingState = "key";
	setSpriteState(startingState,true);

	if (targetIDs.empty())
	{
		printf("ERROR: Key \"%s\" without a target exit!\n",id.c_str());
		result = false;
	}

	return result;
}

bool Key::processParameter(const PARAMETER_TYPE& value)
{
	if (BaseUnit::processParameter(value))
		return true;

	bool parsed = true;

	switch (stringToProp[value.first])
	{
	case BaseUnit::upTarget:
	{
		parsed = pLoadUintIDs( value.second, targetIDs );
		break;
	}
	default:
		parsed = false;
	}

	return parsed;
}

void Key::update()
{
	if (!targetIDs.empty())
	{
		targets.clear();
		parent->getUnitsByID(targetIDs,targets);
		targetIDs.clear();
		for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
		{
			if ((*I)->tag == "exit")
				((Exit*)(*I))->keys.insert(this);
		}
	}
	BaseUnit::update();
}

void Key::reset()
{
	BaseUnit::reset();
	for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
	{
		if ((*I)->tag == "exit")
			((Exit*)(*I))->keys.insert(this);
	}
}

void Key::hitUnit(const UnitCollisionEntry& entry)
{
	if (entry.unit->isPlayer)
	{
		for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
		{
			if ((*I)->tag == "exit")
				((Exit*)(*I))->keys.erase(this);
		}
		toBeRemoved = true;
	}
}

///---protected---

///---private---
