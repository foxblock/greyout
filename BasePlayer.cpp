#include "BasePlayer.h"

#include "Level.h"

BasePlayer::BasePlayer(Level* newParent) : ControlUnit(newParent)
{
    //
}

BasePlayer::~BasePlayer()
{
    //
}

// Custom load implementation to ensure the unit is initialized properly after loading
bool BasePlayer::load(const PARAMETER_TYPE& params)
{
    bool result = BaseUnit::load(params);

    if (imageOverwrite[0] == 0)
    {
        imageOverwrite = "images/player/m_black.png";
    }
    AnimatedSprite* temp = new AnimatedSprite;
    temp->loadFrames(getSurface(imageOverwrite),5,3,10,5);
    temp->setTransparentColour(MAGENTA);
    temp->setFrameRate(DECI_SECONDS);
    temp->setPlayMode(pmPulse);
    currentSprite = temp;
    states["temp"] = temp;

    return result;
}

void BasePlayer::control(SimpleJoy* input)
{
    if (input->isLeft())
    {
        acceleration[0].x = -2;
        acceleration[1].x = -16;
    }
    else if (input->isRight())
    {
        acceleration[0].x = 2;
        acceleration[1].x = 16;
    }
    else
    {
        acceleration[0].x = 0;
        acceleration[1].x = 0;
        velocity.x = 0;
    }/*
    if (input->isUp())
    {
        acceleration[0].y = -2;
        acceleration[1].y = -16;
    }
    else if (input->isDown())
    {
        acceleration[0].y = 2;
        acceleration[1].y = 16;
    }
    else
    {
        acceleration[0].y = 0;
        acceleration[1].y = 0;
        velocity.y = 0;
    }*/
    if (input->isA())
    {
        velocity.y = -20;
    }
}

void BasePlayer::update()
{
    BaseUnit::update();

    #ifdef _DEBUG
    parent->debugString += "P: " + StringUtility::vecToString(position) + "\n" +
        "V: " + StringUtility::vecToString(velocity) + "\n" +
        "A: " + StringUtility::vecToString(acceleration[0]) + " to " + StringUtility::vecToString(acceleration[1]) + "\n";
    #endif
}
