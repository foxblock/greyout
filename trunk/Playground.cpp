#include "Playground.h"

#include "LevelLoader.h"
#include "userStates.h"
#include "BaseUnit.h"
#include "ControlUnit.h"
#include "PixelParticle.h"

Playground::Playground()
{
    rectangleLayer = NULL;
}

Playground::~Playground()
{
    for (vector<Rectangle*>::iterator curr = mouseRects.begin(); curr != mouseRects.end(); ++curr)
    {
        delete (*curr);
    }
    mouseRects.clear();
    if (rectangleLayer)
        SDL_FreeSurface(rectangleLayer);
}

///---public---

bool Playground::load(const PARAMETER_TYPE& params)
{
    if (Level::load(params))
    {
        // The rectangles will be painted to this surface
        rectangleLayer = SDL_CreateRGBSurface(SDL_SWSURFACE,getWidth(),getHeight(),GFX::getVideoSurface()->format->BitsPerPixel,0,0,0,0);

        // Fill it with magenta and apply a colour key to make it transparent
        SDL_FillRect(rectangleLayer, NULL, SDL_MapRGB(rectangleLayer->format,255,0,255));
        SDL_SetColorKey(rectangleLayer, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(rectangleLayer->format,255,0,255));
        return true;
    }
    else
        return false;
}

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
        setNextState(STATE_MAIN);
        return;
    }
    if (input->isY())
    {
        // spwan a black box
        list<pair<string,string> > params;
        params.push_back(make_pair("class","pushablebox"));
        params.push_back(make_pair("collision","0,255"));
        params.push_back(make_pair("size","32,32"));
        BaseUnit* box = LEVEL_LOADER->createUnit(params,this);
        box->position = drawOffset + input->getMouse() - Vector2df(16,16);
        units.push_back(box);

        input->resetY();
    }
    if (input->isX())
    {
        // spwan a white box
        list<pair<string,string> > params;
        params.push_back(make_pair("class","pushablebox"));
        params.push_back(make_pair("collision","white,255"));
        params.push_back(make_pair("size","32,32"));
        params.push_back(make_pair("colour","white"));
        BaseUnit* box = LEVEL_LOADER->createUnit(params,this);
        box->position = drawOffset + input->getMouse() - Vector2df(16,16);
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
            for (list<ControlUnit*>::iterator unit = players.begin(); unit != players.end(); ++unit)
                (*unit)->takesControl = not (*unit)->takesControl;
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
}

void Playground::render(SDL_Surface* screen)
{
    levelImage->renderImage(screen,-drawOffset);

    if (rectangleLayer)
    {
        for (vector<Rectangle*>::iterator curr = mouseRects.begin(); curr != mouseRects.end(); ++curr)
        {
            (*curr)->render(rectangleLayer);
            delete (*curr);
        }
        mouseRects.clear();
        SDL_BlitSurface(rectangleLayer,NULL,screen,NULL);
    }

    for (list<BaseUnit*>::iterator curr = units.begin(); curr != units.end(); ++curr)
    {
        renderUnit(screen,(*curr),drawOffset);
    }


    // draw to image used for collision testing before players get drawn
    SDL_Rect temp;
    temp.x = drawOffset.x;
    temp.y = drawOffset.y;
#ifdef _DEBUG_COL
    SDL_Rect temp2;
    temp2.x = 0;
    temp2.y = 0;
    temp2.w = GFX::getXResolution();
    temp2.h = GFX::getYResolution() / 2.0f;
    SDL_BlitSurface(screen,&temp2,collisionLayer,&temp);
#else
    SDL_BlitSurface(screen,NULL,collisionLayer,&temp);
#endif

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
