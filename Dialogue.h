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

#ifndef DIALOGUE_H
#define DIALOGUE_H

#include <map>
#include <SDL/SDL.h>

#include "PenjinTypes.h"
#include "CountDown.h"
#include "Rectangle.h"
#include "Text.h"

#define DIALOGUE Dialogue::getDialogue()

class BaseUnit;

class Dialogue
{
private:
	Dialogue();
	static Dialogue *self;
public:
	virtual ~Dialogue();
	static Dialogue* getDialogue();

	bool loadFromFile(CRstring filename);
	void clear();

	void update();
	void render();

	string getLine(CRstring key) const;
	void queueLine(CRstring key, const BaseUnit* const unit, CRint time);

	string errorString;

protected:
	struct DialogueItem
	{
		SDL_Surface* surf;
		int time;
		string key;
	};

	std::map<string,string> lines;
	CountDown timer;
	Rectangle rect;
	Text text;
	vector<DialogueItem> queue;
private:

};

#endif // DIALOGUE_H

