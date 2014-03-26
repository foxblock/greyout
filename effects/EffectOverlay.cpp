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

#include "EffectOverlay.h"

EffectOverlay::EffectOverlay(const Colour& col) : BaseEffect()
{
    rect.setPosition(0,0);
    rect.setDimensions(GFX::getXResolution(),GFX::getYResolution());
    rect.setColour(col);

    type = etOverlay;
    limit = 1;
}

EffectOverlay::~EffectOverlay()
{

}

void EffectOverlay::update()
{
    //
}

void EffectOverlay::render()
{
    rect.render();
}
