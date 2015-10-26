/*
	Greyout - a colourful platformer about love

	Greyout is Copyright (c)2011-2014 Janek Schäfer

	This file is part of Greyout.

	Greyout is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Greyout is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Please direct any feedback, questions or comments to
	Janek Schäfer (foxblock), foxblock_at_gmail_dot_com
*/

#include "MyGame.h"

#include "userStateList.h"
#include "GreySurfaceCache.h"
#include "MusicCache.h"
#include "LevelLoader.h"
#include "Savegame.h"
#include "Dialogue.h"

#include "StringUtility.h"
#include "IMG_savepng.h"
#include <time.h>
#ifndef _WIN32
#include <sys/stat.h>
#endif // _WIN32
#ifndef EEXIST // Hard-coded fallback for Pandora compiler
#define EEXIST 17
#endif // EEXIST

#include "version.h"

#define SAVE_FILE "save.me"
#define FPS_FONT_SIZE 24

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
	settings = NULL;
	videoCaptureRunning = false;
	videoFile = NULL;
	videoCounter = 0;
	videoTempCounter = 0;
	#ifdef _DEBUG
	frameAdvance = false;
	#endif // _DEBUG
}

MyGame::~MyGame()
{
	SAVEGAME->autoSave = false;
	delete settings; // this saves
	SAVEGAME->writeData("restarts",StringUtility::intToString(restartCounter));
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
	printf("GREYOUT version %s built %s.%s.%s\n",AutoVersion::FULLVERSION_STRING,
			AutoVersion::DATE,AutoVersion::MONTH,AutoVersion::YEAR);
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
	#ifdef _DEBUG
	SAVEGAME->setEncryption(false);
	#endif // _DEBUG
	settings = new Settings();
	restartCounter = StringUtility::stringToInt(SAVEGAME->getData("restarts"));
	activeChapter = SAVEGAME->getData("activechapter");
	#ifdef _WIN32
	int result = mkdir("screenshots");
	#else
	int result = mkdir("screenshots", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	#endif // _WIN32
	if (result == 0)
		printf("Screenshots folder created successfully.\n");
	#ifdef _WIN32
	else if (*_errno() == EEXIST)
	#else
	else if (result == EEXIST)
	#endif // _WIN32
		printf("Screenshots folder already exists.\n");
	else
		printf("Error creating screenshots folder, error code: %i\n",
	#ifdef _WIN32
		*_errno());
	#else
		result);
	#endif

#ifdef PENJIN_CALC_FPS
	fpsDisplay = new Text();
	fpsDisplay->loadFont(DEBUG_FONT,FPS_FONT_SIZE);
	fpsDisplay->setColour(GREEN);
	fpsDisplay->setPosition(GFX::getXResolution(),0);
	fpsDisplay->setAlignment(RIGHT_JUSTIFIED);
#endif

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
					// Finish default chapter -> unlock speedrun mode!
					if (currentChapter->filename == DEFAULT_CHAPTER)
						SAVEGAME->writeData("speedrun", "true", true);
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
	if ( next != STATE_LEVEL )
	{
		chapterTrial = false; // reset trial mode when going to anything but a level state
		timeTrial = false;
	}

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
				//  Load CMF - Custom controls
				case 'c':
				case 'C':
				{
					customControlMap = argv[arg+1];
					break;
				}
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
#ifdef _DEBUG
	while ( frameAdvance && !input->isSelect() )
	{
		input->update();
		if ( input->isKey("f") )
		{
			frameAdvance = false;
			input->resetKeys();
		}
		if (input->isQuit())
		{
			state->nullifyState();
			return false;
		}
	}
	if ( frameAdvance )
		input->resetSelect();
#endif

	if (chapterTrial && not chapterTrialPaused)
		++chapterTrialTimer;

	//  Check state for exit condition
	if(state->getNullifyState())
	{
		state->nullifyState();
		return false;  // End program execution
	}
	else if (state->getNeedInit() == false)
	{
		//  Update physics
		state->unlimitedUpdate();
		if(state->getNeedInit())
			return true;

		// the following will always last at least the time of one frame
		gameTimer->start();
		input->update();
		#ifdef _DEBUG
		if (input->isKey("f") && !input->isPollingKeyboard())
		{
			frameAdvance = true;
		}
		#endif // _DEBUG
		if (input->isKey("F5") || input->isKey("PRINT"))
		{
			int result = takeScreenshot(settings->getScreenshotCompression());
			if (result != 0)
				printf("Error saving screenshot: %s\n", SDL_GetError());
			input->resetKeys();
		}
		else if (input->isKey("F8"))
		{
			int result = startVideoCapture();
			if (result == 0)
				takeScreenshot(settings->getVideoCompression());
			input->resetKeys();
		}
		else if (input->isKey("F9"))
		{
			stopVideoCapture();
			input->resetKeys();
		}
		else if (videoCaptureRunning)
		{
			if (--videoTempCounter < 0)
			{
				int result = takeScreenshot(settings->getVideoCompression());
				videoTempCounter = settings->getVideoFrameskip();
				if (result != 0)
				{
					printf("Error saving screenshot: %s\n", SDL_GetError());
					stopVideoCapture();
				}
			}
		}

		if (input->isQuit())
		{
			state->nullifyState();
			return false;
		}
		//  Update timer and check if ticks have passed
		if(state->getIsPaused())
		{
			// check if it is only just paused and run tasks on pausing
			if(!state->getFirstPaused())
			{
				state->onPause();
				state->setFirstPaused(true);
			}
			if (settings->isActive())
			{
				settings->userInput(input);
				settings->update();
				state->pauseScreen();
				settings->render(GFX::getVideoSurface());
			}
			else
			{
				state->pauseInput();
				state->pauseUpdate();
				state->pauseScreen();
			}
		}
		else if(!state->getIsPaused() && state->getFirstPaused())
		{
			state->onResume();
			state->setFirstPaused(false);
		}
		else
		{
			if (settings->isActive())
			{
				settings->userInput(input);
				settings->update();
			}
			else
			{
				state->userInput();
				state->update();
			}
			#ifdef USE_ACHIEVEMENTS
				ACHIEVEMENTS->update();
			#endif

			if(state->getNeedInit())
				return true;
			//  Render objects
			state->render();
			if (settings->isActive())
			{
				settings->render(GFX::getVideoSurface());
			}
			#ifdef USE_ACHIEVEMENTS
				#ifdef PENJIN_SDL
					ACHIEVEMENTS->render(GFX::getVideoSurface());
				#else
					ACHIEVEMENTS->render();
				#endif
			#endif
			#ifdef _DEBUG
				#ifdef PENJIN_SDL
					if(frameCount>=20)//only update if there are a reasonable number of redundant updates
					{
						//  This code seems to slow down Linux builds majorly.
						SDL_WM_SetCaption((Penjin::getApplicationName() + " V" + AutoVersion::FULLVERSION_STRING
						+ AutoVersion::STATUS_SHORT
						+ " "
						+ StringUtility::intToString(frameCount)
						+ " DEBUG "
						+ AutoVersion::DATE + "-"
						+ AutoVersion::MONTH + "-"
						+ AutoVersion::YEAR).c_str(), NULL );
						//frameCount = 0;
					}
				#endif
			#endif
		}
		#ifndef PENJIN_ASCII
			#ifdef PENJIN_CALC_FPS
			if (settings->getDrawFps())
				fpsDisplay->print(StringUtility::intToString(frameCount));
			if (settings->getWriteFps())
				printf("%i\n",frameCount);
			#endif
			GFX::forceBlit();
		#endif

		#ifdef PENJIN_CALC_FPS
			frameCount = calcFPS();
		#endif
		// if done in time, wait for the rest of the frame
		#ifdef _DEBUG
		if (!frameAdvance)
		#endif // _DEBUG
		limitFPS(gameTimer->getScaler() - gameTimer->getTicks());
		return true;   // Continue program execution
	}
	else
	{
		// check and change states
		getVariables();
		stateManagement();
		setVariables();

		// Initialise the changed state
		state->init();
		state->setNeedInit(false);    // Set that we have performed the init
		return true;                  // Continue program execution
	}
	//  Should never reach here
	return false;
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

int MyGame::takeScreenshot(int compression)
{
	if (videoCaptureRunning)
	{
		char file[256];
		sprintf(file, videoFile, videoCounter++);
		return takeScreenshot(file, compression);
	}
	else
	{
		time_t timeval;
		struct tm *timestruct;
		time(&timeval);
		timestruct = localtime(&timeval);
		char file[256];
		sprintf(file, "screenshots/shot_%02i%02i%02i_%02i%02i%02i.png",
				timestruct->tm_year + 1900, timestruct->tm_mon + 1,
				timestruct->tm_mday, timestruct->tm_hour,
				timestruct->tm_min, timestruct->tm_sec);
		printf("Saving screenshot to %s...\n", file);
		return takeScreenshot(file, compression);
	}
}

int MyGame::takeScreenshot(char *filename, int compression)
{
	return IMG_SavePNG(filename, GFX::getVideoSurface(), compression);
}

int MyGame::startVideoCapture()
{
	if (videoCaptureRunning)
	{
		printf("Video capture already running!");
		return -1;
	}
	struct tm *timestruct;
	time(&videoStart);
	timestruct = localtime(&videoStart);
	char *file = new char[256];
	sprintf(file, "screenshots/video_%02i%02i%02i_%02i%02i%02i",
			timestruct->tm_year + 1900, timestruct->tm_mon + 1,
			timestruct->tm_mday, timestruct->tm_hour,
			timestruct->tm_min, timestruct->tm_sec);
	#ifndef _WIN32
	int result = mkdir(file, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	#else
	int result = mkdir(file);
	#endif // _WIN32
	if (result == 0)
		printf("Video folder created successfully.\n");
	else
	{
		printf("Error creating video folder, error code: %i\n", result);
		return result;
	}
	printf("Starting video capture to folder %s\n", file);
	sprintf(file, "%s/shot_%s.png", file, "%05i");
	delete videoFile;
	videoFile = file;
	videoCounter = 0;
	videoTempCounter = settings->getVideoFrameskip();
	videoCaptureRunning = true;
	return 0;

}
void MyGame::stopVideoCapture()
{
	videoCaptureRunning = false;
	time_t videoEnd;
	time(&videoEnd);
	printf("Stopping video capture. recorded %i frames in %i seconds (%.2f fps).\n",
			videoCounter, videoEnd - videoStart, (float)videoCounter / (float)(videoEnd - videoStart));
}


/// ---private---

BaseState* MyGame::createState(CRuint stateID, CRstring parameter)
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
	case STATE_EDITOR:
#ifdef _DEBUG
		printf("Level Editor\n");
#endif
		nextState = new Editor();
		break;
	default:
		printf("%s\n",ErrorHandler().getErrorString(PENJIN_UNDEFINED_STATE).c_str());
		return createState(STATE_ERROR,"Undefined state in MyGame::createState()!");
	}
	return nextState;
}
