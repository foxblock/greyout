#ifndef LEVEL_H
#define LEVEL_H

#include <list>
#include <map>

#include "PenjinTypes.h"
#include "Image.h"
#include "Text.h"
#include "Rectangle.h"
#include "CountDown.h"
#include "BaseState.h"

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

class Level : public BaseState
{
public:
    Level();
    virtual ~Level();

    // Loads the classes parameters from the passed map of key=value pairs
    // passes each pair to processParameter
    // returns true on success and false otherwise
    virtual bool load(const PARAMETER_TYPE& params);

    // framework related
    virtual void init();
    virtual void userInput();
    virtual void update();
    // draws everything
    virtual void render();
    // draws level data onto passed surface and passed surface onto collisionLayer
    virtual void render(SDL_Surface* screen);

    // size of the level in pixels
    virtual int getWidth() const;
    virtual int getHeight() const;

    // this transforms a passed coordinate to conform to the level flags
    // so for example if the level repeats, this will get applied here
    // you should always pass a coordinate to this before it can be worked with
    Vector2df transformCoordinate(const Vector2df& coord) const;

    // checks whether the unit is leaving the bounds (aka standing on the edge of
    // the screen) and returns an adjusted position
    Vector2df boundsCheck(const BaseUnit* const unit) const;

    // returns the first player unit with takesControl == true
    // returns NULL if none is found
    ControlUnit* getFirstActivePlayer() const;

    void swapControl();
    void lose();
    void win();

    // adds a formatted particle to the list
    void addParticle(const BaseUnit* const caller, const Colour& col, const Vector2df& pos, const Vector2df& vel, CRint lifeTime);

    list<ControlUnit*> players;
    list<BaseUnit*> units;
    list<PixelParticle*> effects;
    Image* levelImage;
    Colour noCollision; // used to clear units in clearUnitFromCollision
    SimpleFlags flags;

    string levelFileName;
    string chapterPath;

    #ifdef _DEBUG
    string debugString;
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
        lfScale = 64
    };
    map<string,int> stringToFlag;

    Vector2df drawOffset;
    Camera cam;

    // essentially counts how many players have left through exits
    int winCounter;
    bool isEnding;

protected:
    // deletes the passed unit from the collision surface (to avoid checking
    // the unit agains itself)
    void clearUnitFromCollision(SDL_Surface* const surface, BaseUnit* const unit);

    // this renders a unit taking into account the flags, so if a unit moves out
    // of bounds and the level is set to repeat it will also get drawn on the
    // opposite side of the screen
    // specify offset to pass to updateScreenPosition
    void renderUnit(SDL_Surface* const surface, BaseUnit* const unit, const Vector2df& offset);

    // checks whether the unit has left the bounds and adjust position accordingly
    void adjustPosition(BaseUnit* const unit);

    // processes a single key=value pair for loading
    // this function can be overwritten in child classes to allow for custom data fiels
    // return true if the data has been successfully processed, false otherwise
    virtual bool processParameter(const pair<string,string>& value);

    static void loseCallback(void* data);
    static void winCallback(void* data);

    enum LevelProp
    {
        lpUnknown,
        lpImage,
        lpFlags,
        lpFilename,
        lpOffset,
        lpBackground,
        lpBoundaries,
        lpEOL
    };
    map<string,int> stringToProp;

    #ifdef _DEBUG
    Text debugText;
    Text fpsDisplay;
    #endif
    SDL_Surface* collisionLayer;
    Rectangle unitRect; // used to clear units from the collision layer
    CountDown logicTimer; // used in logicPause
    CountDown eventTimer;
};

#endif
