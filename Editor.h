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

#ifndef EDITOR_H
#define EDITOR_H

#include "Level.h"

class Editor : public Level
{
public:
	Editor();
	virtual ~Editor();

	void userInput();
	void update();
	void render(SDL_Surface* screen);

#ifdef _DEBUG
	string debugInfo();
#endif

private:
	void inputStart();
	void inputSettings();
	void inputDraw();
	void inputUnits();
	void inputTest();

	void renderStart(SDL_Surface *screen);
	void renderSettings(SDL_Surface *screen);
	void renderDraw(SDL_Surface *screen);
	void renderUnits(SDL_Surface *screen);
	void renderTest(SDL_Surface *screen);

	int editorState;
	int lastState; // jump to out of test play
	enum EditorState
	{
		 esNone = 0
		,esStart
		,esSettings
		,esDraw
		,esUnits
		,esTest
	};

	vector<string> startItems;
	vector<string> settingsItems;

	AnimatedSprite bg;
	Vector2di lastPos;
	bool mouseInBounds;
	Text entriesText;
	Text menuText;
	SDL_Rect rect;
	AnimatedSprite arrows;
	int startSel;
	int settingsSel;

	string testString;
};


#endif // EDITOR_H

