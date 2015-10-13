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

#ifndef USERSTATES_H
#define USERSTATES_H

// A list of identifiers for the states
// You may include this in any state you need to manually transition to another
// state by a call to setNextState
enum STATE_MODES
{
    STATE_ERROR,
	STATE_TITLE,
	STATE_MAIN,
	STATE_LEVEL,
	STATE_NEXT, // special state indicating the switch to the next level in a chapter (not an actual state)
	STATE_BENCHMARK,
	STATE_LEVELSELECT,
	STATE_CHAPTERTRIAL,
	STATE_EDITOR
};

#endif	//	USERSTATES_H
