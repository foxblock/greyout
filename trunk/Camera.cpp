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
}

Camera::~Camera()
{
    //
}

// TODO: Maybe implement non-moving area in which the player can move without the camera moving

void Camera::update()
{
    parent->drawOffset += speed;
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
        parent->drawOffset = result;
    else
        speed = (result - parent->drawOffset) / (float)time / (float)FRAME_RATE * 1000.0f;
}
