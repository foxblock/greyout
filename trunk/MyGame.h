#ifndef MYGAME_H
#define MYGAME_H

#include "Engine.h"
#include "PenjinTypes.h"
#include "Text.h"
#include "Chapter.h"

#define ENGINE MyGame::getMyGame()

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
        void playChapter(CRstring filename, CRint startLevel=0);

        string stateParameter; // this might be the current level filename or an error string
        Chapter* currentChapter;
        uint currentState;
        uint returnState;
    private:
        // creates a state from a defined set, gets called by MyGame::stateManangement()
        BaseState* createState(CRuint stateID, CRstring parameter="");

};


#endif	//	MYGAME_H
