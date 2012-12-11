#ifndef TITLEMENU_H
#define TITLEMENU_H

#include "BaseState.h"
#include "PenjinTypes.h"
#include "AnimatedSprite.h"

#if defined(_DEBUG) && !defined(PENJIN_CALC_FPS)
#define PENJIN_CALC_FPS
#endif

#ifdef PENJIN_CALC_FPS
#include "Text.h"
#endif

class TitleMenu : public BaseState
{
    public:
        TitleMenu();
        virtual ~TitleMenu();

        virtual void init();
        virtual void userInput();
        virtual void update();
        virtual void render();
    protected:

    private:
        void setSelection(CRbool immediate);
        void incSelection();
        void decSelection();
        void doSelection();
        void inverse(SDL_Surface* const surf, const SDL_Rect& rect);

        AnimatedSprite bg;
        vector<SDL_Surface*> inverseBG;
        AnimatedSprite marker;
        int selection;
        SDL_Rect invertRegion;
        Vector2di lastPos;

        #ifdef PENJIN_CALC_FPS
        Text fpsDisplay;
        #endif
};


#endif // TITLEMENU_H

