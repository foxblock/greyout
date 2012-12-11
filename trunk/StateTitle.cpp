#include "StateTitle.h"

#include "userStates.h"
#include "Colour.h"
#include "Vector2di.h"
#include "SurfaceCache.h"
#include "gameDefines.h"

StateTitle::StateTitle()
{
    nullify = false;

    #ifdef _MEOW
    pandora.loadSprite(SURFACE_CACHE->loadSurface("images/general/gp2x_logo.png"));
    pandora.setPosition(30,70);
    #elif defined(PLATFORM_PANDORA)
    pandora.loadSprite(SURFACE_CACHE->loadSurface("images/general/Pandora_logo-unofficial.png"));
    pandora.setPosition(100,115);
    #else
    pandora.loadSprite(SURFACE_CACHE->loadSurface("images/general/Pandora_logo-unofficial.png"));
    pandora.setPosition(100,115);
    #endif
    pandora.setTransparentColour(MAGENTA);
    pandora.setAlpha(0);

    penjin.loadSprite(SURFACE_CACHE->loadSurface("images/general/penjinsigok5.png"));
    #ifdef _MEOW
    penjin.setPosition(54,80);
    #else
    penjin.setPosition(294,200);
    #endif
    penjin.disableTransparentColour();
    penjin.setAlpha(0);

    #ifdef _MEOW
    text.loadFont(GAME_FONT,24);
    #else
    text.loadFont(GAME_FONT,48);
    #endif
    text.setColour(BLACK);
    text.setBoundaries(Vector2di(0,0),Vector2di(GFX::getXResolution(),GFX::getYResolution()));
    text.setAlignment(CENTRED);
    text.setRelativity(true);

    vector<string> lines;
    #ifdef _MEOW
    lines.push_back("GP2x - oldschool gaming");
    #else
    lines.push_back("www.openPandora.org");
    #endif
    /*#ifdef _MEOW
    lines.push_back("Oldschool yo!");
    lines.push_back("Alles andere ist Spielzeug");
    #elif !defined(PLATFORM_PANDORA)
    lines.push_back("Hail to the evil dragon");
    lines.push_back("It's open and girls love it");
    lines.push_back("Not an iPhone!");
    lines.push_back("What's your order number?");
    lines.push_back("The wait is over aparently");
    lines.push_back("Like god, but it exists - some day");
    #else
    lines.push_back("www.openPandora.org");
    lines.push_back("It's open and girls love it");
    lines.push_back("Not an iPhone!");
    #endif
    lines.push_back("Free is always better");
    lines.push_back("Free as in beer");
    lines.push_back("Free as in speech");*/
    tagline = lines[rand() % lines.size()];
    counter.setMode(CUSTOM);
    counter.setScaler(1000/255);
    counter.start();

    state = 0;
    once = true;
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
    if(input->isAny() || input->isLeftClick())
    {
		setNextState(STATE_MAIN);
		input->resetKeys();
    }
}

void StateTitle::render()
{
    GFX::clearScreen();

    if (state == 1)
    {
        pandora.render();
        #ifdef _MEOW
        text.setPosition(0,140);
        #elif defined(PLATFORM_PANDORA)
        text.setPosition(0,260);
        #else
        text.setPosition(0,260);
        #endif
        text.print(tagline);
    }
    else if (state == 2)
    {
        penjin.render();
        #ifdef _MEOW
        text.setPosition(0,150);
        #else
        text.setPosition(0,270);
        #endif
        text.print("Penjin powered");
    }
    else if (state == 3)
    {
        #ifdef _MEOW
        text.setPosition(0,30);
        #else
        text.setPosition(0,130);
        #endif
        text.print("A game by");
        text.print("\n\n");
        text.print("Janek Schaefer");
        text.print("\n");
        text.print("foxblock@gmail.com");
    }
    else if (state == 4)
    {
        #ifdef _MEOW
        text.setPosition(0,30);
        #else
        text.setPosition(0,130);
        #endif
        text.print("Music by");
        text.print("\n\n");
        text.print("Nick May");
        text.print("\n");
        text.print("www.nickmaymusic.co.uk");
    }
    else if (state == 5)
    {
        if (once)
        {
            #ifdef _MEOW
            text.setFontSize(12);
            text.setBoundaries(Vector2di(0,0),Vector2di(GFX::getXResolution()-14,GFX::getYResolution()));
            #else
            text.setFontSize(24);
            #endif
            once = false;
        }
        #ifdef _MEOW
        text.setPosition(0,20);
        #else
        text.setPosition(0,50);
        #endif
        text.print("Heavily inspired by");
        text.print("\n\n");
        text.print("Three Hundred Mechanics (#1,2,114)");
        text.print("\n");
        text.print("by Sean Howard");
        text.print("\n");
        text.print("http://www.squidi.net/three/index.php");
        text.print("\n\n");
        text.print("Shift");
        text.print("\n");
        text.print("by Antony Lavelle");
        text.print("\n");
        text.print("http://armorgames.com/play/751/shift");
        text.print("\n\n");
        text.print("Super Mario, Braid, VVVVVV, Portal, NIDHOGG");
    }
    else if (state == 6)
    {
        #ifdef _MEOW
        text.setPosition(0,10);
        #else
        text.setPosition(0,20);
        #endif
        text.print("Sounds:");
        text.print("\n\n");
        text.print("http://www.freesound.org/ - ID:114789");
        text.print("\n");
        text.print("http://www.freesound.org/ - ID:25879");
        text.print("\n\n\n");
        text.print("Thanks:");
        text.print("\n\n");
        text.print("B-Zar, CME, cosurgi, EvilDragon");
        text.print("\n");
        text.print("Esn, Ivanovic, milkshake, Ziz");
    }
}

void StateTitle::update()
{
    int ticks = counter.getScaledTicks();

    if (ticks > 100)
    {
        if (ticks <= (100 + 255)) // Pandora logo
        {
            int value = ticks - 100;
            pandora.setAlpha(value);
            text.setColour(Colour(value,value,value));
            state = 1;
        }
        else if (ticks > 600 && ticks <= (600 + 255)) // fade out
        {
            int value = max(855 - ticks,0);
            pandora.setAlpha(value);
            text.setColour(Colour(value,value,value));
        }
        else if (ticks > 1000 && ticks < (1000 + 255)) // Penjin logo
        {
            int value = ticks - 1000;
            penjin.setAlpha(value);
            text.setColour(Colour(value,value,value));
            state = 2;
        }
        else if (ticks > 1500 && ticks <= (1500 + 255)) // fade out
        {
            int value = 1500 + 255 - ticks;
            penjin.setAlpha(value);
            text.setColour(Colour(value,value,value));
        }
        else if (ticks > 1900 && ticks <= (1900 + 255)) // A game by
        {
            int value = ticks - 1900;
            text.setColour(Colour(value,value,value));
            state = 3;
        }
        else if (ticks > 2400 && ticks <= (2400 + 255))
        {
            int value = 2400 + 255 - ticks;
            text.setColour(Colour(value,value,value));
        }
        else if (ticks > 2800 && ticks <= (2800 + 255)) // Music
        {
            int value = ticks - 2800;
            text.setColour(Colour(value,value,value));
            state = 4;
        }
        else if (ticks > 3300 && ticks <= (3300 + 255))
        {
            int value = 3300 + 255 - ticks;
            text.setColour(Colour(value,value,value));
        }
        else if (ticks > 3600 && ticks <= (3600 + 255)) // Inspiration
        {
            int value = ticks - 3600;
            text.setColour(Colour(value,value,value));
            state = 5;
        }
        else if (ticks > 5000 && ticks <= (5000 + 255))
        {
            int value = 5000 + 255 - ticks;
            text.setColour(Colour(value,value,value));
        }
        else if (ticks > 5300 && ticks <= (5300 + 255)) // Sounds
        {
            int value = ticks - 5300;
            text.setColour(Colour(value,value,value));
            state = 6;
        }
        else if (ticks > 6300 && ticks <= (6300 + 255))
        {
            int value = 6300 + 255 - ticks;
            text.setColour(Colour(value,value,value));
        }
        else if (ticks > 6500)
            setNextState(STATE_MAIN);
    }
}
