#ifndef STATELEVELSELECT_H
#define STATELEVELSELECT_H

#include "BaseState.h"

#include <vector>
#include <SDL/SDL_mutex.h>
#include <SDL/SDL_thread.h>

#include "AnimatedSprite.h"
#include "Rectangle.h"
#include "Vector2di.h"
#include "FileLister.h"
#include "Text.h"

struct PreviewData;

class StateLevelSelect : public BaseState
{
    public:
        StateLevelSelect();
        virtual ~StateLevelSelect();

        virtual void init();
        virtual void clear();
        virtual void clearLevelListing();
        virtual void clearChapterListing();

        virtual void userInput();
        virtual void update();
        virtual void render();

        virtual void renderPreviews(const vector<PreviewData>& data, SDL_Surface* const target, CRint addOffset);

        virtual void setLevelDirectory(CRstring dir);
        virtual void setChapterDirectory(CRstring dir);

        virtual void exploreChapter(CRstring filename);

        enum LevelSelectState
        {
            lsChapter,
            lsIntermediate,
            lsLevel
        };
        LevelSelectState state; // 0 - chapter select, 1 - play/explore, 2 - level select

    protected:
        void switchState();

        void checkSelection(const vector<PreviewData>& data, Vector2di& sel);

        static int loadLevelPreviews(void* data);
        static int loadChapterPreviews(void* data);

        AnimatedSprite bg;
        AnimatedSprite error;
        AnimatedSprite loading;
        AnimatedSprite arrows;
        Rectangle cursor;
        SDL_Surface* previewDraw;
        #ifdef _DEBUG
        Text fpsDisplay;
        #endif
        Text imageText; // fallback text when encountering chapter without image

        // map<filename,pair<surface, hasbeenloaded>
        // if hasBeenLoaded is true but surface is NULL an error occurred while loading
        vector<PreviewData> levelPreviews;
        vector<PreviewData> chapterPreviews;

        // mutex to prevent sharing violations between main and loading thread
        SDL_mutex* levelLock;
        SDL_Thread* levelThread;
        bool abortLevelLoading; // if true level preview generation will exit

        SDL_mutex* chapterLock;
        SDL_Thread* chapterThread;
        bool abortChapterLoading; // if true chapter preview generation will exit

        SDL_mutex* coutLock;

        Vector2di size;
        Vector2di spacing;
        Vector2di selection;
        int gridOffset; // the topmost column to render (=offset)
        int lastDraw; // indicates the last drawn preview image to speed up rendering
        bool firstDraw; // if true the all preview images will be rendered (also set on scrolling)

        FileLister levelLister;
        FileLister dirLister;
        string chapterPath;
    private:

};

#endif // STATELEVELSELECT_H



