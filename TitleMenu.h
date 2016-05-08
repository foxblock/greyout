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

#ifndef TITLEMENU_H
#define TITLEMENU_H

#include "BaseState.h"
#include "PenjinTypes.h"
#include "AnimatedSprite.h"
#include "Rectangle.h"

#if defined(_DEBUG) && !defined(PENJIN_CALC_FPS)
#define PENJIN_CALC_FPS
#endif

#ifdef PENJIN_CALC_FPS
#include "Text.h"
#endif

class TitleMenu : public BaseState
{
	public:
		TitleMenu();
		virtual ~TitleMenu();

		virtual void init();
		virtual void userInput();
		virtual void update();
		virtual void render();
	protected:

	private:
		void updateSelection(CRbool immediate);
		void incSelection();
		void decSelection();
		void doSelection();
		void inverse(SDL_Surface* const surf, const SDL_Rect& rect);

		SDL_Surface *bg;
		SDL_Surface *settingsBg;
		AnimatedSprite marker;
		AnimatedSprite title;
		AnimatedSprite items;
		static int selection;
		bool mouseInBounds;
		SDL_Rect invertRegion;
		SDL_Rect bgRegion;
		Vector2di lastPos;
		Vector2df bgPos;
		Vector2df bgVel;
		Vector2df sizeDiff;
		Rectangle overlay;

		int fadeTimer;
};


#endif // TITLEMENU_H

