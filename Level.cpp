#include "Level.h"

#include "StringUtility.h"

#include "BaseUnit.h"
#include "ControlUnit.h"
#include "PixelParticle.h"
#include "Physics.h"
#include "MyGame.h"
#include "userStates.h"
#include "GreySurfaceCache.h"
#include "effects/Hollywood.h"
#include "MusicCache.h"
#include "Dialogue.h"
#include "Savegame.h"

#ifdef _MEOW
#define NAME_TEXT_SIZE 24
#define NAME_RECT_HEIGHT 18
#else
#define NAME_TEXT_SIZE 48
#define NAME_RECT_HEIGHT 35
#endif

#ifdef _MEOW
#define PAUSE_MENU_SPACING 4
#define PAUSE_MENU_OFFSET_Y -20
#else
#define PAUSE_MENU_SPACING 10
#define PAUSE_MENU_OFFSET_Y 0
#endif
#define PAUSE_MENU_OFFSET_X 20
#ifdef _MEOW
#define PAUSE_VOLUME_SLIDER_SIZE 125
#else
#define PAUSE_VOLUME_SLIDER_SIZE 400
#endif
#define PAUSE_MENU_SPACING_EXTRA 20

#ifdef _MEOW
#define TIME_TRIAL_OFFSET_Y 5
#define TIME_TRIAL_SPACING_Y 0
#define TIME_TRIAL_MENU_OFFSET_Y 30
#else
#define TIME_TRIAL_OFFSET_Y 10
#define TIME_TRIAL_SPACING_Y 10
#define TIME_TRIAL_MENU_OFFSET_Y 80
#endif

#define END_TIMER_ANIMATION_STEP 30

#define XOR(a,b) ((a) && !(b)) || (!(a) && (b))

map<string,int> Level::stringToFlag;
map<string,int> Level::stringToProp;

Level::Level()
{
    // set-up static string to int conversion maps
    stringToFlag["repeatx"] = lfRepeatX;
    stringToFlag["repeaty"] = lfRepeatY;
    stringToFlag["scrollx"] = lfScrollX;
    stringToFlag["scrolly"] = lfScrollY;
    stringToFlag["disableswap"] = lfDisableSwap;
    stringToFlag["keepcentred"] = lfKeepCentred;
    stringToFlag["scalex"] = lfScaleX;
    stringToFlag["scaley"] = lfScaleY;
    stringToFlag["splitx"] = lfSplitX;
    stringToFlag["splity"] = lfSplitY;
    stringToFlag["drawpattern"] = lfDrawPattern;

    stringToProp["image"] = lpImage;
    stringToProp["flags"] = lpFlags;
    stringToProp["filename"] = lpFilename;
    stringToProp["offset"] = lpOffset;
    stringToProp["background"] = lpBackground;
    stringToProp["boundaries"] = lpBoundaries;
    stringToProp["name"] = lpName;
    stringToProp["music"] = lpMusic;
    stringToProp["dialogue"] = lpDialogue;
    stringToProp["gravity"] = lpGravity;
    stringToProp["terminalvelocity"] = lpTerminalVelocity;

    levelImage = NULL;
    collisionLayer = NULL;
    levelFileName = "";
    chapterPath = "";
    errorString = "";
    drawOffset = Vector2df(0,0);
    idCounter = 0;
    PHYSICS->reset();

    eventTimer.init(1000,MILLI_SECONDS);
    eventTimer.setRewind(STOP_AND_REWIND);
    winCounter = 0;
    firstLoad = true;
    trialEnd = false;

#ifdef _MUSIC
	musicLister.loadFont(GAME_FONT,32);
    musicLister.setTextColour(WHITE);
    musicLister.setTextSelectionColour(RED);
    musicLister.addFilter("ogg");
    musicLister.addFilter("mp3");
    musicLister.addFilter("wav");
    musicLister.setPath("music");
    showMusicList = false;
#endif

#ifdef _DEBUG
    debugText.loadFont(DEBUG_FONT,8);
    debugText.setColour(50,217,54);
    debugString = "";
    frameLimiter = true;
#endif
#ifdef PENJIN_CALC_FPS
    fpsDisplay.loadFont(DEBUG_FONT,24);
    fpsDisplay.setColour(GREEN);
    fpsDisplay.setPosition(GFX::getXResolution(),0);
    fpsDisplay.setAlignment(RIGHT_JUSTIFIED);
#endif
    nameText.loadFont(GAME_FONT,NAME_TEXT_SIZE);
    nameText.setColour(WHITE);
    nameText.setAlignment(CENTRED);
    nameText.setUpBoundary(Vector2di(GFX::getXResolution(),GFX::getYResolution()));
    nameText.setPosition(0.0f,(GFX::getYResolution() - NAME_RECT_HEIGHT) / 2.0f + NAME_RECT_HEIGHT - NAME_TEXT_SIZE);
    nameRect.setDimensions(GFX::getXResolution(),NAME_RECT_HEIGHT);
    nameRect.setPosition(0.0f,(GFX::getYResolution() - NAME_RECT_HEIGHT) / 2.0f);
    nameRect.setColour(BLACK);
    nameTimer.init(2000,MILLI_SECONDS);

    if (ENGINE->timeTrial)
    {
        timeTrialText.loadFont(GAME_FONT,int(NAME_TEXT_SIZE * 1.5));
        timeTrialText.setColour(WHITE);
        timeTrialText.setAlignment(RIGHT_JUSTIFIED);
        timeTrialText.setUpBoundary(Vector2di(GFX::getXResolution()-PAUSE_MENU_OFFSET_X,GFX::getYResolution()-10));
        timeTrialText.setPosition(PAUSE_MENU_OFFSET_X,TIME_TRIAL_OFFSET_Y);
    }

    cam.parent = this;

    pauseSurf = SDL_CreateRGBSurface(SDL_SWSURFACE,GFX::getXResolution(),GFX::getYResolution(),GFX::getVideoSurface()->format->BitsPerPixel,0,0,0,0);
    pauseSelection = 0;
    overlay.setDimensions(GFX::getXResolution(),GFX::getYResolution());
    overlay.setPosition(0,0);
    overlay.setColour(BLACK);
    overlay.setAlpha(100);

    hideHor = false;
    hideVert = false;

    lastPos = Vector2di(0,0);

	if (ENGINE->currentState != STATE_LEVELSELECT)
		GFX::showCursor(false);
}

Level::~Level()
{
	if (ENGINE->currentState != STATE_LEVELSELECT)
	{
#ifdef _MUSIC
		saveMusicToFile(MUSIC_CACHE->getPlaying());
#endif
		GFX::showCursor(true);
	}

    SDL_FreeSurface(collisionLayer);
    for (vector<ControlUnit*>::iterator curr = players.begin(); curr != players.end(); ++curr)
    {
        delete (*curr);
    }
    players.clear();
    for (vector<ControlUnit*>::iterator curr = removedPlayers.begin(); curr != removedPlayers.end(); ++curr)
    {
        delete (*curr);
    }
    removedPlayers.clear();
    for (vector<BaseUnit*>::iterator curr = units.begin(); curr != units.end(); ++curr)
    {
        delete (*curr);
    }
    units.clear();
    for (vector<BaseUnit*>::iterator curr = removedUnits.begin(); curr != removedUnits.end(); ++curr)
    {
        delete (*curr);
    }
    removedUnits.clear();
    for (vector<PixelParticle*>::iterator curr = effects.begin(); curr != effects.end(); ++curr)
    {
        delete (*curr);
    }
    effects.clear();
    #ifdef _DEBUG
    debugUnits.clear();
    ENGINE->setFrameRate(FRAME_RATE);
    #endif

    // reset background colour
    GFX::setClearColour(BLACK);

    SDL_FreeSurface(pauseSurf);

    parameters.clear();
}

/// ---public---

bool Level::load(const list<PARAMETER_TYPE >& params)
{
    for (list<PARAMETER_TYPE >::const_iterator value = params.begin(); value != params.end(); ++value)
    {
        if (not processParameter(make_pair(value->first,value->second)) && value->first != CLASS_STRING)
        {
            string className = params.front().second;
            printf("WARNING: Unprocessed parameter \"%s\" on level \"%s\"\n",value->first.c_str(),className.c_str());
        }
    }

    if (not levelImage)
    {
        errorString = "ERROR: No image has been specified or image file could not be loaded! (critical)";
        return false;
    }
    else
    {
        collisionLayer = SDL_CreateRGBSurface(SDL_SWSURFACE,levelImage->w,levelImage->h,GFX::getVideoSurface()->format->BitsPerPixel,0,0,0,0);
    }

    tilingSetup();

    return true;
}

void Level::tilingSetup()
{
    if (getWidth() < GFX::getXResolution())
        hideHor = true;
    if (getHeight() < GFX::getYResolution())
        hideVert = true;
}

bool Level::processParameter(const PARAMETER_TYPE& value)
{
    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case lpImage:
    {
        levelImage = SURFACE_CACHE->loadSurface(value.second,chapterPath);
        if (levelImage == SURFACE_CACHE->getErrorSurface())
			levelImage = NULL;
        if (levelImage)
        {
            if (levelImage->w < GFX::getXResolution())
                drawOffset.x = ((int)levelImage->w - (int)GFX::getXResolution()) / 2.0f;
            if (levelImage->h < GFX::getYResolution())
                drawOffset.y = ((int)levelImage->h - (int)GFX::getYResolution()) / 2.0f;
        }
        else
            parsed = false;
        break;
    }
    case lpFlags:
    {
        flags.clear();
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
        drawOffset.x = StringUtility::stringToFloat(token[0]);
        drawOffset.y = StringUtility::stringToFloat(token[1]);
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
        {
            if (value.second == "none")
                MUSIC_CACHE->stopMusic();
            else
                MUSIC_CACHE->playMusic(value.second,chapterPath);
        }
        break;
    }
    case lpDialogue:
    {
        if (ENGINE->currentState != STATE_LEVELSELECT)
            DIALOGUE->loadFromFile(chapterPath + value.second);
        break;
    }
    case lpGravity:
    {
        vector<string> token;
        StringUtility::tokenize(value.second,token,DELIMIT_STRING);
        PHYSICS->gravity = Vector2df(0,0);
        switch (token.size())
        {
        case 1:
            PHYSICS->gravity.x = StringUtility::stringToFloat(token[0]);
            break;
        case 2:
            PHYSICS->gravity.x = StringUtility::stringToFloat(token[0]);
            PHYSICS->gravity.y = StringUtility::stringToFloat(token[1]);
            break;
        default:
            parsed = false;
            break;
        }
        break;
    }
    case lpTerminalVelocity:
    {
        vector<string> token;
        StringUtility::tokenize(value.second,token,DELIMIT_STRING);
        if (token.size() != 2)
        {
            parsed = false;
            break;
        }
        PHYSICS->maximum.x = StringUtility::stringToFloat(token[0]);
        PHYSICS->maximum.y = StringUtility::stringToFloat(token[1]);
        break;
    }
    default:
        parsed = false;
    }
    return parsed;
}

void Level::reset()
{
    for (vector<ControlUnit*>::iterator player = removedPlayers.begin(); player != removedPlayers.end();)
    {
        players.push_back(*player);
        player = removedPlayers.erase(player);
    }
    for (vector<BaseUnit*>::iterator unit = removedUnits.begin(); unit != removedUnits.end();)
    {
        units.push_back(*unit);
        unit = removedUnits.erase(unit);
    }

    vector<bool> playersControl;
    // Check which units are currently taking control
    for (vector<ControlUnit*>::iterator I = players.begin(); I != players.end(); ++I)
        playersControl.push_back((*I)->takesControl);

    for (vector<ControlUnit*>::iterator player = players.begin(); player != players.end(); ++player)
    {
        (*player)->reset();
    }
    // Reset control state
    for (int I = players.size()-1; I >= 0; --I)
    {
        players[I]->takesControl = playersControl[I];
        if (players[I]->startingState == "wave" || players[I]->startingState == "stand")
            playersControl[I] ? players[I]->startingState = "wave" : players[I]->startingState = "stand";
        players[I]->setSpriteState(players[I]->startingState);
    }
    playersControl.clear();

    for (vector<BaseUnit*>::iterator unit = units.begin(); unit != units.end(); ++unit)
    {
        (*unit)->reset();
    }
    for (vector<PixelParticle*>::iterator curr = effects.begin(); curr != effects.end(); ++curr)
    {
        delete (*curr);
    }
    effects.clear();

    firstLoad = false;
    ENGINE->restartCounter++;
    cam.reset();
    PHYSICS->reset();
    drawOffset = Vector2df(0,0);

    load(parameters);
    init();
}

void Level::init()
{
    winCounter = 0;
    for (vector<ControlUnit*>::const_iterator I = players.begin(); I != players.end(); ++I)
    {
        if ((*I)->flags.hasFlag(BaseUnit::ufMissionObjective))
            ++winCounter;
    }
    if (winCounter == 0)
        winCounter = 1; // prevent insta-win

    if (firstLoad)
    {
        nameTimer.start(2000);
        if (ENGINE->currentState != STATE_LEVELSELECT)
            EFFECTS->fadeIn(1000);
    }
    timeCounter = 0;
    newRecord = false;

    SDL_BlitSurface(levelImage,NULL,collisionLayer,NULL);
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
    if (input->isA() && input->isX())
    {
        for (vector<ControlUnit*>::iterator iter = players.begin(); iter != players.end(); ++iter)
        {
            (*iter)->explode();
        }
        lose();
        input->resetKeys();
    }

#ifdef _DEBUG
    if (input->isLeftClick())
    {
        Vector2df pos = input->getMouse() + drawOffset;
        for (vector<BaseUnit*>::iterator I = units.begin(); I != units.end(); ++I)
        {
            if (pos.inRect((*I)->getRect()))
                debugUnits.push_back(*I);
        }
        for (vector<ControlUnit*>::iterator I = players.begin(); I != players.end(); ++I)
        {
            if (pos.inRect((*I)->getRect()))
                debugUnits.push_back(*I);
        }
    }
    if (input->isRightClick())
        debugUnits.clear();
    input->resetMouseButtons();

    if (input->isKey("b"))
	{
		if (frameLimiter)
			ENGINE->setFrameRate(1000);
		else
			ENGINE->setFrameRate(FRAME_RATE);
		frameLimiter = !frameLimiter;
	}

    if ((input->isL() || input->isR() || input->isSelect()) && not flags.hasFlag(lfDisableSwap))
#else
    if ((input->isL() || input->isR() || input->isLeftClick() || input->isRightClick() || input->isSelect())
        && not flags.hasFlag(lfDisableSwap))
#endif
    {
        if (players.size() > 1)
        {
            swapControl();
        }
        input->resetL();
        input->resetR();
        input->resetMouseButtons();
        input->resetSelect();
    }

    for (vector<ControlUnit*>::iterator curr = players.begin(); curr != players.end(); ++curr)
    {
        if ((*curr)->takesControl)
            (*curr)->control(input);
        else
            (*curr)->control(NULL);
    }
    input->resetB();
    input->resetY();
}

void Level::update()
{
    ++timeCounter;

    // Check for units to be removed, also reset temporary data
    for (vector<ControlUnit*>::iterator player = players.begin(); player != players.end();)
    {
        (*player)->resetTemporary();
        if ((*player)->toBeRemoved)
        {
            clearUnitFromCollision(collisionLayer,*player);
            removedPlayers.push_back(*player);
            player = players.erase(player);
        }
        else
        {
            ++player;
        }
    }
    for (vector<BaseUnit*>::iterator unit = units.begin();  unit != units.end();)
    {
        (*unit)->resetTemporary();
        if ((*unit)->toBeRemoved)
        {
            clearUnitFromCollision(collisionLayer,*unit);
            removedUnits.push_back(*unit);
            unit = units.erase(unit);
        }
        else
        {
            ++unit;
        }
    }
    for (vector<PixelParticle*>::iterator part = effects.begin();  part != effects.end();)
    {
        (*part)->resetTemporary();
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
    for (vector<PixelParticle*>::iterator curr = effects.begin(); curr != effects.end(); ++curr)
    {
        PHYSICS->applyPhysics(*curr);
        PHYSICS->particleMapCollision(this,collisionLayer,(*curr));
        (*curr)->update();
    }

    // physics (acceleration, friction, etc)
    for (vector<BaseUnit*>::iterator unit = units.begin();  unit != units.end(); ++unit)
    {
        adjustPosition(*unit);
        PHYSICS->applyPhysics(*unit);
    }
    // cache unit collision data for ALL units
    for (vector<ControlUnit*>::iterator player = players.begin(); player != players.end(); ++player)
    {
        clearUnitFromCollision(collisionLayer,*player);
        adjustPosition( *player, (*player)->takesControl );
        PHYSICS->applyPhysics(*player);
        for (vector<BaseUnit*>::iterator unit = units.begin();  unit != units.end(); ++unit)
        {
            PHYSICS->playerUnitCollision(this,(*player),(*unit));
        }
    }
    for (vector<BaseUnit*>::iterator unit = units.begin(); unit != units.end(); ++unit)
    {
        vector<BaseUnit*>::iterator next = unit;
        for (vector<BaseUnit*>::iterator unit2 = ++next; unit2 != units.end(); ++unit2)
            PHYSICS->playerUnitCollision(this,(*unit2),(*unit));

        for (vector<UnitCollisionEntry>::iterator item = (*unit)->collisionInfo.units.begin();
            item != (*unit)->collisionInfo.units.end(); ++item)
        {
            if (item->unit->hitUnitCheck(*unit))
                (*unit)->hitUnit(*item);
        }
    }

    // TODO: Clearing and redrawing units causes problems with the order
    // Make the map collision routine check every pixel also for a unit collision
    // instead and handle that (instead of a separate unit collision test)
    // also if a sinlge pixel only collides with the unit itself disregard that

    // map collision
    for (vector<BaseUnit*>::iterator curr = units.begin(); curr != units.end(); ++curr)
    {
        clearUnitFromCollision(collisionLayer,(*curr));
        // check for overwritten units by last clearUnitFromCollision call and redraw them
        for (vector<UnitCollisionEntry>::iterator item = (*curr)->collisionInfo.units.begin();
			item != (*curr)->collisionInfo.units.end(); ++item)
        {
            if (not item->unit->isPlayer)
                renderUnit(collisionLayer,item->unit,Vector2df(0,0));
        }

        if (not (*curr)->flags.hasFlag(BaseUnit::ufNoMapCollision))
        {
            PHYSICS->unitMapCollision(this,collisionLayer,(*curr));
        }

        // else still update unit on collision surface for player-map collision
        if (not (*curr)->flags.hasFlag(BaseUnit::ufNoUpdate))
            (*curr)->update();
        renderUnit(collisionLayer,(*curr),Vector2df(0,0));
    }

    // units always on top
    // This is hacked as fuck to work around my brain-dead render system, but at
    // this point I just don't care and can't be bothered to fix it properly
    for (vector<BaseUnit*>::iterator curr = units.begin(); curr != units.end(); ++curr)
    {
        if ((*curr)->flags.hasFlag(BaseUnit::ufAlwaysOnTop))
            renderUnit(collisionLayer,*curr,Vector2df(0,0));
    }

    // player-map collision
    for (vector<ControlUnit*>::iterator curr = players.begin(); curr != players.end(); ++curr)
    {
        // players should always have map collision enabled, so don't check for that here
        PHYSICS->unitMapCollision(this,collisionLayer,(*curr));
        (*curr)->update();
    }

    // other update stuff
    if (flags.hasFlag(lfKeepCentred))
        cam.centerOnUnit(getFirstActivePlayer(),500);
    eventTimer.update();
    DIALOGUE->update();

    if (winCounter <= 0)
    {
        win();
    }

    EFFECTS->update();

    cam.update();

#ifdef _DEBUG
    debugString = debugInfo();
    for (vector<BaseUnit*>::const_iterator I = debugUnits.begin(); I != debugUnits.end(); ++I)
    {
        debugString += (*I)->debugInfo();
    }
#endif
}

void Level::render()
{
    //GFX::clearScreen();

    render(GFX::getVideoSurface());

    // if level is smaller hide outside area
    SimpleFlags sides;
    SDL_Rect src, dst;
	dst.w = 0;
	dst.h = 0;
    if ( drawOffset.x < 0.0f ) // level has void on left side
	{
		sides.addFlag( 1 );
		dst.x = 0.0f;
		dst.y = max( -drawOffset.y, 0.0f );
		if ( flags.hasFlag( lfRepeatX ) && flags.hasFlag( lfDrawPattern ) )
		{
			src.x = max( getWidth() + drawOffset.x, 0.0f );
			src.y = max( drawOffset.y, 0.0f );
			src.w = -drawOffset.x;
			src.h = min( getHeight(), (int)GFX::getYResolution() );
			SDL_BlitSurface( collisionLayer,&src,GFX::getVideoSurface(),&dst );
		}
		else
		{
			dst.w = -drawOffset.x;
			dst.h = min( getHeight(), (int)GFX::getYResolution() );
            SDL_FillRect( GFX::getVideoSurface(),&dst,GFX::getClearColour().getSDL_Uint32Colour(GFX::getVideoSurface()) );
		}
	}
	if ( getWidth() - drawOffset.x < GFX::getXResolution() ) // void on right side
	{
		sides.addFlag( 2 );
		dst.x = getWidth() - drawOffset.x;
		dst.y = max( -drawOffset.y, 0.0f );
		if ( flags.hasFlag( lfRepeatX ) && flags.hasFlag( lfDrawPattern ) )
		{
			src.x = 0.0f;
			src.y = max( drawOffset.y, 0.0f );
			src.w = (int)GFX::getXResolution() - getWidth() - drawOffset.x;
			src.h = min( getHeight(), (int)GFX::getYResolution() );
			SDL_BlitSurface( collisionLayer,&src,GFX::getVideoSurface(),&dst );
		}
		else
		{
			dst.w = (int)GFX::getXResolution() - getWidth() - drawOffset.x;
			dst.h = min( getHeight(), (int)GFX::getYResolution() );
            SDL_FillRect( GFX::getVideoSurface(),&dst,GFX::getClearColour().getSDL_Uint32Colour(GFX::getVideoSurface()) );
		}
	}
	if ( drawOffset.y < 0.0f ) // void on top
	{
		sides.addFlag( 4 );
		dst.x = max( -drawOffset.x, 0.0f );
		dst.y = 0.0f;
		if ( flags.hasFlag( lfRepeatY ) && flags.hasFlag( lfDrawPattern ) )
		{
			src.x = max( drawOffset.x, 0.0f );
			src.y = max( getHeight() + drawOffset.y, 0.0f );
			src.w = min( getWidth(), (int)GFX::getXResolution() );
			src.h = -drawOffset.y;
			SDL_BlitSurface( collisionLayer,&src,GFX::getVideoSurface(),&dst );
		}
		else
		{
			dst.w = min( getWidth(), (int)GFX::getXResolution() );
			dst.h = -drawOffset.y;
            SDL_FillRect( GFX::getVideoSurface(),&dst,GFX::getClearColour().getSDL_Uint32Colour(GFX::getVideoSurface()) );
		}
	}
	if ( getHeight() - drawOffset.y < GFX::getYResolution() ) // void on bottom
	{
		sides.addFlag( 8 );
		dst.x = max( -drawOffset.x, 0.0f );
		dst.y = getHeight() - drawOffset.y;
		if ( flags.hasFlag( lfRepeatY ) && flags.hasFlag( lfDrawPattern ) )
		{
			src.x = max( drawOffset.x, 0.0f );
			src.y = 0.0f;
			src.w = min( getWidth(), (int)GFX::getXResolution() );
			src.h = (int)GFX::getYResolution() - getHeight() - drawOffset.y;
			SDL_BlitSurface( collisionLayer,&src,GFX::getVideoSurface(),&dst );
		}
		else
		{
			dst.w = min( getWidth(), (int)GFX::getXResolution() );
			dst.h = (int)GFX::getYResolution() - getHeight() - drawOffset.y;
            SDL_FillRect( GFX::getVideoSurface(),&dst,GFX::getClearColour().getSDL_Uint32Colour(GFX::getVideoSurface()) );
		}
	}
	if ( sides.hasFlag( 1 ) && sides.hasFlag( 4 ) ) // top-left corner
	{
		dst.x = 0.0f;
		dst.y = 0.0f;
		if ( flags.hasFlag( lfRepeatX ) && flags.hasFlag( lfRepeatY ) && flags.hasFlag( lfDrawPattern ) )
		{
			src.x = max( getWidth() + drawOffset.x, 0.0f );
			src.y = max( getHeight() + drawOffset.y, 0.0f );
			src.w = -drawOffset.x;
			src.h = -drawOffset.y;
			SDL_BlitSurface( collisionLayer,&src,GFX::getVideoSurface(),&dst );
		}
		else
		{
			dst.w = -drawOffset.x;
			dst.h = -drawOffset.y;
            SDL_FillRect( GFX::getVideoSurface(),&dst,GFX::getClearColour().getSDL_Uint32Colour(GFX::getVideoSurface()) );
		}
	}
	if ( sides.hasFlag( 2 ) && sides.hasFlag( 4 ) ) // top-right corner
	{
		dst.x = getWidth() - drawOffset.x;
		dst.y = 0.0f;
		if ( flags.hasFlag( lfRepeatX ) && flags.hasFlag( lfRepeatY ) && flags.hasFlag( lfDrawPattern ) )
		{
			src.x = 0.0f;
			src.y = max( getHeight() + drawOffset.y, 0.0f );
			src.w = (int)GFX::getXResolution() - getWidth() - drawOffset.x;
			src.h = -drawOffset.y;
			SDL_BlitSurface( collisionLayer,&src,GFX::getVideoSurface(),&dst );
		}
		else
		{
			dst.w = (int)GFX::getXResolution() - getWidth() - drawOffset.x;
			dst.h = -drawOffset.y;
            SDL_FillRect( GFX::getVideoSurface(),&dst,GFX::getClearColour().getSDL_Uint32Colour(GFX::getVideoSurface()) );
		}
	}
	if ( sides.hasFlag( 1 ) && sides.hasFlag( 8 ) ) // bottom-left corner
	{
		dst.x = 0.0f;
		dst.y = getHeight() - drawOffset.y;
		if ( flags.hasFlag( lfRepeatX ) && flags.hasFlag( lfRepeatY ) && flags.hasFlag( lfDrawPattern ) )
		{
			src.x = max( getWidth() + drawOffset.x, 0.0f );
			src.y = 0.0f;
			src.w = -drawOffset.x;
			src.h = (int)GFX::getYResolution() - getHeight() - drawOffset.y;
			SDL_BlitSurface( collisionLayer,&src,GFX::getVideoSurface(),&dst );
		}
		else
		{
			dst.w = -drawOffset.x;
			dst.h = (int)GFX::getYResolution() - getHeight() - drawOffset.y;
            SDL_FillRect( GFX::getVideoSurface(),&dst,GFX::getClearColour().getSDL_Uint32Colour(GFX::getVideoSurface()) );
		}
	}
	if ( sides.hasFlag( 2 ) && sides.hasFlag( 8 ) ) // bottom-right corner
	{
		dst.x = getWidth() - drawOffset.x;
		dst.y = getHeight() - drawOffset.y;
		if ( flags.hasFlag( lfRepeatX ) && flags.hasFlag( lfRepeatY ) && flags.hasFlag( lfDrawPattern ) )
		{
			src.x = 0.0f;
			src.y = 0.0f;
			src.w = (int)GFX::getXResolution() - getWidth() - drawOffset.x;
			src.h = (int)GFX::getYResolution() - getHeight() - drawOffset.y;
			SDL_BlitSurface( collisionLayer,&src,GFX::getVideoSurface(),&dst );
		}
		else
		{
			dst.w = (int)GFX::getXResolution() - getWidth() - drawOffset.x;
			dst.h = (int)GFX::getYResolution() - getHeight() - drawOffset.y;
            SDL_FillRect( GFX::getVideoSurface(),&dst,GFX::getClearColour().getSDL_Uint32Colour(GFX::getVideoSurface()) );
		}
	}

    // scaling (very unoptimized and slow!)
    // TODO: Implement properly
    if (flags.hasFlag(lfScaleX) && getWidth() != GFX::getXResolution() ||
            flags.hasFlag(lfScaleY) && getHeight() != GFX::getYResolution())
    {
        static float fx = (float)GFX::getXResolution() / (float)getWidth();
        static float fy = (float)GFX::getYResolution() / (float)getHeight();
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
        scaleRect.h = GFX::getYResolution();
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
    //GFX::renderPixelBuffer();

#ifdef _DEBUG
    debugText.setPosition(10,10);
    debugText.print(debugString);
#endif
#ifdef PENJIN_CALC_FPS
    fpsDisplay.print(StringUtility::intToString((int)MyGame::getMyGame()->getFPS()));
#endif
}

void Level::render(SDL_Surface* screen)
{
    // split screen on small screens and multiple players
    if (not hideHor && not hideVert && (flags.hasFlag(lfSplitX) || flags.hasFlag(lfSplitY)) && players.size() > 1 && not playersVisible())
    {
        // TODO: Proper algorithm here, not centring on one player, but "between" both and smooth
        // disconnection of screen parts (make X and Y split automatically maybe)

        // caching values
        int count = players.size();
        bool horizontally = flags.hasFlag(lfSplitX); // else vertically
        int size = 0;
        if (horizontally)
            size = GFX::getYResolution() / count;
        else
            size = GFX::getXResolution() / count;

        // players don't get drawn to the collision surface for collision testing
        for (vector<ControlUnit*>::iterator curr = players.begin(); curr != players.end(); ++curr)
        {
            renderUnit(collisionLayer,(*curr),Vector2df(0,0));
        }

        // sort players (not efficient, but we are talking low numbers here, usually 2)
        vector<ControlUnit*> temp;
        for (vector<ControlUnit*>::const_iterator curr = players.begin(); curr != players.end(); ++curr)
        {
            vector<ControlUnit*>::iterator ins;
            for (ins = temp.begin(); ins != temp.end(); ++ins)
            {
                if (horizontally)
                {
                    if ((*ins)->position.y > (*curr)->position.y)
                        break;
                }
                else
                {
                    if ((*ins)->position.x > (*curr)->position.x)
                        break;
                }
            }
            temp.insert(ins,*curr);
        }

        for (int I = 0; I < count; ++I)
        {
            SDL_Rect src;
            SDL_Rect dst;
            if (horizontally)
            {
                src.w = GFX::getXResolution();
                src.h = size;
                dst.x = 0;
                dst.y = size * I;
            }
            else
            {
                src.w = size;
                src.h = GFX::getYResolution();
                dst.x = size * I;
                dst.y = 0;
            }
            src.x = max(temp[I]->position.x - src.w / 2.0f,0.0f);
            src.y = max(temp[I]->position.y - src.h / 2.0f,0.0f);
            SDL_BlitSurface(collisionLayer,&src,screen,&dst);
        }
        temp.clear();
    }
    else
    {
        SDL_Rect src;
        SDL_Rect dst;
        dst.x = max(-drawOffset.x,0.0f);
        dst.y = max(-drawOffset.y,0.0f);
        src.x = max(drawOffset.x,0.0f);
        src.y = max(drawOffset.y,0.0f);
        src.w = min((int)GFX::getXResolution(),getWidth() - src.x);
        src.h = min((int)GFX::getYResolution(),getHeight() - src.y);

        // players don't get drawn to the collision surface for collision testing
        for (vector<ControlUnit*>::iterator curr = players.begin(); curr != players.end(); ++curr)
        {
            renderUnit(collisionLayer,(*curr),Vector2df(0,0));
        }

        SDL_BlitSurface(collisionLayer,&src,screen,&dst);
    }

    // particles
    for (vector<PixelParticle*>::iterator curr = effects.begin(); curr != effects.end(); ++curr)
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
    if (trialEnd)
    {
        pauseSelection = 0;
        timeDisplay = 0;
        pauseItems.push_back("RESTART");
        pauseItems.push_back("NEXT");
        pauseItems.push_back("EXIT");
    }
    else
    {
        pauseSelection = 2;
	#ifdef _MUSIC
		pauseItems.push_back("MUSIC FILE:");
	#endif
        pauseItems.push_back("MUSIC VOL:");
        pauseItems.push_back("SOUND VOL:");
        pauseItems.push_back("RETURN");
        #ifdef _DEBUG
        pauseItems.push_back("RELOAD");
        #else
        pauseItems.push_back("RESTART");
        #endif
        pauseItems.push_back("EXIT");
    }
    nameText.setAlignment(LEFT_JUSTIFIED);
    input->resetKeys();
    GFX::showCursor(true);
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
    timeTrialText.setPosition(PAUSE_MENU_OFFSET_X,TIME_TRIAL_OFFSET_Y);
    timeTrialText.setAlignment(RIGHT_JUSTIFIED);
    timeTrialText.setColour(WHITE);
    pauseItems.clear();
    #ifdef _MUSIC
    showMusicList = false;
    #endif
    GFX::showCursor(false);
}

void Level::pauseInput()
{
    input->update();

#ifdef PLATFORM_PC
    if (input->isQuit())
    {
        nullifyState();
        return;
    }
#endif

#ifdef _MUSIC
	if (showMusicList)
	{
        if(input->isUp())
            musicLister.menuUp();
        else if(input->isDown())
            musicLister.menuDown();

		if (ACCEPT_KEY)
		{
			string file = musicLister.enter();
			MUSIC_CACHE->playMusic(file);
		}
		else if (CANCEL_KEY)
			showMusicList = false;

        if (input->isStart())
            pauseToggle();

		input->resetKeys();
		return;
	}
#endif

    if (trialEnd)
    {
        if (input->isUp() && pauseSelection > 0)
        {
            --pauseSelection;
            input->resetUp();
        }
        if (input->isDown() && pauseSelection < pauseItems.size()-1)
        {
            ++pauseSelection;
            input->resetDown();
        }
        if (ACCEPT_KEY || input->isLeftClick())
        {
            switch (pauseSelection)
            {
            case 0:
                reset();
                trialEnd = false;
                pauseToggle();
                break;
            case 1:
                eventTimer.setCallback(this,Level::winCallback);
                eventTimer.start(1000);
                EFFECTS->fadeOut(1000);
                pauseToggle();
                break;
            case 2:
                setNextState(STATE_MAIN);
                MUSIC_CACHE->playSound("sounds/menu_back.wav");
                break;
            default:
                break;
            }
        }
    }
    else
    {
        if (input->isUp() && pauseSelection > 0)
        {
            --pauseSelection;
            input->resetUp();
        }
        if (input->isDown() && pauseSelection < pauseItems.size()-1)
        {
            ++pauseSelection;
            input->resetDown();
        }

        if (input->isLeft())
        {
            if (pauseItems[pauseSelection] == "MUSIC VOL:")
            {
                int vol = MUSIC_CACHE->getMusicVolume();
                if (vol > 0)
                    MUSIC_CACHE->setMusicVolume(vol-8);
            }
            else if (pauseItems[pauseSelection] == "SOUND VOL:")
            {
                int vol = MUSIC_CACHE->getSoundVolume();
                if (vol > 0)
                {
                    MUSIC_CACHE->setSoundVolume(vol-8);
                    MUSIC_CACHE->playSound("sounds/level_select.wav");
                }
            }
        }
        else if (input->isRight())
        {
            if (pauseItems[pauseSelection] == "MUSIC VOL:")
            {
                int vol = MUSIC_CACHE->getMusicVolume();
                if (vol < MUSIC_CACHE->getMaxVolume())
                    MUSIC_CACHE->setMusicVolume(vol+8);
            }
            else if (pauseItems[pauseSelection] == "SOUND VOL:")
            {
                int vol = MUSIC_CACHE->getSoundVolume();
                if (vol < MUSIC_CACHE->getMaxVolume())
                {
                    MUSIC_CACHE->setSoundVolume(vol+8);
                    MUSIC_CACHE->playSound("sounds/level_select.wav");
                }
            }
        }

        if (ACCEPT_KEY || input->isLeftClick())
        {
            if (pauseItems[pauseSelection] == "RETURN")
                pauseToggle();
			else if (pauseItems[pauseSelection] == "RELOAD")
                setNextState(STATE_LEVEL);
            else if (pauseItems[pauseSelection] == "RESTART")
            {
                pauseToggle();
                for (vector<ControlUnit*>::iterator iter = players.begin(); iter != players.end(); ++iter)
                {
                    (*iter)->explode();
                }
                lose();
			}
            else if (pauseItems[pauseSelection] == "EXIT")
			{
                setNextState(STATE_MAIN);
                MUSIC_CACHE->playSound("sounds/menu_back.wav");
			}
			else if (pauseItems[pauseSelection] == "MUSIC VOL:")
			{
				if (input->isLeftClick())
				{
					float factor = (float)(lastPos.x - (GFX::getXResolution() - PAUSE_VOLUME_SLIDER_SIZE - PAUSE_MENU_OFFSET_X)) / (float)PAUSE_VOLUME_SLIDER_SIZE;
					if (factor >= 0.0f && factor <= 1.0f)
						MUSIC_CACHE->setMusicVolume((float)MUSIC_CACHE->getMaxVolume() * factor);
				}
			}
			else if (pauseItems[pauseSelection] == "SOUND VOL:")
			{
				if (input->isLeftClick())
				{
					float factor = (float)(lastPos.x - (GFX::getXResolution() - PAUSE_VOLUME_SLIDER_SIZE - PAUSE_MENU_OFFSET_X)) / (float)PAUSE_VOLUME_SLIDER_SIZE;
					if (factor >= 0.0f && factor <= 1.0f)
						MUSIC_CACHE->setSoundVolume((float)MUSIC_CACHE->getMaxVolume() * factor);
				}
			}
			#ifdef _MUSIC
			else if (pauseItems[pauseSelection] == "MUSIC FILE:")
			{
				showMusicList = true;
				input->resetKeys();
			}
			#endif
        }
        if (CANCEL_KEY)
		{
            if (pauseItems[pauseSelection] == "MUSIC VOL:")
				MUSIC_CACHE->setMusicVolume(0);
            else if (pauseItems[pauseSelection] == "SOUND VOL:")
				MUSIC_CACHE->setSoundVolume(0);
		#ifdef _MUSIC
			else if (pauseItems[pauseSelection] == "MUSIC FILE:")
				MUSIC_CACHE->stopMusic();
		#endif
		}

        if (input->isStart() || input->isRightClick())
            pauseToggle();
    }
}

void Level::pauseUpdate()
{
    if (trialEnd)
    {
        if (timeDisplay < timeCounter)
            timeDisplay += min(timeCounter - timeDisplay, timeCounter / END_TIMER_ANIMATION_STEP);
        else
        {
            if (newRecord)
                timeTrialText.setColour(ORANGE);
        }
    }
    #ifdef _MUSIC
    if (showMusicList)
	{
		musicLister.update();
	}
	#endif
}

void Level::pauseScreen()
{
    SDL_BlitSurface(pauseSurf,NULL,GFX::getVideoSurface(),NULL);

#ifdef _MUSIC
	if (showMusicList)
	{
		musicLister.render();
		return;
	}
#endif

    int offset = 0;
    if (trialEnd)
    {
        offset = TIME_TRIAL_MENU_OFFSET_Y;
    }
    else
    {
        offset = PAUSE_MENU_OFFSET_Y;
    }
    #ifdef _MUSIC
    int pos = (GFX::getYResolution() - PAUSE_MENU_SPACING * (pauseItems.size())) / 2 - PAUSE_MENU_OFFSET_X * 2 + offset;
    #else
    int pos = (GFX::getYResolution() - PAUSE_MENU_SPACING * (pauseItems.size()-1)) / 2 + offset;
    #endif

    // render text and selection
	Vector2di mousePos = input->getMouse();
	if (mousePos != lastPos)
		lastPos = mousePos;
	else
		mousePos = Vector2di(-1,-1);
    for (int I = 0; I < pauseItems.size(); ++I)
    {
    	// NOTE: do mouse selection handling here, so I don't have to copy code
		if (mousePos.y >= pos && mousePos.y <= pos + NAME_RECT_HEIGHT)
		{
			pauseSelection = I;
		}

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

		if (pauseItems[I] == "MUSIC VOL:")
		{
			// render volume sliders
			float factor = (float)MUSIC_CACHE->getMusicVolume() / (float)MUSIC_CACHE->getMaxVolume();
			nameRect.setDimensions((float)PAUSE_VOLUME_SLIDER_SIZE * factor,NAME_RECT_HEIGHT);
			nameRect.setPosition((int)GFX::getXResolution() - PAUSE_VOLUME_SLIDER_SIZE - PAUSE_MENU_OFFSET_X,pos);
			if (pauseSelection == I)
				nameRect.setColour(BLACK);
			else
				nameRect.setColour(WHITE);
			nameRect.render();
			nameRect.setDimensions(GFX::getXResolution(),NAME_RECT_HEIGHT);
		}
		else if (pauseItems[I] == "SOUND VOL:")
		{
			float factor = (float)MUSIC_CACHE->getSoundVolume() / (float)MUSIC_CACHE->getMaxVolume();
			nameRect.setDimensions(PAUSE_VOLUME_SLIDER_SIZE * factor,NAME_RECT_HEIGHT);
			nameRect.setPosition((int)GFX::getXResolution() - PAUSE_VOLUME_SLIDER_SIZE - PAUSE_MENU_OFFSET_X,pos);
			if (pauseSelection == I)
				nameRect.setColour(BLACK);
			else
				nameRect.setColour(WHITE);
			nameRect.render();
			nameRect.setDimensions(GFX::getXResolution(),NAME_RECT_HEIGHT);
			pos += PAUSE_MENU_SPACING_EXTRA; // extra offset
		}
	#ifdef _MUSIC
		else if (pauseItems[I] == "MUSIC FILE:")
		{
			nameText.setPosition( nameText.getPosition().x + PAUSE_MENU_OFFSET_X, nameText.getPosition().y );
			nameText.print(MUSIC_CACHE->getPlaying());
			pos += PAUSE_MENU_SPACING_EXTRA; // extra offset
		}
	#endif

        pos += NAME_RECT_HEIGHT + PAUSE_MENU_SPACING;
    }

    // in time trial mode also render text
    if (ENGINE->timeTrial)
    {
        timeTrialText.setAlignment(LEFT_JUSTIFIED);
        timeTrialText.setPosition(PAUSE_MENU_OFFSET_X,TIME_TRIAL_OFFSET_Y);
        timeTrialText.print("TIME: ");
        timeTrialText.setAlignment(RIGHT_JUSTIFIED);
        if (trialEnd)
            timeTrialText.print(ticksToTimeString(timeDisplay));
        else
            timeTrialText.print(ticksToTimeString(timeCounter));
        if (not trialEnd || timeDisplay == timeCounter)
        {
            timeTrialText.setAlignment(LEFT_JUSTIFIED);
            timeTrialText.setPosition(PAUSE_MENU_OFFSET_X,TIME_TRIAL_OFFSET_Y + TIME_TRIAL_SPACING_Y + NAME_TEXT_SIZE * 1.5);
            timeTrialText.print("BEST: ");
            timeTrialText.setAlignment(RIGHT_JUSTIFIED);
            timeTrialText.print(ticksToTimeString(SAVEGAME->getLevelStats(levelFileName).time));
        }
    }

#ifdef PENJIN_CALC_FPS
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
    for (vector<ControlUnit*>::const_iterator unit = players.begin(); unit != players.end(); ++unit)
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
    for (vector<ControlUnit*>::iterator unit = players.begin(); unit != players.end(); ++unit)
    {
        if (not (*unit)->takesControl && not (*unit)->toBeRemoved)
        {
            valid = true;
            break;
        }
    }

    if (valid)
    {
        for (vector<ControlUnit*>::iterator unit = players.begin(); unit != players.end(); ++unit)
        {
            (*unit)->takesControl = not (*unit)->takesControl;
            if ((*unit)->takesControl)
                (*unit)->setSpriteState("wave",true);
			else
			{
				(*unit)->velocity.x = 0;
				(*unit)->acceleration[0].x = 0;
				(*unit)->acceleration[1].x = 0;
			}
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
        Savegame::LevelStats stats = {timeCounter};
        newRecord = SAVEGAME->setLevelStats(levelFileName,stats);
        if (ENGINE->timeTrial)
        {
            trialEnd = true;
            pauseToggle();
        }
        else
        {
            eventTimer.setCallback(this,Level::winCallback);
            eventTimer.start(1000);
            EFFECTS->fadeOut(1000);
        }
    }
}

void Level::getUnitsByID(const vector<string>& IDs, vector<BaseUnit*>& unitVector) const
{
	for (vector<BaseUnit*>::const_iterator I = units.begin(); I != units.end(); ++I)
	{
		for (vector<string>::const_iterator str = IDs.begin(); str != IDs.end(); ++str)
		{
			if ((*I)->id == (*str))
				unitVector.push_back(*I);
		}
	}
	for (vector<ControlUnit*>::const_iterator I = players.begin(); I != players.end(); ++I)
	{
		for (vector<string>::const_iterator str = IDs.begin(); str != IDs.end(); ++str)
		{
			if ((*I)->id == (*str))
				unitVector.push_back(*I);
		}
	}
}

void Level::addParticle(const BaseUnit* const caller, const Colour& col, const Vector2df& pos, const Vector2df& vel, CRint lifeTime)
{
    PixelParticle* temp = new PixelParticle(this,lifeTime);
    // copy the collision colours from the calling unit to mimic behaviour
    temp->collisionColours.insert(caller->collisionColours.begin(),caller->collisionColours.end());
    temp->position = pos;
    temp->velocity = vel;
    temp->col = col;
    effects.push_back(temp);
}

#ifdef _DEBUG
string Level::debugInfo()
{
    string result = "";
    result += "Players alive: " + StringUtility::intToString(players.size()) + "\n";
    result += "Units alive: " + StringUtility::intToString(units.size()) + "\n";
    result += "Particles: " + StringUtility::intToString(effects.size()) + "\n";
    result += "Camera: " + StringUtility::vecToString(drawOffset) + " | " +
        StringUtility::vecToString(cam.getDest()) + " | " +
        StringUtility::vecToString(cam.getSpeed()) + "\n";
    result += "Flags: " + StringUtility::intToString(flags.flags) + "\n";
    if (input)
		result += "Mouse: " + StringUtility::vecToString(input->getMouse()) + "\n";
    result += "---\n";
    return result;
}
#endif

/// ---protected---

void Level::clearUnitFromCollision(SDL_Surface* const surface, BaseUnit* const unit)
{
	if ( unit->flags.hasFlag(BaseUnit::ufNoRender) )
		return;

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
    int tempW = min(surface->w - unitRect.x,width - max((int)-posX,0));
    int tempH = min(surface->h - unitRect.y,height - max((int)-posY,0));
    if (tempW <= 0 || tempH <= 0)
        return;
    unitRect.w = tempW;
    unitRect.h = tempH;

    SDL_BlitSurface(levelImage,&unitRect,surface,&unitRect);
}

void Level::renderUnit(SDL_Surface* const surface, BaseUnit* const unit, const Vector2df& offset)
{
	if ( unit->flags.hasFlag(BaseUnit::ufNoRender) )
		return;

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

bool Level::adjustPosition( BaseUnit* const unit, const bool adjustCamera )
{
    // 1 = out of right/bottom bounds, -1 = out of left/top bounds
    int boundsX = (unit->position.x > getWidth()) - (unit->position.x + unit->getWidth() < 0);
    int boundsY = (unit->position.y > getHeight()) - (unit->position.y + unit->getHeight() < 0);
    bool changed = false;

    if (boundsX != 0 && flags.hasFlag(lfRepeatX))
    {
        unit->position.x -= getWidth() * boundsX;
        if ( adjustCamera && getWidth() > GFX::getXResolution() )
			drawOffset.x -= getWidth() * boundsX;
        changed = true;
    }
    if (boundsY != 0 && flags.hasFlag(lfRepeatY))
    {
        unit->position.y -= getHeight() * boundsY;
        if ( adjustCamera && getHeight() > GFX::getYResolution() )
			drawOffset.y -= getHeight() * boundsY;
        changed = true;
    }
    if ((boundsX + boundsY != 0) && not changed && !unit->flags.hasFlag(BaseUnit::ufDisregardBoundaries))
    {
        unit->position = unit->startingPosition;
        changed = true;
    }
    return changed;
}

string Level::ticksToTimeString(CRint ticks)
{
    if (ticks < 0)
        return "NONE";

    int time = (float)ticks / (float)FRAME_RATE * 100.0f; // convert to centi-seconds
    string cs = "00" + StringUtility::intToString(time % 100);
    string s = "00" + StringUtility::intToString((time / 100) % 60);
    string m = "";
    if (time / 6000 > 0)
    {
        m = StringUtility::intToString(time / 6000) + "'";
    }
    return (m + s.substr(s.length()-2,2) + "''" + cs.substr(cs.length()-2,2));
}

void Level::loseCallback(void* data)
{
    ((Level*)data)->eventTimer.setCallback(data,Level::lose2Callback);
    ((Level*)data)->eventTimer.start(125);
    EFFECTS->fadeOut(125,WHITE);
}

void Level::lose2Callback(void* data)
{
    ((Level*)data)->reset();
    EFFECTS->fadeIn(125,WHITE);
}

void Level::winCallback(void* data)
{
    Level* self = (Level*)data;
    self->setNextState(STATE_NEXT);
}

bool Level::playersVisible() const
{
    SDL_Rect screen = {drawOffset.x,drawOffset.y,GFX::getXResolution(),GFX::getYResolution()};
    for (vector<ControlUnit*>::const_iterator I = players.begin(); I != players.end(); ++I)
    {
        if (not (*I)->position.inRect(screen))
            return false;
    }
    return true;
}

#ifdef _MUSIC
void Level::saveMusicToFile(CRstring musicFile)
{
	string line;
	fstream file(levelFileName.c_str(), fstream::in);
	vector<string> lines;

    if (file.fail())
    {
        errorString = "Failed to open file to write music info!";
        return;
    }

    while (file.good())
	{
        getline(file,line);
        lines.push_back(line);
	}

	for (vector<string>::iterator I = lines.begin(); I != lines.end(); ++I)
	{
		if (StringUtility::lower((*I).substr(0,5)) == "music")
		{
			if (musicFile[0] == 0)
				(*I) = "music=none";
			else
				(*I) = "music=" + musicFile;
			break;
		}
	}
	file.close();
	file.open(levelFileName.c_str(), fstream::out | fstream::trunc);
	for (vector<string>::const_iterator I = lines.begin(); I != lines.end(); ++I)
	{
		file << *I << endl;
	}
}
#endif
