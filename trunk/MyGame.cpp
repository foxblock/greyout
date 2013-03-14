#include "MyGame.h"

#include "userStateList.h"
#include "GreySurfaceCache.h"
#include "MusicCache.h"
#include "LevelLoader.h"
#include "Savegame.h"
#include "Dialogue.h"

#define SAVE_FILE "save.me"

MyGame* MyGame::m_MyGame = NULL;

MyGame::MyGame()
{
    m_MyGame = this;

    Penjin::setApplicationName("Greyout");
    stateParameter = "";
    currentState = 0;
    currentChapter = NULL;
    returnState = STATE_MAIN;
    timeTrial = false;
    chapterTrial = false;
    chapterTrialPaused = false;
    chapterTrialTimer = 0;
    restartCounter = 0;
    icon = NULL;
}

MyGame::~MyGame()
{
    SAVEGAME->autoSave = false;
    SAVEGAME->writeData("restarts",StringUtility::intToString(restartCounter));
    SAVEGAME->writeData("musicvolume",StringUtility::intToString(MUSIC_CACHE->getMusicVolume()),true);
    SAVEGAME->writeData("soundvolume",StringUtility::intToString(MUSIC_CACHE->getSoundVolume()),true);
    SAVEGAME->writeData("activechapter",activeChapter,true);
    SAVEGAME->save();
    SURFACE_CACHE->clear();
    MUSIC_CACHE->clear();
    SDL_FreeSurface(icon);
}

MyGame *MyGame::getMyGame()
{
    return m_MyGame;
}

PENJIN_ERRORS MyGame::init()
{
    //Set up first level password and filename
    setInitialState(STATE_TITLE);
    gameTimer->start();
    icon = IMG_Load("images/general/icon_win.png");
    SDL_WM_SetIcon(icon,NULL);
    #ifdef _MEOW
    GFX::setResolution(320,240);
    #elif defined(PLATFORM_PANDORA)
    GFX::setResolution(800,480);
    #else
    GFX::setResolution(800,480);
    #endif
    GFX::resetScreen();
    setFrameRate(FRAME_RATE);
    SAVEGAME->setFile(SAVE_FILE);
    if (SAVEGAME->hasData("musicvolume"))
        MUSIC_CACHE->setMusicVolume(StringUtility::stringToInt(SAVEGAME->getData("musicvolume")));
    if (SAVEGAME->hasData("soundvolume"))
        MUSIC_CACHE->setSoundVolume(StringUtility::stringToInt(SAVEGAME->getData("soundvolume")));
    restartCounter = StringUtility::stringToInt(SAVEGAME->getData("restarts"));
    activeChapter = SAVEGAME->getData("activechapter");
    return PENJIN_OK;
}

void MyGame::stateManagement()
{
    uint next;
    if(state) // normal state swap
    {
        next = state->getNextState();
        delete state;
        state = NULL;
        SURFACE_CACHE->clear(); // clear all images loaded by previous state
        MUSIC_CACHE->clearMusic(false); // clear all unused music
    }
    else // first normal call of the game
    {
        next = STATE_TITLE;
    }

    if (next == STATE_NEXT)
    {
        if (currentChapter) // we are playing a chapter
        {
            // TODO: Find a clean way to save progress when completing a level
            // in time trial mode and then restarting it
            stateParameter = currentChapter->getNextLevelAndSave(stateParameter);

            if (stateParameter == "") // no next level found
            {
                if (currentChapter->errorString[0] == 0) // no error -> we reached the end
                {
                	if (chapterTrial)
					{
						next = STATE_CHAPTERTRIAL;
						chapterTrial = false;
					}
					else
					{
						next = STATE_TITLE;
						delete currentChapter;
						currentChapter = NULL;
					}
                }
                else // error -> show it to the world
                {
                    next = STATE_ERROR;
                    state = createState(next,currentChapter->errorString);
                    delete currentChapter;
                    currentChapter = NULL;
                    return;
                }
            }
            else // next level found, let's play it
            {
                next = STATE_LEVEL;
            }
        }
        else // single level -> return to previously set return state
        {
            next = returnState;
            returnState = STATE_MAIN; // reset return state
        }
    }
    else
		chapterTrial = false; // reset trial mode when going to anything but a level state

    currentState = next;
    state = createState(next,stateParameter);
}

PENJIN_ERRORS MyGame::argHandler(int argc, char **argv)
{
    for (int I = 0; I < argc; ++I)
    {
       printf("%s ",argv[I]);
    }
    printf("\n");

    if(argc <= 1)
        return	PENJIN_NO_COMMANDLINE;
    else
    {
        //	Do further CMD processing
        for (int arg = 1; arg < argc; ++arg)
        {
            //	Check for commandline escape chars
            if(argv[arg][0] == '-' || argv[arg][0] == '/')
            {
                //	Look for arguments
                switch(argv[arg][1])
                {
                    //	Set Fullscreen
                case 'f':
                case 'F':
                {
                    GFX::setFullscreen(true);
                    break;
                }
#ifdef PLATFORM_GP2X
                case 'M':
                case 'm':
                {
                    loadMenu = true;
                    break;
                }
#endif
                default:
                {
                    return PENJIN_INVALID_COMMANDLINE;
                }
                }
            }
        }
    }
    return	PENJIN_OK;
}

bool MyGame::stateLoop()
{
	if (chapterTrial && not chapterTrialPaused)
		++chapterTrialTimer;
	return Engine::stateLoop();
}

Level* MyGame::getCurrentLevel() const
{
    return (Level*)state;
}

void MyGame::playSingleLevel(CRstring filename, CRuint returnState)
{
    delete currentChapter;
    currentChapter = NULL;

    stateParameter = filename;
    this->returnState = returnState;
    state->setNextState(STATE_LEVEL);
}

void MyGame::playChapter(CRstring filename, CRint startLevel)
{
    delete currentChapter;
    currentChapter = new Chapter;

    if (not currentChapter->loadFromFile(filename)) // on error
    {
        stateParameter = currentChapter->errorString;
        delete currentChapter;
        currentChapter = NULL;
        state->setNextState(STATE_ERROR);
    }
    else
    {
        // set stateParameter to level before the one we want to play
        if (startLevel < 0 || startLevel >= currentChapter->levels.size()) // no startLevel -> load progress
        {
            stateParameter = currentChapter->getLevelFilename(currentChapter->getProgress()-1);
        }
        else
            stateParameter = currentChapter->getLevelFilename(startLevel-1);
        if (currentChapter->dialogueFile[0] != 0)
            DIALOGUE->loadFromFile(currentChapter->path + currentChapter->dialogueFile);

        activeChapter = filename;

        state->setNextState(STATE_NEXT);
    }
}

string MyGame::ticksToTimeString(CRint ticks)
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

void MyGame::startChapterTrial()
{
	chapterTrial = true;
	chapterTrialPaused = false;
	chapterTrialTimer = 0;
}

/// ---private---

BaseState* MyGame::createState(CRuint stateID,CRstring parameter)
{
    BaseState* nextState = NULL;
    switch(stateID) // normal state check
    {
    case STATE_ERROR:
#ifdef _DEBUG
        printf("Error state\n");
#endif
        nextState = new StateError;
        ((StateError*)nextState)->errorString = parameter;
        ((StateError*)nextState)->returnState = STATE_MAIN;
        break;
    case STATE_TITLE: // Title: logos, intro
#ifdef _DEBUG
        printf("Title State\n");
#endif
        nextState = new StateTitle;
        break;
    case STATE_MAIN: // Main menu
#ifdef _DEBUG
        printf("Title Menu State\n");
#endif
        nextState = new TitleMenu;
        break;
    case STATE_LEVEL: // Any playable level
#ifdef _DEBUG
        printf("Level State\n");
#endif
        if (currentChapter)
            nextState = LEVEL_LOADER->loadLevelFromFile(parameter,currentChapter->path);
        else
            nextState = LEVEL_LOADER->loadLevelFromFile(parameter);
        if (not nextState)
        {
            printf("ERROR: %s\n",LEVEL_LOADER->errorString.c_str());
            string e = "ERROR: " + LEVEL_LOADER->errorString;
            return createState(STATE_ERROR,e);
        }
        break;
    case STATE_BENCHMARK:
#ifdef _DEBUG
        printf("Benchmark started\n");
#endif
        nextState = LEVEL_LOADER->loadLevelFromFile(BENCHMARK_LEVEL);
        if (not nextState)
        {
            string e = LEVEL_LOADER->errorString + "\n" +
                        "Don't play around with the benchmark level file! :<";
            return createState(STATE_ERROR,e);
        }
        break;
    case STATE_LEVELSELECT:
#ifdef _DEBUG
        printf("Level selection screen\n");
#endif
        nextState = new StateLevelSelect;
        break;
	case STATE_CHAPTERTRIAL:
#ifdef _DEBUG
        printf("Chapter time trial result screen\n");
#endif
		nextState = new StateChapterTrial;
		break;
    default:
        printf("%s\n",ErrorHandler().getErrorString(PENJIN_UNDEFINED_STATE).c_str());
        return createState(STATE_ERROR,"Undefined state in MyGame::createState()!");
    }
    return nextState;
}
