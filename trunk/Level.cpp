#include "Level.h"

#include "StringUtility.h"

#include "BaseUnit.h"
#include "ControlUnit.h"
#include "PixelParticle.h"
#include "Physics.h"
#include "MyGame.h"
#include "userStates.h"
#include "SurfaceCache.h"
#include "effects/Hollywood.h"
#include "MusicCache.h"
#include "Dialogue.h"

#define NAME_TEXT_SIZE 48
#define NAME_RECT_HEIGHT 35

#define PAUSE_MENU_ITEM_COUNT 5
#define PAUSE_MENU_SPACING 10
#define PAUSE_MENU_OFFSET_Y 0
#define PAUSE_MENU_OFFSET_X 20
#define PAUSE_VOLUME_SLIDER_SIZE 400

Level::Level()
{
    noCollision = MAGENTA;
    levelImage = NULL;
    collisionLayer = NULL;
    levelFileName = "";
    chapterPath = "";
    errorString = "";
    drawOffset = Vector2df(0,0);

    eventTimer.init(1000,MILLI_SECONDS);
    eventTimer.setRewind(STOP);
    winCounter = 0;
    firstLoad = true;

    stringToFlag["repeatx"] = lfRepeatX;
    stringToFlag["repeaty"] = lfRepeatY;
    stringToFlag["scrollx"] = lfScrollX;
    stringToFlag["scrolly"] = lfScrollY;
    stringToFlag["disableswap"] = lfDisableSwap;
    stringToFlag["keepcentred"] = lfKeepCentred;
    stringToFlag["scalex"] = lfScaleX;
    stringToFlag["scaley"] = lfScaleY;

    stringToProp["image"] = lpImage;
    stringToProp["flags"] = lpFlags;
    stringToProp["filename"] = lpFilename;
    stringToProp["offset"] = lpOffset;
    stringToProp["background"] = lpBackground;
    stringToProp["boundaries"] = lpBoundaries;
    stringToProp["name"] = lpName;
    stringToProp["music"] = lpMusic;
    stringToProp["dialogue"] = lpDialogue;

#ifdef _DEBUG
    debugText.loadFont("fonts/unispace.ttf",12);
    debugText.setColour(GREEN);
    debugString = "";
    fpsDisplay.loadFont("fonts/unispace.ttf",24);
    fpsDisplay.setColour(GREEN);
    fpsDisplay.setPosition(GFX::getXResolution(),0);
    fpsDisplay.setAlignment(RIGHT_JUSTIFIED);
#endif
    nameText.loadFont("fonts/Lato-Bold.ttf",48);
    nameText.setColour(WHITE);
    nameText.setAlignment(CENTRED);
    nameText.setUpBoundary(Vector2di(GFX::getXResolution(),GFX::getYResolution()));
    nameText.setPosition(0.0f,(GFX::getYResolution() - NAME_RECT_HEIGHT) / 2.0f + NAME_RECT_HEIGHT - NAME_TEXT_SIZE);
    nameRect.setDimensions(GFX::getXResolution(),NAME_RECT_HEIGHT);
    nameRect.setPosition(0.0f,(GFX::getYResolution() - NAME_RECT_HEIGHT) / 2.0f);
    nameRect.setColour(BLACK);
    nameTimer.init(2000,MILLI_SECONDS);

    cam.parent = this;

    pauseSurf = SDL_CreateRGBSurface(SDL_SWSURFACE,GFX::getXResolution(),GFX::getYResolution(),GFX::getVideoSurface()->format->BitsPerPixel,0,0,0,0);
    pauseSelection = 0;
    overlay.setDimensions(GFX::getXResolution(),GFX::getYResolution());
    overlay.setPosition(0,0);
    overlay.setColour(BLACK);
    overlay.setAlpha(100);

    hidex = NULL;
    hidey = NULL;
}

Level::~Level()
{
    SDL_FreeSurface(collisionLayer);
    for (list<ControlUnit*>::iterator curr = players.begin(); curr != players.end(); ++curr)
    {
        delete (*curr);
    }
    players.clear();
    for (list<BaseUnit*>::iterator curr = units.begin(); curr != units.end(); ++curr)
    {
        delete (*curr);
    }
    units.clear();
    for (list<PixelParticle*>::iterator curr = effects.begin(); curr != effects.end(); ++curr)
    {
        delete (*curr);
    }
    effects.clear();
    delete hidex;
    delete hidey;

    // reset background colour
    GFX::setClearColour(BLACK);
}

/// ---public---

bool Level::load(const PARAMETER_TYPE& params)
{
    for (PARAMETER_TYPE::const_iterator value = params.begin(); value != params.end(); ++value)
    {
        if (not processParameter(make_pair(value->first,value->second)) && value->first != "class")
        {
            string className = params.front().second;
            cout << "Warning: Unprocessed parameter \"" << value->first << "\" on level \"" << className << "\"" << endl;
        }
    }

    if (not levelImage)
    {
        errorString = "Error: No image has been specified! (critical)";
        return false;
    }
    else
    {
        collisionLayer = SDL_CreateRGBSurface(SDL_SWSURFACE,levelImage->w,levelImage->h,GFX::getVideoSurface()->format->BitsPerPixel,0,0,0,0);
        SDL_BlitSurface(levelImage,NULL,collisionLayer,NULL);
    }
    if (getWidth() < GFX::getXResolution())
    {
        hidex = new Rectangle;
        #ifdef _DEBUG_COL
        hidex->setDimensions((GFX::getXResolution() - getWidth()) / 2.0f,GFX::getYResolution() / 2.0f);
        #else
        hidex->setDimensions((GFX::getXResolution() - getWidth()) / 2.0f,GFX::getYResolution());
        #endif
        hidex->setColour(GFX::getClearColour());
    }
    #ifdef _DEBUG_COL
    if (getHeight() < GFX::getYResolution() / 2.0f)
    #else
    if (getHeight() < GFX::getYResolution())
    #endif
    {
        hidey = new Rectangle;
        #ifdef _DEBUG_COL
        hidey->setDimensions(GFX::getXResolution() - max((int)GFX::getXResolution() - getWidth(),0),(GFX::getYResolution() / 2.0f - getHeight()) / 2.0f);
        #else
        hidey->setDimensions(GFX::getXResolution() - max((int)GFX::getXResolution() - getWidth(),0),(GFX::getYResolution() - getHeight()) / 2.0f);
        #endif
        hidey->setColour(GFX::getClearColour());
    }

    return true;
}

void Level::init()
{
    if (players.size() == 0)
        winCounter = 1; // never win
    else
        winCounter = players.size();

    if (firstLoad)
    {
        nameTimer.start(2000);
        if (ENGINE->currentState != STATE_LEVELSELECT)
            EFFECTS->fadeIn(1000);
    }
}

void Level::userInput()
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
    if (input->isB() && input->isY())
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

    for (list<ControlUnit*>::iterator curr = players.begin(); curr != players.end(); ++curr)
    {
        if ((*curr)->takesControl)
            (*curr)->control(input);
    }
    input->resetA();
    input->resetX();
}

void Level::update()
{
#ifdef _DEBUG
    debugString = "";
#endif
    // Check for units to be removed
    for (list<ControlUnit*>::iterator player = players.begin(); player != players.end();)
    {
        if ((*player)->toBeRemoved)
        {
            delete (*player);
            player = players.erase(player);
        }
        else
        {
            ++player;
        }
    }
    for (list<BaseUnit*>::iterator unit = units.begin();  unit != units.end();)
    {
        if ((*unit)->toBeRemoved)
        {
            delete (*unit);
            unit = units.erase(unit);
        }
        else
        {
            ++unit;
        }
    }
    for (list<PixelParticle*>::iterator part = effects.begin();  part != effects.end();)
    {
        if ((*part)->toBeRemoved)
        {
            delete (*part);
            part = effects.erase(part);
        }
        else
        {
            ++part;
        }
    }

    // particle-map collision
    // and update (velocity, gravity, etc.)
    for (list<PixelParticle*>::iterator curr = effects.begin(); curr != effects.end(); ++curr)
    {
        PHYSICS->applyPhysics((*curr));
        PHYSICS->particleMapCollision(this,collisionLayer,(*curr));
        (*curr)->update();
    }

    // physics (acceleration, friction, etc)
    // and unit collision
    for (list<BaseUnit*>::iterator unit = units.begin();  unit != units.end(); ++unit)
    {
        adjustPosition((*unit));
        PHYSICS->applyPhysics((*unit));
    }
    for (list<ControlUnit*>::iterator player = players.begin(); player != players.end(); ++player)
    {
        adjustPosition((*player));
        PHYSICS->applyPhysics((*player));
        for (list<BaseUnit*>::iterator unit = units.begin();  unit != units.end(); ++unit)
        {
            if (not (*unit)->flags.hasFlag(BaseUnit::ufNoUnitCollision))
                PHYSICS->playerUnitCollision(this,(*player),(*unit));
        }
    }

    // map collision
    for (list<BaseUnit*>::iterator curr = units.begin(); curr != units.end(); ++curr)
    {
        if (not (*curr)->flags.hasFlag(BaseUnit::ufNoCollisionUpdate))
        {
            clearUnitFromCollision(collisionLayer,(*curr));
            if (not (*curr)->flags.hasFlag(BaseUnit::ufNoMapCollision))
            {
                // check for overwritten units by last clearUnitFromCollision call and redraw them
                for (list<BaseUnit*>::iterator other = units.begin(); other != units.end(); ++other)
                {
                    if ((*curr) != (*other) && PHYSICS->checkUnitCollision(this,(*curr),(*other)))
                        renderUnit(collisionLayer,(*other),Vector2df(0,0));
                }
                PHYSICS->unitMapCollision(this,collisionLayer,(*curr));
            }
            // else still update unit on collision surface for player-map collision
            (*curr)->update();
            renderUnit(collisionLayer,(*curr),Vector2df(0,0));
        }
        else
        {
            (*curr)->update();
        }
    }

    // player-map collision
    for (list<ControlUnit*>::iterator curr = players.begin(); curr != players.end(); ++curr)
    {
        // players should always have map collision enabled, so don't check for that here
        PHYSICS->unitMapCollision(this,collisionLayer,(*curr));
        (*curr)->update();
        // players are never drawn to the collision surface to not interfere with
        // unit-map collision testing
    }

    // other update stuff
    if (flags.hasFlag(lfKeepCentred))
        cam.centerOnUnit(getFirstActivePlayer());
    eventTimer.update();
    DIALOGUE->update();

    if (winCounter <= 0)
    {
        win();
    }

    EFFECTS->update();
}

void Level::render()
{
    GFX::clearScreen();

    render(GFX::getVideoSurface());

    // if level is smaller hide outside area
    if (hidex)
    {
        hidex->setPosition(0,0);
        hidex->render();
        hidex->setPosition(getWidth() - drawOffset.x,0.0f);
        hidex->render();
    }
    if (hidey)
    {
        hidey->setPosition(max(-drawOffset.x,0.0f),0.0f);
        hidey->render();
        hidey->setPosition(max(-drawOffset.x,0.0f),getHeight() - drawOffset.y);
        hidey->render();
    }

    // scaling (very unoptimized and slow!)
    // TODO: Implement properly
#ifdef _DEBUG_COL
    if (flags.hasFlag(lfScaleX) && getWidth() != GFX::getXResolution() ||
            flags.hasFlag(lfScaleY) && getHeight() != GFX::getYResolution() / 2.0f)
#else
    if (flags.hasFlag(lfScaleX) && getWidth() != GFX::getXResolution() ||
            flags.hasFlag(lfScaleY) && getHeight() != GFX::getYResolution())
#endif
    {
        static float fx = (float)GFX::getXResolution() / (float)getWidth();
#ifdef _DEBUG_COL
        static float fy = (float)GFX::getYResolution() / 2.0f / (float)getHeight();
#else
        static float fy = (float)GFX::getYResolution() / (float)getHeight();
#endif
        SDL_Surface* scaled;
        SDL_Rect scaleRect;
        if (not flags.hasFlag(lfScaleX))
        {
            scaled = zoomSurface(GFX::getVideoSurface(),fy,fy,SMOOTHING_OFF);
            scaleRect.x = (float)-drawOffset.x * fy;
            scaleRect.y = (float)-drawOffset.y * fy;
        }
        else if (not flags.hasFlag(lfScaleY))
        {
            scaled = zoomSurface(GFX::getVideoSurface(),fx,fx,SMOOTHING_OFF);
            scaleRect.x = (float)-drawOffset.x * fx;
            scaleRect.y = (float)-drawOffset.y * fx + (scaled->h - GFX::getYResolution());
        }
        else
        {
            scaled = zoomSurface(GFX::getVideoSurface(),fx,fy,SMOOTHING_OFF);
            scaleRect.x = (float)-drawOffset.x * fx;
            scaleRect.y = (float)-drawOffset.y * fy;
        }
        scaleRect.w = GFX::getXResolution();
#ifdef _DEBUG_COL
        scaleRect.h = GFX::getYResolution() / 2.0f;
#else
        scaleRect.h = GFX::getYResolution();
#endif
        SDL_BlitSurface(scaled,&scaleRect,GFX::getVideoSurface(),NULL);
        SDL_FreeSurface(scaled);
    }

    // draw level name overlay
    if (firstLoad)
    {
        if (name[0] != 0 && not nameTimer.hasFinished())
        {
            nameRect.render();
            nameText.print(name);
        }
    }

    DIALOGUE->render();

    EFFECTS->render();

    // draw the cursor
    Vector2df pos = input->getMouse();
    GFX::setPixel(pos,RED);
    GFX::setPixel(pos+Vector2df(1,1),RED);
    GFX::setPixel(pos+Vector2df(-1,1),RED);
    GFX::setPixel(pos+Vector2df(1,-1),RED);
    GFX::setPixel(pos+Vector2df(-1,-1),RED);
    GFX::renderPixelBuffer();

#ifdef _DEBUG
    debugString += "Players alive: " + StringUtility::intToString(players.size()) + "\n";
    debugString += "Units alive: " + StringUtility::intToString(units.size()) + "\n";
    debugString += "Particles: " + StringUtility::intToString(effects.size()) + "\n";
    debugText.setPosition(10,10);
    debugText.print(debugString);
    fpsDisplay.print(StringUtility::intToString((int)MyGame::getMyGame()->getFPS()));
#endif
}

void Level::render(SDL_Surface* screen)
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

    SDL_BlitSurface(levelImage,&src,screen,&dst);

    for (list<BaseUnit*>::iterator curr = units.begin(); curr != units.end(); ++curr)
    {
        renderUnit(screen,(*curr),drawOffset);
    }

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
    SDL_BlitSurface(screen,&src,collisionLayer,&dst);

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

void Level::onPause()
{
    SDL_BlitSurface(GFX::getVideoSurface(),NULL,pauseSurf,NULL);
    overlay.render(pauseSurf);
    pauseSelection = 2;
    nameText.setAlignment(LEFT_JUSTIFIED);
    input->resetKeys();
}

void Level::onResume()
{
    nameRect.setDimensions(GFX::getXResolution(),NAME_RECT_HEIGHT);
    nameRect.setPosition(0.0f,(GFX::getYResolution() - NAME_RECT_HEIGHT) / 2.0f);
    nameRect.setColour(BLACK);
    nameText.setAlignment(CENTRED);
    nameText.setPosition(0.0f,(GFX::getYResolution() - NAME_RECT_HEIGHT) / 2.0f + NAME_RECT_HEIGHT - NAME_TEXT_SIZE);
    nameText.setColour(WHITE);
    input->resetKeys();
}

void Level::pauseInput()
{
    input->update();

    if (input->isUp() && pauseSelection > 0)
    {
        --pauseSelection;
        input->resetUp();
    }
    if (input->isDown() && pauseSelection < PAUSE_MENU_ITEM_COUNT-1)
    {
        ++pauseSelection;
        input->resetDown();
    }

    if (input->isLeft())
    {
        if (pauseSelection == 0)
        {
            int vol = MUSIC_CACHE->getMusicVolume();
            if (vol > 0)
                MUSIC_CACHE->setMusicVolume(vol-8);
        }
        else if (pauseSelection == 1)
        {
            int vol = MUSIC_CACHE->getSoundVolume();
            if (vol > 0)
                MUSIC_CACHE->setSoundVolume(vol-8);
        }
    }
    else if (input->isRight())
    {
        if (pauseSelection == 0)
        {
            int vol = MUSIC_CACHE->getMusicVolume();
            if (vol < MUSIC_CACHE->getMaxVolume())
                MUSIC_CACHE->setMusicVolume(vol+8);
        }
        else if (pauseSelection == 1)
        {
            int vol = MUSIC_CACHE->getSoundVolume();
            if (vol < MUSIC_CACHE->getMaxVolume())
                MUSIC_CACHE->setSoundVolume(vol+8);
        }
    }

    if (input->isA() || input->isX())
    {
        if (pauseSelection == 2)
            pauseToggle();
        else if (pauseSelection == 3)
        {
            pauseToggle();
            for (list<ControlUnit*>::iterator iter = players.begin(); iter != players.end(); ++iter)
            {
                (*iter)->explode();
            }
            lose();
        }
        else if (pauseSelection == 4)
        {
            setNextState(STATE_MAIN);
            MUSIC_CACHE->playSound("sounds/menu_back.wav");
        }
    }

    if (input->isStart())
        pauseToggle();
}

void Level::pauseUpdate()
{
    //
}

void Level::pauseScreen()
{
    SDL_BlitSurface(pauseSurf,NULL,GFX::getVideoSurface(),NULL);

    string pauseItems[PAUSE_MENU_ITEM_COUNT] = {"MUSIC VOL:","SOUND VOL:","RETURN","SUICIDE","EXIT"};
    int pos = (GFX::getYResolution() - PAUSE_MENU_SPACING * (PAUSE_MENU_ITEM_COUNT-1)) / 2 + PAUSE_MENU_OFFSET_Y;

    // render text and selection
    for (int I = 0; I < PAUSE_MENU_ITEM_COUNT; ++I)
    {
        nameRect.setPosition(0,pos);
        nameText.setPosition(PAUSE_MENU_OFFSET_X,pos + NAME_RECT_HEIGHT - NAME_TEXT_SIZE);
        if (I == pauseSelection)
        {
            nameRect.setColour(WHITE);
            nameText.setColour(BLACK);
        }
        else
        {
            nameRect.setColour(BLACK);
            nameText.setColour(WHITE);
        }
        nameRect.render();
        nameText.print(pauseItems[I]);

        if (I == 0)
        {
            // render volume sliders
            float factor = (float)MUSIC_CACHE->getMusicVolume() / (float)MUSIC_CACHE->getMaxVolume();
            nameRect.setDimensions((float)PAUSE_VOLUME_SLIDER_SIZE * factor,NAME_RECT_HEIGHT);
            nameRect.setPosition((int)GFX::getXResolution() - PAUSE_VOLUME_SLIDER_SIZE - PAUSE_MENU_OFFSET_X,pos);
            if (pauseSelection == 0)
                nameRect.setColour(BLACK);
            else
                nameRect.setColour(WHITE);
            nameRect.render();
            nameRect.setDimensions(GFX::getXResolution(),NAME_RECT_HEIGHT);
        }
        else if (I == 1)
        {
            float factor = (float)MUSIC_CACHE->getSoundVolume() / (float)MUSIC_CACHE->getMaxVolume();
            nameRect.setDimensions(PAUSE_VOLUME_SLIDER_SIZE * factor,NAME_RECT_HEIGHT);
            nameRect.setPosition((int)GFX::getXResolution() - PAUSE_VOLUME_SLIDER_SIZE - PAUSE_MENU_OFFSET_X,pos);
            if (pauseSelection == 1)
                nameRect.setColour(BLACK);
            else
                nameRect.setColour(WHITE);
            nameRect.render();
            nameRect.setDimensions(GFX::getXResolution(),NAME_RECT_HEIGHT);
            pos += 20; // extra offset
        }

        pos += NAME_RECT_HEIGHT + PAUSE_MENU_SPACING;
    }

#ifdef _DEBUG
    fpsDisplay.print(StringUtility::intToString((int)MyGame::getMyGame()->getFPS()));
#endif
}

int Level::getWidth() const
{
    if (levelImage)
        return levelImage->w;
    return -1;
}

int Level::getHeight() const
{
    if (levelImage)
        return levelImage->h;
    return -1;
}

Vector2df Level::transformCoordinate(const Vector2df& coord) const
{
    Vector2df result = coord;

    // cache some values
    int width = getWidth();
    int height = getHeight();

    // assuming the coordinate is not THAT much out of bounds (else should use modulo)
    if (flags.hasFlag(lfRepeatX))
    {
        if (result.x >= width)
            result.x -= width;
        else if (result.x < 0)
            result.x += width;
    }
    if (flags.hasFlag(lfRepeatY))
    {
        if (result.y >= height)
            result.y -= height;
        else if (result.y < 0)
            result.y += height;
    }
    return result;
}

Vector2df Level::boundsCheck(const BaseUnit* const unit) const
{
    Vector2df result = unit->position;

    // cache some values
    int width = getWidth();
    int height = getHeight();

    if (flags.hasFlag(lfRepeatX))
    {
        if (result.x < 0)
            result.x += width;
        else if (result.x + unit->getWidth() > width)
            result.x -= width;
    }
    if (flags.hasFlag(lfRepeatY))
    {
        if (result.y < 0)
            result.y += height;
        else if (result.y + unit->getHeight() > height)
            result.y -= height;
    }
    return result;
}

ControlUnit* Level::getFirstActivePlayer() const
{
    for (list<ControlUnit*>::const_iterator unit = players.begin(); unit != players.end(); ++unit)
    {
        if ((*unit)->takesControl)
            return (*unit);
    }
    return NULL;
}

void Level::swapControl()
{
    bool valid = false;
    // check whether there is a player without control
    for (list<ControlUnit*>::iterator unit = players.begin(); unit != players.end(); ++unit)
    {
        if (not (*unit)->takesControl)
        {
            valid = true;
            break;
        }
    }

    if (valid)
    {
        for (list<ControlUnit*>::iterator unit = players.begin(); unit != players.end(); ++unit)
        {
            (*unit)->takesControl = not (*unit)->takesControl;
            if ((*unit)->takesControl)
                (*unit)->setSpriteState("wave",true);
        }
    }
}

void Level::lose()
{
    if (not eventTimer.isStarted())
    {
        eventTimer.setCallback(this,Level::loseCallback);
        eventTimer.start(750);
    }
}

void Level::win()
{
    if (not eventTimer.isStarted())
    {
        eventTimer.setCallback(this,Level::winCallback);
        eventTimer.start(1000);
        EFFECTS->fadeOut(1000);
    }
}

void Level::addParticle(const BaseUnit* const caller, const Colour& col, const Vector2df& pos, const Vector2df& vel, CRint lifeTime)
{
    PixelParticle* temp = new PixelParticle(this,lifeTime);
    // copy the collision colours from the calling unit to mimic behaviour
    for (list<Colour>::const_iterator iter = caller->collisionColours.begin(); iter != caller->collisionColours.end(); ++iter)
        temp->collisionColours.push_back(*iter);
    temp->position = pos;
    temp->velocity = vel;
    temp->col = col;
    effects.push_back(temp);
}

/// ---protected---

void Level::clearUnitFromCollision(SDL_Surface* const surface, BaseUnit* const unit)
{
    clearRectangle(surface,unit->position.x,unit->position.y,unit->getWidth(),unit->getHeight());

    Vector2df pos2 = boundsCheck(unit);
    if (pos2 != unit->position)
    {
        clearRectangle(surface,pos2.x,pos2.y,unit->getWidth(),unit->getHeight());
    }
}

void Level::clearRectangle(SDL_Surface* const surface, CRfloat posX, CRfloat posY, CRint width, CRint height)
{
    SDL_Rect unitRect;
    unitRect.x = max(posX,0.0f);
    unitRect.y = max(posY,0.0f);
    unitRect.w = min(surface->w - unitRect.x,width - max((int)-posX,0));
    unitRect.h = min(surface->h - unitRect.y,height - max((int)-posY,0));

    SDL_BlitSurface(levelImage,&unitRect,surface,&unitRect);
}

void Level::renderUnit(SDL_Surface* const surface, BaseUnit* const unit, const Vector2df& offset)
{
    unit->updateScreenPosition(offset);
    unit->render(surface);
    Vector2df pos2 = boundsCheck(unit);
    if (pos2 != unit->position)
    {
        Vector2df temp = unit->position;
        unit->position = pos2;
        unit->updateScreenPosition(offset);
        unit->render(surface);
        unit->position = temp;
    }
}

void Level::adjustPosition(BaseUnit* const unit)
{
    // 1 = out of right/bottom bounds, -1 = out of left/top bounds
    int boundsX = (unit->position.x > getWidth()) - (unit->position.x + unit->getWidth() < 0);
    int boundsY = (unit->position.y > getHeight()) - (unit->position.y + unit->getHeight() < 0);
    bool changed = false;

    if (boundsX != 0 && flags.hasFlag(lfRepeatX))
    {
        unit->position.x -= getWidth() * boundsX;
        changed = true;
    }
    if (boundsY != 0 && flags.hasFlag(lfRepeatY))
    {
        unit->position.y -= getHeight() * boundsY;
        changed = true;
    }
    if ((boundsX + boundsY != 0) && not changed)
    {
        unit->position = unit->startingPosition;
        changed = true;
    }
}

bool Level::processParameter(const pair<string,string>& value)
{
    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case lpImage:
    {
        bool fromCache;
        levelImage = SURFACE_CACHE->getSurface(value.second,chapterPath,fromCache);
        if (levelImage)
        {
            if (levelImage->w < GFX::getXResolution())
                drawOffset.x = ((int)levelImage->w - (int)GFX::getXResolution()) / 2.0f;
#ifdef _DEBUG_COL
            if (levelImage->h < GFX::getYResolution() / 2)
                drawOffset.y = ((int)levelImage->h - (int)GFX::getYResolution() / 2.0f) / 2.0f;
#else
            if (levelImage->h < GFX::getYResolution())
                drawOffset.y = ((int)levelImage->h - (int)GFX::getYResolution()) / 2.0f;
#endif
        }
        else
            parsed = false;
        break;
    }
    case lpFlags:
    {
        vector<string> props;
        StringUtility::tokenize(value.second,props,",");
        for (vector<string>::const_iterator str = props.begin(); str < props.end(); ++str)
            flags.addFlag(stringToFlag[*str]);
        break;
    }
    case lpFilename:
    {
        levelFileName = value.second;
        break;
    }
    case lpOffset:
    {
        vector<string> token;
        StringUtility::tokenize(value.second,token,DELIMIT_STRING);
        if (token.size() != 2)
        {
            parsed = false;
            break;
        }
        drawOffset.x = StringUtility::stringToFloat(token.at(0));
        drawOffset.y = StringUtility::stringToFloat(token.at(1));
        break;
    }
    case lpBackground:
    {
        int val = StringUtility::stringToInt(value.second);
        if (val > 0 || value.second == "0") // passed parameter is a numeric colour code
            GFX::setClearColour(Colour(val));
        else // string colour code
            GFX::setClearColour(Colour(value.second));
        break;
    }
    case lpBoundaries:
    {
        cam.disregardBoundaries = not StringUtility::stringToBool(value.second);
        break;
    }
    case lpName:
    {
        name = StringUtility::upper(value.second);
        break;
    }
    case lpMusic:
    {
        if (ENGINE->currentState != STATE_LEVELSELECT)
            MUSIC_CACHE->playMusic(value.second,chapterPath);
        break;
    }
    case lpDialogue:
    {
        if (ENGINE->currentState != STATE_LEVELSELECT)
            DIALOGUE->loadFromFile(chapterPath + value.second);
        break;
    }
    default:
        parsed = false;
    }
    return parsed;
}

void Level::loseCallback(void* data)
{
    ((Level*)data)->setNextState(STATE_THIS);
}

void Level::winCallback(void* data)
{
    ((Level*)data)->setNextState(STATE_NEXT);
}
