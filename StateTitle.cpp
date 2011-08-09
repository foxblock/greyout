#include "StateTitle.h"

#include "userStates.h"
#include "Colour.h"
#include "Vector2di.h"

StateTitle::StateTitle()
{
    nullify = false;

    #ifdef _MEOW
    pandora.loadSprite("images/general/gp2x_logo.png");
    pandora.setPosition(30,70);
    #elif defined(PLATFORM_PANDORA)
    pandora.loadSprite("images/general/Pandora_logo-unofficial.png");
    pandora.setPosition(100,115);
    #else
    pandora.loadSprite("images/general/Pandora_logo-unofficial.png");
    pandora.setPosition(100,115);
    #endif
    pandora.setTransparentColour(MAGENTA);
    pandora.setAlpha(0);

    penjin.loadSprite("images/general/penjinsigok5.png");
    #ifdef _MEOW
    penjin.setPosition(54,80);
    #else
    penjin.setPosition(294,200);
    #endif
    penjin.disableTransparentColour();
    penjin.setAlpha(0);

    #ifdef _MEOW
    text.loadFont("fonts/Lato-Bold.ttf",24);
    #else
    text.loadFont("fonts/Lato-Bold.ttf",48);
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
    if(input->isAny())
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
        text.setPosition(0,40);
        #else
        text.setPosition(0,150);
        #endif
        text.print("A game by");
        text.print("\n\n");
        text.print("Janek Schaefer");
        text.print("\n");
        text.print("foxblock@gmail.com");
    }
    else if (state == 4)
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
        text.print("by Tony");
        text.print("\n");
        text.print("http://armorgames.com/play/751/shift");
        text.print("\n\n");
        text.print("Super Mario, Braid, VVVVVV, Portal, NIDHOGG");
    }

    else if (state == 5)
    {
        #ifdef _MEOW
        text.setPosition(0,10);
        #else
        text.setPosition(0,20);
        #endif
        text.print("Music:");
        text.print("\n\n");
        text.print("Return to Lavender Town [Menu]");
        text.print("\n");
        text.print("by Alex Baker (picklesandwichh)");
        text.print("\n\n");
        text.print("Quietly [Alone]");
        text.print("\n");
        text.print("by Thomas L (Computer112)");
        text.print("\n\n");
        text.print("Amber Waves of Grain [Separate]");
        text.print("\n");
        text.print("by Lenard Kritzer (Pocketpod)");
        text.print("\n\n");
        text.print("The Color's Gray [Together]");
        text.print("\n");
        text.print("by Ailad");
        text.print("\n\n");
        text.print("All from the Newgrounds.com Audio portal");
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
        text.print("CME, EvilDragon, Ivanovic, milkshake, Ziz");
    }
}

void StateTitle::update()
{
    int ticks = counter.getScaledTicks();

    if (ticks > 100)
    {
        if (ticks <= (100 + 255)) // fade in (pandora)
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
        else if (ticks > 1000 && ticks < (1000 + 255)) // fade in (penjin)
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
        else if (ticks > 1900 && ticks <= (1900 + 255))
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
        else if (ticks > 2800 && ticks <= (2800 + 255))
        {
            int value = ticks - 2800;
            text.setColour(Colour(value,value,value));
            state = 4;
        }
        else if (ticks > 3800 && ticks <= (3800 + 255))
        {
            int value = 3800 + 255 - ticks;
            text.setColour(Colour(value,value,value));
        }
        else if (ticks > 4100 && ticks <= (4100 + 255))
        {
            int value = ticks - 4100;
            text.setColour(Colour(value,value,value));
            state = 5;
        }
        else if (ticks > 5500 && ticks <= (5500 + 255))
        {
            int value = 5500 + 255 - ticks;
            text.setColour(Colour(value,value,value));
        }
        else if (ticks > 5800 && ticks <= (5800 + 255))
        {
            int value = ticks - 5800;
            text.setColour(Colour(value,value,value));
            state = 6;
        }
        else if (ticks > 6800 && ticks <= (6800 + 255))
        {
            int value = 6800 + 255 - ticks;
            text.setColour(Colour(value,value,value));
        }
        else if (ticks > 7100)
            setNextState(STATE_MAIN);
    }
}
