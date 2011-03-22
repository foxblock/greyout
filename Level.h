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

/**
Base level class interacting with the Penjin framwork through userInput,update and render
Does input processing, collision testing and bounds checking
**/

class BaseUnit;
class ControlUnit;

class Level : public BaseState
{
public:
    Level();
    virtual ~Level();

    // Loads the classes parameters from the passed map of key=value pairs
    // passes each pair to processParameter
    // returns true on success and false otherwise
    virtual bool load(const list<pair<string,string> >& params);

    // framework related
    virtual void init();
    virtual void userInput();
    virtual void update();
    virtual void render();
    virtual void render(SDL_Surface* screen);

    // this is a second pause mode, where only the game logic is paused
    //(movement, input, etc.), but no pause sceen is shown and the screen updates normally
    virtual void logicPauseToggle();
    virtual void logicPause(CRint time);
    virtual void logicPauseUpdate();

    // size of the level in pixels
    virtual int getWidth() const;
    virtual int getHeight() const;

    // this transforms a passed coordinate to conform to the level flags
    // so for example if the level repeats, this will get applied here
    // you should always pass a coordinate to this before it can be worked with
    virtual Vector2df transformCoordinate(const Vector2df& coord) const;

    // checks whether the unit is leaving the bounds (aka standing on the edge of
    // the screen) and returns an adjusted position
    virtual Vector2df boundsCheck(const BaseUnit* const unit) const;

    list<ControlUnit*> players;
    list<BaseUnit*> units;
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
        lfDisableSwap = 16
    };
    map<string,int> stringToFlag;

protected:
    // deletes the passed unit from the collision surface (to avoid checking
    // the unit agains itself)
    void clearUnitFromCollision(SDL_Surface* const surface, BaseUnit* const unit);

    // this renders a unit taking into account the flags, so if a unit moves out
    // of bounds and the level is set to repeat it will also get drawn on the
    // opposite side of the screen
    virtual void renderUnit(SDL_Surface* const surface, BaseUnit* const unit);

    // checks whether the unit has left the bounds and adjust position accordingly
    virtual void adjustPosition(BaseUnit* const unit);

    // processes a single key=value pair for loading
    // this function can be overwritten in child classes to allow for custom data fiels
    // return true if the data has been successfully processed, false otherwise
    virtual bool processParameter(const pair<string,string>& value);

    enum LevelProp
    {
        lpUnknown,
        lpImage,
        lpFlags,
        lpFilename,
        lpEOL
    };
    map<string,int> stringToProp;

    #ifdef _DEBUG
    Text debugText;
    Text fpsDisplay;
    #endif
    SDL_Surface* collisionLayer;
    Rectangle unitRect; // used to clear units from the collision layer
    vector<Rectangle*> mouseRects; // temp
    SDL_Surface* rectangleLayer; // temp
    CountDown logicTimer; // used in logicPause
};

#endif
