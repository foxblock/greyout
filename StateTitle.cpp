#include "StateTitle.h"

#include "userStates.h"
#include "Colour.h"
#include "Vector2di.h"

StateTitle::StateTitle()
{
    nullify = false;

    pandora.loadSprite("images/general/Pandora_logo-unofficial.png");
    pandora.setPosition(100,115);
    pandora.setTransparentColour(MAGENTA);
    pandora.setAlpha(0);

    penjin.loadSprite("images/general/penjinsigok5.png");
    penjin.setPosition(294,200);
    penjin.disableTransparentColour();
    penjin.setAlpha(0);

    text.loadFont("fonts/Lato-Bold.ttf",48);
    text.setColour(BLACK);
    text.setBoundaries(Vector2di(0,0),Vector2di(GFX::getXResolution(),GFX::getYResolution()));
    text.setAlignment(CENTRED);

    counter.setMode(CUSTOM);
    counter.setScaler(1000/255);
    counter.start();
}


StateTitle::~StateTitle()
{
    //
}

///---public---

void StateTitle::userInput()
{
    input->update();

    #ifdef PLATFORM_PC
        if(input->isQuit()) {
            nullifyState();
            return;
        }
    #endif
    if(input->isAny())
    {
      setNextState(STATE_MAIN);
      input->resetKeys();
    }
}

void StateTitle::render()
{
    GFX::clearScreen();

    if (pandora.getAlpha() > 0)
    {
        pandora.render();
        text.setPosition(0,260);
        text.print("www.openPandora.org");
    }
    else if (penjin.getAlpha() > 0)
    {
        penjin.render();
        text.setPosition(0,270);
        text.print("Penjin powered");
    }
}

void StateTitle::update()
{
    int ticks = counter.getScaledTicks();

    if (ticks > 100)
    {
        if (ticks <= (100 + 255))
        {
            int value = ticks - 100;
            pandora.setAlpha(value);
            text.setColour(Colour(value,value,value));
        }
        else if (ticks > (855 - 255) && ticks <= 900)
        {
            int value = max(855 - ticks,0);
            pandora.setAlpha(value);
            text.setColour(Colour(value,value,value));
        }
        else if (ticks > 1000 && ticks < (1000 + 255))
        {
            int value = ticks - 1000;
            penjin.setAlpha(value);
            text.setColour(Colour(value,value,value));
        }
        else if (ticks > (1500) && ticks <= (1500 + 255))
        {
            int value = 1500 + 255 - ticks;
            penjin.setAlpha(value);
            text.setColour(Colour(value,value,value));
        }
        else if (ticks > 1800)
            setNextState(STATE_MAIN);
    }
}
