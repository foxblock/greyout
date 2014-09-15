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

#ifndef _KEY_H
#define _KEY_H

#include "BaseUnit.h"

class Key : public BaseUnit
{
public:
	Key(Level* newParent);
	virtual ~Key();

	virtual bool load(list<PARAMETER_TYPE >& params);
	virtual bool processParameter(const PARAMETER_TYPE& value);
	virtual void update();
	virtual void reset();


	virtual void hitUnit(const UnitCollisionEntry& entry);

protected:
	vector<BaseUnit*> targets;
	vector<string> targetIDs;

private:

};


#endif // _KEY_H
