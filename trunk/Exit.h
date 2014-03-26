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

#ifndef EXIT_H
#define EXIT_H

#include "BaseUnit.h"

class Exit : public BaseUnit
{
public:
    Exit(Level* newParent);
    virtual ~Exit();

    virtual bool load(list<PARAMETER_TYPE >& params);
    virtual bool processParameter(const PARAMETER_TYPE& param);
    virtual void reset();

    virtual void update();

    virtual void hitUnit(const UnitCollisionEntry& entry);

    bool isExiting;
    bool allExited;
    set<BaseUnit*> keys;

protected:
	enum ExitProp {
		epLink=BaseUnit::upEOL,
		epEOL
	};

	bool checkAllExited() const;

    vector<BaseUnit*> targets;
    vector<string> targetIDs;

	int linkTimer;
	int lastKeys;
private:

};


#endif // EXIT_H

