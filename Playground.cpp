#include "Playground.h"

#include "LevelLoader.h"
#include "userStates.h"
#include "BaseUnit.h"
#include "ControlUnit.h"
#include "PixelParticle.h"

Playground::Playground()
{
    //
}

Playground::~Playground()
{
    for (vector<Rectangle*>::iterator curr = mouseRects.begin(); curr != mouseRects.end(); ++curr)
    {
        delete (*curr);
    }
    mouseRects.clear();
}

///---public---

void Playground::userInput()
{
    input->update();

#ifdef PLATFORM_PC
    if (input->isQuit())
    {
        nullifyState();
        return;
    }
#endif
    if (input->isStart())
    {
        pauseToggle();
        return;
    }
    if (input->isY())
    {
        // spwan a black box
        list<PARAMETER_TYPE > params;
        Vector2df pos = drawOffset + input->getMouse() - Vector2df(16,16);
        params.push_back(make_pair("class","pushablebox"));
        params.push_back(make_pair("collision","0,255"));
        params.push_back(make_pair("size","32,32"));
        params.push_back(make_pair("position",StringUtility::vecToString(pos)));
        BaseUnit* box = LEVEL_LOADER->createUnit(params,this);
        units.push_back(box);

        input->resetY();
    }
    if (input->isX())
    {
        // spwan a white box
        list<PARAMETER_TYPE > params;
        Vector2df pos = drawOffset + input->getMouse() - Vector2df(16,16);
        params.push_back(make_pair("class","pushablebox"));
        params.push_back(make_pair("collision","white,255"));
        params.push_back(make_pair("size","32,32"));
        params.push_back(make_pair("colour","white"));
        params.push_back(make_pair("position",StringUtility::vecToString(pos)));
        BaseUnit* box = LEVEL_LOADER->createUnit(params,this);
        units.push_back(box);

        input->resetX();
    }
    if (input->isA())
    {
        for (vector<ControlUnit*>::iterator iter = players.begin(); iter != players.end(); ++iter)
        {
            (*iter)->explode();
        }
        lose();
        input->resetKeys();
    }


    if ((input->isL() || input->isR()) && not flags.hasFlag(lfDisableSwap))
    {
        if (players.size() > 1)
        {
            swapControl();
        }
        input->resetL();
        input->resetR();
    }

    if (input->isLeftClick())
    {
        Rectangle* temp = new Rectangle;
        temp->setColour(BLACK);
        temp->setDimensions(24,24);
        temp->setPosition(drawOffset + input->getMouse() - Vector2df(12,12));
        mouseRects.push_back(temp);
    }
    else if (input->isRightClick())
    {
        Rectangle* temp = new Rectangle;
        temp->setColour(WHITE);
        temp->setDimensions(24,24);
        temp->setPosition(input->getMouse() - Vector2df(12,12));
        mouseRects.push_back(temp);
    }

    for (vector<ControlUnit*>::iterator curr = players.begin(); curr != players.end(); ++curr)
    {
        if ((*curr)->takesControl)
            (*curr)->control(input);
    }
    input->resetA();
}

void Playground::render(SDL_Surface* screen)
{
    SDL_Rect src;
    SDL_Rect dst;
    dst.x = max(-drawOffset.x,0.0f);
    dst.y = max(-drawOffset.y,0.0f);
    src.x = max(drawOffset.x,0.0f);
    src.y = max(drawOffset.y,0.0f);
    src.w = min((int)GFX::getXResolution(),getWidth() - src.x);
    src.h = min((int)GFX::getYResolution(),getHeight() - src.y);

    for (vector<Rectangle*>::iterator curr = mouseRects.begin(); curr != mouseRects.end(); ++curr)
    {
        (*curr)->render(collisionLayer);
        (*curr)->render(levelImage);
        delete (*curr);
    }
    mouseRects.clear();

    SDL_BlitSurface(collisionLayer,&src,screen,&dst);

    dst.x = max(drawOffset.x,0.0f);
    dst.y = max(drawOffset.y,0.0f);
    src.x = max(-drawOffset.x,0.0f);
    src.y = max(-drawOffset.y,0.0f);
    src.w = min((int)GFX::getXResolution(),getWidth());
    src.h = min((int)GFX::getYResolution(),getHeight());

    // draw to image used for collision testing before players get drawn
    //SDL_BlitSurface(screen,&src,collisionLayer,&dst);

    // players don't get drawn to the collision surface
    for (vector<ControlUnit*>::iterator curr = players.begin(); curr != players.end(); ++curr)
    {
        renderUnit(screen,(*curr),drawOffset);
    }

    // particles
    for (vector<PixelParticle*>::iterator curr = effects.begin(); curr != effects.end(); ++curr)
    {
        (*curr)->updateScreenPosition(drawOffset);
        (*curr)->render(screen);
        GFX::renderPixelBuffer();
    }
}

///---protected---

///---private---
