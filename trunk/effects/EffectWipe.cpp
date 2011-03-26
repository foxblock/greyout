#include "EffectWipe.h"

EffectWipe::EffectWipe(CRint time, const SimpleDirection& startingDirection, const Colour& wipeColour, CRbool inverted) : BaseEffect()
{
    direction = startingDirection;
    isInverted = inverted;
    int temp = 0;

    if (isInverted)
    {
        rect.setDimensions(GFX::getXResolution(),GFX::getYResolution());
        rect.setPosition(0,0);
        if (direction.xDirection() != 0)
        {
            temp = direction.xDirection();
        }
        else
        {
            temp = direction.yDirection();
        }
    }
    else
    {
        if (direction.xDirection() != 0)
        {
            temp = direction.xDirection();
            rect.setDimensions(0,GFX::getYResolution());
            // position left or right
            rect.setPosition((int)GFX::getXResolution() / 2 + (int)GFX::getXResolution() / 2 * temp,0);
        }
        else
        {
            temp = direction.yDirection();
            rect.setDimensions(GFX::getXResolution(),0);
            // position top or bottom
            rect.setPosition(0,(int)GFX::getYResolution() / 2 + (int)GFX::getYResolution() / 2 * temp);
        }
    }

    rect.setColour(wipeColour);
    timer.setRewind(STOP);
    timer.start(time);

    type = etWipe;
    limit = 1;
}

EffectWipe::~EffectWipe()
{

}

void EffectWipe::update()
{
    if (not timer.hasFinished())
    {
        if (isInverted)
        {
            if (direction.xDirection() != 0) // wipe left/right
            {
                int width = GFX::getXResolution() * ((float)timer.getTimeLeft() / (float)timer.getLimit());
                rect.setWidth(width);
                if (direction.xDirection() < 0) // wipe from left to right (but inverted, so the box has to move across the screen)
                {
                    rect.setPosition((int)GFX::getXResolution() - width,0);
                }
            }
            else // wipe top/bottom
            {
                int height = GFX::getYResolution() * ((float)timer.getTimeLeft() / (float)timer.getLimit());
                rect.setHeight(height);
                if (direction.yDirection() < 0) // wipe from top to bottom (but inverted, so the box has to move across the screen)
                {
                    rect.setPosition(0,(int)GFX::getYResolution() - height);
                }
            }
        }
        else
        {
            if (direction.xDirection() != 0) // wipe left/right
            {
                int width = GFX::getXResolution() * ((float)timer.getScaledTicks() / (float)timer.getLimit());
                rect.setWidth(width);
                if (direction.xDirection() > 0) // wipe from right to left
                {
                    // rectangle has to be positioned relative to right border of the screen
                    rect.setPosition((int)GFX::getXResolution() - width,0);
                }
            }
            else // wipe top/bottom
            {
                int height = GFX::getYResolution() * ((float)timer.getScaledTicks() / (float)timer.getLimit());
                rect.setHeight(height);
                if (direction.yDirection() > 0) // wipe from bottom to top
                {
                    rect.setPosition(0,(int)GFX::getYResolution() - height);
                }
            }
        }
    }
    else
        finished = true;
}

void EffectWipe::render()
{
    rect.render();
}
