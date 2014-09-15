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

#include "Playground.h"

#include "LevelLoader.h"
#include "userStates.h"
#include "BaseUnit.h"
#include "ControlUnit.h"
#include "PixelParticle.h"
#include "Link.h"

Playground::Playground()
{
#ifdef _DEBUG
	mouseDraw = true;
#endif
}

Playground::~Playground()
{
	for (vector<Rectangle*>::iterator curr = mouseRects.begin(); curr != mouseRects.end(); ++curr)
	{
		delete (*curr);
	}
	mouseRects.clear();
}

///---public---

void Playground::userInput()
{
	if (input->isStart())
	{
		pauseToggle();
		return;
	}
	if (input->isY())
	{
		// spwan a black box
		list<PARAMETER_TYPE > params;
		Vector2df pos = drawOffset + input->getMouse() - Vector2df(16,16);
		params.push_back(make_pair("class","pushablebox"));
		params.push_back(make_pair("collision","0,255"));
		params.push_back(make_pair("size","32,32"));
		params.push_back(make_pair("position",StringUtility::vecToString(pos)));
		BaseUnit* box = LEVEL_LOADER->createUnit(params,this);
		units.push_back(box);

		input->resetY();
	}
	if (input->isX())
	{
		// spwan a white box
		list<PARAMETER_TYPE > params;
		Vector2df pos = drawOffset + input->getMouse() - Vector2df(16,16);
		params.push_back(make_pair("class","pushablebox"));
		params.push_back(make_pair("collision","white,255"));
		params.push_back(make_pair("size","32,32"));
		params.push_back(make_pair("colour","white"));
		params.push_back(make_pair("position",StringUtility::vecToString(pos)));
		BaseUnit* box = LEVEL_LOADER->createUnit(params,this);
		units.push_back(box);

		input->resetX();
	}
	if (input->isA())
	{
		for (vector<ControlUnit*>::iterator iter = players.begin(); iter != players.end(); ++iter)
		{
			(*iter)->explode();
		}
		lose();
		input->resetKeys();
	}


	if ((input->isL() || input->isR()) && not flags.hasFlag(lfDisableSwap))
	{
		if (players.size() > 1)
		{
			swapControl( input->isR() );
		}
		input->resetL();
		input->resetR();
	}

	if (input->isLeftClick())
	{
	#ifdef _DEBUG
		if (mouseDraw)
		{
	#endif
		Rectangle* temp = new Rectangle;
		if (input->isRightClick())
			temp->setColour(Colour(9999763));
		else
			temp->setColour(BLACK);
		temp->setDimensions(24,24);
		temp->setPosition(drawOffset + input->getMouse() - Vector2df(12,12));
		mouseRects.push_back(temp);
	#ifdef _DEBUG
		}
		else
		{
			Vector2df pos = input->getMouse() + drawOffset;
			for (vector<BaseUnit*>::iterator I = units.begin(); I != units.end(); ++I)
			{
				if (pos.inRect((*I)->getRect()))
					debugUnits.push_back(*I);
			}
			for (vector<ControlUnit*>::iterator I = players.begin(); I != players.end(); ++I)
			{
				if (pos.inRect((*I)->getRect()))
					debugUnits.push_back(*I);
			}
		}
	#endif
	}
	else if (input->isRightClick())
	{
	#ifdef _DEBUG
		if (mouseDraw)
		{
	#endif
		Rectangle* temp = new Rectangle;
		temp->setColour(WHITE);
		temp->setDimensions(24,24);
		temp->setPosition(input->getMouse() - Vector2df(12,12));
		mouseRects.push_back(temp);
	#ifdef _DEBUG
		}
		else
		{
			if (input->isRightClick())
				debugUnits.clear();
		}
	#endif
	}
#ifdef _DEBUG
	if (input->isSelect())
	{
		mouseDraw = !mouseDraw;
		input->resetSelect();
	}
	if (!mouseDraw)
		input->resetMouseButtons();
#endif

	for (vector<ControlUnit*>::iterator curr = players.begin(); curr != players.end(); ++curr)
	{
		if ((*curr)->takesControl)
			(*curr)->control(input);
	}
	input->resetA();
}

void Playground::render(SDL_Surface* screen)
{
	SDL_Rect src;
	SDL_Rect dst;
	dst.x = max(-drawOffset.x,0.0f);
	dst.y = max(-drawOffset.y,0.0f);
	src.x = max(drawOffset.x,0.0f);
	src.y = max(drawOffset.y,0.0f);
	src.w = min((int)GFX::getXResolution(),getWidth() - src.x);
	src.h = min((int)GFX::getYResolution(),getHeight() - src.y);

	for (vector<Rectangle*>::iterator curr = mouseRects.begin(); curr != mouseRects.end(); ++curr)
	{
		(*curr)->render(collisionLayer);
		(*curr)->render(levelImage);
		delete (*curr);
	}
	mouseRects.clear();

	SDL_BlitSurface(collisionLayer,&src,screen,&dst);

	dst.x = max(drawOffset.x,0.0f);
	dst.y = max(drawOffset.y,0.0f);
	src.x = max(-drawOffset.x,0.0f);
	src.y = max(-drawOffset.y,0.0f);
	src.w = min((int)GFX::getXResolution(),getWidth());
	src.h = min((int)GFX::getYResolution(),getHeight());

	// draw to image used for collision testing before players get drawn
	//SDL_BlitSurface(screen,&src,collisionLayer,&dst);

	// players don't get drawn to the collision surface
	for (vector<ControlUnit*>::iterator curr = players.begin(); curr != players.end(); ++curr)
	{
		renderUnit(screen,(*curr),drawOffset);
	}

	// particles
	for (vector<PixelParticle*>::iterator curr = effects.begin(); curr != effects.end(); ++curr)
	{
		(*curr)->updateScreenPosition(drawOffset);
		(*curr)->render(screen);
		GFX::renderPixelBuffer();
	}

	// links
	for (vector<Link*>::iterator I = links.begin(); I != links.end(); ++I)
		(*I)->render(screen);
}

#ifdef _DEBUG
string Playground::debugInfo()
{
	if (mouseDraw)
		return "DRAW MODE\n" + Level::debugInfo();
	else
		return "SELECT MODE\n" + Level::debugInfo();
}
#endif

///---protected---

///---private---
