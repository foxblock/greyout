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

#include "BaseUnitContainer.h"

#include "NumberUtility.h"

#include "Physics.h"

BaseUnitContainer::BaseUnitContainer()
{
    //
}

BaseUnitContainer::~BaseUnitContainer()
{
    units.clear();
}

void BaseUnitContainer::update()
{
    for (vector<BaseUnit*>::iterator curr = units.begin(); curr != units.end(); ++curr)
    {
        (*curr)->update();
    }
}

void BaseUnitContainer::updateScreenPosition(const Vector2df& offset)
{
    for (vector<BaseUnit*>::iterator curr = units.begin(); curr != units.end(); ++curr)
    {
        (*curr)->updateScreenPosition(offset);
    }
}

void BaseUnitContainer::render(SDL_Surface* surf)
{
    for (vector<BaseUnit*>::iterator curr = units.begin(); curr != units.end(); ++curr)
    {
        (*curr)->render(surf);
    }
}

void BaseUnitContainer::handleMapCollision(SDL_Surface* const level, const Vector2df& mapOffset)
{
    Vector2df correctionMax(0,0);
    for (vector<BaseUnit*>::iterator curr = units.begin(); curr != units.end(); ++curr)
    {
        //PHYSICS->unitMapCollision(this,level,(*curr),mapOffset);
        correctionMax.x = NumberUtility::furthestFromZero(correctionMax.x,(*curr)->collisionInfo.correction.x);
        correctionMax.y = NumberUtility::furthestFromZero(correctionMax.y,(*curr)->collisionInfo.correction.y);
    }
    for (vector<BaseUnit*>::iterator curr = units.begin(); curr != units.end(); ++curr)
    {
        (*curr)->hitMap(correctionMax);
    }
}

void BaseUnitContainer::handleUnitCollision()
{
}
