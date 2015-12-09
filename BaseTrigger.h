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

#ifndef BASE_TRIGGER_H
#define BASE_TRIGGER_H

#include "BaseUnit.h"

class BaseTrigger : public BaseUnit
{
public:
	BaseTrigger(Level* newParent);
	virtual ~BaseTrigger();

	virtual bool processParameter(const PARAMETER_TYPE& value);

	virtual void reset();

	virtual void update();
	virtual void updateScreenPosition(const Vector2di& offset);
	virtual void render(SDL_Surface* surf);

	virtual void hitUnit(const UnitCollisionEntry& entry);
	virtual bool checkCollisionColour(const Colour& col) const;

	int getWidth() const {return size.x;}
	int getHeight() const {return size.y;}

	#ifdef _DEBUG
	virtual string debugInfo();
	#endif

	bool enabled;

protected:
	virtual void doTrigger(const UnitCollisionEntry& entry);

	Vector2di size;
	Vector2di screenPosition;
	Colour triggerCol;
	vector<BaseUnit*> targets;
	vector<string> targetIDs;
	vector<BaseUnit*> activators;
	vector<string> activatorIDs;
	PARAMETER_TYPE targetParam;

	bool autoReEnable;
	int enableTimer;
	bool actionHitTarget; // if true action can be performed on the hitUnit entry if targets is empty

	enum BaseTriggerProps
	{
		bpEnabled=BaseUnit::upEOL,
		bpAction,
		bpActivator,
		bpAutoReEnable,
		bpEOL
	};
private:

};


#endif // BASE_TRIGGER_H

