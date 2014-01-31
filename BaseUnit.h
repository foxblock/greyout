#ifndef BASEUNIT_H
#define BASEUNIT_H

#include <map>
#include <list>
#include <set>
#include <vector>
#include <string>

#include "Vector2di.h"
#include "Vector2df.h"
#include "SimpleDirection.h"
#include "Colour.h"
#include "CollisionObject.h"
#include "SimpleFlags.h"
#include "GFX.h"
#include "AnimatedSprite.h"
#include "Vector3df.h"

#include "fileTypeDefines.h"
#include "gameDefines.h"

/**
This is the base class for all units in the game
It provides functions for moving, rendering, hit testing, etc.
and properties like position, velocity, sprites, acceleration, etc.
**/

class Level;

class BaseUnit
{
public:
    BaseUnit(Level* newParent);
    BaseUnit(const BaseUnit& source);
    virtual ~BaseUnit();

    // loads the unit's parameters from a map<key,value> passed from the level loading
    // passes the individual key=value pairs to processParameter
    // returns true on success and false on fail
    virtual bool load(list<PARAMETER_TYPE >& params);
    // processes a single key=value pair for loading
    // this function can be overwritten in child classes to allow for custom data fiels
    // return true if the data has been successfully processed, false otherwise
    virtual bool processParameter(const PARAMETER_TYPE& value);

    // resets the unit to its initial state (right after loading)
    virtual void reset();
    // restarts the order system
    void resetOrder(const bool &clear=false);
    // resets the unit temporary values like collisionInfo
    virtual void resetTemporary();

    // loads an image file through the surface cache and returns the result
    SDL_Surface* getSurface(CRstring filename, CRbool optimize=false) const;

    virtual int getHeight() const;
    virtual int getWidth() const;
    virtual Vector2di getSize() const;
    // returns the coordinate of an edge pixel (edge indicated by passed direction)
    virtual Vector2df getPixel(const SimpleDirection& dir) const;
    virtual SDL_Rect getRect() const;

    virtual void update();
    virtual void updateScreenPosition(const Vector2di& offset);
    virtual void render() {render(GFX::getVideoSurface());}
    virtual void render(SDL_Surface* surf);

    // sets the currently displayed sprite to a state linked in the map
    // sets to fallbackState if newState is not found, does not set anything if that is not found either
    // if reset is true the new sprite will be set to the first frame before returning
    // returns the new currentSprite
    AnimatedSprite* setSpriteState(CRstring newState, CRbool reset=false, CRstring fallbackState="");

    // called after a collision check with the map
    virtual void hitMap(const Vector2df& correctionOverride);
    // checks whether the unit collides with the passed colour
    virtual bool checkCollisionColour(const Colour& col) const;
    // called when a collision with another unit occurs, checks whether this unit
    // wants to be affected by the other
    virtual bool hitUnitCheck(const BaseUnit* const caller) const;
    // this now actually does the effect this unit has on the other
    virtual void hitUnit(const UnitCollisionEntry& entry);

    // kills the unit
    virtual void explode();

    #ifdef _DEBUG
    virtual string debugInfo();
    #endif

    Vector2df position;
    Vector2df startingPosition;
    Vector2df velocity; // velocity caused by the unit's movement
    Vector2df acceleration[2]; // 0 - increment, 1 - maximum
    int direction; // the direction the unit is facing (used for sprite orientation)
    set<int> collisionColours;
    CollisionObject collisionInfo; // contains colliding pixels, correction, etc.
    unsigned int unitCollisionMode; // 0 - never collide (be affected by other units),
                                    // 1 - always, 2 - yes, but check collision colours
    SimpleFlags flags;
    Colour col; // the colour of this unit


    // this is only a "working" pointer, it will not get deleted
    AnimatedSprite* currentSprite;
    string currentState;
    string startingState;

    bool toBeRemoved; // if true, this unit will be deleted by Level on the next tick
    Level* parent; // the unit is currently owned by this Level class

    string tag; // class of the unit
    bool isPlayer; // hacky, used in conditions in several hitUnit functions

    string id;

    bool orderRunning; // is this unit currently executing orders?

    enum UnitFlag
    {
        ufNoMapCollision=1, // no map collision check for this unit
        ufNoUnitCollision=2, // does not let other units affect this unit (depracated!)
        ufNoGravity=4,
        ufInvincible=8, // will not explode by environmental hazards
        ufMissionObjective=16, // level will restart when this unit is killed (also winCounter increases for ControlUnits)
        ufNoUpdate=32, // update won't be called for this unit (effectively freezing it in place)
        ufNoRender=66, // do not render this unit (includes NoUnitCollision)
        ufDisregardBoundaries=128, // don't reset this unit's position when moved outside the level boundaries
        ufAlwaysOnTop=256, // render this unit again at the end of the cycle to ensure it's shown
        ufEOL=512
    };
    // converts a string from a level file to a usable flag
    // you can simply add unit-specific flags in child classes
    static map<string,int> stringToFlag;

    // Copy of the parameters this unit was loaded with
    list<PARAMETER_TYPE > parameters;

protected:
	// commonly used parameter load functions
	static bool pLoadColour( CRstring input, Colour &output );
	static bool pLoadVector( CRstring input, Vector2df &output );
	static bool pLoadVector( CRstring input, Vector2di &output );
	static bool pLoadUintIDs( CRstring input, vector<string> &output );

    // basically just a lazy way of writing position += velocity
    virtual void move();

    enum UnitProp // enum used for string-int conversion in loading
    {
        upUnknown=0,
        upClass,
        upState,
        upPosition,
        upVelocity,
        upFlags,
        upCollision,
        upImageOverwrite,
        upTilesheet,
        upFramerate,
        upLoops,
        upTransCol,
        upColour,
        upHealth,
        upID,
        upOrder,
        upSize, // reserved for child classes (not implemented in BaseUnit)
        upTarget, // reserved
        upCollisionMode,
        upEOL // end of list value, starting point for child classes' lists
    };
    // converts a string from a level file to a propIdent usable in a switch statement
    // simply add unit-specific props in child classes' constructors
    static map<string,int> stringToProp;

    // Store all the sprites in here along with an unique string for identifing
    // These sprites WILL get deleted on destruction of the unit
    map<string,AnimatedSprite*> states;

    /// Order system
    struct Order
    {
        int key;
        string value; // consists of several items, mostly time and something like position, speed, etc.
    };
    enum OrderKey
    {
        okUnknown=0,
        okIdle,
        okPosition,
        okRepeat,
        okColour,
        okExplode,
        okRemove,
        okIncrement,
        okEOL
    };
    // processes the next order to start
    virtual bool processOrder(Order& next);
    // periodically updates orders
    virtual bool updateOrder(const Order& curr);
    // finishes up an order, aka makes sure the final position/size/whatever is right
    // this is just working around the limitations of frame-based movement
    // Lesson: Don't use frame-based movement in your games, if you can avoid it
    virtual bool finishOrder(const Order& curr);
    static map<string,int> stringToOrder;
    vector<Order> orderList;
    int currentOrder;
    int orderTimer;
    bool initOrders;

    Vector3df tempColour;
    Vector3df tempColourChange;

    string imageOverwrite; // used for level-specific customisation
    Vector2di tiles;
    int framerate;
    int loops;
    Colour transCol;

	// Used for big position changes after Level::load
	Vector2df teleportPosition;
	bool isTeleporting;
private:
};

#endif
