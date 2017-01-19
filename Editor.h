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
	void inputMessageBox();

	void updateStart();

	void renderStart();
	void renderSettings();
	void renderFlags();
	void renderDraw();
	void renderUnits();
	void renderTest();
	void renderMenu();
	void renderFileList();
	void renderMessageBox();

	int save(); // 0 - saved successfully, non-zero - error

	void goToMenu();
	void goToFileList(string path, string filters, string *target);
	void showMessageBox(string message, string buttons);
	void switchState(int toState); // Change editor state, do clean-up and preparation
	void switchDrawTool(int newTool);

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
	vector<string> flagsItems;
	vector<string> menuItems;
	vector<string> messageBoxItems;

	/// Startup + Settings
	string loadFile;
	AnimatedSprite bg;
	Vector2di lastPos;
	bool mouseInBounds;
	Text entriesText;
	Text menuText; // Used for multiple menus
	SDL_Rect rect; // Dummy rect object used for drawing
	AnimatedSprite arrows;
	int mouseOnScrollItem; // 0 - not, 1 - on bar, 2 - up button, 3 - down button
	int startSel;
	int settingsSel;
	int settingsOffset;
	int flagsSel;
	int flagsOffset;
	string filename;
	bool editingFlags; // Intermediate state on settings state used for level flags
	bool inputVecXCoord; // True when user is editing the first (X) coordinate of a vector, false when second (Y) coordinate. Checked in combination with keyboard input
	string vecInputTemp;
	string keyboardInputBackup; // Used to revert keyboard input, when user cancels out
	string musicFile;
	/// Draw
	int drawTool;
	enum _DrawTool
	{
		 dtNone=0
		,dtBrush
		,dtCrop
		,dtStamp
		,dtSelect
		,dtBucket
	};
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
	SDL_Rect selectArea; // Position in game coordinates
	Vector2di selectAnchorPos; // Position of first click, used when moving to top or left
	SDL_Surface *copyBuffer;
	bool gridActive;
	int gridSize;
	int snapDistance; // Distance in which brush will snap to grid (in pixels), based on grid size (this is used in the snapping code)
	int snapDistancePercent; // In Percent. This will be set by the user in the tool settings panel and then converted to snapDistance in pixels
	Vector2di straightLinePos; // Mouse starting position for straight lines (shift modifier)
	int straightLineDirection; // 0 - none, 1 - undecided, 2 - horizontal, 3 - vertical
	Text panelText;
	string panelInputTemp; // Temporary string for holding keyboard input in panels
	string panelInputBackup; // Backup string for keybaord input in panels (holds starting input, used for reset when user cancels keyboard input)
	int panelActiveSlider; // 0 - none, 1 - colour_red, 2 - colour_green, 3 - colour_blue, 4 - brushSize, 5 - gridSize, 6 - snapDistance
	int panelInputTarget; // 0 - none, 1 - colour_red, 2 - colour_green, 3 - colour_blue, 4 - brushSize, 5 - gridSize, 6 - snapDistance
	EditorPanel colourPanel;
	bool drawUnits;
	void drawColourPanel(SDL_Surface *target);
	EditorPanel toolPanel;
	AnimatedSprite toolButtons;
	void drawToolPanel(SDL_Surface *target);
	EditorPanel toolSettingPanel;
	void drawToolSettingPanel(SDL_Surface *target);
	EditorPanel stampPanel;
	int stampButtonHover;
	int stampButtonSelected;
	SDL_Surface	*currentStamp;
	vector<SDL_Surface*> stampImagesGlobal;     // Full scale stamps, owned by surface cache
	vector<SDL_Surface*> stampThumbnailsGlobal; // For menu buttons scaled to 32x32px, order corresponding to stampImages, not cached, owned by Editor class
	vector<string> stampImageFilenamesGlobal;
	vector<SDL_Surface*> stampImagesChapter;    // Chapter specific stamps
	vector<SDL_Surface*> stampThumbnailsChapter;
	vector<string> stampImageFilenamesChapter;
	FileLister stampImageLister;
	void generateStampListing(FileLister *lister, string path, vector<SDL_Surface*> &stampTarget, vector<SDL_Surface*> &thumbnailTarget, vector<string> &filenameTarget);
	void drawStampPanel(SDL_Surface *target);
	/// Units
	EditorPanel unitPanel;
	AnimatedSprite unitButtons;
	int unitButtonHover; // Mouse-over unit button index
	int unitButtonSelected; // Clicked unit button index
	BaseUnit *currentUnit;
	vector<BaseUnit*> selectedUnits;
	bool movingCurrentUnit; // Set to true when user clicks on a selected unit to drag-move it, set to false when LMB is released
	bool currentUnitPlaced; // True when the currently selected unit has been freshly created and not placed in the level yet
	Vector2di unitMoveMouseOffset; // Offset of mousePos to the center of the unit, when the user clicks on a unit to move it
	void drawUnitPanel(SDL_Surface *target);
	EditorPanel paramsPanel;
	void drawParamsPanel(SDL_Surface *target);
	int paramsSel; // Selected unit parameter index
	int paramsOffset; // Draw offset for the params list in pixels (all others are in # of menu items)
	vector<int> paramsYPos; // List of vertical starting positions of unit parameters (used for rect-checks when user clicks on panel). Set in panel draw function
	int paramsSize; // Height of all unit parameters (used for sizing scrollbar for example). Set in panel draw function
	bool addingParam; // Indicating that user is currently adding a new unit parameter
	string addingParamTemp;
	/// Menu
	SDL_Surface *menuBg;
	int menuSel;
	bool menuActive;
	/// File Lister
	FileLister fileList;
	bool fileListActive;
	int fileListOffset;
	int fileListSel;
	string *fileListTarget;
	/// Message Box
	bool messageBoxActive;
	int messageBoxResult;
	int messageBoxSel;
	string messageBoxContent;
	SDL_Surface *messageBoxBg;
	Text messageBoxText;
};


#endif // EDITOR_H

