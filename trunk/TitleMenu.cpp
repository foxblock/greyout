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

#include "TitleMenu.h"

#include "userStates.h"
#include "Colour.h"
#include "NumberUtility.h"

#include "MyGame.h"
#include "GreySurfaceCache.h"
#include "effects/Hollywood.h"
#include "MusicCache.h"
#include "Music.h"
#include "globalControls.h"

#define DEFAULT_SELECTION 0
#define MENU_ITEM_COUNT 4

#ifdef _MEOW
#define MENU_OFFSET_Y 101
#define MENU_OFFSET_TITLE_Y 18
#define MENU_ITEM_HEIGHT 27
#define MENU_ITEM_SPACING 0
#define MARKER_SPEED 3
#else
#define MENU_OFFSET_Y 178
#define MENU_OFFSET_TITLE_Y 40
#define MENU_ITEM_HEIGHT 61
#define MENU_ITEM_SPACING 0
#define MARKER_SPEED 5
#endif

int TitleMenu::selection = DEFAULT_SELECTION;

TitleMenu::TitleMenu()
{
	invertRegion.w = GFX::getXResolution();
	invertRegion.h = MENU_ITEM_HEIGHT;
	invertRegion.x = 0;
	invertRegion.y = 0;
	bgRegion.w = GFX::getXResolution();
	bgRegion.h = GFX::getYResolution();
	bgRegion.x = 0;
	bgRegion.y = 0;

#ifdef _MEOW
	bg = SURFACE_CACHE->loadSurface("images/menu/title_320_240_bg.png");
	SDL_Surface* temp2 = SURFACE_CACHE->loadSurface("images/menu/title_320_240_items.png");
#else
	bg = SURFACE_CACHE->loadSurface("images/menu/title_800_480_bg.png");
	SDL_Surface* temp2 = SURFACE_CACHE->loadSurface("images/menu/title_800_480_items.png");
#endif
#ifdef _MEOW
	marker.loadFrames(SURFACE_CACHE->loadSurface("images/menu/title_320_240_marker.png"), 1, 2, 0, 0);
#else
	marker.loadFrames(SURFACE_CACHE->loadSurface("images/menu/title_800_480_marker.png"), 1, 2, 0, 0);
#endif
	title.loadFrames(SURFACE_CACHE->loadSurface("images/menu/title_800_480_header.png"), 1, 1, 0, 0);
	title.setPosition((GFX::getXResolution() - title.getWidth()) / 2.0f, MENU_OFFSET_TITLE_Y);
	items.loadFrames(temp2, 1, 1, 0, 0);
	items.setPosition(0, MENU_OFFSET_Y - (items.getHeight() - (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING) * MENU_ITEM_COUNT + MENU_ITEM_SPACING) / 2.0f);
	updateSelection(true);
	lastPos = Vector2di(0, 0);
	mouseInBounds = false;
	fadeTimer = -1;
	sizeDiff.x = bg->w - GFX::getXResolution();
	sizeDiff.y = bg->h - GFX::getYResolution();
	bgPos.x = sizeDiff.x;
	bgPos.y = 0;
	bgVel.x = -(sizeDiff.x) / 120.0f;
	bgVel.y = (sizeDiff.y) / 120.0f;

	overlay.setDimensions(GFX::getXResolution(), GFX::getYResolution());
	overlay.setPosition(0, 0);
	overlay.setColour(BLACK);
	overlay.setAlpha(128);
}

TitleMenu::~TitleMenu()
{
	//W
}

void TitleMenu::init()
{
	input->resetKeys(); // avoid sticky keys when returning from level
	EFFECTS->fadeIn(30);

	MUSIC_CACHE->playMusic("music/title_menu.ogg");
}

void TitleMenu::userInput()
{
	if ( fadeTimer >= 0 )
		return;
	if (lastPos != input->getMouse())
	{
		if ( input->getMouseY() > MENU_OFFSET_Y &&
				input->getMouseY() < MENU_OFFSET_Y + (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING) * MENU_ITEM_COUNT)
		{
			int prevSel = selection;
			selection = (input->getMouseY() - MENU_OFFSET_Y) / (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING);
			mouseInBounds = true;
			updateSelection(false);
			lastPos = input->getMouse();
			if (selection != prevSel)
				MUSIC_CACHE->playSound("sounds/menu.wav");
		}
		else
			mouseInBounds = false;
	}

	if (input->isUp())
		decSelection();
	else if (input->isDown())
		incSelection();

	if ( isAcceptKey(input) || ( input->isLeftClick() && mouseInBounds ) )
		doSelection();

	input->resetKeys();
}

void TitleMenu::update()
{
	updateSelection(false);
	EFFECTS->update();
	if ( fadeTimer > 0 )
		--fadeTimer;
	else if ( fadeTimer == 0 )
	{
		doSelection();
	}
	bgPos += bgVel;
	if (bgPos.x <= 0 && bgPos.y >= sizeDiff.y)
	{
		bgPos.x = sizeDiff.x;
		bgPos.y = 0;
	}
	bgRegion.x = bgPos.x;
	bgRegion.y = bgPos.y;
}

void TitleMenu::render()
{
	GFX::clearScreen();

	SDL_BlitSurface(bg, &bgRegion, GFX::getVideoSurface(), NULL);

	title.render();
	items.render();

	inverse(GFX::getVideoSurface(), invertRegion);

	marker.setY(invertRegion.y - marker.getHeight());
	marker.setCurrentFrame(0);
	marker.render();
	marker.setY(invertRegion.y + invertRegion.h);
	marker.setCurrentFrame(1);
	marker.render();

	if (ENGINE->settings->isActive())
		overlay.render();

	EFFECTS->render();
}

void TitleMenu::updateSelection(CRbool immediate)
{
	int destination = MENU_OFFSET_Y + (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING) * selection;

	if (immediate)
	{
		invertRegion.y = destination;
	}
	else
	{
		int diff = destination - invertRegion.y;
		invertRegion.y = invertRegion.y + NumberUtility::closestToZero(MARKER_SPEED * NumberUtility::sign(diff), diff);
	}
}

void TitleMenu::incSelection()
{
	if (selection < MENU_ITEM_COUNT - 1)
		++selection;
	updateSelection(false);
	MUSIC_CACHE->playSound("sounds/menu.wav");
}

void TitleMenu::decSelection()
{
	if (selection > 0)
		--selection;
	updateSelection(false);
	MUSIC_CACHE->playSound("sounds/menu.wav");
}

void TitleMenu::doSelection()
{
	if ( selection != 2 && fadeTimer < 0 )
	{
		EFFECTS->fadeOut(30);
		fadeTimer = 30;
		return;
	}
	switch (selection)
	{
	case 0:
		if (ENGINE->activeChapter[0] != 0)
			ENGINE->playChapter(ENGINE->activeChapter);
		else
			ENGINE->playChapter(DEFAULT_CHAPTER);
		break;
	case 1:
		setNextState(STATE_LEVELSELECT);
		break;
	case 2:
		ENGINE->settings->show();
		break;
	case 3:
		nullifyState();
		break;
	default:
		setNextState(STATE_LEVEL);
	}
}

void TitleMenu::inverse(SDL_Surface* const surf, const SDL_Rect& rect)
{
	if (SDL_MUSTLOCK(surf))
		SDL_LockSurface(surf);
	int bpp = surf->format->BytesPerPixel;
	for (int X = rect.x; X < rect.x + rect.w; ++X)
	{
		for (int Y = rect.y; Y < rect.y + rect.h; ++Y)
		{
			Uint8 *p = (Uint8 *)surf->pixels + Y * surf->pitch + X * bpp;
			switch(bpp)
			{
			case 1:
				*p = Uint8(-1) - *p;
				break;
			case 2:
				*(Uint16 *)p = Uint16(-1) - *(Uint16 *)p;
				break;
			case 3:
			{
				Uint32 *pixel = (Uint32 *)p;
				if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				{
					p[0] = -1 << 16 - (*pixel >> 16) & 0xff;
					p[1] = -1 << 8 - (*pixel >> 8) & 0xff;
					p[2] = -1 - *pixel & 0xff;
				}
				else
				{
					p[0] = -1 - *pixel & 0xff;
					p[1] = -1 << 8 - (*pixel >> 8) & 0xff;
					p[2] = -1 << 16 - (*pixel >> 16) & 0xff;
				}
				break;
			}
			case 4:
				*(Uint32 *)p = Uint32(-1) - *(Uint32 *)p;
				break;
			}
		}
	}
	if (SDL_MUSTLOCK(surf))
		SDL_UnlockSurface(surf);
}
