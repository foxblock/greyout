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

#include "PixelParticle.h"

PixelParticle::PixelParticle(Level* const parent, CRint lifeTime) : BaseUnit(parent)
{
	toBeRemoved = false;
	counter = lifeTime;
	offset = Vector2di(0,0);
}

PixelParticle::~PixelParticle()
{
	//
}

void PixelParticle::update()
{
	if ( counter > 0 )
		--counter;
	else
		toBeRemoved = true;

	position += velocity;
}

void PixelParticle::updateScreenPosition(const Vector2di& offset)
{
	this->offset = offset;
}

void PixelParticle::render(SDL_Surface* screen)
{
	GFX::setPixel(screen,position.x-offset.x,position.y-offset.y,col);
}

void PixelParticle::hitMap(const Vector2df& correction)
{
	if (abs(correction.x) > abs(correction.y))
	{
		velocity.x *= -1;
	}
	else if (correction.y != 0)
	{
		velocity.y *= -0.5;
	}
}
