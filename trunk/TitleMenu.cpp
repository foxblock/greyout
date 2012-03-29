#include "TitleMenu.h"

#include "userStates.h"
#include "Colour.h"
#include "NumberUtility.h"

#include "MyGame.h"
#include "GreySurfaceCache.h"
#include "effects/Hollywood.h"
#include "MusicCache.h"
#include "Music.h"

#define DEFAULT_SELECTION 0
#define MENU_ITEM_COUNT 4

#ifdef _MEOW
#define MENU_OFFSET_X 101
#define MENU_ITEM_HEIGHT 27
#define MENU_ITEM_SPACING 0
#define MARKER_SPEED 3
#else
#define MENU_OFFSET_X 173
#define MENU_ITEM_HEIGHT 61
#define MENU_ITEM_SPACING 0
#define MARKER_SPEED 5
#endif


TitleMenu::TitleMenu()
{
    selection = DEFAULT_SELECTION;
    invertRegion.w = GFX::getXResolution();
    invertRegion.h = MENU_ITEM_HEIGHT;
    invertRegion.x = 0;
    currentFps = 30;

    #ifdef _MEOW
    SDL_Surface* temp = SURFACE_CACHE->loadSurface("images/menu/title_320_240.png");
    #else
    SDL_Surface* temp = SURFACE_CACHE->loadSurface("images/menu/title_800_480.png");
    #endif
    bg.loadFrames(temp,temp->w / GFX::getXResolution(),temp->h / GFX::getYResolution(),0,0);
    bg.disableTransparentColour();
    bg.setPosition(0,0);
    bg.setFrameRate(THIRTY_FRAMES);
    #ifdef _MEOW
    marker.loadFrames(SURFACE_CACHE->loadSurface("images/menu/title_marker_320_240.png"),1,1,0,0);
    #else
    marker.loadFrames(SURFACE_CACHE->loadSurface("images/menu/title_marker_800_480.png"),1,1,0,0);
    #endif
    marker.setX(0);
    setSelection(true);

    // cache the inverted surfaces for faster drawing
    for (int I = 0; I < bg.frameCount(); ++I)
    {
        SDL_Surface* temp = SDL_CreateRGBSurface(SDL_SWSURFACE,bg.getWidth(),bg.getHeight(),GFX::getVideoSurface()->format->BitsPerPixel,0,0,0,0);
        bg.setCurrentFrame(I);
        bg.render(temp);
        SDL_Rect rect = {0,0,bg.getWidth(),bg.getHeight()};
        inverse(temp,rect);
        inverseBG.push_back(temp);
    }

    #ifdef PENJIN_CALC_FPS
    fpsDisplay.loadFont(GAME_FONT,24);
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
    EFFECTS->fadeIn(1000);
    ENGINE->timeTrial = false; // reset time trial mode

    MUSIC_CACHE->playMusic("music/title_menu.ogg");
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

    if (ACCEPT_KEY)
        doSelection();

    input->resetKeys();
}

void TitleMenu::update()
{
    setSelection(false);
    bg.update();
    EFFECTS->update();
}

void TitleMenu::render()
{
    GFX::clearScreen();

    bg.render();

    SDL_BlitSurface(inverseBG[bg.getCurrentFrame()],&invertRegion,GFX::getVideoSurface(),&invertRegion);

    marker.render();

    EFFECTS->render();

    #ifdef PENJIN_CALC_FPS
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
    MUSIC_CACHE->playSound("sounds/menu.wav");
}

void TitleMenu::decSelection()
{
    --selection;
    if (selection < 0)
        selection = MENU_ITEM_COUNT - 1;
    setSelection(false);
    MUSIC_CACHE->playSound("sounds/menu.wav");
}

void TitleMenu::doSelection()
{
    switch (selection)
    {
    case 0:
        if (ENGINE->activeChapter[0] != 0)
            ENGINE->playChapter(ENGINE->activeChapter);
        else
            ENGINE->playChapter(DEFAULT_CHAPTER);
        break;
    case 1:
        ENGINE->timeTrial = true;
        setNextState(STATE_LEVELSELECT);
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
