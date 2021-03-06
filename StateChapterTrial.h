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

#ifndef STATECHAPTERTRIAL_H
#define STATECHAPTERTRIAL_H

#include "BaseState.h"
#include "Text.h"
#include "AnimatedSprite.h"
#include "Rectangle.h"

class StateChapterTrial : public BaseState
{
	public:
		StateChapterTrial();
		virtual ~StateChapterTrial();

		virtual void userInput();
		virtual void update();
		virtual void render();

	protected:
		Text timeTrialText;
		AnimatedSprite bg;
		int menuSelection;
		Vector2di lastPos;
		Rectangle menuRect;
		Text menuText;
		vector<string> pauseItems;
		bool newRecord;
		int timeDisplay;
	private:

};

#endif // STATECHAPTERTRIAL_H
