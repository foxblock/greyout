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

#include "Editor.h"

#include "userStates.h"
#include "gameDefines.h"
#include "globalControls.h"
#include "SurfaceCache.h"
#include "BaseUnit.h"
#include "ControlUnit.h"
#include "LevelLoader.h"
#include "fileTypeDefines.h"
#include "Physics.h"

#include "IMG_savepng.h"
#include <SDL/SDL_gfxPrimitives.h>
#include <iostream>
#include <fstream>

#define EDITOR_HEADLINE_SIZE 72
#define EDITOR_TEXT_SIZE 48
#define EDITOR_RECT_HEIGHT 35
#define EDITOR_CHECK_HEIGHT 27
#define EDITOR_MENU_SPACING 10
#define EDITOR_SETTINGS_OFFSET_Y 20
#define EDITOR_ENTRY_SIZE 400
#define EDITOR_VEC_ENTRY_SIZE 147
#define EDITOR_RETURN_Y_POS 400
#define EDITOR_MENU_OFFSET_X 20
#define EDITOR_MENU_SPACING_EXTRA 10
#define EDITOR_MAX_MENU_ITEMS_SCREEN 10

#define EDITOR_DEFAULT_WIDTH 400
#define EDITOR_DEFAULT_HEIGHT 240
#define EDITOR_MIN_WIDTH 32
#define EDITOR_MIN_HEIGHT 32
#define EDITOR_CROP_RECT_HALFHEIGHT 10 // The small box drawn in the middle of the crop rectangle
#define EDITOR_CROP_RECT_WIDTH 5
#define EDITOR_CROP_COLOUR 0x3399FFFF // SDL_gfx expects colour values to be in RGBA
#define EDITOR_GRID_COLOUR 0xCCCCCCFF
#define EDITOR_GRID_SNAP 0.2f // Fractions of gridSize
#define EDITOR_GRID_SPACING 3 // Spacing of grid dots in pixels (0 = solid line)
#define EDITOR_PANEL_TEXT_SIZE 12
#define EDITOR_SLIDER_HEIGHT 16
#define EDITOR_SLIDER_WIDTH 128
#define EDITOR_SLIDER_INDICATOR_WIDTH 2

#define EDITOR_COLOUR_PANEL_WIDTH 212
#define EDITOR_COLOUR_PANEL_HEIGHT 84
#define EDITOR_COLOUR_PANEL_SPACING 4 // Border around UI elements in pixels
#define EDITOR_COLOUR_PANEL_OFFSET 48 // X-Offset of the colour sliders

#define EDITOR_UNIT_PANEL_WIDTH 200
#define EDITOR_UNIT_PANEL_HEIGHT 380
#define EDITOR_UNIT_PANEL_SPACING 4
#define EDITOR_UNIT_BUTTON_SIZE 32
#define EDITOR_UNIT_BUTTON_BORDER 4
#define EDITOR_UNIT_BUTTONS_IMAGE "images/general/editor_buttons.png"
#define EDITOR_UNIT_PLAYER_START 0
#define EDITOR_UNIT_PLAYER_COUNT 4
#define EDITOR_UNIT_UNITS_START 16
#define EDITOR_UNIT_UNITS_COUNT 14
#define EDITOR_UNIT_TRIGGER_START 48
#define EDITOR_UNIT_TRIGGER_COUNT 6

#define EDITOR_PARAMS_PANEL_WIDTH 200
#define EDITOR_PARAMS_PANEL_HEIGHT 300
#define EDITOR_PARAMS_SPACING 4

Editor::Editor()
{
	l = NULL;
	#ifdef _DEBUG
	debugText.loadFont(DEBUG_FONT,8);
	debugText.setColour(50,217,54);
	debugString = "";
	#endif

	editorState = esStart;
	lastState = esStart;
	GFX::showCursor(true);

	bg.loadFrames(SURFACE_CACHE->loadSurface("images/menu/error_bg_800_480.png"),1,1,0,0);
	bg.disableTransparentColour();
	bg.setPosition(0,0);

	menuText.loadFont(GAME_FONT, EDITOR_TEXT_SIZE);
	menuText.setColour(WHITE);
	menuText.setAlignment(LEFT_JUSTIFIED);
	menuText.setUpBoundary(Vector2di(GFX::getXResolution(), GFX::getYResolution()));
	entriesText.loadFont(GAME_FONT, EDITOR_TEXT_SIZE);
	entriesText.setColour(WHITE);
	entriesText.setAlignment(CENTRED);
	entriesText.setUpBoundary(Vector2di((int)GFX::getXResolution() - EDITOR_MENU_OFFSET_X, GFX::getYResolution()));
	startItems.push_back("NEW LEVEL");
	startItems.push_back("LOAD LEVEL");
	startItems.push_back("NEW CHAPTER");
	startItems.push_back("LOAD CHAPTER");
	settingsItems.push_back("NAME:");
	settingsItems.push_back("FILENAME:");
	settingsItems.push_back("EDIT FLAGS");
	settingsItems.push_back("MUSIC:");
	settingsItems.push_back("CAM OFFSET:");
	settingsItems.push_back("BACKGROUND:");
	settingsItems.push_back("BOUNDARIES:");
	settingsItems.push_back("DIALOGUE:");
	settingsItems.push_back("GRAVITY:");
	settingsItems.push_back("MAX SPEED:");
	settingsItems.push_back("SAVE");
	settingsItems.push_back("CONTINUE");
	flagsItems.push_back("SCROLL X:");
	flagsItems.push_back("SCROLL Y:");
	flagsItems.push_back("REPEAT X:");
	flagsItems.push_back("REPEAT Y:");
	flagsItems.push_back("DISABLE SWAP:");
	flagsItems.push_back("KEEP CENTRED:");
	flagsItems.push_back("DRAW PATTERN:");
	flagsItems.push_back("CYCLE PLAYERS:");
	flagsItems.push_back("SCALE X:");
	flagsItems.push_back("SCALE Y:");
	flagsItems.push_back("SPLIT X:");
	flagsItems.push_back("SPLIT Y:");
	flagsItems.push_back("BACK");
	startSel = 0;
	settingsSel = 0;
	settingsOffset = 0;
	flagsSel = 0;
	flagsOffset = 0;
	editingFlags = false;
	inputVecXCoord = false;

	ownsImage = false;
	brushCol.setColour(BLACK);
	brushCol2.setColour(WHITE);
	brushSize = 32;
	mousePos.x = 0;
	mousePos.y = 0;
	editorOffset.x = 0;
	editorOffset.y = 0;
	cropSize.x = 0;
	cropSize.y = 0;
	cropOffset.x = 0;
	cropOffset.y = 0;
	mouseCropOffset.x = 0;
	mouseCropOffset.y = 0;
	cropEdge = diNONE;
	drawTool = dtBrush;
	gridActive = false;
	gridSize = 32;
	snapDistance = gridSize * EDITOR_GRID_SNAP;
	straightLinePos.x = 0;
	straightLinePos.y = 0;
	straightLineDirection = 0;
	panelText.loadFont(GAME_FONT, EDITOR_PANEL_TEXT_SIZE);
	panelText.setColour(WHITE);
	panelText.setAlignment(LEFT_JUSTIFIED);
	panelInputTemp = "0";
	panelActiveSlider = 0;
	panelInputTarget = 0;
	colourPanel.surf = SDL_CreateRGBSurface(SDL_SWSURFACE, EDITOR_COLOUR_PANEL_WIDTH, EDITOR_COLOUR_PANEL_HEIGHT, GFX::getVideoSurface()->format->BitsPerPixel, 0, 0, 0, 0);
	colourPanel.pos.x = GFX::getXResolution() - EDITOR_COLOUR_PANEL_WIDTH;
	colourPanel.pos.y = GFX::getYResolution() - EDITOR_COLOUR_PANEL_HEIGHT;
	colourPanel.active = false;
	colourPanel.transparent = false;
	colourPanel.userIsInteracting = false;
	colourPanel.changed = false;
	drawUnits = false;

	unitPanel.surf = SDL_CreateRGBSurface(SDL_SWSURFACE, EDITOR_UNIT_PANEL_WIDTH, EDITOR_UNIT_PANEL_HEIGHT, GFX::getVideoSurface()->format->BitsPerPixel, 0, 0, 0, 0);
	unitPanel.pos.x = 0;
	unitPanel.pos.y = 50;
	unitPanel.active = false;
	unitPanel.transparent = false;
	unitPanel.userIsInteracting = false;
	unitPanel.changed = false;
	unitButtons.loadFrames(SURFACE_CACHE->loadSurface(EDITOR_UNIT_BUTTONS_IMAGE), 8, 8, 0, 0, true);
	unitButtons.setTransparentColour(MAGENTA);
	hoverUnitButton = -1;
	selectedUnitButton = -1;
	currentUnit = NULL;
	movingCurrentUnit = false;
	currentUnitPlaced = false;
	unitMoveMouseOffset.x = 0;
	unitMoveMouseOffset.y = 0;
	paramsPanel.surf = SDL_CreateRGBSurface(SDL_SWSURFACE, EDITOR_PARAMS_PANEL_WIDTH, EDITOR_PARAMS_PANEL_HEIGHT, GFX::getVideoSurface()->format->BitsPerPixel, 0, 0, 0, 0);
	paramsPanel.pos.x = GFX::getXResolution() - EDITOR_PARAMS_PANEL_WIDTH;
	paramsPanel.pos.y = 0;
	paramsPanel.active = false;
	paramsPanel.transparent = false;
	paramsPanel.userIsInteracting = false;
	paramsPanel.changed = false;
}

Editor::~Editor()
{
	if (ownsImage)
		SDL_FreeSurface(l->levelImage);
	delete l;
	if (colourPanel.surf)
		SDL_FreeSurface(colourPanel.surf);
	if (unitPanel.surf)
		SDL_FreeSurface(unitPanel.surf);
}

///---public---

/*
		// Code to spawn a box example for creating a new unit)
		list<PARAMETER_TYPE > params;
		Vector2df pos = drawOffset + input->getMouse() - Vector2df(16,16);
		params.push_back(make_pair("class","pushablebox"));
		params.push_back(make_pair("collision","0,255"));
		params.push_back(make_pair("size","32,32"));
		params.push_back(make_pair("position",StringUtility::vecToString(pos)));
		BaseUnit* box = LEVEL_LOADER->createUnit(params,this);
		units.push_back(box);

		// Draw a Rect

			Rectangle* temp = new Rectangle;
			temp->setColour(WHITE);
			temp->setDimensions(24,24);
			temp->setPosition(input->getMouse() - Vector2df(12,12));
			mouseRects.push_back(temp);

		// Code to save PNG
		if (input->isKey("F2"))
		{
			IMG_SavePNG("images/test.png", collisionLayer, IMG_COMPRESS_MAX);
		}
*/

void Editor::userInput()
{
	switch (editorState)
	{
	case esStart:
		inputStart();
		break;
	case esSettings:
		if (editingFlags)
			inputFlags();
		else
			inputSettings();
		break;
	case esDraw:
		inputDraw();
		break;
	case esUnits:
		inputUnits();
		break;
	case esTest:
		inputTest();
		break;
	default:
		return;
	}
}

void Editor::update()
{
#ifdef _DEBUG
	debugString = debugInfo();
#endif

	switch (editorState)
	{
	case esStart:
	{
		break;
	}
	case esSettings:
	{
		break;
	}
	case esDraw:
	{
		break;
	}
	case esUnits:
	{
		break;
	}
	case esTest:
	{
		break;
	}
	default:
		return;
	}
}

void Editor::render()
{
	switch (editorState)
	{
	case esStart:
		renderStart();
		break;
	case esSettings:
		if (editingFlags)
			renderFlags();
		else
			renderSettings();
		break;
	case esDraw:
		renderDraw();
		break;
	case esUnits:
		renderUnits();
		break;
	case esTest:
		renderTest();
		break;
	default:
		return;
	}

#ifdef _DEBUG
	hlineColor(GFX::getVideoSurface(), 0, GFX::getXResolution()-1, input->getMouseY(), 0xFF0000AA);
	vlineColor(GFX::getVideoSurface(), input->getMouseX(), 0, GFX::getYResolution()-1, 0xFF0000AA);
	debugText.setPosition(10,10);
	debugText.print(debugString);
#endif
}

#ifdef _DEBUG
string Editor::debugInfo()
{
	switch (editorState)
	{
		case esStart:
			return "STARTUP\n";
		case esSettings:
			return "LEVEL SETTINGS\n";
		case esDraw:
			return "DRAW MODE\nBrush: " + StringUtility::intToString(brushSize) + "\nGrid: " + StringUtility::intToString(gridSize) + "\n";
		case esUnits:
			return "UNIT MODE\n";
		case esTest:
			return "TEST PLAY\n" + l->debugInfo();
		default:
			return "ERROR\n";
	}
}
#endif

///---protected---

///---private---

void Editor::inputStart()
{
	mousePos = input->getMouse();
	if (input->isUp())
	{
		startSel -= (startSel > 0) ? 1 : 0;
	}
	else if (input->isDown())
	{
		startSel += (startSel < startItems.size()-1) ? 1 : 0;
	}

	if (mousePos != lastPos)
	{
		int pos = (GFX::getYResolution() - EDITOR_MENU_SPACING * (startItems.size()-1) - EDITOR_RECT_HEIGHT * startItems.size()) / 2;
		int temp = -1;
		for (int I = 0; I < startItems.size(); ++I)
		{
			if (mousePos.y >= pos && mousePos.y < pos + EDITOR_RECT_HEIGHT)
			{
				temp = I;
				break;
			}
			pos += EDITOR_RECT_HEIGHT + EDITOR_MENU_SPACING;
		}
		if ( temp != -1 )
		{
			startSel = temp;
			mouseInBounds = true;
		}
		else
			mouseInBounds = false;

		lastPos = mousePos;
	}
	if (isAcceptKey(input) || (input->isLeftClick() && mouseInBounds))
	{
		switch (startSel)
		{
		case 0: // New level
			editorState = esSettings;
			ownsImage = true;
			l = new Level();
			l->levelImage = SDL_CreateRGBSurface(SDL_SWSURFACE, EDITOR_DEFAULT_WIDTH, EDITOR_DEFAULT_HEIGHT,
					GFX::getVideoSurface()->format->BitsPerPixel, 0, 0, 0, 0);
			SDL_FillRect(l->levelImage, NULL, -1);
			editorOffset.x = ((int)l->levelImage->w - (int)GFX::getXResolution()) / 2;
			editorOffset.y = ((int)l->levelImage->h - (int)GFX::getYResolution()) / 2;
			break;
		case 1: // Open level
		{
			editorState = esSettings;
			ownsImage = false;
			string temp = "levels/playground.txt";
			l = LEVEL_LOADER->loadLevelFromFile(temp);
			if (!l)
			{
				printf("ERROR: Failed to load level file in editor: %s", temp.c_str());
				input->resetKeys();
				setNextState(STATE_MAIN);
			}
			break;
		}
		case 2: // New chapter
			break;
		case 3: // Open chapter
			break;
		default:
			break;
		}
		GFX::showCursor(true);
	}
	else if (isCancelKey(input))
	{
		input->resetKeys();
		setNextState(STATE_MAIN);
	}
	input->resetKeys();
}

void Editor::inputSettings()
{
	if (input->isPollingKeyboard())
	{
		if (input->isLeft() && !inputVecXCoord)
		{
			if (settingsSel == 4)
			{
				l->drawOffset.y = StringUtility::stringToFloat(vecInputTemp);
				vecInputTemp = vecInputBackup = StringUtility::floatToString(l->drawOffset.x);
			}
			else if (settingsSel == 8)
			{
				PHYSICS->gravity.y = StringUtility::stringToFloat(vecInputTemp);
				vecInputTemp = vecInputBackup = StringUtility::floatToString(PHYSICS->gravity.x);
			}
			else if (settingsSel == 9)
			{
				PHYSICS->maximum.y = StringUtility::stringToFloat(vecInputTemp);
				vecInputTemp = vecInputBackup = StringUtility::floatToString(PHYSICS->maximum.x);
			}
			inputVecXCoord = true;
			input->resetLeft();
		}
		if (input->isRight() && inputVecXCoord)
		{
			if (settingsSel == 4)
			{
				l->drawOffset.x = StringUtility::stringToFloat(vecInputTemp);
				vecInputTemp = vecInputBackup = StringUtility::floatToString(l->drawOffset.y);
			}
			else if (settingsSel == 8)
			{
				PHYSICS->gravity.x = StringUtility::stringToFloat(vecInputTemp);
				vecInputTemp = vecInputBackup = StringUtility::floatToString(PHYSICS->gravity.y);
			}
			else if (settingsSel == 9)
			{
				PHYSICS->maximum.x = StringUtility::stringToFloat(vecInputTemp);
				vecInputTemp = vecInputBackup = StringUtility::floatToString(PHYSICS->maximum.y);
			}
			inputVecXCoord = false;
			input->resetRight();
		}
		if (isAcceptKey(input))
		{
			input->stopKeyboardInput();
			if (settingsSel == 4)
			{
				if (inputVecXCoord)
					l->drawOffset.x = StringUtility::stringToFloat(vecInputTemp);
				else
					l->drawOffset.y = StringUtility::stringToFloat(vecInputTemp);
			}
			else if (settingsSel == 8)
			{
				if (inputVecXCoord)
					PHYSICS->gravity.x = StringUtility::stringToFloat(vecInputTemp);
				else
					PHYSICS->gravity.y = StringUtility::stringToFloat(vecInputTemp);
			}
			else if (settingsSel == 9)
			{
				if (inputVecXCoord)
					PHYSICS->maximum.x = StringUtility::stringToFloat(vecInputTemp);
				else
					PHYSICS->maximum.y = StringUtility::stringToFloat(vecInputTemp);
			}
			inputVecXCoord = true;
		}
		else if (isCancelKey(input))
		{
			input->stopKeyboardInput();
			if (settingsSel == 4)
			{
				if (inputVecXCoord)
					l->drawOffset.x = StringUtility::stringToFloat(vecInputBackup);
				else
					l->drawOffset.y = StringUtility::stringToFloat(vecInputBackup);
			}
			else if (settingsSel == 8)
			{
				if (inputVecXCoord)
					PHYSICS->gravity.x = StringUtility::stringToFloat(vecInputBackup);
				else
					PHYSICS->gravity.y = StringUtility::stringToFloat(vecInputBackup);
			}
			else if (settingsSel == 9)
			{
				if (inputVecXCoord)
					PHYSICS->maximum.x = StringUtility::stringToFloat(vecInputBackup);
				else
					PHYSICS->maximum.y = StringUtility::stringToFloat(vecInputBackup);
			}
			inputVecXCoord = true;
		}
		input->resetKeys();
		return;
	}

	int pos = EDITOR_SETTINGS_OFFSET_Y;
	mouseInBounds = false;
	if (input->getMouse() != lastPos || input->isLeftClick() || input->isRightClick())
	{
		for (int I = settingsOffset; I < min((int)settingsItems.size(), settingsOffset + EDITOR_MAX_MENU_ITEMS_SCREEN); ++I)
		{
			// Check Y-Position - Mouse is on menu item vertically
			if (input->getMouseY() >= pos && input->getMouseY() < pos + EDITOR_RECT_HEIGHT)
			{
				settingsSel = I;
				// Check X-Position depending on menu item
				if (I == 6) // Checkbox
				{
					int temp = (int)GFX::getXResolution() - EDITOR_ENTRY_SIZE / 2 - EDITOR_RECT_HEIGHT / 2 - EDITOR_MENU_OFFSET_X;
					mouseInBounds = (input->getMouseX() >= temp && input->getMouseX() < temp + EDITOR_RECT_HEIGHT);
				}
				else if (I == 4 || I == 8 || I == 9)
				{
					int temp = (int)GFX::getXResolution() - EDITOR_ENTRY_SIZE - EDITOR_MENU_OFFSET_X + EDITOR_TEXT_SIZE;
					if (input->getMouseX() >= temp && input->getMouseX() < temp + EDITOR_VEC_ENTRY_SIZE)
					{
						mouseInBounds = true;
						inputVecXCoord = true;
					}
					else if (input->getMouseX() >= temp + EDITOR_VEC_ENTRY_SIZE + EDITOR_TEXT_SIZE + EDITOR_MENU_SPACING && input->getMouseX() < temp + EDITOR_VEC_ENTRY_SIZE * 2 + EDITOR_TEXT_SIZE + EDITOR_MENU_SPACING)
					{
						mouseInBounds = true;
						inputVecXCoord = false;
					}
				}
				else
					mouseInBounds = true;
			}
//			if (I == settingsItems.size()-3)
//				pos = EDITOR_RETURN_Y_POS - EDITOR_RECT_HEIGHT - EDITOR_MENU_SPACING;
//			else
				pos += EDITOR_RECT_HEIGHT + EDITOR_MENU_SPACING;
		}
		lastPos = input->getMouse();
	}

	if (input->isUp() && settingsSel > 0)
	{
		--settingsSel;
		if (settingsSel < settingsOffset)
			--settingsOffset;
		input->resetUp();
	}
	else if (input->isDown() && settingsSel < settingsItems.size() - 1)
	{
		++settingsSel;
		if (settingsSel >= settingsOffset + EDITOR_MAX_MENU_ITEMS_SCREEN)
			++settingsOffset;
		input->resetDown();
	}
	if (input->getMouseWheelDelta())
	{
		settingsOffset -= input->getMouseWheelDelta();
		if (settingsOffset < 0)
			settingsOffset = 0;
		else if (settingsOffset > settingsItems.size() - EDITOR_MAX_MENU_ITEMS_SCREEN)
			settingsOffset = settingsItems.size() - EDITOR_MAX_MENU_ITEMS_SCREEN;
		input->resetMouseWheel();
	}

	if(input->isLeft())
	{
		//
		input->resetLeft();
	}
	else if(input->isRight())
	{
		//
		input->resetRight();
	}

	if (isAcceptKey(input) || (input->isLeftClick() && mouseInBounds))
	{
//// Input Code for selection box
//		if (sel == 0)
//		{
//			if(mousePos.x > (int)GFX::getXResolution() - EDITOR_ENTRY_SIZE - EDITOR_MENU_OFFSET_X &&
//					mousePos.x < (int)GFX::getXResolution() - EDITOR_ENTRY_SIZE * 0.85f - EDITOR_MENU_OFFSET_X)
//				setDrawPattern(getDrawPattern() - 1);
//			else if(mousePos.x > (int)GFX::getXResolution() - EDITOR_ENTRY_SIZE * 0.15f - EDITOR_MENU_OFFSET_X &&
//					mousePos.x < (int)GFX::getXResolution() - EDITOR_MENU_OFFSET_X)
//				setDrawPattern(getDrawPattern() + 1);
//		}
		switch (settingsSel)
		{
		case 0: // Name
			input->pollKeyboardInput(&l->name, KEYBOARD_MASK_ASCII);
			break;
		case 1: // Filename
			input->pollKeyboardInput(&filename, KEYBOARD_MASK_ASCII);
			break;
		case 2: // Flags
			editingFlags = true;
			break;
		case 3: // Music
			break;
		case 4: // Offset
			if (inputVecXCoord)
				vecInputTemp = vecInputBackup = StringUtility::floatToString(l->drawOffset.x);
			else
				vecInputTemp = vecInputBackup = StringUtility::floatToString(l->drawOffset.y);
			input->pollKeyboardInput(&vecInputTemp, KEYBOARD_MASK_FLOAT);
			break;
		case 5: // Background colour
			break;
		case 6: // Bounmdaries
			l->cam.disregardBoundaries = !(l->cam.disregardBoundaries);
			break;
		case 7: // Dialogue
			break;
		case 8: // Gravity
			if (inputVecXCoord)
				vecInputTemp = vecInputBackup = StringUtility::floatToString(PHYSICS->gravity.x);
			else
				vecInputTemp = vecInputBackup = StringUtility::floatToString(PHYSICS->gravity.y);
			input->pollKeyboardInput(&vecInputTemp, KEYBOARD_MASK_FLOAT);
			break;
		case 9: // Terminal velocity
			if (inputVecXCoord)
				vecInputTemp = vecInputBackup = StringUtility::floatToString(PHYSICS->maximum.x);
			else
				vecInputTemp = vecInputBackup = StringUtility::floatToString(PHYSICS->maximum.y);
			input->pollKeyboardInput(&vecInputTemp, KEYBOARD_MASK_FLOAT);
			break;
		case 10: // Save
			save();
			break;
		case 11: // Continue
			editorState = esDraw;
			GFX::showCursor(drawTool != dtBrush);
			break;
		default:
			break;
		}
		input->resetKeys();
	}
	else if (isCancelKey(input))
	{
		input->resetKeys();
		setNextState(STATE_MAIN);
	}
}

void Editor::inputFlags()
{
	// TODO: Mouse-Input für die Scrollbar. Variablen einführen: mouseOnScrollItem (0 - not, 1 - leiste, 2 - oberer button, 3 - unterer button)
	// TODO: Grafik von Scrollbar entsprechend der mouseOnScrollItem Variable anpassen/invertieren
	// TODO: Input für die anderen Menü-Items in X-Richtung beschränken
	int pos = EDITOR_SETTINGS_OFFSET_Y;
	mouseInBounds = false;
	if (input->getMouse() != lastPos || input->isLeftClick() || input->isRightClick())
	{
		for (int I = flagsOffset; I < min((int)flagsItems.size(), flagsOffset + EDITOR_MAX_MENU_ITEMS_SCREEN); ++I)
		{
			if (input->getMouseY() >= pos && input->getMouseY() < pos + EDITOR_RECT_HEIGHT)
			{
				// Last item in list is always back button
				if (I == flagsOffset + EDITOR_MAX_MENU_ITEMS_SCREEN - 1)
				{
					flagsSel = flagsItems.size() - 1;
					mouseInBounds = true;
				}
				else if (input->getMouseX() < GFX::getXResolution() - EDITOR_RECT_HEIGHT - EDITOR_MENU_SPACING)
				{
					flagsSel = I;
					// Check whether mouse is on checkbox or back button area
					int temp = (int)GFX::getXResolution() - EDITOR_ENTRY_SIZE / 2 - EDITOR_RECT_HEIGHT / 2 - EDITOR_MENU_OFFSET_X;
					if (input->getMouseX() >= temp && input->getMouseX() < temp + EDITOR_RECT_HEIGHT)
						mouseInBounds = true;
					break;
				}
			}
				pos += EDITOR_RECT_HEIGHT + EDITOR_MENU_SPACING;
		}
		lastPos = input->getMouse();
	}

	if (input->isUp() && flagsSel > 0)
	{
		--flagsSel;
		if (flagsSel < flagsOffset)
			--flagsOffset;
		else if (flagsSel >= flagsOffset + EDITOR_MAX_MENU_ITEMS_SCREEN - 1) // Coming from fixed "back item" after moving there with the mouse
			flagsOffset = flagsSel - EDITOR_MAX_MENU_ITEMS_SCREEN + 2;
		input->resetUp();
	}
	else if (input->isDown() && flagsSel < flagsItems.size() - 1)
	{
		++flagsSel;
		if (flagsSel >= flagsOffset + EDITOR_MAX_MENU_ITEMS_SCREEN - 1 && flagsSel != flagsItems.size()-1)
			++flagsOffset;
		input->resetDown();
	}
	if (input->getMouseWheelDelta())
	{
		flagsOffset -= input->getMouseWheelDelta();
		if (flagsOffset < 0)
			flagsOffset = 0;
		else if (flagsOffset > flagsItems.size() - EDITOR_MAX_MENU_ITEMS_SCREEN)
			flagsOffset = flagsItems.size() - EDITOR_MAX_MENU_ITEMS_SCREEN;
		input->resetMouseWheel();
	}

	if(input->isLeft() || input->isRight())
	{
		switch (flagsSel)
		{
		case 0: // scroll x
			l->flags.switchFlag(Level::lfScrollX);
			break;
		case 1: // scroll y
			l->flags.switchFlag(Level::lfScrollY);
			break;
		case 2: // repeat x
			l->flags.switchFlag(Level::lfRepeatX);
			break;
		case 3: // repeat y
			l->flags.switchFlag(Level::lfRepeatY);
			break;
		case 4: // disable swap
			l->flags.switchFlag(Level::lfDisableSwap);
			break;
		case 5: // keep centred
			l->flags.switchFlag(Level::lfKeepCentred);
			break;
		case 6: // draw pattern
			l->flags.switchFlag(Level::lfDrawPattern);
			break;
		case 7: // cycle players
			l->flags.switchFlag(Level::lfCyclePlayers);
			break;
		case 8: // scale x
			l->flags.switchFlag(Level::lfScaleX);
			break;
		case 9: // scale y
			l->flags.switchFlag(Level::lfScaleY);
			break;
		case 10: // split x
			l->flags.switchFlag(Level::lfSplitX);
			break;
		case 11: // split y
			l->flags.switchFlag(Level::lfSplitY);
			break;
		default:
			break;
		}
		input->resetLeft();
		input->resetRight();
	}

	if (isAcceptKey(input) || (input->isLeftClick() && mouseInBounds))
	{
		switch (flagsSel)
		{
		case 0: // scroll x
			l->flags.switchFlag(Level::lfScrollX);
			break;
		case 1: // scroll y
			l->flags.switchFlag(Level::lfScrollY);
			break;
		case 2: // repeat x
			l->flags.switchFlag(Level::lfRepeatX);
			break;
		case 3: // repeat y
			l->flags.switchFlag(Level::lfRepeatY);
			break;
		case 4: // disable swap
			l->flags.switchFlag(Level::lfDisableSwap);
			break;
		case 5: // keep centred
			l->flags.switchFlag(Level::lfKeepCentred);
			break;
		case 6: // draw pattern
			l->flags.switchFlag(Level::lfDrawPattern);
			break;
		case 7: // cycle players
			l->flags.switchFlag(Level::lfCyclePlayers);
			break;
		case 8: // scale x
			l->flags.switchFlag(Level::lfScaleX);
			break;
		case 9: // scale y
			l->flags.switchFlag(Level::lfScaleY);
			break;
		case 10: // split x
			l->flags.switchFlag(Level::lfSplitX);
			break;
		case 11: // split y
			l->flags.switchFlag(Level::lfSplitY);
			break;
		case 12:
			editingFlags = false;
			break;
		default:
			break;
		}
		input->resetKeys();
	}
	else if (isCancelKey(input))
	{
		editingFlags = false;
		input->resetKeys();
	}
}

void Editor::inputDraw()
{
	/// Button handling
	if (!input->isPollingKeyboard())
	{
		// Hotkeys
		if (isCancelKey(input))
		{
			panelActiveSlider = 0;
			colourPanel.userIsInteracting = false;
			editorState = esSettings;
			GFX::showCursor(true);
			input->resetKeys();
			return;
		}
		if (isAcceptKey(input))
		{
			panelActiveSlider = 0;
			colourPanel.userIsInteracting = false;
			editorState = esUnits;
			GFX::showCursor(true);
			input->resetKeys();
			return;
		}
		if (input->isLeft())
			editorOffset.x -= 2;
		else if (input->isRight())
			editorOffset.x += 2;
		if (input->isUp())
			editorOffset.y -= 2;
		else if (input->isDown())
			editorOffset.y += 2;

		if (input->isKey("F6") && !colourPanel.userIsInteracting)
		{
			colourPanel.active = !colourPanel.active;
			if (colourPanel.active)
				drawColourPanel(colourPanel.surf);
			input->resetKeys();
		}
		if (input->isKey("b"))
		{
			drawTool = dtBrush;
			GFX::showCursor(false);
			input->resetKeys();
		}
		else if (input->isKey("c"))
		{
			drawTool = dtCrop;
			GFX::showCursor(true);
			cropOffset.x = 0;
			cropOffset.y = 0;
			cropSize.x = l->levelImage->w;
			cropSize.y = l->levelImage->h;
			input->resetKeys();
		}
		if (input->isKey("g"))
		{
			gridActive = !gridActive;
			input->resetKeys();
		}
		if (input->isKey("m"))
		{
			++brushSize;
			input->resetKeys();
		}
		if (input->isKey("n"))
		{
			if (brushSize > 1)
				--brushSize;
			input->resetKeys();
		}
		if (input->isKey("u"))
		{
			drawUnits = !drawUnits;
			input->resetKeys();
		}
		if (input->isKey("v"))
		{
			if (brushCol == BLACK)
				brushCol.setColour(WHITE);
			else if (brushCol == WHITE)
				brushCol.setColour(147, 149, 152);
			else if (brushCol == Colour(147, 149, 152))
				brushCol.setColour(RED);
			else
				brushCol.setColour(BLACK);
			colourPanel.changed = true;
			input->resetKeys();
		}
		if (input->isKey("x"))
		{
			Colour temp(brushCol);
			brushCol.setColour(brushCol2);
			brushCol2.setColour(temp);
			colourPanel.changed = true;
			input->resetKeys();
		}
	}
	else
	{
		// Keyboard input
		if (input->keyboardBufferHasChanged())
		{
			switch (panelInputTarget)
			{
			case 1:
				brushCol.red = std::min(StringUtility::stringToInt(panelInputTemp), 255);
				panelInputTemp = StringUtility::intToString(brushCol.red);
				colourPanel.changed = true;
				break;
			case 2:
				brushCol.green = std::min(StringUtility::stringToInt(panelInputTemp), 255);
				panelInputTemp = StringUtility::intToString(brushCol.green);
				colourPanel.changed = true;
				break;
			case 3:
				brushCol.blue = std::min(StringUtility::stringToInt(panelInputTemp), 255);
				panelInputTemp = StringUtility::intToString(brushCol.blue);
				colourPanel.changed = true;
				break;
			}
		}
		else if (input->isKey("RETURN"))
		{
			input->stopKeyboardInput();
			if (panelInputTarget >= 1 && panelInputTarget <= 3)
			{
				colourPanel.userIsInteracting = false;
				colourPanel.changed = true;
			}
			panelInputTarget = 0;
			input->resetKeys();
		}
		else if (input->isKey("ESCAPE"))
		{
			input->stopKeyboardInput();
			switch (panelInputTarget)
			{
			case 1:
				brushCol.red = std::min(StringUtility::stringToInt(panelInputBackup), 255);
				colourPanel.changed = true;
				colourPanel.userIsInteracting = false;
				break;
			case 2:
				brushCol.green = std::min(StringUtility::stringToInt(panelInputBackup), 255);
				colourPanel.changed = true;
				colourPanel.userIsInteracting = false;
				break;
			case 3:
				brushCol.blue = std::min(StringUtility::stringToInt(panelInputBackup), 255);
				colourPanel.changed = true;
				colourPanel.userIsInteracting = false;
				break;
			}
			panelInputTarget = 0;
			input->resetKeys();
		}
		mousePos = input->getMouse();
		return; // Skip over mouse handling entirely
	}
	/// Mouse position and button handling starts here (might be skipped if keyboard is being polled)
	mousePos = input->getMouse();
	/// Panel interaction
	if (colourPanel.active && (colourPanel.userIsInteracting || mousePos.inRect(colourPanel.pos.x, colourPanel.pos.y, EDITOR_COLOUR_PANEL_WIDTH, EDITOR_COLOUR_PANEL_HEIGHT)))
	{
		// Make transparent if user is drawing/cropping over panel, else show cursor
		if ((input->isLeftClick() == SimpleJoy::sjHELD || input->isRightClick() == SimpleJoy::sjHELD) && !colourPanel.userIsInteracting)
		{
			SDL_SetAlpha(colourPanel.surf, SDL_SRCALPHA, 128);
			colourPanel.transparent = true;
		}
		else
		{
			GFX::showCursor(true);
		}
		if (input->isLeftClick() && !colourPanel.transparent) // user is actually interacting with the panel contents
		{
			mousePos -= colourPanel.pos;
			if (input->isLeftClick() == SimpleJoy::sjPRESSED) // Initial mouse press on the panel
			{
				// Text fields
				if (mousePos.x >= EDITOR_COLOUR_PANEL_OFFSET + EDITOR_SLIDER_WIDTH + EDITOR_SLIDER_INDICATOR_WIDTH + EDITOR_COLOUR_PANEL_SPACING &&
					mousePos.x < EDITOR_COLOUR_PANEL_WIDTH - EDITOR_COLOUR_PANEL_SPACING)
				{
					if (mousePos.y >= EDITOR_COLOUR_PANEL_SPACING && mousePos.y < EDITOR_COLOUR_PANEL_SPACING + EDITOR_SLIDER_HEIGHT)
					{
						panelInputTarget = 1;
						panelInputTemp = panelInputBackup = StringUtility::intToString(brushCol.red);
					}
					else if (mousePos.y >= EDITOR_COLOUR_PANEL_SPACING * 2 + EDITOR_SLIDER_HEIGHT && mousePos.y < EDITOR_COLOUR_PANEL_SPACING * 2 + EDITOR_SLIDER_HEIGHT * 2)
					{
						panelInputTarget = 2;
						panelInputTemp = panelInputBackup = StringUtility::intToString(brushCol.green);
					}
					else if (mousePos.y >= EDITOR_COLOUR_PANEL_SPACING * 3 + EDITOR_SLIDER_HEIGHT * 2 && mousePos.y < EDITOR_COLOUR_PANEL_SPACING * 3 + EDITOR_SLIDER_HEIGHT * 3)
					{
						panelInputTarget = 3;
						panelInputTemp = panelInputBackup = StringUtility::intToString(brushCol.blue);
					}
					if (panelInputTarget > 0)
					{
						input->pollKeyboardInput(&panelInputTemp, KEYBOARD_MASK_NUMBERS);
						colourPanel.userIsInteracting = true;
						colourPanel.changed = true;
						mousePos += colourPanel.pos;
						return;
					}
				}
				// Sliders
				if (mousePos.x >= EDITOR_COLOUR_PANEL_OFFSET && mousePos.x < EDITOR_COLOUR_PANEL_OFFSET + EDITOR_SLIDER_WIDTH + EDITOR_SLIDER_INDICATOR_WIDTH)
				{
					if (mousePos.y >= EDITOR_COLOUR_PANEL_SPACING && mousePos.y < EDITOR_COLOUR_PANEL_SPACING + EDITOR_SLIDER_HEIGHT)
						panelActiveSlider = 1;
					else if (mousePos.y >= EDITOR_COLOUR_PANEL_SPACING * 2 + EDITOR_SLIDER_HEIGHT && mousePos.y < EDITOR_COLOUR_PANEL_SPACING * 2 + EDITOR_SLIDER_HEIGHT * 2)
						panelActiveSlider = 2;
					else if (mousePos.y >= EDITOR_COLOUR_PANEL_SPACING * 3 + EDITOR_SLIDER_HEIGHT * 2 && mousePos.y < EDITOR_COLOUR_PANEL_SPACING * 3 + EDITOR_SLIDER_HEIGHT * 3)
						panelActiveSlider = 3;
				}
				if (mousePos.y >= EDITOR_COLOUR_PANEL_SPACING * 4 + EDITOR_SLIDER_HEIGHT * 3 && mousePos.y < EDITOR_COLOUR_PANEL_SPACING * 4 + EDITOR_SLIDER_HEIGHT * 4)
				{
					if ((mousePos.x - EDITOR_COLOUR_PANEL_SPACING) % (EDITOR_COLOUR_PANEL_SPACING + EDITOR_SLIDER_HEIGHT) < EDITOR_SLIDER_HEIGHT)
					{
						brushCol.setColour(GFX::getPixel(colourPanel.surf, mousePos.x, mousePos.y));
						colourPanel.changed = true;
					}
				}
				colourPanel.userIsInteracting = true;
			}
			switch (panelActiveSlider)
			{
			case 1:
				brushCol.red = std::min(std::max((int)ceil((mousePos.x - EDITOR_COLOUR_PANEL_OFFSET - EDITOR_SLIDER_INDICATOR_WIDTH / 2.0f) / EDITOR_SLIDER_WIDTH * 255), 0), 255);
				colourPanel.changed = true;
				break;
			case 2:
				brushCol.green = std::min(std::max((int)ceil((mousePos.x - EDITOR_COLOUR_PANEL_OFFSET - EDITOR_SLIDER_INDICATOR_WIDTH / 2.0f) / EDITOR_SLIDER_WIDTH * 255), 0), 255);
				colourPanel.changed = true;
				break;
			case 3:
				brushCol.blue = std::min(std::max((int)ceil((mousePos.x - EDITOR_COLOUR_PANEL_OFFSET - EDITOR_SLIDER_INDICATOR_WIDTH / 2.0f) / EDITOR_SLIDER_WIDTH * 255), 0), 255);
				colourPanel.changed = true;
				break;
			}
			mousePos += colourPanel.pos;
			return; // Skip over rest of mouse handling
		}
		else
		{
			panelActiveSlider = 0;
			colourPanel.userIsInteracting = false;
		}
	}
	else
	{
		GFX::showCursor(drawTool != dtBrush);
	}
	// Reset panel transparency
	if (colourPanel.active && colourPanel.transparent && ((!input->isLeftClick() && !input->isRightClick()) || !mousePos.inRect(colourPanel.pos.x, colourPanel.pos.y, EDITOR_COLOUR_PANEL_WIDTH, EDITOR_COLOUR_PANEL_HEIGHT)))
	{
		SDL_SetAlpha(colourPanel.surf, 0, 255);
		colourPanel.transparent = false;
	}
	/// Drawing area interaction (might be skipped if user is interacting with any panel)
	// Snap mouse to grid (changes mousePos)
	// Everything before works with the real mousePos, everything after works with a new virtual/aligned mousePos
	if (gridActive)
	{
		if (drawTool == dtBrush)
		{
			mousePos.x -= brushSize / 2;
			mousePos.y -= brushSize / 2;
			int temp = (mousePos.x + editorOffset.x) % gridSize;
			if (temp != 0) // Possibly need to make a correction
			{
				if (temp < 0) // Wrap negative values (left of level area)
					temp += gridSize;
				if (temp <= snapDistance) // If in snap distance right side of grid line
					mousePos.x -= temp;
				else if (gridSize - temp <= snapDistance) // In snap distance left side of grid line
					mousePos.x += gridSize - temp;
				else // If not snapped to left of brush, check right side of brush
				{
					temp = (mousePos.x + editorOffset.x + brushSize) % gridSize;
					if (temp < 0)
						temp += gridSize;
					if (temp <= snapDistance)
						mousePos.x -= temp;
					else if (gridSize - temp <= snapDistance)
						mousePos.x += gridSize - temp;
				}
			}
			temp = (mousePos.y + editorOffset.y) % gridSize;
			if (temp != 0)
			{
				if (temp < 0) // Wrap negative values (top of level area)
					temp += gridSize;
				if (temp <= snapDistance)
					mousePos.y -= temp;
				else if (gridSize - temp <= snapDistance)
					mousePos.y += gridSize - temp;
				else
				{
					temp = (mousePos.y + editorOffset.y + brushSize) % gridSize;
					if (temp < 0)
						temp += gridSize;
					if (temp <= snapDistance)
						mousePos.y -= temp;
					else if (gridSize - temp <= snapDistance)
						mousePos.y += gridSize - temp;
				}
			}
			mousePos.x += brushSize / 2;
			mousePos.y += brushSize / 2;
		} // brush
		else if (drawTool == dtCrop)
		{
			if (cropEdge == diLEFT || cropEdge == diRIGHT)
			{
				int temp = (mousePos.x + editorOffset.x - mouseCropOffset.x) % gridSize;
				if (temp < 0)
					temp += gridSize;
				if (temp <= snapDistance)
					mousePos.x -= temp;
				else if (gridSize - temp <= snapDistance)
					mousePos.x += gridSize - temp;
			}
			else if (cropEdge == diTOP || cropEdge == diBOTTOM)
			{
				int temp = (mousePos.y + editorOffset.y - mouseCropOffset.y) % gridSize;
				if (temp < 0)
					temp += gridSize;
				if (temp <= snapDistance)
					mousePos.y -= temp;
				else if (gridSize - temp <= snapDistance)
					mousePos.y += gridSize - temp;
			}
		} // crop
	} // grid active
	// Mouse button handling
	if (drawTool == dtBrush)
	{
		if ((input->isLeftClick() == SimpleJoy::sjPRESSED || input->isRightClick() == SimpleJoy::sjPRESSED) && !input->isKey("LEFT_SHIFT") && !input->isKey("RIGHT_SHIFT"))
			lastPos = mousePos; // Reset lastPos on first press, except when user explicitly wants to connect the dots
		if (input->isLeftClick() || input->isRightClick())
		{
			// Straight lines
			if (input->isKey("LEFT_SHIFT") || input->isKey("RIGHT_SHIFT"))
			{
				if (straightLineDirection == 0)
				{
					straightLinePos = mousePos;
					straightLineDirection = 1;
				}
				else if (straightLineDirection == 1)
				{
					if (std::abs(straightLinePos.x - mousePos.x) > std::abs(straightLinePos.y - mousePos.y))
						straightLineDirection = 2;
					else if (std::abs(straightLinePos.x - mousePos.x) < std::abs(straightLinePos.y - mousePos.y))
						straightLineDirection = 3;
				}
				else
				{
					if (straightLineDirection == 2)
						mousePos.y = straightLinePos.y;
					else
						mousePos.x = straightLinePos.x;
				}
			}
			else
			{
				straightLineDirection = 0;
			}
			mousePos.x -= brushSize / 2;
			mousePos.y -= brushSize / 2;
			lastPos.x -= brushSize / 2;
			lastPos.y -= brushSize / 2;
			Sint16 polX[6];
			Sint16 polY[6];
			if (mousePos.x < lastPos.x)
			{
				polX[0] = lastPos.x + editorOffset.x + brushSize - 1;
				polY[0] = lastPos.y + editorOffset.y;
				polX[2] = mousePos.x + editorOffset.x;
				polY[2] = mousePos.y + editorOffset.y;
				polX[3] = mousePos.x + editorOffset.x;
				polY[3] = mousePos.y + editorOffset.y + brushSize - 1;
				polX[5] = lastPos.x + editorOffset.x + brushSize - 1;
				polY[5] = lastPos.y + editorOffset.y + brushSize - 1;
				if (mousePos.y < lastPos.y)
				{
					polX[1] = mousePos.x + editorOffset.x + brushSize - 1;
					polY[1] = mousePos.y + editorOffset.y;
					polX[4] = lastPos.x + editorOffset.x;
					polY[4] = lastPos.y + editorOffset.y + brushSize - 1;
				}
				else
				{
					polX[1] = lastPos.x + editorOffset.x;
					polY[1] = lastPos.y + editorOffset.y;
					polX[4] = mousePos.x + editorOffset.x + brushSize - 1;
					polY[4] = mousePos.y + editorOffset.y + brushSize - 1;
				}
			}
			else
			{
                polX[0] = lastPos.x + editorOffset.x;
                polY[0] = lastPos.y + editorOffset.y + brushSize - 1;
                polX[2] = mousePos.x + editorOffset.x + brushSize - 1;
                polY[2] = mousePos.y + editorOffset.y + brushSize - 1;
                polX[3] = mousePos.x + editorOffset.x + brushSize - 1;
                polY[3] = mousePos.y + editorOffset.y;
                polX[5] = lastPos.x + editorOffset.x;
                polY[5] = lastPos.y + editorOffset.y;
                if (mousePos.y < lastPos.y)
				{
					polX[1] = lastPos.x + editorOffset.x + brushSize - 1;
					polY[1] = lastPos.y + editorOffset.y + brushSize - 1;
					polX[4] = mousePos.x + editorOffset.x;
					polY[4] = mousePos.y + editorOffset.y;
				}
				else
				{
					polX[1] = mousePos.x + editorOffset.x;
					polY[1] = mousePos.y + editorOffset.y + brushSize - 1;
					polX[4] = lastPos.x + editorOffset.x + brushSize - 1;
					polY[4] = lastPos.y + editorOffset.y;
				}
			}
			if (input->isLeftClick())
				filledPolygonColor(l->levelImage, polX, polY, 6, brushCol.getRGBAvalue());
			else
				filledPolygonColor(l->levelImage, polX, polY, 6, brushCol2.getRGBAvalue());
			mousePos.x += brushSize / 2;
			mousePos.y += brushSize / 2;
			lastPos.x += brushSize / 2;
			lastPos.y += brushSize / 2;
		} // left or right click
		else
		{
			straightLineDirection = 0;
		}
	} // brush
	else if (drawTool == dtCrop)
	{
		if (input->isLeftClick())
		{
			if (cropEdge == diNONE) // selecting an edge
			{
				if (mousePos.inRect(-editorOffset.x + cropOffset.x - EDITOR_CROP_RECT_WIDTH - 1, -editorOffset.y + cropOffset.y, EDITOR_CROP_RECT_WIDTH + 1, cropSize.y))
				{
					cropEdge = diLEFT;
					mouseCropOffset.x = mousePos.x + editorOffset.x - cropOffset.x;
				}
				else if (mousePos.inRect(-editorOffset.x + cropOffset.x + cropSize.x, -editorOffset.y + cropOffset.y, EDITOR_CROP_RECT_WIDTH + 1, cropSize.y))
				{
					cropEdge = diRIGHT;
					mouseCropOffset.x = mousePos.x + editorOffset.x - cropOffset.x - cropSize.x;
				}
				else if (mousePos.inRect(-editorOffset.x + cropOffset.x, -editorOffset.y + cropOffset.y - EDITOR_CROP_RECT_WIDTH - 1, cropSize.x, EDITOR_CROP_RECT_WIDTH + 1))
				{
					cropEdge = diTOP;
					mouseCropOffset.y = mousePos.y + editorOffset.y - cropOffset.y;
				}
				else if (mousePos.inRect(-editorOffset.x + cropOffset.x, -editorOffset.y + cropOffset.y + cropSize.y, cropSize.x, EDITOR_CROP_RECT_WIDTH + 1))
				{
					cropEdge = diBOTTOM;
					mouseCropOffset.y = mousePos.y + editorOffset.y - cropOffset.y - cropSize.y;
				}
				else
				{
					cropEdge = diMIDDLE; // No valid edge
					mouseCropOffset.x = 0;
					mouseCropOffset.y = 0;
				}
			}
			else if (mousePos != lastPos) // moving an edge
			{
				switch (cropEdge.value)
				{
					case diLEFT: cropOffset.x += mousePos.x - lastPos.x; cropSize.x -= mousePos.x - lastPos.x; break;
					case diRIGHT: cropSize.x += mousePos.x - lastPos.x; break;
					case diTOP: cropOffset.y += mousePos.y - lastPos.y; cropSize.y -= mousePos.y - lastPos.y; break;
					case diBOTTOM: cropSize.y += mousePos.y - lastPos.y; break;
				}
				if (cropSize.x < 0)
				{
					cropOffset.x += cropSize.x;
					cropSize.x *= -1;
					cropEdge.makeOpposite();
				}
				if (cropSize.y < 0)
				{
					cropOffset.y += cropSize.y;
					cropSize.y *= -1;
					cropEdge.makeOpposite();
				}
			}
		}
		else if (cropEdge != diNONE)
		{
			if (cropSize.x < EDITOR_MIN_WIDTH)
			{
				if (cropEdge == diLEFT)
					cropOffset.x -= EDITOR_MIN_WIDTH - cropSize.x;
				cropSize.x = EDITOR_MIN_WIDTH;
			}
			if (cropSize.y < EDITOR_MIN_HEIGHT)
			{
				if (cropEdge == diTOP)
					cropOffset.y -= EDITOR_MIN_HEIGHT - cropSize.y;
				cropSize.y = EDITOR_MIN_HEIGHT;
			}
			cropEdge = diNONE;
		}
		if (isAcceptKey(input) && drawTool == dtCrop)
		{
			if (l->collisionLayer)
				SDL_FreeSurface(l->collisionLayer);
			l->collisionLayer = SDL_CreateRGBSurface(SDL_SWSURFACE, cropSize.x, cropSize.y, GFX::getVideoSurface()->format->BitsPerPixel, 0, 0, 0, 0);
			SDL_FillRect(l->collisionLayer, NULL, -1);
			SDL_Rect srcRect;
			srcRect.x = std::max(cropOffset.x, 0);
			srcRect.y = std::max(cropOffset.y, 0);
			srcRect.w = std::min(l->levelImage->w, l->collisionLayer->w);
			srcRect.h = std::min(l->levelImage->h, l->collisionLayer->h);
			SDL_Rect dstRect;
			dstRect.x = std::max(-cropOffset.x, 0);
			dstRect.y = std::max(-cropOffset.y, 0);
            SDL_BlitSurface(l->levelImage, &srcRect, l->collisionLayer, &dstRect);
            if (!ownsImage)
				SDL_FreeSurface(l->levelImage);
			l->levelImage = SDL_CreateRGBSurface(SDL_SWSURFACE, cropSize.x, cropSize.y, GFX::getVideoSurface()->format->BitsPerPixel, 0, 0, 0, 0);
			ownsImage = true;
			SDL_BlitSurface(l->collisionLayer, NULL, l->levelImage, NULL);
			editorOffset -= cropOffset;
			for (vector<BaseUnit*>::iterator I = l->units.begin(); I != l->units.end(); ++I)
			{
				(*I)->position -= cropOffset;
				(*I)->startingPosition -= cropOffset;
			}
			for (vector<ControlUnit*>::iterator I = l->players.begin(); I != l->players.end(); ++I)
			{
				(*I)->position -= cropOffset;
				(*I)->startingPosition -= cropOffset;
			}
			cropOffset.x = 0;
			cropOffset.y = 0;
			drawTool = dtBrush;
			GFX::showCursor(false);
			input->resetKeys();
		}
	} // crop
	if (input->isLeftClick() || input->isRightClick())
		lastPos = mousePos;
}

void Editor::inputUnits()
{
	/// Button handling
	if (!input->isPollingKeyboard())
	{
		if (isCancelKey(input))
		{
			editorState = esDraw;
			if (currentUnit && !currentUnitPlaced)
				delete currentUnit;
			currentUnit = NULL;
			GFX::showCursor(false);
			input->resetKeys();
		}
		if (input->isLeft())
			editorOffset.x -= 2;
		else if (input->isRight())
			editorOffset.x += 2;
		if (input->isUp())
			editorOffset.y -= 2;
		else if (input->isDown())
			editorOffset.y += 2;
		if (input->isKey("g"))
		{
			gridActive = !gridActive;
			input->resetKeys();
		}
		if (input->isKey("p"))
		{
			paramsPanel.active = !paramsPanel.active;
			if (paramsPanel.active)
				drawParamsPanel(paramsPanel.surf);
			input->resetKeys();
		}
		if (input->isKey("u"))
		{
			drawUnits = !drawUnits;
			input->resetKeys();
		}
		if (input->isKey("F7") && !unitPanel.userIsInteracting)
		{
			unitPanel.active = !unitPanel.active;
			if (unitPanel.active)
				drawUnitPanel(unitPanel.surf);
			input->resetKeys();
		}
		if (currentUnit)
		{
			if (input->isKey("DELETE") || input->isKey("BACKSPACE"))
			{
				for (vector<ControlUnit*>::iterator I = l->players.begin(); I != l->players.end(); ++I)
				{
					if (*I == currentUnit)
					{
						l->players.erase(I);
						break;
					}
				}
				for (vector<BaseUnit*>::iterator I = l->units.begin(); I != l->units.end(); ++I)
				{
					if (*I == currentUnit)
					{
						l->units.erase(I);
						break;
					}
				}
				delete currentUnit;
				currentUnit = NULL;
				paramsPanel.changed = true;
			}
		}
	}
	else
	{
		return; // Skip over mouse handling entirely
	}
	/// Mouse position and button handling starts here (might be skipped if keyboard is being polled)
	mousePos = input->getMouse();
	/// Panel interaction
	// Units panel
	if (unitPanel.active && (unitPanel.userIsInteracting || mousePos.inRect(unitPanel.pos.x, unitPanel.pos.y, EDITOR_UNIT_PANEL_WIDTH, EDITOR_UNIT_PANEL_HEIGHT)))
	{
		// Make transparent if user is drawing/cropping over panel, else show cursor
		if ((input->isLeftClick() == SimpleJoy::sjHELD || input->isRightClick() == SimpleJoy::sjHELD) && !unitPanel.userIsInteracting)
		{
			SDL_SetAlpha(unitPanel.surf, SDL_SRCALPHA, 128);
			unitPanel.transparent = true;
		}
		if (!unitPanel.transparent && (input->getMouse() != lastPos || input->isLeftClick() || input->isRightClick())) // user is actually interacting with the panel contents
		{
			Vector2di backupPos = mousePos;
			backupPos -= unitPanel.pos;
			hoverUnitButton = -1; // Reset selection
			float columns = EDITOR_UNIT_PANEL_WIDTH / (EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 + EDITOR_UNIT_PANEL_SPACING);
			for (int I = 0; I <= 2; ++I) // Go through categories (players, units, triggers)
			{
				int tempCount = 0;
				int tempOffset = 0;
				switch (I)
				{
					case 0: // Offset by selected unit text and "Players:" label
						backupPos.y -= (EDITOR_PANEL_TEXT_SIZE + EDITOR_UNIT_PANEL_SPACING) * 2;
						tempCount = EDITOR_UNIT_PLAYER_COUNT;
						tempOffset = EDITOR_UNIT_PLAYER_START;
						break;
					case 1: // Offset by player buttons and "units" label
						backupPos.y -= ceil(EDITOR_UNIT_PLAYER_COUNT / columns) * (EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 + EDITOR_UNIT_PANEL_SPACING) + (EDITOR_PANEL_TEXT_SIZE + EDITOR_UNIT_PANEL_SPACING);
						tempCount = EDITOR_UNIT_UNITS_COUNT;
						tempOffset = EDITOR_UNIT_UNITS_START;
						break;
					case 2: // Offset by unit buttons and "triggers" label
						backupPos.y -= ceil(EDITOR_UNIT_UNITS_COUNT / columns) * (EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 + EDITOR_UNIT_PANEL_SPACING) + (EDITOR_PANEL_TEXT_SIZE + EDITOR_UNIT_PANEL_SPACING);
						tempCount = EDITOR_UNIT_TRIGGER_COUNT;
						tempOffset = EDITOR_UNIT_TRIGGER_START;
						break;
				}
				if (backupPos.y < 0)
				{
					break; // outside of any buttons
				}
				else if (backupPos.y < ceil(tempCount / columns) * (EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 + EDITOR_UNIT_PANEL_SPACING))
				{
					backupPos.x -= EDITOR_UNIT_PANEL_SPACING; // Button x offset
					int x = backupPos.x / (EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 + EDITOR_UNIT_PANEL_SPACING);
					int y = backupPos.y / (EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 + EDITOR_UNIT_PANEL_SPACING);
					if (x < columns && y * columns + x < tempCount)
						hoverUnitButton = tempOffset + y * columns + x;
					break;
				}
			}
			if (input->isLeftClick() == SimpleJoy::sjPRESSED && hoverUnitButton != -1)
			{
				if (!currentUnitPlaced)
				{
					delete currentUnit;
					currentUnit = NULL;
				}
				list<PARAMETER_TYPE > params;
				switch (hoverUnitButton)
				{
					case  0:
						params.push_back(make_pair(CLASS_STRING, "black"));
						break;
					case  1:
						params.push_back(make_pair(CLASS_STRING, "white"));
						break;
					case  2:
						params.push_back(make_pair(CLASS_STRING, "black"));
						params.push_back(make_pair("imageoverwrite", "images/player/orange_big.png"));
						params.push_back(make_pair("collision", "black,0r51g255b,red"));
						params.push_back(make_pair("colour", "255r153g0b"));
						break;
					case  3:
						params.push_back(make_pair(CLASS_STRING, "black"));
						params.push_back(make_pair("imageoverwrite", "images/player/blue_big.png"));
						params.push_back(make_pair("collision", "black,255r153g0b,red"));
						params.push_back(make_pair("colour", "0r51g255b"));
						break;
					case 16:
						params.push_back(make_pair(CLASS_STRING, "pushablebox"));
						break;
					case 17:
						params.push_back(make_pair(CLASS_STRING, "solidbox"));
						break;
					case 18: // fading (b/w)
						params.push_back(make_pair(CLASS_STRING, "fadingbox"));
						params.push_back(make_pair("colour", "black"));
						params.push_back(make_pair("farcolour", "147r149g152b"));
						params.push_back(make_pair("faderadius", "48,72"));
						params.push_back(make_pair("fadesteps", "3"));
						params.push_back(make_pair("collision", "black"));
						break;
					case 19: // fading (orange)
						params.push_back(make_pair(CLASS_STRING, "fadingbox"));
						params.push_back(make_pair("colour", "255r153g0b"));
						params.push_back(make_pair("farcolour", "255r220g168b"));
						params.push_back(make_pair("faderadius", "58,80"));
						params.push_back(make_pair("fadesteps", "3"));
						params.push_back(make_pair("collision", "black,255r153g0b"));
						break;
					case 20:  // fading (blue)
						params.push_back(make_pair(CLASS_STRING, "fadingbox"));
						params.push_back(make_pair("colour", "0r51g255b"));
						params.push_back(make_pair("farcolour", "178r193g255b"));
						params.push_back(make_pair("faderadius", "58,80"));
						params.push_back(make_pair("fadesteps", "3"));
						params.push_back(make_pair("collision", "black,0r51g255b"));
						break;
					case 21:  // fading (orange blue mix)
						params.push_back(make_pair(CLASS_STRING, "fadingbox"));
						params.push_back(make_pair("colour", "black"));
						params.push_back(make_pair("farcolour", "216r206g212b"));
						params.push_back(make_pair("faderadius", "58,80"));
						params.push_back(make_pair("collision", "black,255r153g0b,0r51g255b"));
						break;
					case 22:
						params.push_back(make_pair(CLASS_STRING, "exit"));
						params.push_back(make_pair("collision", "black,white"));
						break;
					case 23:
						params.push_back(make_pair(CLASS_STRING, "key"));
						break;
					case 24:
						params.push_back(make_pair(CLASS_STRING, "switch"));
						break;
					case 25:
						params.push_back(make_pair(CLASS_STRING, "gear"));
						params.push_back(make_pair("colour", "black"));
						break;
					case 26:
						params.push_back(make_pair(CLASS_STRING, "gear"));
						params.push_back(make_pair("colour", "white"));
						break;
					case 27:
						params.push_back(make_pair(CLASS_STRING, "text"));
						params.push_back(make_pair("font", "fonts/lato.ttf"));
						params.push_back(make_pair("text", "YOUR TEXT HERE"));
						break;
					case 28:
						params.push_back(make_pair(CLASS_STRING, "emitter"));
						params.push_back(make_pair("direction", "0.5,-1"));
						params.push_back(make_pair("directionscatter", "20"));
						params.push_back(make_pair("power", "8"));
						params.push_back(make_pair("powerscatter", "4"));
						params.push_back(make_pair("delay", "5"));
						params.push_back(make_pair("delayscatter", "0"));
						params.push_back(make_pair("lifetime", "1000"));
						params.push_back(make_pair("collision", "black"));
						params.push_back(make_pair("colour", "black"));
						params.push_back(make_pair("multiplier", "10"));
						break;
					case 29:
						params.push_back(make_pair(CLASS_STRING, "controlsprite"));
						params.push_back(make_pair("startingstate", "jump"));
						break;
					case 48:
						params.push_back(make_pair(CLASS_STRING, "basetrigger"));
						break;
					case 49:
						params.push_back(make_pair(CLASS_STRING, "dialoguetrigger"));
						params.push_back(make_pair("time", "5000"));
						params.push_back(make_pair("textkey", "playground"));
						break;
					case 50:
						params.push_back(make_pair(CLASS_STRING, "exittrigger"));
						break;
					case 51:
						params.push_back(make_pair(CLASS_STRING, "soundtrigger"));
						params.push_back(make_pair("file", "sounds/die.wav"));
						break;
					case 52:
						params.push_back(make_pair(CLASS_STRING, "cameratrigger"));
						params.push_back(make_pair("destination", "128,128"));
						params.push_back(make_pair("time", "2000"));
						break;
					case 53:
						params.push_back(make_pair(CLASS_STRING, "leveltrigger"));
						params.push_back(make_pair("action", "gravity=0,0.5"));
						break;
					default:
						params.clear();
				}
				if (!params.empty())
				{
					params.push_back(make_pair("position",StringUtility::vecToString(editorOffset + Vector2di(GFX::getXResolution(), GFX::getYResolution())/2.0f)));
					if (hoverUnitButton >= EDITOR_UNIT_PLAYER_START && hoverUnitButton < EDITOR_UNIT_PLAYER_START + EDITOR_UNIT_PLAYER_COUNT)
						currentUnit = LEVEL_LOADER->createPlayer(params, l, -1);
					else
						currentUnit = LEVEL_LOADER->createUnit(params, l, -1);
				}
				if (currentUnit)
				{
					selectedUnitButton = hoverUnitButton;
					currentUnitPlaced = false;
				}
				else
					selectedUnitButton = -1;
			}
			if (input->isLeftClick() || input->isRightClick())
				unitPanel.userIsInteracting = true;
			else
				unitPanel.userIsInteracting = false;
			unitPanel.changed = true;
			return; // Skip over rest of mouse handling
		}
		else
		{
			unitPanel.userIsInteracting = false;
		}
	}
	// Reset panel transparency
	if (unitPanel.active && unitPanel.transparent && ((!input->isLeftClick() && !input->isRightClick()) || !mousePos.inRect(unitPanel.pos.x, unitPanel.pos.y, EDITOR_UNIT_PANEL_WIDTH, EDITOR_UNIT_PANEL_HEIGHT)))
	{
		SDL_SetAlpha(unitPanel.surf, 0, 255);
		unitPanel.transparent = false;
	}
	// END units panel
	// Params panel
	if (paramsPanel.active && (paramsPanel.userIsInteracting || mousePos.inRect(paramsPanel.pos.x, paramsPanel.pos.y, EDITOR_PARAMS_PANEL_WIDTH, EDITOR_PARAMS_PANEL_HEIGHT)))
	{
		// Make transparent if user is drawing/cropping over panel
		if ((input->isLeftClick() == SimpleJoy::sjHELD || input->isRightClick() == SimpleJoy::sjHELD) && !paramsPanel.userIsInteracting)
		{
			SDL_SetAlpha(paramsPanel.surf, SDL_SRCALPHA, 128);
			paramsPanel.transparent = true;
		}
		if (!paramsPanel.transparent && (input->getMouse() != lastPos || input->isLeftClick() || input->isRightClick())) // user is actually interacting with the panel contents
		{
			if (input->isLeftClick() || input->isRightClick())
				paramsPanel.userIsInteracting = true;
			else
				paramsPanel.userIsInteracting = false;
			return;
		}
	}
	if (paramsPanel.active && paramsPanel.transparent && ((!input->isLeftClick() && !input->isRightClick()) || !mousePos.inRect(paramsPanel.pos.x, paramsPanel.pos.y, EDITOR_PARAMS_PANEL_WIDTH, EDITOR_PARAMS_PANEL_HEIGHT)))
	{
		SDL_SetAlpha(paramsPanel.surf, 0, 255);
		paramsPanel.transparent = false;
	}
	// END params panel
	/// Drawing area interaction (might be skipped if user is interacting with any panel)
	// Snap mouse to grid
	if (gridActive)
	{
		if (currentUnit)
		{
			mousePos -= currentUnit->getSize() / 2;
			int temp = (mousePos.x + editorOffset.x) % gridSize;
			if (temp != 0) // Possibly need to make a correction
			{
				if (temp < 0) // Wrap negative values (left of level area)
					temp += gridSize;
				if (temp <= snapDistance) // If in snap distance right side of grid line
					mousePos.x -= temp;
				else if (gridSize - temp <= snapDistance) // In snap distance left side of grid line
					mousePos.x += gridSize - temp;
				else // If not snapped to left of brush, check right side of brush
				{
					temp = (mousePos.x + editorOffset.x + currentUnit->getWidth()) % gridSize;
					if (temp < 0)
						temp += gridSize;
					if (temp <= snapDistance)
						mousePos.x -= temp;
					else if (gridSize - temp <= snapDistance)
						mousePos.x += gridSize - temp;
				}
			}
			temp = (mousePos.y + editorOffset.y) % gridSize;
			if (temp != 0)
			{
				if (temp < 0) // Wrap negative values (top of level area)
					temp += gridSize;
				if (temp <= snapDistance)
					mousePos.y -= temp;
				else if (gridSize - temp <= snapDistance)
					mousePos.y += gridSize - temp;
				else
				{
					temp = (mousePos.y + editorOffset.y + currentUnit->getHeight()) % gridSize;
					if (temp < 0)
						temp += gridSize;
					if (temp <= snapDistance)
						mousePos.y -= temp;
					else if (gridSize - temp <= snapDistance)
						mousePos.y += gridSize - temp;
				}
			}
			mousePos += currentUnit->getSize() / 2;
		}
	}
	if (currentUnit)
	{
		// Mouse
		if (input->isLeftClick())
		{
			if (!currentUnitPlaced)
			{
				currentUnitPlaced = true;
				if (selectedUnitButton >= EDITOR_UNIT_PLAYER_START && selectedUnitButton < EDITOR_UNIT_PLAYER_START + EDITOR_UNIT_PLAYER_COUNT)
					l->players.push_back((ControlUnit*)currentUnit);
				else
					l->units.push_back(currentUnit);
				selectedUnitButton = -1;
				unitPanel.changed = true;
				input->resetMouseButtons();
			}
			else if (!movingCurrentUnit)
			{
				if ((mousePos + editorOffset).inRect(currentUnit->getRect()))
				{
					movingCurrentUnit = true;
					unitMoveMouseOffset = currentUnit->getPixel(diMIDDLE) - mousePos - editorOffset;
				}
				else
				{
					currentUnit = NULL;
				}
			}
		}
		else if (movingCurrentUnit)
		{
			unitMoveMouseOffset.x = 0;
			unitMoveMouseOffset.y = 0;
			movingCurrentUnit = false;
		}
		if (movingCurrentUnit || !currentUnitPlaced)
		{
			// Place selected unit (either new or old)
			currentUnit->position = mousePos + editorOffset + unitMoveMouseOffset - currentUnit->getSize() / 2.0f;
			currentUnit->startingPosition = currentUnit->position;
			currentUnit->generateParameters();
		}
		if (paramsPanel.active)
			paramsPanel.changed = true;
	}
	else
	{
		// Select already placed unit
		if (input->isLeftClick())
		{
			// Find and select unit under cursor
			Vector2df pos = mousePos + editorOffset;
			for (vector<BaseUnit*>::iterator I = l->units.begin(); I != l->units.end(); ++I)
			{
				if (pos.inRect((*I)->getRect()))
				{
					// If clicking on two overlapping units, always select smaller one
					if (currentUnit && currentUnit->getWidth() * currentUnit->getHeight() > (*I)->getWidth() * (*I)->getHeight())
					{
                        currentUnit = *I;
					}
					if (!currentUnit)
						currentUnit = *I;
				}
			}
			for (vector<ControlUnit*>::iterator I = l->players.begin(); I != l->players.end(); ++I)
			{
				if (pos.inRect((*I)->getRect()))
				{
					// If clicking on two overlapping units, always select smaller one
					if (currentUnit && currentUnit->getWidth() * currentUnit->getHeight() > (*I)->getWidth() * (*I)->getHeight())
					{
                        currentUnit = *I;
					}
					if (!currentUnit)
						currentUnit = *I;
				}
			}
			if (currentUnit)
			{
				currentUnitPlaced = true;
				currentUnit->generateParameters();
				if (paramsPanel.active)
					paramsPanel.changed = true;
			}
			input->resetMouseButtons();
		}
	}

	lastPos = mousePos;
}

void Editor::inputTest()
{
	if (input->isSelect())
		editorState = lastState;
	else
		l->userInput();
}

void Editor::renderStart()
{
	SDL_Surface *screen = GFX::getVideoSurface();
	bg.render(screen);

	int pos = (GFX::getYResolution() - startItems.size() * (EDITOR_RECT_HEIGHT + EDITOR_MENU_SPACING) + EDITOR_MENU_SPACING) / 2;
	for (int I = 0; I < startItems.size(); ++I)
	{
		rect.x = 0;
		rect.y = pos;
		rect.w = GFX::getXResolution();
		rect.h = EDITOR_RECT_HEIGHT;
		entriesText.setPosition(EDITOR_MENU_OFFSET_X / 2, pos + EDITOR_RECT_HEIGHT - EDITOR_TEXT_SIZE);
		if (I == startSel)
		{
			SDL_FillRect(screen, &rect, -1);
			entriesText.setColour(BLACK);
		}
		else
		{
			SDL_FillRect(screen, &rect, 0);
			entriesText.setColour(WHITE);
		}
		entriesText.print(startItems[I]);

		pos += EDITOR_RECT_HEIGHT + EDITOR_MENU_SPACING;
	}
}

void Editor::renderSettings()
{
	SDL_Surface *screen = GFX::getVideoSurface();
	bg.render(screen);

	int pos = EDITOR_SETTINGS_OFFSET_Y;
	for (int I = settingsOffset; I < min((int)settingsItems.size(), settingsOffset + EDITOR_MAX_MENU_ITEMS_SCREEN); ++I)
	{
		rect.x = 0;
		rect.y = pos;
		rect.w = GFX::getXResolution();
		rect.h = EDITOR_RECT_HEIGHT;
		// render text and selection
		menuText.setPosition(EDITOR_MENU_OFFSET_X, pos + EDITOR_RECT_HEIGHT - EDITOR_TEXT_SIZE);
		if (I == settingsSel) // active selection (swap colours)
		{
			if (input->isPollingKeyboard()) // Active input selection
			{
				SDL_FillRect(screen, &rect, 0);
				if (I == 0 || I == 1) // Text input
				{
					rect.x = (int)GFX::getXResolution() - EDITOR_ENTRY_SIZE - EDITOR_MENU_OFFSET_X;
					rect.w = EDITOR_ENTRY_SIZE;
					SDL_FillRect(screen, &rect, -1);
				}
				else if (I == 4 || I == 8 || I == 9)
				{
					if (inputVecXCoord)
					{
						rect.x = (int)GFX::getXResolution() - EDITOR_ENTRY_SIZE - EDITOR_MENU_OFFSET_X + EDITOR_TEXT_SIZE;
						rect.w = EDITOR_VEC_ENTRY_SIZE;
						SDL_FillRect(screen, &rect, -1);
					}
					else
					{
						rect.x = (int)GFX::getXResolution() - EDITOR_ENTRY_SIZE - EDITOR_MENU_OFFSET_X + EDITOR_TEXT_SIZE * 2 + EDITOR_VEC_ENTRY_SIZE + EDITOR_MENU_SPACING;
						rect.w = EDITOR_VEC_ENTRY_SIZE;
						SDL_FillRect(screen, &rect, -1);
					}
				}
				menuText.setColour(WHITE);
			}
			else
			{
				SDL_FillRect(screen, &rect, -1);
				menuText.setColour(BLACK);
			}
		}
		else
		{
			SDL_FillRect(screen, &rect, 0);
			menuText.setColour(WHITE);
		}
		menuText.print(settingsItems[I]);

		// render specific menu item content
		if (I == 0 || I == 1) // Text entry
		{
			entriesText.setPosition((int)GFX::getXResolution() - EDITOR_ENTRY_SIZE - EDITOR_MENU_OFFSET_X, pos + EDITOR_RECT_HEIGHT - EDITOR_TEXT_SIZE);
			if (I == settingsSel)
				entriesText.setColour(BLACK);
			else
				entriesText.setColour(WHITE);
			if (I == 0)
				entriesText.print(l->name);
			else
				entriesText.print(filename);
		}
		else if (I == 4 || I == 8 || I == 9) // Vec input
		{
			entriesText.setAlignment(LEFT_JUSTIFIED);
			entriesText.setPosition((int)GFX::getXResolution() - EDITOR_ENTRY_SIZE - EDITOR_MENU_OFFSET_X, pos + EDITOR_RECT_HEIGHT - EDITOR_TEXT_SIZE);
			if (I == settingsSel && !input->isPollingKeyboard())
				entriesText.setColour(BLACK);
			else
				entriesText.setColour(WHITE);
			entriesText.print("X:");
			entriesText.setPosition(entriesText.getStartPosition().x + EDITOR_TEXT_SIZE + EDITOR_VEC_ENTRY_SIZE + EDITOR_MENU_SPACING, entriesText.getStartPosition().y);
			entriesText.print("Y:");
			if (I == settingsSel && (!input->isPollingKeyboard() || inputVecXCoord))
				entriesText.setColour(BLACK);
			else
				entriesText.setColour(WHITE);
			entriesText.setPosition((int)GFX::getXResolution() - EDITOR_ENTRY_SIZE - EDITOR_MENU_OFFSET_X + EDITOR_TEXT_SIZE, pos + EDITOR_RECT_HEIGHT - EDITOR_TEXT_SIZE);
			if (I == 4)
				entriesText.print((input->isPollingKeyboard() && settingsSel == 4 && inputVecXCoord) ? vecInputTemp : StringUtility::floatToString(l->drawOffset.x));
			else if (I == 8)
				entriesText.print((input->isPollingKeyboard() && settingsSel == 8 && inputVecXCoord) ? vecInputTemp : StringUtility::floatToString(PHYSICS->gravity.x));
			else if (I == 9)
				entriesText.print((input->isPollingKeyboard() && settingsSel == 9 && inputVecXCoord) ? vecInputTemp : StringUtility::floatToString(PHYSICS->maximum.x));
			if (I == settingsSel && (!input->isPollingKeyboard() || !inputVecXCoord))
				entriesText.setColour(BLACK);
			else
				entriesText.setColour(WHITE);
			entriesText.setPosition(entriesText.getStartPosition().x + EDITOR_TEXT_SIZE + EDITOR_VEC_ENTRY_SIZE + EDITOR_MENU_SPACING, entriesText.getStartPosition().y);
			if (I == 4)
				entriesText.print((input->isPollingKeyboard() && settingsSel == 4 && !inputVecXCoord) ? vecInputTemp : StringUtility::floatToString(l->drawOffset.y));
			else if (I == 8)
				entriesText.print((input->isPollingKeyboard() && settingsSel == 8 && !inputVecXCoord) ? vecInputTemp : StringUtility::floatToString(PHYSICS->gravity.y));
			else if (I == 9)
				entriesText.print((input->isPollingKeyboard() && settingsSel == 9 && !inputVecXCoord) ? vecInputTemp : StringUtility::floatToString(PHYSICS->maximum.y));
			entriesText.setAlignment(CENTRED);
		}
		else if (I == 6) // Checkbox
		{
			rect.w = EDITOR_RECT_HEIGHT;
			rect.x = (int)GFX::getXResolution() - EDITOR_ENTRY_SIZE / 2 - EDITOR_RECT_HEIGHT / 2 - EDITOR_MENU_OFFSET_X;
			if (I == settingsSel)
				SDL_FillRect(screen, &rect, 0);
			else
				SDL_FillRect(screen, &rect, -1);
			rect.w = EDITOR_CHECK_HEIGHT;
			rect.h = EDITOR_CHECK_HEIGHT;
			rect.x += (EDITOR_RECT_HEIGHT - EDITOR_CHECK_HEIGHT) / 2;
			rect.y += (EDITOR_RECT_HEIGHT - EDITOR_CHECK_HEIGHT) / 2;
			if (I == settingsSel && !l->cam.disregardBoundaries)
				SDL_FillRect(screen, &rect, -1);
			else if (I != settingsSel && !l->cam.disregardBoundaries)
				SDL_FillRect(screen, &rect, 0);
		}

//		if (I == settingsItems.size()-3)
//			pos = EDITOR_RETURN_Y_POS - EDITOR_RECT_HEIGHT - EDITOR_MENU_SPACING;
//		else
			pos += EDITOR_RECT_HEIGHT + EDITOR_MENU_SPACING;
	}
}

void Editor::renderFlags()
{
	SDL_Surface *screen = GFX::getVideoSurface();
	bg.render(screen);

	int pos = EDITOR_SETTINGS_OFFSET_Y;
	// render text and selection
	for (int I = flagsOffset; I < min((int)flagsItems.size(), flagsOffset + EDITOR_MAX_MENU_ITEMS_SCREEN - 1); ++I)
	{
		rect.x = 0;
		rect.y = pos;
		rect.w = GFX::getXResolution() - EDITOR_RECT_HEIGHT - EDITOR_MENU_SPACING;
		rect.h = EDITOR_RECT_HEIGHT;
		menuText.setPosition(EDITOR_MENU_OFFSET_X, pos + EDITOR_RECT_HEIGHT - EDITOR_TEXT_SIZE);
		if (I == flagsSel)
		{
			SDL_FillRect(screen, &rect, -1);
			menuText.setColour(BLACK);
		}
		else
		{
			SDL_FillRect(screen, &rect, 0);
			menuText.setColour(WHITE);
		}
		menuText.print(flagsItems[I]);

		if (I != flagsItems.size() - 1) // Checkboxes
		{
			rect.w = EDITOR_RECT_HEIGHT;
			rect.x = (int)GFX::getXResolution() - EDITOR_MENU_OFFSET_X - EDITOR_ENTRY_SIZE / 2 - EDITOR_RECT_HEIGHT / 2;
			if (I == flagsSel)
				SDL_FillRect(screen, &rect, 0);
			else
				SDL_FillRect(screen, &rect, -1);
			rect.w = EDITOR_CHECK_HEIGHT;
			rect.h = EDITOR_CHECK_HEIGHT;
			rect.x += (EDITOR_RECT_HEIGHT - EDITOR_CHECK_HEIGHT) / 2;
			rect.y += (EDITOR_RECT_HEIGHT - EDITOR_CHECK_HEIGHT) / 2;
			bool temp = false;
			switch (I)
			{
			case 0: // scroll x
				temp = l->flags.hasFlag(Level::lfScrollX);
				break;
			case 1: // scroll y
				temp = l->flags.hasFlag(Level::lfScrollY);
				break;
			case 2: // repeat x
				temp = l->flags.hasFlag(Level::lfRepeatX);
				break;
			case 3: // repeat y
				temp = l->flags.hasFlag(Level::lfRepeatY);
				break;
			case 4: // disable swap
				temp = l->flags.hasFlag(Level::lfDisableSwap);
				break;
			case 5: // keep centred
				temp = l->flags.hasFlag(Level::lfKeepCentred);
				break;
			case 6: // draw pattern
				temp = l->flags.hasFlag(Level::lfDrawPattern);
				break;
			case 7: // cycle players
				temp = l->flags.hasFlag(Level::lfCyclePlayers);
				break;
			case 8: // scale x
				temp = l->flags.hasFlag(Level::lfScaleX);
				break;
			case 9: // scale y
				temp = l->flags.hasFlag(Level::lfScaleY);
				break;
			case 10: // split x
				temp = l->flags.hasFlag(Level::lfSplitX);
				break;
			case 11: // split y
				temp = l->flags.hasFlag(Level::lfSplitY);
				break;
			default:
				break;
			}
			if (I == flagsSel && !temp)
				SDL_FillRect(screen, &rect, -1);
			else if (I != flagsSel && !temp)
				SDL_FillRect(screen, &rect, 0);
		}

//		if (I == settingsItems.size()-3)
//			pos = EDITOR_RETURN_Y_POS - EDITOR_RECT_HEIGHT - EDITOR_MENU_SPACING;
//		else
			pos += EDITOR_RECT_HEIGHT + EDITOR_MENU_SPACING;
	}
	// render back menu item (always visible)
	rect.x = 0;
	rect.y = pos;
	rect.w = GFX::getXResolution();
	rect.h = EDITOR_RECT_HEIGHT;
	menuText.setPosition(EDITOR_MENU_OFFSET_X, pos + EDITOR_RECT_HEIGHT - EDITOR_TEXT_SIZE);
	if (flagsSel == flagsItems.size() - 1)
	{
		SDL_FillRect(screen, &rect, -1);
		menuText.setColour(BLACK);
	}
	else
	{
		SDL_FillRect(screen, &rect, 0);
		menuText.setColour(WHITE);
	}
	menuText.print(flagsItems.back());
	// render scroll bar
	rect.x = (int)GFX::getXResolution() - EDITOR_RECT_HEIGHT;
	rect.y = EDITOR_SETTINGS_OFFSET_Y;
	rect.w = EDITOR_RECT_HEIGHT;
	rect.h = (EDITOR_MAX_MENU_ITEMS_SCREEN - 1) * (EDITOR_RECT_HEIGHT + EDITOR_MENU_SPACING) - EDITOR_MENU_SPACING;
	SDL_FillRect(screen, &rect, 0);
	filledTrigonColor(screen,
			(int)GFX::getXResolution() - EDITOR_RECT_HEIGHT / 2, EDITOR_SETTINGS_OFFSET_Y + EDITOR_RECT_HEIGHT / 3,
			(int)GFX::getXResolution() - EDITOR_RECT_HEIGHT + (EDITOR_RECT_HEIGHT - EDITOR_CHECK_HEIGHT) / 2, EDITOR_SETTINGS_OFFSET_Y + EDITOR_RECT_HEIGHT / 1.5f,
			(int)GFX::getXResolution() - (EDITOR_RECT_HEIGHT - EDITOR_CHECK_HEIGHT) / 2, EDITOR_SETTINGS_OFFSET_Y + EDITOR_RECT_HEIGHT / 1.5f,
			-1);
	filledTrigonColor(screen,
			(int)GFX::getXResolution() - EDITOR_RECT_HEIGHT / 2, EDITOR_SETTINGS_OFFSET_Y + rect.h - EDITOR_RECT_HEIGHT / 3,
			(int)GFX::getXResolution() - (EDITOR_RECT_HEIGHT - EDITOR_CHECK_HEIGHT) / 2, EDITOR_SETTINGS_OFFSET_Y + rect.h - EDITOR_RECT_HEIGHT / 1.5f,
			(int)GFX::getXResolution() - EDITOR_RECT_HEIGHT + (EDITOR_RECT_HEIGHT - EDITOR_CHECK_HEIGHT) / 2, EDITOR_SETTINGS_OFFSET_Y + rect.h - EDITOR_RECT_HEIGHT / 1.5f,
			-1);
	int fullBarSize = (rect.h - EDITOR_RECT_HEIGHT * 2);
	rect.y = EDITOR_SETTINGS_OFFSET_Y + EDITOR_RECT_HEIGHT + fullBarSize / (flagsItems.size() - 1) * flagsOffset;
	rect.h = fullBarSize / (flagsItems.size() - 1) * (EDITOR_MAX_MENU_ITEMS_SCREEN - 1);
	SDL_FillRect(screen, &rect, -1);
}

void Editor::renderDraw()
{
	SDL_Surface *screen = GFX::getVideoSurface();
	GFX::clearScreen();

	SDL_Rect src;
	SDL_Rect dst;
	dst.x = max(-editorOffset.x, 0);
	dst.y = max(-editorOffset.y, 0);
	src.x = max(editorOffset.x, 0);
	src.y = max(editorOffset.y, 0);
	src.w = min((int)GFX::getXResolution(), l->getWidth() - src.x);
	src.h = min((int)GFX::getYResolution(), l->getHeight() - src.y);
	SDL_BlitSurface(l->levelImage, &src, screen, &dst);
	if (drawUnits)
	{
		for (vector<BaseUnit*>::const_iterator I = l->units.begin(); I != l->units.end(); ++I)
			l->renderUnit(GFX::getVideoSurface(), *I, editorOffset);
		for (vector<ControlUnit*>::const_iterator I = l->players.begin(); I != l->players.end(); ++I)
			l->renderUnit(GFX::getVideoSurface(), *I, editorOffset);
	}

	// Grid
	if (gridActive)
	{
		int startX = -editorOffset.x % gridSize - 1;
		if (startX < 0)
			startX += gridSize;
		for (int I = startX; I < GFX::getXResolution(); I += gridSize)
		{
			for (int K = 0; K < GFX::getYResolution(); K += EDITOR_GRID_SPACING+1)
			{
				pixelColor(screen, I, K, EDITOR_GRID_COLOUR);
			}
		}
		int startY = -editorOffset.y % gridSize - 1;
		if (startY < 0)
			startY += gridSize;
		for (int I = startY; I < GFX::getYResolution(); I += gridSize)
		{
			for (int K = 0; K < GFX::getXResolution(); K += EDITOR_GRID_SPACING+1)
			{
				pixelColor(screen, K, I, EDITOR_GRID_COLOUR);
			}
		}
	}
	// Tool overlay (cursor/crop area, etc.)
	if (drawTool == dtBrush)
	{
		Vector2di pos = mousePos;
		pos.x -= brushSize / 2;
		pos.y -= brushSize / 2;
		if (pos.y >= 0)
		{
			for (int I = max(-pos.x, 0); I < brushSize && pos.x + I < GFX::getXResolution(); ++I)
			{
				GFX::setPixel(screen, pos.x + I, pos.y, Colour(WHITE) - GFX::getPixel(screen, pos.x + I, pos.y));
			}
		}
		if (brushSize > 1 && pos.y + brushSize - 1 < GFX::getYResolution())
		{
			for (int I = max(-pos.x, 0); I < brushSize && pos.x + I < GFX::getXResolution(); ++I)
			{
				GFX::setPixel(screen, pos.x + I, pos.y + brushSize - 1, Colour(WHITE) - GFX::getPixel(screen, pos.x + I, pos.y + brushSize - 1));
			}
		}
		if (pos.x >= 0)
		{
			for (int I = max(-pos.y, 1); I < brushSize - 1 && pos.y + I < GFX::getYResolution(); ++I)
			{
				GFX::setPixel(screen, pos.x, pos.y + I, Colour(WHITE) - GFX::getPixel(screen, pos.x , pos.y + I));
			}
		}
		if (pos.x + brushSize - 1 < GFX::getXResolution())
		{
			for (int I = max(-pos.y, 1); I < brushSize - 1 && pos.y + I < GFX::getYResolution(); ++I)
			{
				GFX::setPixel(screen, pos.x + brushSize - 1, pos.y + I, Colour(WHITE) - GFX::getPixel(screen, pos.x + brushSize - 1, pos.y + I));
			}
		}
		// Cross at exact mouse position
		if (brushSize > 5)
		{
			for (int I = -2; I <= 2 && input->getMouseX() + I < GFX::getXResolution(); ++I)
				GFX::setPixel(screen, input->getMouseX() + I, input->getMouseY(), Colour(WHITE) - GFX::getPixel(screen, input->getMouseX() + I, input->getMouseY()));
			for (int I = -2; I <= 2 && input->getMouseY() + I < GFX::getYResolution(); I == -1 ? I += 2 : ++I)
				GFX::setPixel(screen, input->getMouseX(), input->getMouseY() + I, Colour(WHITE) - GFX::getPixel(screen, input->getMouseX(), input->getMouseY() + I));
		}
	}
	else if (drawTool == dtCrop)
	{
		hlineColor(screen, -editorOffset.x + cropOffset.x - 1, -editorOffset.x + cropOffset.x + cropSize.x, -editorOffset.y + cropOffset.y - 1, EDITOR_CROP_COLOUR);
		hlineColor(screen, -editorOffset.x + cropOffset.x - 1, -editorOffset.x + cropOffset.x + cropSize.x, -editorOffset.y + cropOffset.y + cropSize.y, EDITOR_CROP_COLOUR);
		vlineColor(screen, -editorOffset.x + cropOffset.x - 1, -editorOffset.y + cropOffset.y, -editorOffset.y + cropOffset.y + cropSize.y, EDITOR_CROP_COLOUR);
		vlineColor(screen, -editorOffset.x + cropOffset.x + cropSize.x, -editorOffset.y + cropOffset.y, -editorOffset.y + cropOffset.y + cropSize.y, EDITOR_CROP_COLOUR);
		boxColor(screen, -editorOffset.x + cropOffset.x + cropSize.x / 2 - EDITOR_CROP_RECT_HALFHEIGHT, -editorOffset.y + cropOffset.y - EDITOR_CROP_RECT_WIDTH - 1,
				-editorOffset.x + cropOffset.x + cropSize.x / 2 + EDITOR_CROP_RECT_HALFHEIGHT - 1, -editorOffset.y + cropOffset.y - 2, EDITOR_CROP_COLOUR);
		boxColor(screen, -editorOffset.x + cropOffset.x + cropSize.x / 2 - EDITOR_CROP_RECT_HALFHEIGHT, -editorOffset.y + cropOffset.y + cropSize.y + 1,
				-editorOffset.x + cropOffset.x + cropSize.x / 2 + EDITOR_CROP_RECT_HALFHEIGHT - 1, -editorOffset.y + cropOffset.y + cropSize.y + EDITOR_CROP_RECT_WIDTH, EDITOR_CROP_COLOUR);
		boxColor(screen, -editorOffset.x + cropOffset.x - 1 - EDITOR_CROP_RECT_WIDTH, -editorOffset.y + cropOffset.y + cropSize.y / 2 - EDITOR_CROP_RECT_HALFHEIGHT,
				-editorOffset.x + cropOffset.x - 2, -editorOffset.y + cropOffset.y + cropSize.y / 2 + EDITOR_CROP_RECT_HALFHEIGHT - 1, EDITOR_CROP_COLOUR);
		boxColor(screen, -editorOffset.x + cropOffset.x + cropSize.x + 1, -editorOffset.y + cropOffset.y + cropSize.y / 2 - EDITOR_CROP_RECT_HALFHEIGHT,
				-editorOffset.x + cropOffset.x  + cropSize.x + EDITOR_CROP_RECT_WIDTH, -editorOffset.y + cropOffset.y + cropSize.y / 2 + EDITOR_CROP_RECT_HALFHEIGHT - 1, EDITOR_CROP_COLOUR);
	}
	if (colourPanel.active)
	{
		if (colourPanel.changed)
		{
			drawColourPanel(colourPanel.surf);
			colourPanel.changed = false;
		}
		SDL_Rect temp = {colourPanel.pos.x, colourPanel.pos.y, 0, 0};
		SDL_BlitSurface(colourPanel.surf, NULL, GFX::getVideoSurface(), &temp);
	}
}

void Editor::renderUnits()
{
	SDL_Surface *screen = GFX::getVideoSurface();
	GFX::clearScreen();

	SDL_Rect src;
	SDL_Rect dst;
	dst.x = max(-editorOffset.x, 0);
	dst.y = max(-editorOffset.y, 0);
	src.x = max(editorOffset.x, 0);
	src.y = max(editorOffset.y, 0);
	src.w = min((int)GFX::getXResolution(), l->getWidth() - src.x);
	src.h = min((int)GFX::getYResolution(), l->getHeight() - src.y);
	SDL_BlitSurface(l->levelImage, &src, screen, &dst);
	if (drawUnits)
	{
		for (vector<BaseUnit*>::const_iterator I = l->units.begin(); I != l->units.end(); ++I)
			l->renderUnit(GFX::getVideoSurface(), *I, editorOffset);
		for (vector<ControlUnit*>::const_iterator I = l->players.begin(); I != l->players.end(); ++I)
			l->renderUnit(GFX::getVideoSurface(), *I, editorOffset);
	}
	if (currentUnit)
	{
		l->renderUnit(GFX::getVideoSurface(), currentUnit, editorOffset);
		Vector2df temp = currentUnit->position - editorOffset;
		hlineColor(GFX::getVideoSurface(), temp.x - 1, temp.x + currentUnit->getWidth(), temp.y - 1, 0x32D936FF);
		hlineColor(GFX::getVideoSurface(), temp.x - 1, temp.x + currentUnit->getWidth(), temp.y + currentUnit->getHeight(), 0x32D936FF);
		vlineColor(GFX::getVideoSurface(), temp.x - 1, temp.y, temp.y + currentUnit->getHeight() - 1, 0x32D936FF);
		vlineColor(GFX::getVideoSurface(), temp.x + currentUnit->getWidth(), temp.y, temp.y + currentUnit->getHeight() - 1, 0x32D936FF);
	}

	// Grid
	if (gridActive)
	{
		int startX = -editorOffset.x % gridSize - 1;
		if (startX < 0)
			startX += gridSize;
		for (int I = startX; I < GFX::getXResolution(); I += gridSize)
		{
			for (int K = 0; K < GFX::getYResolution(); K += EDITOR_GRID_SPACING+1)
			{
				pixelColor(screen, I, K, EDITOR_GRID_COLOUR);
			}
		}
		int startY = -editorOffset.y % gridSize - 1;
		if (startY < 0)
			startY += gridSize;
		for (int I = startY; I < GFX::getYResolution(); I += gridSize)
		{
			for (int K = 0; K < GFX::getXResolution(); K += EDITOR_GRID_SPACING+1)
			{
				pixelColor(screen, K, I, EDITOR_GRID_COLOUR);
			}
		}
	}

	if (unitPanel.active)
	{
		if (unitPanel.changed)
		{
			drawUnitPanel(unitPanel.surf);
			unitPanel.changed = false;
		}
		SDL_Rect temp = {unitPanel.pos.x, unitPanel.pos.y, 0, 0};
		SDL_BlitSurface(unitPanel.surf, NULL, GFX::getVideoSurface(), &temp);
	}
	if (paramsPanel.active)
	{
		if (paramsPanel.changed)
		{
			drawParamsPanel(paramsPanel.surf);
			paramsPanel.changed = false;
		}
		SDL_Rect temp = {paramsPanel.pos.x, paramsPanel.pos.y, 0, 0};
		SDL_BlitSurface(paramsPanel.surf, NULL, GFX::getVideoSurface(), &temp);
	}
}

void Editor::renderTest()
{
	l->render(GFX::getVideoSurface());
}

void Editor::save()
{
	if (filename[0] != 0)
	{
		char imgFilename[256] = "images/levels/";
		strcat(imgFilename, filename.c_str());
		strcat(imgFilename, ".png");
		IMG_SavePNG(imgFilename, l->levelImage, IMG_COMPRESS_DEFAULT);
		l->imageFileName = imgFilename;
		ofstream file;
		char lvlFilename[256] = "levels/";
		strcat(lvlFilename, filename.c_str());
		strcat(lvlFilename, ".txt");
		file.open(lvlFilename, ios::out | ios::trunc);
		file << "[Level]\n";
		l->generateParameters();
		for (list<PARAMETER_TYPE >::iterator I = l->parameters.begin(); I != l->parameters.end(); ++I)
		{
			file << I->first << VALUE_STRING << I->second << "\n";
		}
		for (vector<ControlUnit*>::iterator I = l->players.begin(); I != l->players.end(); ++I)
		{
			file << "[Player]\n";
			(*I)->generateParameters();
			for (list<PARAMETER_TYPE >::iterator K = (*I)->parameters.begin(); K != (*I)->parameters.end(); ++K)
			{
				file << K->first << VALUE_STRING << K->second << "\n";
			}
		}
		for (vector<BaseUnit*>::iterator I = l->units.begin(); I != l->units.end(); ++I)
		{
			file << "[Unit]\n";
			(*I)->generateParameters();
			for (list<PARAMETER_TYPE >::iterator K = (*I)->parameters.begin(); K != (*I)->parameters.end(); ++K)
			{
				file << K->first << VALUE_STRING << K->second << "\n";
			}
		}
		file.close();
		l->levelFileName = lvlFilename;
	}
}

/// Panel implementation

void Editor::drawColourPanel(SDL_Surface *target)
{
	bg.render(target);
	// Draw primary and secondary colour panel
	int xPos = EDITOR_COLOUR_PANEL_SPACING + EDITOR_SLIDER_HEIGHT / 2;
	int yPos = EDITOR_COLOUR_PANEL_SPACING * 2 + EDITOR_SLIDER_HEIGHT * 1.25f;
	boxColor(target, xPos, yPos, xPos + EDITOR_SLIDER_HEIGHT - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, brushCol2.getRGBAvalue());
	xPos -= EDITOR_SLIDER_HEIGHT / 2;
	yPos -= EDITOR_SLIDER_HEIGHT / 2;
	boxColor(target, xPos, yPos, xPos + EDITOR_SLIDER_HEIGHT - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, brushCol.getRGBAvalue());
    xPos = EDITOR_COLOUR_PANEL_SPACING + EDITOR_SLIDER_HEIGHT * 1.5f + EDITOR_COLOUR_PANEL_SPACING;
	yPos = EDITOR_COLOUR_PANEL_SPACING;
	// Draw sliders
	int indicatorPos = EDITOR_SLIDER_WIDTH * brushCol.red / 255;
    panelText.setAlignment(LEFT_JUSTIFIED);
    panelText.setUpBoundary(GFX::getXResolution(), GFX::getYResolution());
	panelText.setPosition(xPos + 2, yPos);
	panelText.print(target, "R");
	xPos = EDITOR_COLOUR_PANEL_OFFSET;
    boxColor(target, xPos, yPos, xPos + EDITOR_SLIDER_WIDTH + EDITOR_SLIDER_INDICATOR_WIDTH - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0x000000FF);
    boxColor(target, xPos + indicatorPos, yPos, xPos + indicatorPos + EDITOR_SLIDER_INDICATOR_WIDTH - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0xFFFFFFFF);
    xPos += EDITOR_SLIDER_WIDTH + EDITOR_COLOUR_PANEL_SPACING;
    panelText.setPosition(xPos + 2, yPos);
	if (panelInputTarget == 1)
	{
		boxColor(target, xPos, yPos, EDITOR_COLOUR_PANEL_WIDTH - EDITOR_COLOUR_PANEL_SPACING - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0xFFFFFFFF);
		panelText.setColour(BLACK);
		panelText.print(target, brushCol.red);
		panelText.setColour(WHITE);
	}
	else
		panelText.print(target, brushCol.red);
    yPos += EDITOR_SLIDER_HEIGHT + EDITOR_COLOUR_PANEL_SPACING;
    xPos = EDITOR_COLOUR_PANEL_SPACING + EDITOR_SLIDER_HEIGHT * 1.5f + EDITOR_COLOUR_PANEL_SPACING;
	panelText.setPosition(xPos + 2, yPos);
	panelText.print(target, "G");
	xPos = EDITOR_COLOUR_PANEL_OFFSET;
	indicatorPos = EDITOR_SLIDER_WIDTH * brushCol.green / 255;
    boxColor(target, xPos, yPos, xPos + EDITOR_SLIDER_WIDTH + EDITOR_SLIDER_INDICATOR_WIDTH - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0x000000FF);
    boxColor(target, xPos + indicatorPos, yPos, xPos + indicatorPos + EDITOR_SLIDER_INDICATOR_WIDTH - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0xFFFFFFFF);
    xPos += EDITOR_SLIDER_WIDTH + EDITOR_COLOUR_PANEL_SPACING;
    panelText.setPosition(xPos + 2, yPos);
	if (panelInputTarget == 2)
	{
		boxColor(target, xPos, yPos, EDITOR_COLOUR_PANEL_WIDTH - EDITOR_COLOUR_PANEL_SPACING - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0xFFFFFFFF);
		panelText.setColour(BLACK);
		panelText.print(target, brushCol.green);
		panelText.setColour(WHITE);
	}
	else
		panelText.print(target, brushCol.green);
    yPos += EDITOR_SLIDER_HEIGHT + EDITOR_COLOUR_PANEL_SPACING;
    xPos = EDITOR_COLOUR_PANEL_SPACING + EDITOR_SLIDER_HEIGHT * 1.5f + EDITOR_COLOUR_PANEL_SPACING;
	panelText.setPosition(xPos + 2, yPos);
	panelText.print(target, "B");
	xPos = EDITOR_COLOUR_PANEL_OFFSET;
	indicatorPos = EDITOR_SLIDER_WIDTH * brushCol.blue / 255;
    boxColor(target, xPos, yPos, xPos + EDITOR_SLIDER_WIDTH + EDITOR_SLIDER_INDICATOR_WIDTH - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0x000000FF);
    boxColor(target, xPos + indicatorPos, yPos, xPos + indicatorPos + EDITOR_SLIDER_INDICATOR_WIDTH - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0xFFFFFFFF);
    xPos += EDITOR_SLIDER_WIDTH + EDITOR_COLOUR_PANEL_SPACING;
    panelText.setPosition(xPos + 2, yPos);
	if (panelInputTarget == 3)
	{
		boxColor(target, xPos, yPos, EDITOR_COLOUR_PANEL_WIDTH - EDITOR_COLOUR_PANEL_SPACING - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0xFFFFFFFF);
		panelText.setColour(BLACK);
		panelText.print(target, brushCol.blue);
		panelText.setColour(WHITE);
	}
	else
		panelText.print(target, brushCol.blue);
	// Draw colour presets
	xPos = EDITOR_COLOUR_PANEL_SPACING;
    yPos += EDITOR_SLIDER_HEIGHT + EDITOR_COLOUR_PANEL_SPACING;
    boxColor(target, xPos, yPos, xPos + EDITOR_SLIDER_HEIGHT - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0x000000FF);
    xPos += EDITOR_SLIDER_HEIGHT + EDITOR_COLOUR_PANEL_SPACING;
    boxColor(target, xPos, yPos, xPos + EDITOR_SLIDER_HEIGHT - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0xFFFFFFFF);
    xPos += EDITOR_SLIDER_HEIGHT + EDITOR_COLOUR_PANEL_SPACING;
    boxColor(target, xPos, yPos, xPos + EDITOR_SLIDER_HEIGHT - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0x939598FF);
    xPos += EDITOR_SLIDER_HEIGHT + EDITOR_COLOUR_PANEL_SPACING;
    boxColor(target, xPos, yPos, xPos + EDITOR_SLIDER_HEIGHT - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0xFF0000FF);
    xPos += EDITOR_SLIDER_HEIGHT + EDITOR_COLOUR_PANEL_SPACING;
    boxColor(target, xPos, yPos, xPos + EDITOR_SLIDER_HEIGHT - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0x32D936FF);
    xPos += EDITOR_SLIDER_HEIGHT + EDITOR_COLOUR_PANEL_SPACING;
    boxColor(target, xPos, yPos, xPos + EDITOR_SLIDER_HEIGHT - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0xFF9900FF);
    xPos += EDITOR_SLIDER_HEIGHT + EDITOR_COLOUR_PANEL_SPACING;
    boxColor(target, xPos, yPos, xPos + EDITOR_SLIDER_HEIGHT - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0xFFDCA8FF);
    xPos += EDITOR_SLIDER_HEIGHT + EDITOR_COLOUR_PANEL_SPACING;
    boxColor(target, xPos, yPos, xPos + EDITOR_SLIDER_HEIGHT - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0x0033FFFF);
    xPos += EDITOR_SLIDER_HEIGHT + EDITOR_COLOUR_PANEL_SPACING;
    boxColor(target, xPos, yPos, xPos + EDITOR_SLIDER_HEIGHT - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0xB2C1FFFF);
    xPos += EDITOR_SLIDER_HEIGHT + EDITOR_COLOUR_PANEL_SPACING;
    boxColor(target, xPos, yPos, xPos + EDITOR_SLIDER_HEIGHT - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, 0xD8CED4FF);
}

void Editor::drawUnitPanel(SDL_Surface *target)
{
	bg.render(target);
	panelText.setUpBoundary(EDITOR_UNIT_PANEL_WIDTH, EDITOR_UNIT_PANEL_HEIGHT);
	panelText.setAlignment(CENTRED);
	panelText.setPosition(0, 0);
	int temp = (hoverUnitButton == -1) ? selectedUnitButton : hoverUnitButton;
	switch (temp)
	{
		case  0: panelText.print(target, "BLACK PLAYER"); break;
		case  1: panelText.print(target, "WHITE PLAYER"); break;
		case  2: panelText.print(target, "ORANGE PLAYER"); break;
		case  3: panelText.print(target, "BLUE PLAYER"); break;
		case 16: panelText.print(target, "PUSHABLE BOX"); break;
		case 17: panelText.print(target, "SOLID BOX"); break;
		case 18: panelText.print(target, "FADING BOX (B/W)"); break;
		case 19: panelText.print(target, "FADING BOX (ORANGE)"); break;
		case 20: panelText.print(target, "FADING BOX (BLUE)"); break;
		case 21: panelText.print(target, "FADING BOX (MIX)"); break;
		case 22: panelText.print(target, "EXIT"); break;
		case 23: panelText.print(target, "KEY"); break;
		case 24: panelText.print(target, "SWITCH"); break;
		case 25: panelText.print(target, "BLACK GEAR"); break;
		case 26: panelText.print(target, "WHITE GEAR"); break;
		case 27: panelText.print(target, "TEXT"); break;
		case 28: panelText.print(target, "PARTICLE EMITTER"); break;
		case 29: panelText.print(target, "CONTROL HELP"); break;
		case 48: panelText.print(target, "BASE TRIGGER"); break;
		case 49: panelText.print(target, "DIALOGUE TRIGGER"); break;
		case 50: panelText.print(target, "EXIT TRIGGER"); break;
		case 51: panelText.print(target, "SOUND TRIGGER"); break;
		case 52: panelText.print(target, "CAMERA TRIGGER"); break;
		case 53: panelText.print(target, "LEVEL TRIGGER"); break;
		default: panelText.print(target, "-NONE SELECTED-");
	}
	panelText.setAlignment(LEFT_JUSTIFIED);
	int xPos = EDITOR_UNIT_PANEL_SPACING;
	int yPos = EDITOR_PANEL_TEXT_SIZE + EDITOR_UNIT_PANEL_SPACING;
	panelText.setPosition(xPos, yPos);
	panelText.print(target, "PLAYERS:");
	yPos += EDITOR_PANEL_TEXT_SIZE + EDITOR_UNIT_PANEL_SPACING;
	for (int I = 0; I < 64; ++I)
	{
		if (I == selectedUnitButton)
		{
			boxColor(target, xPos, yPos, xPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 - 1, yPos + EDITOR_UNIT_BUTTON_BORDER - 1, 0xFFFFFFFF);
			boxColor(target, xPos, yPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER, xPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 - 1, yPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 - 1, 0xFFFFFFFF);
			boxColor(target, xPos, yPos + EDITOR_UNIT_BUTTON_BORDER, xPos + EDITOR_UNIT_BUTTON_BORDER - 1, yPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER - 1, 0xFFFFFFFF);
			boxColor(target, xPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER, yPos + EDITOR_UNIT_BUTTON_BORDER / 2, xPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 - 1, yPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER - 1, 0xFFFFFFFF);
		}
		else if (I == hoverUnitButton)
		{
			boxColor(target, xPos, yPos, xPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 - 1, yPos + EDITOR_UNIT_BUTTON_BORDER / 2 - 1, 0xFFFFFFFF);
			boxColor(target, xPos, yPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 1.5f, xPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 - 1, yPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 - 1, 0xFFFFFFFF);
			boxColor(target, xPos, yPos + EDITOR_UNIT_BUTTON_BORDER / 2, xPos + EDITOR_UNIT_BUTTON_BORDER / 2 - 1, yPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 1.5f - 1, 0xFFFFFFFF);
			boxColor(target, xPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 1.5f, yPos + EDITOR_UNIT_BUTTON_BORDER / 2, xPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 - 1, yPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 1.5f - 1, 0xFFFFFFFF);
		}
		else
		{
			boxColor(target, xPos, yPos, xPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 - 1, yPos + EDITOR_UNIT_BUTTON_BORDER / 2 - 1, 0x000000FF);
			boxColor(target, xPos, yPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 1.5f, xPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 - 1, yPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 - 1, 0x000000FF);
			boxColor(target, xPos, yPos + EDITOR_UNIT_BUTTON_BORDER / 2, xPos + EDITOR_UNIT_BUTTON_BORDER / 2 - 1, yPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 1.5f - 1, 0x000000FF);
			boxColor(target, xPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 1.5f, yPos + EDITOR_UNIT_BUTTON_BORDER / 2, xPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 - 1, yPos + EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 1.5f - 1, 0x000000FF);
		}
		unitButtons.setCurrentFrame(I);
		unitButtons.setPosition(xPos + EDITOR_UNIT_BUTTON_BORDER, yPos + EDITOR_UNIT_BUTTON_BORDER);
		unitButtons.render(target);
		if (I == EDITOR_UNIT_PLAYER_START + EDITOR_UNIT_PLAYER_COUNT - 1) // Last "player" index
		{
			I = EDITOR_UNIT_UNITS_START - 1;
			xPos = EDITOR_UNIT_PANEL_SPACING;
			yPos += EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 + EDITOR_UNIT_PANEL_SPACING;
			panelText.setPosition(xPos, yPos);
			panelText.print(target, "UNITS:");
			yPos += EDITOR_PANEL_TEXT_SIZE + EDITOR_UNIT_PANEL_SPACING;
		}
		else if (I == EDITOR_UNIT_UNITS_START + EDITOR_UNIT_UNITS_COUNT - 1) // Last "unit" index
		{
			I = EDITOR_UNIT_TRIGGER_START - 1; // First "trigger" index - 1
			xPos = EDITOR_UNIT_PANEL_SPACING;
			yPos += EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 + EDITOR_UNIT_PANEL_SPACING;
			panelText.setPosition(xPos, yPos);
			panelText.print(target, "TRIGGERS:");
			yPos += EDITOR_PANEL_TEXT_SIZE + EDITOR_UNIT_PANEL_SPACING;
		}
		else if (I == EDITOR_UNIT_TRIGGER_START + EDITOR_UNIT_TRIGGER_COUNT - 1) // Last "trigger" index
		{
			break;
		}
		else
		{
			xPos += EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 + EDITOR_UNIT_PANEL_SPACING;
			if (xPos > EDITOR_UNIT_PANEL_WIDTH - EDITOR_UNIT_BUTTON_SIZE - EDITOR_UNIT_BUTTON_BORDER * 2 - EDITOR_UNIT_PANEL_SPACING)
			{
				xPos = EDITOR_UNIT_PANEL_SPACING;
				yPos += EDITOR_UNIT_BUTTON_SIZE + EDITOR_UNIT_BUTTON_BORDER * 2 + EDITOR_UNIT_PANEL_SPACING;
			}
		}
		if (yPos > EDITOR_UNIT_PANEL_HEIGHT - EDITOR_UNIT_BUTTON_SIZE - EDITOR_UNIT_BUTTON_BORDER * 2 - EDITOR_UNIT_PANEL_SPACING)
			break;
	}
}

void Editor::drawParamsPanel(SDL_Surface* target)
{
	bg.render(target);
	if (!currentUnit)
		return;
	panelText.setUpBoundary(EDITOR_PARAMS_PANEL_WIDTH - EDITOR_PARAMS_SPACING * 2, EDITOR_PARAMS_PANEL_HEIGHT - EDITOR_PARAMS_SPACING * 2);
	panelText.setWrapping(true);
	panelText.setAlignment(LEFT_JUSTIFIED);
	panelText.setPosition(EDITOR_PARAMS_SPACING, EDITOR_PARAMS_SPACING);
	string temp;
	for (list<PARAMETER_TYPE >::iterator I = currentUnit->parameters.begin(); I != currentUnit->parameters.end(); ++I)
		temp += I->first + VALUE_STRING + I->second + "\n";
	panelText.print(target, temp);
}
