#include "BasePlayer.h"

#include "Level.h"

#define SPRITESHEET_W 8
#define SPRITESHEET_H 9
#define FRAMERATE DECI_SECONDS

BasePlayer::BasePlayer(Level* newParent) : ControlUnit(newParent)
{
    canJump = false;
    isJumping = false;
    flags.addFlag(ufMissionObjective);
    fallCounter.init(500,MILLI_SECONDS);
}

BasePlayer::~BasePlayer()
{
    //
}

///---public---

// Custom load implementation to ensure the unit is initialized properly after loading
bool BasePlayer::load(list<PARAMETER_TYPE >& params)
{
    bool result = BaseUnit::load(params);

    if (imageOverwrite[0] == 0)
    {
        imageOverwrite = "images/player/black_big.png";
    }
    else // clear sprites loaded by BaseUnit
    {
        for (map<string,AnimatedSprite*>::iterator I = states.begin(); I != states.end(); ++I)
        {
            delete I->second;
        }
        states.clear();
    }
    SDL_Surface* surf = getSurface(imageOverwrite);
    AnimatedSprite* temp;

    loadFrames(surf,0,1,false,"stand");
    temp = loadFrames(surf,1,3,false,"wave");
    temp->setLooping(2);
    temp->setPlayMode(pmPulse);
    temp->setFrameRate(FIFTEEN_FRAMES);

    temp = loadFrames(surf,32,7,true,"fallright");
    temp->setPlayMode(pmReverse);
    temp = loadFrames(surf,8,3,false,"turnright");
    temp->setFrameRate(FIFTEEN_FRAMES);
    temp = loadFrames(surf,48,7,true,"pushright");
    temp->setFrameRate(CUSTOM);
    temp->setTimerScaler(1000/7);
    temp = loadFrames(surf,16,6,true,"runright");
    temp->setPlayMode(pmPulse);
    loadFrames(surf,64,2,false,"jumpright");
    loadFrames(surf,65,1,false,"flyright");

    temp = loadFrames(surf,40,7,true,"fallleft");
    temp->setPlayMode(pmReverse);
    temp = loadFrames(surf,11,3,false,"turnleft");
    temp->setFrameRate(FIFTEEN_FRAMES);
    temp = loadFrames(surf,56,7,true,"pushleft");
    temp->setFrameRate(CUSTOM);
    temp->setTimerScaler(1000/7);
    temp->setPlayMode(pmReverse);
    temp = loadFrames(surf,24,6,true,"runleft");
    temp->setPlayMode(pmPulse);
    loadFrames(surf,66,2,false,"jumpleft");
    loadFrames(surf,67,1,false,"flyleft");

    if (startingState[0] == 0 || startingState == "default")
    {
        if (takesControl)
            startingState = "wave";
        else
            startingState = "stand";
    }
    setSpriteState(startingState,true);

    return result;
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
                setSpriteState("runleft");
            else if (velocity.x > 0.0f)
                setSpriteState("runright");
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
                            setSpriteState("fallleft");
                        else
                            setSpriteState("fallright");
                    }
                }
                else if (isJumping)
                {
                    if (direction < 0)
                        setSpriteState("flyleft");
                    else
                        setSpriteState("flyright");
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
        isJumping = false;
    else if (correctionOverride.y > 0.0f)
    {
        acceleration[0].y = 0;
        acceleration[1].y = 0;
    }
}


void BasePlayer::control(SimpleJoy* input)
{
	if (!input)
		return;

    if (input->isLeft())
    {
        if ((direction > 0 || (int)velocity.x == 0) && ((int)velocity.y == 0))
        {
            setSpriteState("turnleft",true);
        }
        acceleration[0].x = -1;
        acceleration[1].x = -4;
    }
    else if (input->isRight())
    {
        if ((direction < 0 || (int)velocity.x == 0) && ((int)velocity.y == 0))
        {
            setSpriteState("turnright",true);
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
        if ((int)collisionInfo.correction.y != 0)
        {
            acceleration[0].x = 0;
            acceleration[1].x = 0;
            velocity.x = 0;
        }
        else
        {
            acceleration[0].x = (float)NumberUtility::sign(velocity.x) / -4.0f;
            acceleration[1].x = 0;
        }
    }
    if ((input->isB() || input->isY()) && canJump && !isJumping)
    {
        acceleration[0].y = -4;
        acceleration[1].y = -9;
        //velocity.y = -10;
        canJump = false;
        isJumping = true;
        if (direction < 0)
            setSpriteState("jumpleft",true);
        else
            setSpriteState("jumpright",true);
        fallCounter.start();
    }
    if (!(input->isB() || input->isY()))
		canJump = true;
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
