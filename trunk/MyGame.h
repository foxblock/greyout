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

#ifndef MYGAME_H
#define MYGAME_H

#include "Engine.h"
#include "PenjinTypes.h"
#include "Text.h"
#include "Chapter.h"
#include "Settings.h"

#include "gameDefines.h"

#define ENGINE MyGame::getMyGame()

#define DEFAULT_CHAPTER "chapters/newDefault/info.txt"
#define BENCHMARK_LEVEL "data/benchmark.txt"
#define PLAYGROUND_LEVEL "levels/playground.txt"

class Level;

class MyGame : public Engine
{
	private:
		static MyGame* m_MyGame;
	public:
		static MyGame* getMyGame();

		MyGame();
		virtual ~MyGame();

		virtual PENJIN_ERRORS init();
		virtual void stateManagement();
		virtual PENJIN_ERRORS argHandler(int argc, char **argv);
		virtual bool stateLoop();

		#ifdef PENJIN_CALC_FPS
		virtual float getFPS() const {return frameCount;}
		#endif

		Level* getCurrentLevel() const {return (Level*)state;}
		BaseState* getCurrentState() const {return state;}
		void playSingleLevel(CRstring filename, CRuint returnState);
		void playChapter(CRstring filename, CRint startLevel=-1);

		static string ticksToTimeString(CRint ticks);

		void startChapterTrial();

		int takeScreenshot(int compression = -1);
		int takeScreenshot(char *filename, int compression = -1);
		int startVideoCapture();
		void stopVideoCapture();
		bool videoCaptureRunning;
		char *videoFile;
		int videoCounter;
		int videoTempCounter;
		time_t videoStart;

		string stateParameter; // this might be the current level filename or an error string
		Chapter* currentChapter;
		uint currentState;
		uint returnState;
		bool timeTrial; // whether to start a level in time trial mode or normal mode
		bool chapterTrial; // full chapter time trial
		uint chapterTrialTimer;
		bool chapterTrialPaused;
		uint restartCounter;
		string activeChapter; // the chapter started when selecting "Start Game" in the main menu
		#ifdef _DEBUG
		bool frameAdvance;
		#endif

		Settings *settings;

		#ifdef PENJIN_CALC_FPS
		Text *fpsDisplay;
		#endif
	private:
		// creates a state from a defined set, gets called by MyGame::stateManangement()
		BaseState* createState(CRuint stateID, CRstring parameter="");

		SDL_Surface* icon;

};


#endif	//	MYGAME_H
