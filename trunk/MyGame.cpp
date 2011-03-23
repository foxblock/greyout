#include "MyGame.h"

#include "userStateList.h"
#include "SurfaceCache.h"
#include "LevelLoader.h"
#include "Savegame.h"

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
}

MyGame::~MyGame()
{
    SURFACE_CACHE->clear();
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
    #ifdef _DEBUG_COL
    GFX::setResolution(800,960);
    #else
    GFX::setResolution(800,480);
    #endif
    GFX::resetScreen();
    #ifdef _DEBUG
    GFX::showCursor(true);
    #endif
    setFrameRate(30);
    SAVEGAME->setFile(SAVE_FILE);
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
    }
    else // first normal call of the game
    {
        next = STATE_TITLE;
    }

    if (next == STATE_NEXT)
    {
        if (currentChapter) // we are playing a chapter
        {
            stateParameter = currentChapter->getNextLevelAndSave(stateParameter);

            if (stateParameter == "") // no next level found
            {
                if (currentChapter->errorString[0] == 0) // no error -> we reached the end
                {
                    next = STATE_TITLE;
                    delete currentChapter;
                    currentChapter = NULL;
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

    currentState = next;
    state = createState(next,stateParameter);
}

PENJIN_ERRORS MyGame::argHandler(int argc, char **argv)
{
    for (int I = 0; I < argc; ++I)
    {
        cout << argv[I] << " ";
    }
    cout << endl;

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

Level* MyGame::getCurrentLevel() const
{
    return (Level*)state;
}

void MyGame::playSingleLevel(CRstring filename, CRuint returnState)
{
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
        if (startLevel <= 0 || startLevel >= currentChapter->levels.size()) // no startLevel -> load progress
        {
            stateParameter = currentChapter->getLevelFilename(currentChapter->getProgress()-1);
        }
        else
            stateParameter = currentChapter->getLevelFilename(startLevel-1);

        state->setNextState(STATE_NEXT);
    }
}

/// ---private---

BaseState* MyGame::createState(CRuint stateID,CRstring parameter)
{
    BaseState* nextState = NULL;
    switch(stateID) // normal state check
    {
    case STATE_ERROR:
#ifdef _DEBUG
        cout << "Error state" << endl;
#endif
        nextState = new StateError;
        ((StateError*)nextState)->errorString = parameter;
        ((StateError*)nextState)->returnState = STATE_MAIN;
        break;
    case STATE_TITLE: // Title: logos, intro
#ifdef _DEBUG
        cout << "Title State" << endl;
#endif
        nextState = new StateTitle;
        break;
    case STATE_MAIN: // Main menu
#ifdef _DEBUG
        cout << "Title Menu State" << endl;
#endif
        nextState = new TitleMenu;
        break;
    case STATE_LEVEL: // Any playable level
#ifdef _DEBUG
        cout << "Level State" << endl;
#endif
        if (currentChapter)
            nextState = LEVEL_LOADER->loadLevelFromFile(parameter,currentChapter->path);
        else
            nextState = LEVEL_LOADER->loadLevelFromFile(parameter);
        if (not nextState)
        {
            cout << LEVEL_LOADER->errorString << endl;
            string e = "Error: " + LEVEL_LOADER->errorString;
            return createState(STATE_ERROR,e);
        }
        break;
    case STATE_BENCHMARK:
#ifdef _DEBUG
        cout << "Benchmark started" << endl;
#endif
        nextState = LEVEL_LOADER->loadLevelFromFile("levels/benchmark.txt");
        if (not nextState)
        {
            string e = "Error: " + LEVEL_LOADER->errorString + "\n" +
                        "Don't play around with the benchmark level file! :<";
            return createState(STATE_ERROR,e);
        }
        break;
    case STATE_LEVELSELECT:
#ifdef _DEBUG
        cout << "Level selection screen" << endl;
#endif
        nextState = new StateLevelSelect;
        break;
    default:
        cout << ErrorHandler().getErrorString(PENJIN_UNDEFINED_STATE);
        exit(PENJIN_UNDEFINED_STATE);
    }
    return nextState;
}
