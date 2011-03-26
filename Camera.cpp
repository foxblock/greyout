#include "Camera.h"

#include "GFX.h"

#include "BaseUnit.h"
#include "Level.h"

Camera::Camera()
{
    parent = NULL;
    disregardBoundaries = false;
}

Camera::~Camera()
{
    //
}

// TODO: Implement time
// TODO: Maybe implement non-moving area in which the player can move without the camera moving

void Camera::centerOnUnit(const BaseUnit* const unit, CRint time)
{
    if (not parent || not unit)
        return;

    centerOnPos(unit->getPixel(diMIDDLE),time);
}

void Camera::centerOnPos(const Vector2df& pos, CRint time)
{
    Vector2df result(0,0);
    result.x = pos.x - (GFX::getXResolution() / 2.0f);
    #ifdef _DEBUG_COL
    result.y = pos.y - (GFX::getYResolution() / 4.0f);
    #else
    result.y = pos.y - (GFX::getYResolution() / 2.0f);
    #endif

    #ifdef _DEBUG_COL
    if (not disregardBoundaries)
    #else
    if (not disregardBoundaries )
    #endif
    {
        // make sure not moving past the levels boundaries
        if (parent->getWidth() >= GFX::getXResolution())
        {
            result.x += max(-result.x,0.0f); // left
            result.x += min(parent->getWidth() - (result.x + GFX::getXResolution()),0.0f); // right
        }
        #ifdef _DEBUG_COL
        if (parent->getHeight() >= GFX::getYResolution() / 2.0f)
        {
            result.y += min(parent->getHeight() - (result.y + GFX::getYResolution() / 2.0f),0.0f);
        #else
        if ((parent->getHeight() >= GFX::getYResolution()))
        {
            result.y += min(parent->getHeight() - (result.y + GFX::getYResolution()),0.0f); // bottom
        #endif
            result.y += max(-result.y,0.0f); // top
        }
    }

    parent->drawOffset = result;
}
