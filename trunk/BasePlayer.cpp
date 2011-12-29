#include "BasePlayer.h"

#include "Level.h"

#define SPRITESHEET_W 8
#define SPRITESHEET_H 7
#define FRAMERATE DECI_SECONDS

BasePlayer::BasePlayer(Level* newParent) : ControlUnit(newParent)
{
    canJump = false;
    flags.addFlag(ufMissionObjective);
    fallCounter.init(500,MILLI_SECONDS);
}

BasePlayer::~BasePlayer()
{
    //
}

///---public---

// Custom load implementation to ensure the unit is initialized properly after loading
bool BasePlayer::load(const list<PARAMETER_TYPE >& params)
{
    bool result = BaseUnit::load(params);

    if (imageOverwrite[0] == 0)
    {
        imageOverwrite = "images/player/black_big.png";
    }
    SDL_Surface* surf = getSurface(imageOverwrite);
    AnimatedSprite* temp;
    loadFrames(surf,0,1,false,"stand");
    temp = loadFrames(surf,1,7,true,"fallRight");
    temp->setPlayMode(pmReverse);
    temp = loadFrames(surf,8,3,false,"turnRight");
    temp->setFrameRate(FIFTEEN_FRAMES);
    temp = loadFrames(surf,12,4,true,"pushRight");
    temp->setPlayMode(pmPulse);
    temp = loadFrames(surf,16,6,true,"runRight");
    temp->setPlayMode(pmPulse);
    loadFrames(surf,22,2,false,"jumpRight");
    loadFrames(surf,23,1,false,"flyRight");
    temp = loadFrames(surf,25,7,true,"fallLeft");
    temp->setPlayMode(pmReverse);
    temp = loadFrames(surf,32,3,false,"turnLeft");
    temp->setFrameRate(FIFTEEN_FRAMES);
    temp = loadFrames(surf,36,4,true,"pushLeft");
    temp->setPlayMode(pmPulse);
    temp = loadFrames(surf,40,6,true,"runLeft");
    temp->setPlayMode(pmPulse);
    loadFrames(surf,46,2,false,"jumpLeft");
    loadFrames(surf,47,1,false,"flyLeft");
    temp = loadFrames(surf,48,3,false,"wave");
    temp->setLooping(2);
    temp->setPlayMode(pmPulse);
    temp->setFrameRate(FIFTEEN_FRAMES);

    if (takesControl)
        startingState = "wave";
    else
        startingState = "stand";

    return result;
}

void BasePlayer::reset()
{
    if (takesControl)
        startingState = "wave";
    else
        startingState = "stand";
    BaseUnit::reset();
}

void BasePlayer::resetTemporary()
{
    BaseUnit::resetTemporary();

    if (currentSprite->getLoops() == -1 || currentSprite->hasFinished())
    {
        setSpriteState("stand");
    }
}

void BasePlayer::update()
{
    if (collisionInfo.correction.y < 0) // on the ground
    {
        if (currentState == "stand")
        {
            if (velocity.x < 0.0f)
                setSpriteState("runLeft");
            else if (velocity.x > 0.0f)
                setSpriteState("runRight");
        }
    }
    else // air
    {
        if (currentState == "stand")
        {
            if (true)
            {
                if (fallCounter.hasFinished() || not fallCounter.isStarted())
                {
                    if (velocity.y > 3.0f)
                    {
                        if (direction < 0)
                            setSpriteState("fallLeft");
                        else
                            setSpriteState("fallRight");
                    }
                }
                else if (not canJump)
                {
                    if (direction < 0)
                        setSpriteState("flyLeft");
                    else
                        setSpriteState("flyRight");
                }
            }
        }
    }

    BaseUnit::update();

    if ((int)velocity.y != 0)
        canJump = false;
}

void BasePlayer::hitMap(const Vector2df& correctionOverride)
{
    BaseUnit::hitMap(correctionOverride);

    if (correctionOverride.y < 0.0f)
        canJump = true;
    else if (correctionOverride.y > 0.0f)
    {
        acceleration[0].y = 0;
        acceleration[1].y = 0;
    }
}


void BasePlayer::control(SimpleJoy* input)
{
    if (input->isLeft())
    {
        if (direction > 0 || (int)velocity.x == 0)
        {
            setSpriteState("turnLeft",true);
        }
        acceleration[0].x = -1;
        acceleration[1].x = -4;
    }
    else if (input->isRight())
    {
        if (direction < 0 || (int)velocity.x == 0)
        {
            setSpriteState("turnRight",true);
        }
        acceleration[0].x = 1;
        acceleration[1].x = 4;
    }
    else
    {
        if (canJump && currentState != "wave")
        {
            setSpriteState("stand");
        }
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
    if ((input->isB() || input->isY()) && canJump)
    {
        acceleration[0].y = -4;
        acceleration[1].y = -9;
        //velocity.y = -10;
        canJump = false;
        if (direction < 0)
            setSpriteState("jumpLeft",true);
        else
            setSpriteState("jumpRight",true);
        fallCounter.start();
    }
}

///---private---

AnimatedSprite* BasePlayer::loadFrames(SDL_Surface* const surf, CRint skip, CRint num, CRbool loop, CRstring state)
{
    AnimatedSprite* temp = new AnimatedSprite;
    temp->loadFrames(surf,SPRITESHEET_W,SPRITESHEET_H,skip,num);
    temp->setTransparentColour(MAGENTA);
    temp->setFrameRate(FRAMERATE);
    temp->setLooping(loop);
    states[state] = temp;
    return temp;
}
