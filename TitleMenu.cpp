#include "TitleMenu.h"

#include "userStates.h"
#include "Colour.h"
#include "NumberUtility.h"

#include "MyGame.h"
#include "SurfaceCache.h"

#define DEFAULT_SELECTION 0
#define MENU_ITEM_COUNT 4
#define MENU_OFFSET_X 173
#define MENU_ITEM_HEIGHT 61
#define MENU_ITEM_SPACING 0

#define MARKER_SPEED 10


TitleMenu::TitleMenu()
{
    selection = DEFAULT_SELECTION;
    invertRegion.w = GFX::getXResolution();
    invertRegion.h = MENU_ITEM_HEIGHT;
    invertRegion.x = 0;
    currentFps = 30;

    bool fromCache;
    bg.loadFrames(SURFACE_CACHE->getSurface("images/menu/title_800_480.png",fromCache),1,1,0,0);
    bg.disableTransparentColour();
    bg.setPosition(0,0);
    marker.loadFrames(SURFACE_CACHE->getSurface("images/menu/title_marker_800_480.png",fromCache),1,1,0,0);
    marker.setX(0);
    setSelection(true);

    // cache the inverted surfaces for faster drawing
    for (int I = bg.frameCount()-1; I >= 0; --I)
    {
        SDL_Surface* temp = SDL_CreateRGBSurface(SDL_SWSURFACE,bg.getWidth(),bg.getHeight(),GFX::getVideoSurface()->format->BitsPerPixel,0,0,0,0);
        bg.setCurrentFrame(I);
        bg.render(temp);
        SDL_Rect rect = {0,0,bg.getWidth(),bg.getHeight()};
        inverse(temp,rect);
        inverseBG.push_back(temp);
    }

    #ifdef _DEBUG
    fpsDisplay.loadFont("fonts/Lato-Bold.ttf",24);
    fpsDisplay.setColour(GREEN);
    fpsDisplay.setPosition(GFX::getXResolution(),0);
    fpsDisplay.setAlignment(RIGHT_JUSTIFIED);
    #endif
}

TitleMenu::~TitleMenu()
{
    for (vector<SDL_Surface*>::iterator surf = inverseBG.begin(); surf < inverseBG.end(); ++surf)
        SDL_FreeSurface(*surf);
}

void TitleMenu::init()
{
    input->resetKeys(); // avoid sticky keys when returning from level
}

void TitleMenu::userInput()
{
    input->update();

#ifdef PLATFORM_PC
    if (input->isQuit())
    {
        nullifyState();
        return;
    }
#endif

    if (input->isUp())
        decSelection();
    else if (input->isDown())
        incSelection();

    if (input->isA() || input->isStart())
        doSelection();

    input->resetKeys();
}

void TitleMenu::update()
{
    setSelection(false);
}

void TitleMenu::render()
{
    GFX::clearScreen();

    bg.render();

    SDL_BlitSurface(inverseBG.at(bg.getCurrentFrame()),&invertRegion,GFX::getVideoSurface(),&invertRegion);

    marker.render();

    #ifdef _DEBUG
    fpsDisplay.print(StringUtility::intToString(MyGame::getMyGame()->getFPS()));
    #endif
}

void TitleMenu::setSelection(CRbool immediate)
{
    int destination = MENU_OFFSET_X + (MENU_ITEM_HEIGHT + MENU_ITEM_SPACING) * selection;

    if (immediate)
    {
        invertRegion.y = destination;
    }
    else
    {
        int diff = destination - invertRegion.y;
        invertRegion.y = invertRegion.y + NumberUtility::closestToZero(MARKER_SPEED * NumberUtility::sign(diff),diff);
    }
    marker.setY(invertRegion.y - (marker.getHeight() - invertRegion.h) / 2);
}

void TitleMenu::incSelection()
{
    ++selection;
    if (selection >= MENU_ITEM_COUNT)
        selection = 0;
    setSelection(false);
}

void TitleMenu::decSelection()
{
    --selection;
    if (selection < 0)
        selection = MENU_ITEM_COUNT - 1;
    setSelection(false);
}

void TitleMenu::doSelection()
{
    switch (selection)
    {
    case 0:
        ENGINE->playSingleLevel("levels/test.txt",STATE_MAIN);
        break;
    case 1:
        setNextState(STATE_BENCHMARK);
        break;
    case 2:
        setNextState(STATE_LEVELSELECT);
        break;
    case 3:
        nullifyState();
        break;
    default:
        setNextState(STATE_LEVEL);
    }
}

void TitleMenu::inverse(SDL_Surface* const surf, const SDL_Rect& rect)
{
    Colour pixelCol;
    for (int X=rect.x; X < rect.x + rect.w; ++X)
    {
        for (int Y=rect.y; Y < rect.y + rect.h; ++Y)
        {
            pixelCol = GFX::getPixel(surf,X,Y);
            GFX::setPixel(surf,X,Y,Colour(WHITE) - pixelCol);
        }
    }
}
