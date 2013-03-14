#ifndef MYGAME_H
#define MYGAME_H

#include "Engine.h"
#include "PenjinTypes.h"
#include "Text.h"
#include "Chapter.h"

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

        Level* getCurrentLevel() const;
        void playSingleLevel(CRstring filename, CRuint returnState);
        void playChapter(CRstring filename, CRint startLevel=-1);

		static string ticksToTimeString(CRint ticks);

		void startChapterTrial();

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
    private:
        // creates a state from a defined set, gets called by MyGame::stateManangement()
        BaseState* createState(CRuint stateID, CRstring parameter="");

        SDL_Surface* icon;

};


#endif	//	MYGAME_H
