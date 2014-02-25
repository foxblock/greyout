#ifndef LEVEL_H
#define LEVEL_H

#if defined(_DEBUG) && !defined(PENJIN_CALC_FPS)
#define PENJIN_CALC_FPS
#endif

#include <list>
#include <map>

#include "PenjinTypes.h"
#include "Image.h"
#include "Text.h"
#include "Rectangle.h"
#include "BaseState.h"
#include "FileLister.h"
#include "SimpleDirection.h"
#include "AnimatedSprite.h"

#include "SimpleFlags.h"
#include "Camera.h"
#include "fileTypeDefines.h"

/**
Base level class interacting with the Penjin framwork through userInput,update and render
Does input processing, collision testing and bounds checking
**/

class BaseUnit;
class ControlUnit;
class PixelParticle;
class Link;

class Level : public BaseState
{
public:
    Level();
    virtual ~Level();

    // Loads the classes parameters from the passed map of key=value pairs
    // passes each pair to processParameter
    // returns true on success and false otherwise
    virtual bool load(const list<PARAMETER_TYPE >& params);
    void tilingSetup();

    // processes a single key=value pair for loading
    // this function can be overwritten in child classes to allow for custom data fiels
    // return true if the data has been successfully processed, false otherwise
    virtual bool processParameter(const PARAMETER_TYPE& value);

    // reset level to initial state
    virtual void reset();

    // framework related
    virtual void init();
    virtual void userInput();
    virtual void update();
    // draws everything
    virtual void render();
    // draws level data onto passed surface and passed surface onto collisionLayer
    virtual void render(SDL_Surface* screen);
    virtual void onPause();
    virtual void onResume();
    virtual void pauseInput();
    virtual void pauseUpdate();
    virtual void pauseScreen();

    // size of the level in pixels
    int getWidth() const;
    int getHeight() const;

    // this transforms a passed coordinate to conform to the level flags
    // so for example if the level repeats, this will get applied here
    // you should always pass a coordinate to this before it can be worked with
    Vector2df transformCoordinate(const Vector2df& coord) const;

    // checks whether the unit is leaving the bounds (aka standing on the edge of
    // the screen) and returns an adjusted position
    Vector2df boundsCheck(const BaseUnit* const unit) const; // only 2 directions

    // checks whether the unit is leaving the bounds (aka standing on the edge of
    // the screen)
    // Returns a vector of possible/wrapped positions
    // the vector will at least contain the original position on index 0 and no
    // further elements if no wrapping is required
    // Order of the following elements is (if required): x, y, x+y (direction)
    // NOTE: This is more correct than the version above, but it's not actually
    // used since it greatly increases complexity and is only visible is few
    // certain corner situations (literally)
    void boundsCheck( const BaseUnit* const unit, vector<Vector2df> &posVec ) const;

    // returns the first player unit with takesControl == true
    // returns NULL if none is found
    ControlUnit* getFirstActivePlayer() const;

    void swapControl(const bool &cycleForward = true);
    void lose();
    void win();

	// Input is a vector of unit IDs, function will go through unit and player
	// vectors and put all matching units into referenced unitVector
	// used by trigger or switch to get target units for example
    void getUnitsByID(const vector<string>& IDs, vector<BaseUnit*>& unitVector) const;

    // adds a formatted particle to the list
    void addParticle(const BaseUnit* const caller, const Colour& col, const Vector2df& pos, const Vector2df& vel, CRint lifeTime);

    // add/remove Links
    void addLink(BaseUnit *source, BaseUnit *target);
    void removeLink(BaseUnit *source);

    vector<ControlUnit*> players;
    vector<BaseUnit*> units;
    vector<PixelParticle*> effects;
    vector<Link*> links;
    SDL_Surface* levelImage;
    SimpleFlags flags;

    string levelFileName; // chapterPath + filename
    string chapterPath;
    string name;

    #ifdef _DEBUG
    virtual string debugInfo();
    string debugString;
    string controlsString;
    vector<BaseUnit*> debugUnits;
    #endif
    // errors (on loading) will be placed here
    string errorString;

    enum LevelFlag
    {
        lfScrollX = 1,
        lfScrollY = 2,
        lfRepeatX = 4,
        lfRepeatY = 8,
        lfDisableSwap = 16,
        lfKeepCentred = 32,
        lfScaleX = 64,
        lfScaleY = 128,
        lfSplitX = 256,
        lfSplitY = 512,
        lfDrawPattern = 1024,
        lfCyclePlayers = 2048,
        lfEOL = 4096
    };
    static map<string,int> stringToFlag;

    Vector2df drawOffset;
    Camera cam;

    // counts how many players have left through exits
    int winCounter;
	// level time
    int timeCounter;

    int idCounter;

    list<PARAMETER_TYPE > parameters;

protected:
    // deletes the passed unit from the collision surface (to avoid checking
    // the unit agains itself)
    void clearUnitFromCollision(SDL_Surface* const surface, BaseUnit* const unit);

    inline void clearRectangle(SDL_Surface* const surface, CRfloat posX, CRfloat posY, CRint width, CRint height);

    // this renders a unit taking into account the flags, so if a unit moves out
    // of bounds and the level is set to repeat it will also get drawn on the
    // opposite side of the screen
    // specify offset to pass to updateScreenPosition
    void renderUnit(SDL_Surface* const surface, BaseUnit* const unit, const Vector2df& offset);

    void renderTiling( SDL_Surface *src, SDL_Rect *srcRect, SDL_Surface *target, SDL_Rect *targetRect, SimpleDirection dir );

    // checks whether the unit has left the bounds and adjust position accordingly
    bool adjustPosition(BaseUnit* const unit, const bool adjustCamera = false);

    bool playersVisible() const;

    enum LevelProp
    {
        lpUnknown,
        lpImage,
        lpFlags,
        lpFilename,
        lpOffset,
        lpBackground,
        lpBoundaries,
        lpName,
        lpMusic,
        lpDialogue,
        lpGravity,
        lpTerminalVelocity,
        lpEOL
    };
    static map<string,int> stringToProp;

    vector<ControlUnit*> removedPlayers;
    vector<BaseUnit*> removedUnits;

    // set to false on level restart
    bool firstLoad;
    bool trialEnd;

    #ifdef _DEBUG
    Text debugText;
    bool frameLimiter;
    #endif
    SDL_Surface* collisionLayer;
    int eventTimer; // used for fading in and out
    enum LevelFinishState
    {
    	fsNone=0,
    	fsWin,
    	fsLose,
    	fsRestart,
    	fsMenu
    };
    LevelFinishState eventState;

    Text nameText;
    Rectangle nameRect;
    int nameTimer;

    // pause menu is also used for displaying time trial results
    SDL_Surface* pauseSurf;
    vector<string> pauseItems;
    int pauseSelection;
    bool mouseInBounds;
    Vector2di lastPos;
    Rectangle overlay;
    Text timeTrialText;
    int timeDisplay;
    bool newRecord;

    bool hideHor;
    bool hideVert;

    AnimatedSprite arrows;

#ifdef _MUSIC
	void saveMusicToFile(CRstring musicFile);
	FileLister musicLister;
	bool showMusicList;
#endif
};

#endif
