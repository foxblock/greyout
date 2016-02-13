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

#ifndef SIMPLEFLAGS_H
#define SIMPLEFLAGS_H

/**
Combining several binary values in one int container
**/

class SimpleFlags
{
public:
	SimpleFlags() {flags = 0;}
	~SimpleFlags() {}

	int flags;

	void clear() {flags = 0;}
	bool empty() {return (flags == 0);}

	/// All of the following functions also work with multiple (combined) flags passed as "var"
	// Checks whether the passed flag is in flags
	inline bool hasFlag(const int &var) const
	{
		if (var <= 0)
			return false;
		return ((flags & var) == var);
	};

	// Adds a flag to flags
	inline void addFlag(const int &var)
	{
		if (var <= 0)
			return;
		flags = (flags | var);
	};

	// Removes a flag from var (making sure it was actually present)
	inline void removeFlag(const int &var)
	{
		if (var <= 0)
			return;
		flags = (flags ^ (flags & var));
	};

	// If the flag is set it will be removed, else it will be set
	inline void switchFlag(const int &var)
	{
		if (var <= 0)
			return;
		flags = (flags ^ var);
	}
};

#endif // SIMPLEFLAGS_H
