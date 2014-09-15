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

#include "EffectFade.h"

EffectFade::EffectFade(CRint duration, const Colour& col) : BaseEffect()
{
	timer = abs(duration);
	rect.setPosition(0,0);
	rect.setDimensions(GFX::getXResolution(),GFX::getYResolution());
	rect.setColour(col);
	if (duration < 0) // fade out to colour
		rect.setAlpha(0);
	fadeTime = duration;
	type = etFade;
	limit = 1;
}

EffectFade::~EffectFade()
{
	//
}

void EffectFade::update()
{
	if (timer > 0)
	{
		if (fadeTime < 0) // fade out
		{
			rect.setAlpha(255 + round((float)timer / (float)fadeTime * 255.0f));
		}
		else if (fadeTime > 0) // fade in
		{
			rect.setAlpha(round((float)timer / (float)fadeTime * 255.0f));
		}
		--timer;
	}
	else
		finished = true;
}

void EffectFade::render()
{
	rect.render();
}
