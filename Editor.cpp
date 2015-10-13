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

#include "LevelLoader.h"
#include "userStates.h"
#include "BaseUnit.h"
#include "ControlUnit.h"
#include "PixelParticle.h"
#include "Link.h"

#include "IMG_savepng.h"

Editor::Editor() :
	 editorState(esStart)
	,lastState(esStart)
{

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
		renderUnit(screen);
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

}

void Editor::inputSettings()
{

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

}

void Editor::renderSettings(SDL_Surface* screen)
{

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

