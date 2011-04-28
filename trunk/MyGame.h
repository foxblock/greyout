#ifndef MYGAME_H
#define MYGAME_H

#include "Engine.h"
#include "PenjinTypes.h"
#include "Text.h"
#include "Chapter.h"

#define ENGINE MyGame::getMyGame()

#define DEFAULT_CHAPTER "chapters/default/info.txt"
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

        #ifdef _DEBUG
        virtual float getFPS() const {return frameCount;}
        #endif

        Level* getCurrentLevel() const;
        void playSingleLevel(CRstring filename, CRuint returnState);
        void playChapter(CRstring filename, CRint startLevel=-1);

        string stateParameter; // this might be the current level filename or an error string
        Chapter* currentChapter;
        uint currentState;
        uint returnState;
        bool timeTrial; // whether to start a level in time trial mode or normal mode
        uint restartCounter;
    private:
        // creates a state from a defined set, gets called by MyGame::stateManangement()
        BaseState* createState(CRuint stateID, CRstring parameter="");

};


#endif	//	MYGAME_H
