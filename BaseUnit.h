#ifndef BASEUNIT_H
#define BASEUNIT_H

#include <map>
#include <list>

#include "Vector2di.h"
#include "Vector2df.h"
#include "SimpleDirection.h"
#include "Colour.h"
#include "CollisionObject.h"
#include "SimpleFlags.h"
#include "GFX.h"
#include "AnimatedSprite.h"

#include "fileTypeDefines.h"

#define UNIT_COLLISION_DATA_TYPE pair<SimpleDirection, Vector2df>

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

    // loads an image file through the surface cache and returns the result
    SDL_Surface* getSurface(CRstring filename, CRbool optimize=false) const;

    virtual int getHeight() const;
    virtual int getWidth() const;
    virtual Vector2di getSize() const;
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
    // returns the new currentSprite
    AnimatedSprite* setSpriteState(CRstring newState, CRstring fallbackState="");

    // called after a collision check with the map
    virtual void hitMap(const Vector2df& correctionOverride);
    // checks whether the unit collides with the passed colour
    bool checkCollisionColour(const Colour& col) const;
    // called after a successful collision check with another unit
    virtual void hitUnit(const UNIT_COLLISION_DATA_TYPE& collision, BaseUnit* const unit);

    // kills the unit
    virtual void explode();

    Vector2df position;
    Vector2df startingPosition;
    Vector2df velocity; // velocity caused by the unit's movement
    Vector2df startingVelocity;
    Vector2df gravity;  // velocity caused by gravity
    Vector2df acceleration[2]; // 0 - increment, 1 - maximum
    list<Colour> collisionColours;
    CollisionObject collisionInfo; // contains colliding pixels, correction, etc.
    SimpleFlags flags;
    string imageOverwrite; // used for level-specific customisation
    Colour col; // the colour of this unit

    // this is only a "working" pointer, it will not get deleted
    AnimatedSprite* currentSprite;

    bool toBeRemoved; // if true, this unit will be deleted by Level on the next tick

    Level* parent; // the unit is currently owned by this Level class

    string tag; // Delphi programmers will know what this is about

    enum UnitFlag
    {
        ufNoMapCollision=1,
        ufNoUnitCollision=2,
        ufNoGravity=4,
        ufInvincible=8,
        ufNoCollisionDraw=16,
        ufMissionObjective=32,
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
        upPosition,
        upVelocity,
        upFlags,
        upCollision,
        upImageOverwrite,
        upColour,
        upHealth,
        upEOL // end of list value, starting point for child classes' lists
    };
    // converts a string from a level file to a propIdent usable in a switch statement
    // simply add unit-specific props in child classes' constructors
    map<string,int> stringToProp;

    // Store all the sprites in here along with an unique string for identifing
    // These sprites WILL get deleted on destruction of the unit
    map<string,AnimatedSprite*> states;

private:
};

#endif
