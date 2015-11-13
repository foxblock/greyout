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

#include "IMG_savepng.h"
#include <SDL/SDL_gfxPrimitives.h>

#define EDITOR_HEADLINE_SIZE 72
#define EDITOR_TEXT_SIZE 48
#define EDITOR_RECT_HEIGHT 35
#define EDITOR_CHECK_HEIGHT 27
#define EDITOR_MENU_SPACING 10
#define EDITOR_SETTINGS_OFFSET_Y 92
#define EDITOR_ENTRY_SIZE 400
#define EDITOR_RETURN_Y_POS 400
#define EDITOR_MENU_OFFSET_X 20
#define EDITOR_MENU_SPACING_EXTRA 10

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


Editor::Editor()
{
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
	settingsItems.push_back("GRAVITY:");
	settingsItems.push_back("SAVE");
	settingsItems.push_back("CONTINUE");
	startSel = 0;
	settingsSel = 0;
	editorState = esStart;
	lastState = esStart;
	GFX::showCursor(true);

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
	colourPanel.pos.y = GFX::getYResolution() / 2;
	colourPanel.active = false;
	colourPanel.transparent = false;
	colourPanel.userIsInteracting = false;
	colourPanel.changed = false;
	drawUnits = false;

	list<PARAMETER_TYPE > params;
	params.push_back(make_pair(CLASS_STRING,""));
	for (map<string,int>::iterator I = LEVEL_LOADER->unitClasses.begin(); I != LEVEL_LOADER->unitClasses.end(); ++I)
	{
		params.front().second = I->first;
		UnitContainer temp;
		temp.unit = LEVEL_LOADER->createUnit(params, this);
		if (temp.unit)
		{
			temp.img = SDL_CreateRGBSurface(SDL_SWSURFACE, 32, 32, GFX::getVideoSurface()->format->BitsPerPixel, 0, 0, 0, 0);
			SDL_SetColorKey(temp.img, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(temp.img->format,255,0,255));
			SDL_FillRect(temp.img, NULL, SDL_MapRGB(temp.img->format,255,0,255));
			temp.unit->render(temp.img);
			unitButtons.push_back(temp);
		}
	}
}

Editor::~Editor()
{
	if (ownsImage)
		SDL_FreeSurface(levelImage);
	if (colourPanel.surf)
		SDL_FreeSurface(colourPanel.surf);
	for (vector<UnitContainer>::iterator I = unitButtons.begin(); I != unitButtons.end(); ++I)
	{
		if (I->unit)
			delete I->unit;
		if (I->img)
			SDL_FreeSurface(I->img);
	}
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
	debugString = debugInfo();

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
		for (map<string, int>::iterator I = LEVEL_LOADER->unitClasses.begin(); I != LEVEL_LOADER->unitClasses.end(); ++I)
			debugString += I->first + "\n";
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
			return "TEST PLAY\n" + Level::debugInfo();
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
			if (mousePos.y >= pos && mousePos.y <= pos + EDITOR_RECT_HEIGHT)
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
			levelImage = SDL_CreateRGBSurface(SDL_SWSURFACE, EDITOR_DEFAULT_WIDTH, EDITOR_DEFAULT_HEIGHT,
					GFX::getVideoSurface()->format->BitsPerPixel, 0, 0, 0, 0);
			SDL_FillRect(levelImage, NULL, -1);
			editorOffset.x = ((int)levelImage->w - (int)GFX::getXResolution()) / 2;
			editorOffset.y = ((int)levelImage->h - (int)GFX::getYResolution()) / 2;
			break;
		case 1: // Open level
			break;
		case 2: // New chapter
			break;
		case 3: // Open chapter
			break;
		default:
			break;
		}
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
		if (isAcceptKey(input) || isCancelKey(input))
			input->stopKeyboardInput();
		input->resetKeys();
		return;
	}

	int pos = EDITOR_SETTINGS_OFFSET_Y;
	mouseInBounds = false;
	if (input->getMouse() != lastPos || input->isLeftClick() || input->isRightClick())
	{
		for (int I = 0; I < settingsItems.size(); ++I)
		{
			if (input->getMouseY() >= pos && input->getMouseY() <= pos + EDITOR_RECT_HEIGHT)
			{
				settingsSel = I;
				mouseInBounds = true;
			}
			if (I == settingsItems.size()-3)
				pos = EDITOR_RETURN_Y_POS - EDITOR_RECT_HEIGHT - EDITOR_MENU_SPACING;
			else
				pos += EDITOR_RECT_HEIGHT + EDITOR_MENU_SPACING;
		}
		lastPos = input->getMouse();
	}

	if (input->isUp() && settingsSel > 0)
	{
		--settingsSel;
		input->resetUp();
	}
	else if (input->isDown() && settingsSel < settingsItems.size() - 1)
	{
		++settingsSel;
		input->resetDown();
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
			input->pollKeyboardInput(&name, KEYBOARD_MASK_ASCII);
			break;
		case 1: // Filename
			break;
		case 2: // Flags
			break;
		case 3: // Gravity
			break;
		case 4: // Save
			if (name[0] != 0)
			{
                char filename[256] = "images/levels/";
                strcat(filename, name.c_str());
                strcat(filename, ".png");
				IMG_SavePNG(filename, levelImage, IMG_COMPRESS_DEFAULT);
			}
			break;
		case 5: // Continue
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
		}
		if (isAcceptKey(input))
		{
			panelActiveSlider = 0;
			colourPanel.userIsInteracting = false;
			editorState = esUnits;
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
			cropSize.x = levelImage->w;
			cropSize.y = levelImage->h;
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
	// Snap mouse to grid
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
				filledPolygonColor(levelImage, polX, polY, 6, brushCol.getRGBAvalue());
			else
				filledPolygonColor(levelImage, polX, polY, 6, brushCol2.getRGBAvalue());
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
			if (collisionLayer)
				SDL_FreeSurface(collisionLayer);
			collisionLayer = SDL_CreateRGBSurface(SDL_SWSURFACE, cropSize.x, cropSize.y, GFX::getVideoSurface()->format->BitsPerPixel, 0, 0, 0, 0);
			SDL_FillRect(collisionLayer, NULL, -1);
			SDL_Rect srcRect;
			srcRect.x = std::max(cropOffset.x, 0);
			srcRect.y = std::max(cropOffset.y, 0);
			srcRect.w = std::min(levelImage->w, collisionLayer->w);
			srcRect.h = std::min(levelImage->h, collisionLayer->h);
			SDL_Rect dstRect;
			dstRect.x = std::max(-cropOffset.x, 0);
			dstRect.y = std::max(-cropOffset.y, 0);
            SDL_BlitSurface(levelImage, &srcRect, collisionLayer, &dstRect);
            SDL_FreeSurface(levelImage);
			levelImage = SDL_CreateRGBSurface(SDL_SWSURFACE, cropSize.x, cropSize.y, GFX::getVideoSurface()->format->BitsPerPixel, 0, 0, 0, 0);
			SDL_BlitSurface(collisionLayer, NULL, levelImage, NULL);
			editorOffset -= cropOffset;
			for (vector<BaseUnit*>::iterator I = units.begin(); I != units.end(); ++I)
			{
				(*I)->position -= cropOffset;
				(*I)->startingPosition -= cropOffset;
			}
			for (vector<ControlUnit*>::iterator I = players.begin(); I != players.end(); ++I)
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
	if (isCancelKey(input))
	{
		editorState = esDraw;
		input->resetKeys();
	}
	if (input->isKey("u"))
	{
		drawUnits = !drawUnits;
		input->resetKeys();
	}
	if (input->isKey("g"))
	{
		gridActive = !gridActive;
		input->resetKeys();
	}
}

void Editor::inputTest()
{
	if (input->isSelect())
		editorState = lastState;
	else
		Level::userInput();
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
	// render text and selection
	for (int I = 0; I < settingsItems.size(); ++I)
	{
		rect.x = 0;
		rect.y = pos;
		rect.w = GFX::getXResolution();
		rect.h = EDITOR_RECT_HEIGHT;
		menuText.setPosition(EDITOR_MENU_OFFSET_X, pos + EDITOR_RECT_HEIGHT - EDITOR_TEXT_SIZE);
		if (I == settingsSel)
		{
			if (input->isPollingKeyboard())
			{
				SDL_FillRect(screen, &rect, 0);
				rect.x = (int)GFX::getXResolution() - EDITOR_ENTRY_SIZE - EDITOR_MENU_OFFSET_X;
				rect.w = EDITOR_ENTRY_SIZE;
				SDL_FillRect(screen, &rect, -1);
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

		if (I == 0)
		{
			entriesText.setPosition((int)GFX::getXResolution() - EDITOR_ENTRY_SIZE - EDITOR_MENU_OFFSET_X,
									pos + EDITOR_RECT_HEIGHT - EDITOR_TEXT_SIZE);
			if (I == settingsSel)
				entriesText.setColour(BLACK);
			else
				entriesText.setColour(WHITE);
			entriesText.print(name);
		}

		if (I == settingsItems.size()-3)
			pos = EDITOR_RETURN_Y_POS - EDITOR_RECT_HEIGHT - EDITOR_MENU_SPACING;
		else
			pos += EDITOR_RECT_HEIGHT + EDITOR_MENU_SPACING;
	}
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
	src.w = min((int)GFX::getXResolution(), getWidth() - src.x);
	src.h = min((int)GFX::getYResolution(), getHeight() - src.y);
	SDL_BlitSurface(levelImage, &src, screen, &dst);
	if (drawUnits)
	{
		for (vector<BaseUnit*>::const_iterator I = units.begin(); I != units.end(); ++I)
			renderUnit(GFX::getVideoSurface(), *I, editorOffset);
		for (vector<ControlUnit*>::const_iterator I = players.begin(); I != players.end(); ++I)
			renderUnit(GFX::getVideoSurface(), *I, editorOffset);
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
	src.w = min((int)GFX::getXResolution(), getWidth() - src.x);
	src.h = min((int)GFX::getYResolution(), getHeight() - src.y);
	SDL_BlitSurface(levelImage, &src, screen, &dst);
	if (drawUnits)
	{
		for (vector<BaseUnit*>::const_iterator I = units.begin(); I != units.end(); ++I)
			renderUnit(GFX::getVideoSurface(), *I, editorOffset);
		for (vector<ControlUnit*>::const_iterator I = players.begin(); I != players.end(); ++I)
			renderUnit(GFX::getVideoSurface(), *I, editorOffset);
	}

	dst.x = max(-editorOffset.x + 4, 0);
	dst.y = max(-editorOffset.y + 4, 0);
	for (vector<UnitContainer>::iterator I = unitButtons.begin(); I != unitButtons.end(); ++I)
	{
		boxColor(screen, dst.x - 2, dst.y - 2, dst.x + 34, dst.y + 34, 0x000000FF);
		boxColor(screen, dst.x, dst.y, dst.x + 32, dst.y + 32, EDITOR_GRID_COLOUR);
		SDL_BlitSurface(I->img, NULL, screen, &dst);
		dst.x += 40;
		if (dst.x > (-editorOffset.x) + levelImage->w)
		{
			dst.x = max(-editorOffset.x + 4, 0);
			dst.y += 40;
		}
	}
}

void Editor::renderTest()
{
	Level::render(GFX::getVideoSurface());
}

void Editor::drawColourPanel(SDL_Surface *target)
{
	bg.render(target);
	int xPos = EDITOR_COLOUR_PANEL_SPACING + EDITOR_SLIDER_HEIGHT / 2;
	int yPos = EDITOR_COLOUR_PANEL_SPACING * 2 + EDITOR_SLIDER_HEIGHT * 1.25f;
	boxColor(target, xPos, yPos, xPos + EDITOR_SLIDER_HEIGHT - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, brushCol2.getRGBAvalue());
	xPos -= EDITOR_SLIDER_HEIGHT / 2;
	yPos -= EDITOR_SLIDER_HEIGHT / 2;
	boxColor(target, xPos, yPos, xPos + EDITOR_SLIDER_HEIGHT - 1, yPos + EDITOR_SLIDER_HEIGHT - 1, brushCol.getRGBAvalue());
    xPos = EDITOR_COLOUR_PANEL_SPACING + EDITOR_SLIDER_HEIGHT * 1.5f + EDITOR_COLOUR_PANEL_SPACING;
	yPos = EDITOR_COLOUR_PANEL_SPACING;
	int indicatorPos = EDITOR_SLIDER_WIDTH * brushCol.red / 255;
    panelText.setAlignment(LEFT_JUSTIFIED);
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
