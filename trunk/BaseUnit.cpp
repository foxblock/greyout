#include "BaseUnit.h"

#include "StringUtility.h"
#include "Random.h"

#include "Level.h"
#include "SurfaceCache.h"
#include "MusicCache.h"

BaseUnit::BaseUnit(Level* newParent)
{
    currentSprite = NULL;
    position = Vector2df(0.0f,0.0f);
    startingPosition = Vector2df(0.0f,0.0f);
    velocity = Vector2df(0.0f,0.0f);
    startingVelocity = Vector2df(0.0f,0.0f);
    gravity = Vector2df(0.0f,0.0f);
    acceleration[0] = Vector2df(0.0f,0.0f);
    acceleration[1] = Vector2df(0.0f,0.0f);
    toBeRemoved = false;
    parent = newParent;
    tag = "";
    imageOverwrite = "";
    col = WHITE;
    currentState = "";
    direction = 0;

    // set-up conversion maps
    stringToFlag["nomapcollision"] = ufNoMapCollision;
    stringToFlag["nounitcollision"] = ufNoUnitCollision;
    stringToFlag["nogravity"] = ufNoGravity;
    stringToFlag["invincible"] = ufInvincible;
    stringToFlag["missionobjective"] = ufMissionObjective;

    stringToProp["class"] = upClass;
    stringToProp["position"] = upPosition;
    stringToProp["velocity"] = upVelocity;
    stringToProp["flags"] = upFlags;
    stringToProp["collision"] = upCollision;
    stringToProp["imageoverwrite"] = upImageOverwrite;
    stringToProp["colour"] = upColour;
    stringToProp["health"] = upHealth;
}

BaseUnit::BaseUnit(const BaseUnit& source)
{
    //
}

BaseUnit::~BaseUnit()
{
    collisionInfo.clear();
    collisionColours.clear();
    stringToFlag.clear();
    stringToProp.clear();
    for (map<string,AnimatedSprite*>::iterator iter = states.begin(); iter != states.end(); ++iter)
    {
        delete iter->second;
    }
    states.clear();
}

/// ---public---

bool BaseUnit::load(const PARAMETER_TYPE& params)
{
    for (PARAMETER_TYPE::const_iterator value = params.begin(); value != params.end(); ++value)
    {
        if (not processParameter(make_pair(value->first,value->second)))
        {
            string className = params.front().second;
            cout << "Warning: Unprocessed parameter \"" << value->first << "\" on unit \"" << className << "\"" << endl;
        }

    }

#ifdef _DEBUG
    collisionColours.push_back(GREEN); // collision indicator pixels else messing things up
#endif

    return true; // Currently returns always true as no critical error checking is done
}

void BaseUnit::reset()
{
    position = startingPosition;
    velocity = startingVelocity;
    gravity = Vector2df(0,0);
    acceleration[0] = Vector2df(0,0);
    acceleration[1] = Vector2df(0,0);
    collisionInfo.clear();
    toBeRemoved = false;
}

SDL_Surface* BaseUnit::getSurface(CRstring filename, CRbool optimize) const
{
    bool fromCache;
    return SURFACE_CACHE->getSurface(filename,parent->chapterPath,fromCache,optimize);
}

int BaseUnit::getHeight() const
{
    if (currentSprite)
        return currentSprite->getHeight();
    else
        return -1;
}

int BaseUnit::getWidth() const
{
    if (currentSprite)
        return currentSprite->getWidth();
    else
        return -1;
}

Vector2di BaseUnit::getSize() const
{
    return Vector2di(getWidth(),getHeight());
}

Vector2df BaseUnit::getPixel(const SimpleDirection& dir) const
{
    switch (dir.value)
    {
    case diLEFT:
        return Vector2df(position.x,position.y + getHeight() / 2.0f);
    case diRIGHT:
        return Vector2df(position.x + getWidth() - 1,position.y + getHeight() / 2.0f);
    case diTOP:
        return Vector2df(position.x + getWidth() / 2.0f, position.y);
    case diBOTTOM:
        return Vector2df(position.x + getWidth() / 2.0f, position.y + getHeight() - 1);
    case diTOPLEFT:
        return position;
    case diTOPRIGHT:
        return Vector2df(position.x + getWidth() - 1, position.y);
    case diBOTTOMLEFT:
        return Vector2df(position.x, position.y + getHeight() - 1);
    case diBOTTOMRIGHT:
        return position + getSize() - Vector2df(1,1);
    default:
        return position + Vector2df(getWidth() / 2,getHeight() / 2);
    }
}

void BaseUnit::setStartingPosition(const Vector2df& pos)
{
    position = pos;
    startingPosition = pos;
}

void BaseUnit::setStartingVelocity(const Vector2df& vel)
{
    velocity = vel;
    startingVelocity = vel;
}

void BaseUnit::update()
{
    if (not flags.hasFlag(ufInvincible) && not collisionInfo.isHealthy(velocity))
    {
        explode();
        if (flags.hasFlag(ufMissionObjective))
        {
            parent->lose();
        }
    }
    else
    {
        move();
        if (velocity.x > 0)
            direction = 1;
        else if (velocity.x < 0)
            direction = -1;

        if (currentSprite)
            currentSprite->update();
    }
}

void BaseUnit::updateScreenPosition(const Vector2di& offset)
{
    if (currentSprite)
        currentSprite->setPosition(position - offset);
}

void BaseUnit::render(SDL_Surface* surf)
{
    if (currentSprite)
        currentSprite->render(surf);

#ifdef _DEBUG
    for (vector<CollisionEntry>::const_iterator temp = collisionInfo.entries.begin(); temp != collisionInfo.entries.end(); ++temp)
    {
        GFX::setPixel(temp->pixel - parent->drawOffset,GREEN);
    }
    GFX::renderPixelBuffer();
#endif
}

AnimatedSprite* BaseUnit::setSpriteState(CRstring newState, CRbool reset, CRstring fallbackState)
{
    map<string,AnimatedSprite*>::const_iterator state = states.find(newState);
    if (state != states.end()) // set to desired state
    {
        currentSprite = state->second;
        currentState = newState;
    }
    else if (fallbackState[0] != 0) // set to fallback or keep the current one
    {
        state = states.find(fallbackState);
        if (state != states.end())
        {
            currentSprite = state->second;
            currentState = fallbackState;
        }
        else
        {
            cout << "Unrecognized sprite state \"" << newState << "\" and fallback \"" <<
            fallbackState << "\" on unit \"" << tag << "\"" << endl;
        }
    }
    else
        cout << "Unrecognized sprite state \"" << newState << "\" on unit \"" << tag << "\"" << endl;
    if (reset)
        currentSprite->rewind();
    return currentSprite;
}

void BaseUnit::hitMap(const Vector2df& correctionOverride)
{
    if (correctionOverride.y < 0) // hitting the ground
    {
        position.y += velocity.y + correctionOverride.y;
        velocity.y = 0;
        velocity.x += correctionOverride.x;
    }
    else
    {
        velocity += correctionOverride;
    }
}

bool BaseUnit::checkCollisionColour(const Colour& col) const
{
    for (list<Colour>::const_iterator I = collisionColours.begin(); I != collisionColours.end(); ++I)
    {
        if (col == (*I))
        {
            return true;
        }
    }
    return false;
}

void BaseUnit::hitUnit(const UNIT_COLLISION_DATA_TYPE& collision, BaseUnit* const unit)
{
    //
}

void BaseUnit::explode()
{
    if (currentSprite && parent)
    {
        // go through the image and create a particle for every visible pixel
        // only takes every fourth pixel for speed reasons
        Colour none = currentSprite->getTransparentColour();
        Colour pix = MAGENTA;
        Vector2df vel(0,0);
        for (int X = 0; X < currentSprite->getWidth(); X+=2)
        {
            for (int Y = 0; Y < currentSprite->getHeight(); Y+=2)
            {
                pix = currentSprite->getPixel(X,Y);
                if (pix != none)
                {
                    vel.x = Random::nextFloat(-10,10);
                    vel.y = Random::nextFloat(-15,-5);
                    parent->addParticle(this,pix,position + Vector2df(X,Y),vel,750);
                }
            }
        }
        MUSIC_CACHE->playSound("sounds/die.wav",parent->chapterPath);
    }
    toBeRemoved = true;
}

/// ---protected---

bool BaseUnit::processParameter(const pair<string,string>& value)
{
    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case upClass:
    {
        tag = value.second;
        break;
    }
    case upPosition:
    {
        vector<string> token;
        StringUtility::tokenize(value.second,token,DELIMIT_STRING);
        if (token.size() != 2)
        {
            parsed = false;
            break;
        }
        position.x = StringUtility::stringToFloat(token.at(0));
        position.y = StringUtility::stringToFloat(token.at(1));
        startingPosition = position;
        break;
    }
    case upVelocity:
    {
        vector<string> token;
        StringUtility::tokenize(value.second,token,DELIMIT_STRING);
        if (token.size() != 2)
        {
            parsed = false;
            break;
        }
        velocity.x = StringUtility::stringToFloat(token.at(0));
        velocity.y = StringUtility::stringToFloat(token.at(1));
        startingVelocity = velocity;
        break;
    }
    case upFlags:
    {
        flags.clear();
        vector<string> token;
        StringUtility::tokenize(value.second,token,DELIMIT_STRING);
        for (vector<string>::const_iterator flag = token.begin(); flag != token.end(); ++flag)
        {
            flags.addFlag(stringToFlag[*flag]);
        }
        break;
    }
    case upCollision:
    {
        collisionColours.clear();
        vector<string> token;
        StringUtility::tokenize(value.second,token,DELIMIT_STRING);
        for (vector<string>::const_iterator col = token.begin(); col != token.end(); ++col)
        {
            int val = StringUtility::stringToInt((*col));
            if (val > 0 || (*col) == "0") // passed parameter is a numeric colour code
                collisionColours.push_back(Colour(val));
            else // string colour code
                collisionColours.push_back(Colour((*col)));
        }
        break;
    }
    case upImageOverwrite:
    {
        imageOverwrite = value.second;
        break;
    }
    case upColour:
    {
        int val = StringUtility::stringToInt(value.second);
        if (val > 0 || value.second == "0") // passed parameter is a numeric colour code
            col = Colour(val);
        else // string colour code
            col = Colour(value.second);
        collisionColours.push_back(col);
        break;
    }
    case upHealth:
    {
        collisionInfo.squashThreshold = StringUtility::stringToInt(value.second);
        break;
    }
    default:
        parsed = false;
    }

    return parsed;
}

void BaseUnit::move()
{
    position += velocity + gravity;
}
