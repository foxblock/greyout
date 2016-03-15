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

struct EditorPanel
{
	SDL_Surface *surf;
	Vector2di pos;
	bool active;
	bool transparent;
	bool userIsInteracting;
	bool changed;
};

class Editor : public BaseState
{
public:
	Editor();
	virtual ~Editor();

	void userInput();
	void update();
	void render();

#ifdef _DEBUG
	string debugInfo();
	string debugString;
	Text debugText;
#endif
	Level *l;

private:
	void inputStart();
	void inputSettings();
	void inputFlags(); // Child of inputSettings
	void inputDraw();
	void inputUnits();
	void inputTest();
	void inputMenu();
	void inputFileList();

	void updateStart();

	void renderStart();
	void renderSettings();
	void renderFlags();
	void renderDraw();
	void renderUnits();
	void renderTest();
	void renderMenu();
	void renderFileList();

	void save();

	void goToMenu();
	void setUpFileList(string path, string filters, string *target);

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
		,esMenu
	};

	vector<string> startItems;
	vector<string> settingsItems;
	vector<string> flagsItems;
	vector<string> menuItems;

	/// Startup + Settings
	string loadFile;

	AnimatedSprite bg;
	Vector2di lastPos;
	bool mouseInBounds;
	Text entriesText;
	Text menuText;
	SDL_Rect rect; // Dummy rect object used for drawing
	AnimatedSprite arrows;
	int mouseOnScrollItem; // 0 - not, 1 - on bar, 2 - up button, 3 - down button
	int startSel;
	int settingsSel;
	int settingsOffset;
	int flagsSel;
	int flagsOffset;
	string filename;
	bool editingFlags;
	bool inputVecXCoord;
	string vecInputTemp;
	string vecInputBackup;
	FileLister fileList;
	bool fileListActive;
	int fileListOffset;
	int fileListSel;
	string *fileListTarget;
	string musicFile;
	/// Draw
	bool ownsImage; // Set on creating a new blank level, when the image is not loaded from the cache and therefore has to be destroyed manually
	Colour brushCol;
	Colour brushCol2;
	int brushSize;
	Vector2di mousePos; // Current mouse pos, adjusted for snapping to grid and other stuff
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
	int snapDistance; // Distance in which brush will snap to grid (in pixels), based on grid size
	Vector2di straightLinePos; // Mouse starting position for straight lines (shift modifier)
	int straightLineDirection; // 0 - none, 1 - undecided, 2 - horizontal, 3 - vertical
	Text panelText;
	string panelInputTemp;
	string panelInputBackup;
	int panelActiveSlider; // 0 - none, 1 - colour_red, 2 - colour_green, 3 - colour_blue
	int panelInputTarget; // 0 - none, 1 - colour_red, 2 - colour_green, 3 - colour_blue
	EditorPanel colourPanel;
	bool drawUnits;
	void drawColourPanel(SDL_Surface *target);
	/// Units
	EditorPanel unitPanel;
	AnimatedSprite unitButtons;
	int hoverUnitButton;
	int selectedUnitButton;
	BaseUnit *currentUnit;
	bool movingCurrentUnit;
	bool currentUnitPlaced;
	Vector2di unitMoveMouseOffset;
	void drawUnitPanel(SDL_Surface *target);
	EditorPanel paramsPanel;
	void drawParamsPanel(SDL_Surface *target);
	int paramsSel;
	int paramsOffset; // in pixels (all others are in # of menu items)
	vector<int> paramsYPos;
	int paramsSize;
	vector<PARAMETER_TYPE > currentParamsBackup;
	/// Menu
	SDL_Surface *menuBg;
	int menuSel;
};


#endif // EDITOR_H

