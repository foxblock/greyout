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
