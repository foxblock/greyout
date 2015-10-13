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

#include "IMG_savepng.h"

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
	settingsItems.push_back("FLAGS:");
	settingsItems.push_back("SIZE:");
	settingsItems.push_back("GRAVITY:");

	startSel = 0;
	settingsSel = 0;
	editorState = esStart;
	lastState = esStart;
}

Editor::~Editor()
{

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
	Level::update();

	switch (editorState)
	{
	case esStart:
	{
		//
	}
	case esSettings:
	{
		//
	}
	case esDraw:
	{
		//
	}
	case esUnits:
	{
		//
	}
	case esTest:
	{
		//
	}
	default:
		return;
	}
}

void Editor::render(SDL_Surface* screen)
{
	switch (editorState)
	{
	case esStart:
		renderStart(screen);
		break;
	case esSettings:
		renderSettings(screen);
		break;
	case esDraw:
		renderDraw(screen);
		break;
	case esUnits:
		renderUnits(screen);
		break;
	case esTest:
		renderTest(screen);
		break;
	default:
		return;
	}
}

#ifdef _DEBUG
string Editor::debugInfo()
{
	switch (editorState)
	{
		case esStart:
			return "STARTUP";
		case esSettings:
			return "LEVEL SETTINGS";
		case esDraw:
			return "DRAW MODE\n";
		case esUnits:
			return "SELECT MODE\n";
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
	Vector2di mousePos = input->getMouse();
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
		case 0:
			editorState = esSettings;
			testString = "";
			input->pollKeyboardInput(&testString, KEYBOARD_MASK_ASCII);
			break;
		case 1:
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
	Vector2di mousePos = input->getMouse();
	int pos = EDITOR_SETTINGS_OFFSET_Y;
	mouseInBounds = false;
	if (mousePos != lastPos || input->isLeftClick() || input->isRightClick())
	{
		for (int I = 0; I < settingsItems.size(); ++I)
		{
			if (mousePos.y >= pos && mousePos.y <= pos + EDITOR_RECT_HEIGHT)
			{
				settingsSel = I;
				mouseInBounds = true;
			}
			pos += EDITOR_RECT_HEIGHT + EDITOR_MENU_SPACING;
		}
		lastPos = mousePos;
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
		input->resetMouseButtons();
		input->resetB();
	}
	else if (isCancelKey(input) && !input->isPollingKeyboard())
	{
		input->resetKeys();
		setNextState(STATE_MAIN);
	}
}

void Editor::inputDraw()
{

}

void Editor::inputUnits()
{

}

void Editor::inputTest()
{
	if (input->isSelect())
		editorState = lastState;
	else
		Level::userInput();
}

void Editor::renderStart(SDL_Surface* screen)
{
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

void Editor::renderSettings(SDL_Surface* screen)
{
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
			SDL_FillRect(screen, &rect, -1);
			menuText.setColour(BLACK);
		}
		else
		{
			SDL_FillRect(screen, &rect, 0);
			menuText.setColour(WHITE);
		}
		menuText.print(settingsItems[I]);

		if (I < settingsItems.size())
		{
			entriesText.setPosition((int)GFX::getXResolution() - EDITOR_ENTRY_SIZE - EDITOR_MENU_OFFSET_X,
									pos + EDITOR_RECT_HEIGHT - EDITOR_TEXT_SIZE);
			if (I == settingsSel)
				entriesText.setColour(BLACK);
			else
				entriesText.setColour(WHITE);
			entriesText.print(testString);
		}

		pos += EDITOR_RECT_HEIGHT + EDITOR_MENU_SPACING;
	}
}

void Editor::renderDraw(SDL_Surface* screen)
{

}

void Editor::renderUnits(SDL_Surface* screen)
{

}

void Editor::renderTest(SDL_Surface* screen)
{
	Level::render(screen);
}

