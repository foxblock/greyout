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

#include "Camera.h"

#include "GFX.h"

#include "BaseUnit.h"
#include "Level.h"

#define MOVEMENT_VIEW_FACTOR 10.0f

Camera::Camera()
{
    parent = NULL;
    disregardBoundaries = false;
    speed = Vector2df(0,0);
    dest = Vector2df(0,0);
}

Camera::~Camera()
{
    //
}

// TODO: Maybe implement non-moving area in which the player can move without the camera moving

void Camera::update()
{
    parent->drawOffset += speed;
    if ((speed.x != 0 || speed.y != 0) &&
        abs(parent->drawOffset.x - dest.x) < 2 && abs(parent->drawOffset.y - dest.y) < 2)
    {
        speed = Vector2df(0,0);
        parent->drawOffset = dest;
    }
}

void Camera::reset()
{
    speed = Vector2df(0,0);
    dest = Vector2df(0,0);
}

void Camera::centerOnUnit(const BaseUnit* const unit, CRint time)
{
    if (not parent || not unit)
        return;

    centerOnPos(unit->getPixel(diMIDDLE) + unit->velocity * MOVEMENT_VIEW_FACTOR,time);
}

void Camera::centerOnPos(const Vector2df& pos, CRint time)
{
    Vector2df result = parent->drawOffset;

    if (parent->getWidth() > GFX::getXResolution())
        result.x = pos.x - (GFX::getXResolution() / 2.0f);
    if (parent->getHeight() > GFX::getYResolution())
        result.y = pos.y - (GFX::getYResolution() / 2.0f);

    if (not disregardBoundaries)
    {
        // make sure not moving past the levels boundaries
        if (parent->getWidth() >= GFX::getXResolution())
        {
            result.x += max(-result.x,0.0f); // left
            result.x += min(parent->getWidth() - (result.x + GFX::getXResolution()),0.0f); // right
        }
        if ((parent->getHeight() >= GFX::getYResolution()))
        {
            result.y += min(parent->getHeight() - (result.y + GFX::getYResolution()),0.0f); // bottom
            result.y += max(-result.y,0.0f); // top
        }
    }
    if (time == 0)
    {
        parent->drawOffset = result;
        speed = Vector2df(0,0);
    }
    else
    {
    	// todo: use frames instead of ticks
        speed = (result - parent->drawOffset) / (float)time / (float)FRAME_RATE * 1000.0f;
        dest = result;
    }
}
