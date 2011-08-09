#ifndef BASEUNIT_H
#define BASEUNIT_H

#include <map>
#include <list>
#include <set>

#include "Vector2di.h"
#include "Vector2df.h"
#include "SimpleDirection.h"
#include "Colour.h"
#include "CollisionObject.h"
#include "SimpleFlags.h"
#include "GFX.h"
#include "AnimatedSprite.h"

#include "fileTypeDefines.h"

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
    virtual bool load(const PARAMETER_TYPE& params);

    // resets the unit to its initial state (right after loading)
    virtual void reset();
    // resets the unit temporary values like collisionInfo
    virtual void resetTemporary();

    // loads an image file through the surface cache and returns the result
    SDL_Surface* getSurface(CRstring filename, CRbool optimize=false) const;

    virtual inline int getHeight() const;
    virtual inline int getWidth() const;
    virtual inline Vector2di getSize() const;
    // returns the coordinate of an edge pixel (edge indicated by passed direction)
    virtual Vector2df getPixel(const SimpleDirection& dir) const;
    void setStartingPosition(const Vector2df& pos);
    void setStartingVelocity(const Vector2df& vel);

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
    bool checkCollisionColour(const Colour& col) const;
    // called when a collision with another unit occurs, checks whether this unit
    // wants to be affected by the other
    virtual bool hitUnitCheck(const BaseUnit* const caller) const;
    // this now actually does the effect this unit has on the other
    virtual void hitUnit(const UnitCollisionEntry& entry);

    // kills the unit
    virtual void explode();

    Vector2df position;
    Vector2df startingPosition;
    Vector2df velocity; // velocity caused by the unit's movement
    Vector2df startingVelocity;
    Vector2df gravity;  // velocity caused by gravity
    Vector2df acceleration[2]; // 0 - increment, 1 - maximum
    int direction; // the direction the unit is facing (use for sprite orientation)
    set<int> collisionColours;
    CollisionObject collisionInfo; // contains colliding pixels, correction, etc.
    SimpleFlags flags;
    string imageOverwrite; // used for level-specific customisation
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

    enum UnitFlag
    {
        ufNoMapCollision=1, // no map collision check for this unit
        ufNoUnitCollision=2, // does not let other units affect this unit
        ufNoGravity=4,
        ufInvincible=8, // will not explode by environmental hazards
        ufMissionObjective=16, // level will restart when this unit is killed (also winCounter increases for ControlUnits)
        ufNoUpdate=32, // update won't be called for this unit (effectively freezing it in place)
        ufEOL=64
    };
    // converts a string from a level file to a usable flag
    // you can simply add unit-specific flags in child classes
    map<string,UnitFlag> stringToFlag;

protected:
    // processes a single key=value pair for loading
    // this function can be overwritten in child classes to allow for custom data fiels
    // return true if the data has been successfully processed, false otherwise
    virtual bool processParameter(const pair<string,string>& value);

    // basically just a lazy way of writing position += velocity
    virtual void move();

    enum UnitProp // enum used for string-int conversion in loading
    {
        upUnknown,
        upClass,
        upState,
        upPosition,
        upVelocity,
        upFlags,
        upCollision,
        upImageOverwrite,
        upColour,
        upHealth,
        upID,
        upOrder,
        upEOL // end of list value, starting point for child classes' lists
    };
    // converts a string from a level file to a propIdent usable in a switch statement
    // simply add unit-specific props in child classes' constructors
    map<string,int> stringToProp;

    // Store all the sprites in here along with an unique string for identifing
    // These sprites WILL get deleted on destruction of the unit
    map<string,AnimatedSprite*> states;

    /// Order system
    struct Order
    {
        int key;
        string value; // consists of several items, mostly time and something like position, speed, etc.
        void* data; // a data pointer which you can write something to for fast access in updateOrder without the need of re-parsing
    };
    enum OrderKey
    {
        okUnknown,
        okIdle,
        okPosition,
        okRepeat,
        okEOL
    };
    // processes the next order to start
    virtual bool processOrder(Order& next);
    // periodically updates orders
    virtual bool updateOrder(const Order& curr);
    map<string,OrderKey> stringToOrder;
    vector<Order> orderList;
    int currentOrder;
    int orderTimer;
    bool orderRunning; // is this unit currently executing orders?

private:
};

#endif
