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
	void render();

#ifdef _DEBUG
	string debugInfo();
#endif

private:
	void inputStart();
	void inputSettings();
	void inputDraw();
	void inputUnits();
	void inputTest();

	void renderStart();
	void renderSettings();
	void renderDraw();
	void renderUnits();
	void renderTest();

	int editorState;
	int lastState; // jump to out of test play
	enum _EditorState
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

	/// Startup + Settings
	AnimatedSprite bg;
	Vector2di lastPos;
	bool mouseInBounds;
	Text entriesText;
	Text menuText;
	SDL_Rect rect;
	AnimatedSprite arrows;
	int startSel;
	int settingsSel;
	/// Draw
	bool ownsImage; // Set on creating a new blank level, when the image is not loaded from the cache and therefore has to be destroyed manually
	Colour brushCol;
	int brushSize;
	SDL_Rect brushRect;
	Vector2di editorOffset; // Offset of the window relative to the level image
	Vector2di cropSize; // Size of the crop rectangle
	Vector2di cropOffset; // Offset of the upper left corner of the crop offset relative to the level image
	Vector2di mouseCropOffset; // Offset of the mouse to the crop rectangle when moving an edge
	SimpleDirection cropEdge; // Currently selected edge or side to crop, diNONE if no edge is selected (mouse not down), diMIDDLE if mouse down, but no valid edge selected
	int drawTool;
	enum _DrawTool
	{
		 dtNone=0
		,dtBrush
		,dtCrop
	};
	bool gridActive;
	int gridSize;
	int snapDistance;
	Vector2di mousePos;
};


#endif // EDITOR_H

