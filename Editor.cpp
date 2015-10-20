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
#define EDITOR_CROP_RECT_HALFHEIGHT 10
#define EDITOR_CROP_RECT_WIDTH 5
#define EDITOR_CROP_COLOUR 0xCCCCCCFF

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

	editorOffset.x = 0;
	editorOffset.y = 0;
	ownsImage = false;
	brushCol.setColour(BLACK);
	brushSize = 32;
	brushRect = {0, 0, 0, 0};
	cropSize.x = 0;
	cropSize.y = 0;
	cropOffset.x = 0;
	cropOffset.y = 0;
	cropEdge = diNONE;
	drawTool = dtBrush;
}

Editor::~Editor()
{
	if (ownsImage)
		SDL_FreeSurface(levelImage);
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
		input->resetMouseButtons();
		input->resetB();
	}
	else if (isCancelKey(input))
	{
		input->resetKeys();
		setNextState(STATE_MAIN);
	}
}

void Editor::inputDraw()
{
	if (input->isLeft())
		editorOffset.x -= 2;
	else if (input->isRight())
		editorOffset.x += 2;
	if (input->isUp())
		editorOffset.y -= 2;
	else if (input->isDown())
		editorOffset.y += 2;

	if (input->isKey("F6"))
	{
		if (brushCol == BLACK)
			brushCol.setColour(WHITE);
		else if (brushCol == WHITE)
			brushCol.setColour(RED);
		else if (brushCol == RED)
			brushCol.setColour(147, 149, 152);
		else
			brushCol.setColour(BLACK);
		input->resetKeys();
	}
	if (input->isKey("b"))
	{
		drawTool = dtBrush;
		GFX::showCursor(false);
	}
	else if (input->isKey("c"))
	{
		drawTool = dtCrop;
		GFX::showCursor(true);
		cropOffset.x = 0;
		cropOffset.y = 0;
		cropSize.x = levelImage->w;
		cropSize.y = levelImage->h;
	}
	if (input->isKey("m"))
	{
		++brushSize;
		input->resetKeys();
	}
	if (input->isKey("n"))
	{
		brushSize -= (brushSize > 1) ? 1 : 0;
		input->resetKeys();
	}
	if (drawTool == dtBrush)
	{
		if (input->isLeftClick())
		{
			brushRect.x = input->getMouseX() + editorOffset.x - brushSize / 2.0f;
			brushRect.y = input->getMouseY() + editorOffset.y - brushSize / 2.0f;
			brushRect.w = brushSize;
			brushRect.h = brushSize;
			SDL_FillRect(levelImage, &brushRect, brushCol.getSDL_Uint32Colour(GFX::getVideoSurface()));
		}
	}
	else if (drawTool == dtCrop)
	{
		if (input->isLeftClick())
		{
			if (cropEdge == diNONE)
			{
				if (input->getMouse().inRect(-editorOffset.x + cropOffset.x - EDITOR_CROP_RECT_WIDTH - 1, -editorOffset.y + cropOffset.y, EDITOR_CROP_RECT_WIDTH + 1, cropSize.y))
					cropEdge = diLEFT;
				else if (input->getMouse().inRect(-editorOffset.x + cropOffset.x + cropSize.x, -editorOffset.y + cropOffset.y, EDITOR_CROP_RECT_WIDTH + 1, cropSize.y))
					cropEdge = diRIGHT;
				else if (input->getMouse().inRect(-editorOffset.x + cropOffset.x, -editorOffset.y + cropOffset.y - EDITOR_CROP_RECT_WIDTH - 1, cropSize.x, EDITOR_CROP_RECT_WIDTH + 1))
					cropEdge = diTOP;
				else if (input->getMouse().inRect(-editorOffset.x + cropOffset.x, -editorOffset.y + cropOffset.y + cropSize.y, cropSize.x, EDITOR_CROP_RECT_WIDTH + 1))
					cropEdge = diBOTTOM;
				else
					cropEdge = diMIDDLE; // No valid edge
			}
			else if (input->getMouse() != lastPos)
			{
				switch (cropEdge.value)
				{
					case diLEFT: cropOffset.x += input->getMouseX() - lastPos.x; cropSize.x -= input->getMouseX() - lastPos.x; break;
					case diRIGHT: cropSize.x += input->getMouseX() - lastPos.x; break;
					case diTOP: cropOffset.y += input->getMouseY() - lastPos.y; cropSize.y -= input->getMouseY() - lastPos.y; break;
					case diBOTTOM: cropSize.y += input->getMouseY() - lastPos.y; break;
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
		else
		{
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
			cropOffset.x = 0;
			cropOffset.y = 0;
			drawTool = dtBrush;
			GFX::showCursor(false);
			input->resetKeys();
		}
		lastPos = input->getMouse();
	}

	if (isCancelKey(input))
	{
		editorState = esSettings;
		GFX::showCursor(true);
		input->resetKeys();
	}
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

	if (drawTool == dtBrush)
	{
		Vector2di pos = input->getMouse();
		pos.x -= brushSize / 2;
		pos.y -= brushSize / 2;
		if (pos.y >= 0)
		{
			for (int I = max(-pos.x, 0); I < brushSize && pos.x + I < GFX::getXResolution(); ++I)
			{
				GFX::setPixel(screen, pos.x + I, pos.y, Colour(WHITE) - GFX::getPixel(screen, pos.x + I, pos.y));
			}
		}
		if (pos.y + brushSize - 1 < GFX::getYResolution())
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
}

void Editor::renderUnits()
{

}

void Editor::renderTest()
{
	Level::render(GFX::getVideoSurface());
}

