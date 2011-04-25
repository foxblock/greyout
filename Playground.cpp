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
        list<pair<string,string> > params;
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
        list<pair<string,string> > params;
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
    if (input->isB())
    {
        for (list<ControlUnit*>::iterator iter = players.begin(); iter != players.end(); ++iter)
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

    for (list<ControlUnit*>::iterator curr = players.begin(); curr != players.end(); ++curr)
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
#ifdef _DEBUG_COL
    src.h = min((int)GFX::getYResolution() / 2,getHeight() - src.y);
#else
    src.h = min((int)GFX::getYResolution(),getHeight() - src.y);
#endif

    for (vector<Rectangle*>::iterator curr = mouseRects.begin(); curr != mouseRects.end(); ++curr)
    {
        (*curr)->render(collisionLayer);
        (*curr)->render(levelImage);
        delete (*curr);
    }
    mouseRects.clear();

    SDL_BlitSurface(collisionLayer,&src,screen,&dst);

    /*for (list<BaseUnit*>::iterator curr = units.begin(); curr != units.end(); ++curr)
    {
        renderUnit(screen,(*curr),drawOffset);
    }*/

    dst.x = max(drawOffset.x,0.0f);
    dst.y = max(drawOffset.y,0.0f);
    src.x = max(-drawOffset.x,0.0f);
    src.y = max(-drawOffset.y,0.0f);
    src.w = min((int)GFX::getXResolution(),getWidth());
#ifdef _DEBUG_COL
    src.h = min((int)GFX::getYResolution() / 2,getHeight());
#else
    src.h = min((int)GFX::getYResolution(),getHeight());
#endif

    // draw collision surface to lower half of the screen in special debug mode
#ifdef _DEBUG_COL
    SDL_Rect dest;
    dest.x = 0;
    dest.y = 480;
    float factorX = (float)GFX::getXResolution() / (float)collisionLayer->w;
    float factorY = (float)GFX::getYResolution() / 2.0f / (float)collisionLayer->h;
    SDL_Surface* draw = zoomSurface(collisionLayer,factorX,factorY,SMOOTHING_OFF);
    for (list<ControlUnit*>::const_iterator iter = players.begin(); iter != players.end(); ++iter)
    {
        // draw rectangles for players
        Rectangle foo;
        foo.setDimensions((float)((*iter)->getWidth()) * factorX, (float)((*iter)->getHeight()) * factorY);
        Vector2df rectPos = (*iter)->getPixel(diTOPLEFT);
        rectPos.x *= factorX;
        rectPos.y *= factorY;
        foo.setPosition(rectPos);
        foo.setColour(GREEN);
        foo.render(draw);
    }
    if (getWidth() > GFX::getXResolution() || getHeight() > GFX::getYResolution())
    {
        // draw outlined rectangle for screen
        Rectangle scr;
        scr.setDimensions((float)GFX::getXResolution() * factorX,(float)GFX::getYResolution() / 2.0f * factorY);
        scr.setColour(GREEN);
        scr.setThickness(1);
        scr.setPosition(drawOffset.x * factorX, drawOffset.y * factorY);
        scr.render(draw);
    }
    SDL_BlitSurface(draw,NULL,GFX::getVideoSurface(),&dest);
    SDL_FreeSurface(draw);
#endif

    // draw to image used for collision testing before players get drawn
    //SDL_BlitSurface(screen,&src,collisionLayer,&dst);

    // players don't get drawn to the collision surface
    for (list<ControlUnit*>::iterator curr = players.begin(); curr != players.end(); ++curr)
    {
        renderUnit(screen,(*curr),drawOffset);
    }

    // particles
    for (list<PixelParticle*>::iterator curr = effects.begin(); curr != effects.end(); ++curr)
    {
        (*curr)->updateScreenPosition(drawOffset);
        (*curr)->render(screen);
        GFX::renderPixelBuffer();
    }
}

///---protected---

///---private---
