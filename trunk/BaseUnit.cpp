/*
	Greyout - a colourful platformer about love

	Greyout is Copyright (c)2011-2014 Janek Schäfer

	This file is part of Greyout.

    Greyout is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

	Greyout is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Please direct any feedback, questions or comments to
	Janek Schäfer (foxblock), foxblock_at_gmail_dot_com
*/

#include "BaseUnit.h"

#include "StringUtility.h"
#include "Random.h"

#include "Level.h"
#include "GreySurfaceCache.h"
#include "MusicCache.h"
#include "MyGame.h"

map<string,int> BaseUnit::stringToFlag;
map<string,int> BaseUnit::stringToProp;
map<string,int> BaseUnit::stringToOrder;

BaseUnit::BaseUnit(Level* newParent)
{
    // set-up conversion maps
    stringToFlag["nomapcollision"] = ufNoMapCollision;
    stringToFlag["nounitcollision"] = ufNoUnitCollision;
    stringToFlag["nogravity"] = ufNoGravity;
    stringToFlag["invincible"] = ufInvincible;
    stringToFlag["missionobjective"] = ufMissionObjective;
    stringToFlag["noupdate"] = ufNoUpdate;
    stringToFlag["norender"] = ufNoRender;
    stringToFlag["disregardboundaries"] = ufDisregardBoundaries;
    stringToFlag["alwaysontop"] = ufAlwaysOnTop;

    stringToProp["class"] = upClass;
    stringToProp["startingstate"] = upStartingState;
    stringToProp["position"] = upPosition;
    stringToProp["velocity"] = upVelocity;
    stringToProp["flags"] = upFlags;
    stringToProp["collision"] = upCollision;
    stringToProp["imageoverwrite"] = upImageOverwrite;
    stringToProp["tiles"] = upTilesheet;
    stringToProp["framerate"] = upFramerate;
    stringToProp["loops"] = upLoops;
    stringToProp["transparentcolor"] = upTransCol;
    stringToProp["transparentcolour"] = upTransCol;
    stringToProp["colour"] = upColour;
    stringToProp["color"] = upColour;
    stringToProp["health"] = upHealth;
    stringToProp["id"] = upID;
    stringToProp["order"] = upOrder;
    stringToProp["size"] = upSize;
    stringToProp["target"] = upTarget;
    stringToProp["collisionmode"] = upCollisionMode;
    stringToProp["state"] = upState;

    stringToOrder["idle"] = okIdle;
    stringToOrder["position"] = okPosition;
    stringToOrder["repeat"] = okRepeat;
    stringToOrder["colour"] = okColour;
    stringToOrder["color"] = okColour;
    stringToOrder["explode"] = okExplode;
    stringToOrder["remove"] = okRemove;
    stringToOrder["increment"] = okIncrement;
    stringToOrder["parameter"] = okParameter;
    stringToOrder["sound"] = okSound;
    stringToOrder["state"] = okState;

    currentSprite = NULL;
    position = Vector2df(0.0f,0.0f);
    startingPosition = Vector2df(0.0f,0.0f);
    teleportPosition = Vector2df(0.0f,0.0f);
    velocity = Vector2df(0.0f,0.0f);
    acceleration[0] = Vector2df(0.0f,0.0f);
    acceleration[1] = Vector2df(0.0f,0.0f);
    toBeRemoved = false;
    parent = newParent;
    tag = "";
    id = "";
    imageOverwrite = "";
    tiles = Vector2di(1,1);
    framerate = 10;
    loops = 0;
    transCol = MAGENTA;
    col = WHITE;
    currentState = "";
    startingState = "";
    direction = 0;
    isPlayer = false;
    orderRunning = false;
    orderTimer = 0;
    currentOrder = 0;
    unitCollisionMode = 2;
    initOrders = true;
    isTeleporting = false;
}

BaseUnit::BaseUnit(const BaseUnit& source)
{
    //
}

BaseUnit::~BaseUnit()
{
    collisionInfo.clear();
    collisionColours.clear();
    for (map<string,AnimatedSprite*>::iterator iter = states.begin(); iter != states.end(); ++iter)
    {
        delete iter->second;
    }
    states.clear();
    orderList.clear();
    parameters.clear();
}

/// ---public-------------------------------------------------------------------

bool BaseUnit::load(list<PARAMETER_TYPE >& params)
{
    if (id[0] == 0) // always set ID to increasing number (per level) - may be overriden in level file
        id = StringUtility::intToString(++(parent->idCounter));

    string className = params.front().second;

    for (list<PARAMETER_TYPE >::iterator value = params.begin(); value != params.end(); ++value)
    {
        if (not processParameter(*value))
        {
            printf("WARNING: Unprocessed parameter \"%s\" on unit with id \"%s\" (%s)\n",
                   value->first.c_str(),id.c_str(),className.c_str());
        }
    }

    // Not taking the unit's size into account here as it might not be available yet
    int boundsX = (position.x > parent->getWidth()) - (position.x < 0);
    int boundsY = (position.y > parent->getHeight()) - (position.y < 0);
    if ((boundsX + boundsY != 0) && !parent->flags.hasFlag(Level::lfRepeatX) && !parent->flags.hasFlag(Level::lfRepeatY))
    {
        flags.addFlag(ufDisregardBoundaries);
    }

	initOrders = true;

	if (imageOverwrite[0] != 0)
	{
		SDL_Surface *surf = getSurface(imageOverwrite);
		if (!stateParams.empty())
		{
			for (vector<State>::const_iterator I = stateParams.begin(); I != stateParams.end(); ++I)
			{
				loadState(surf, *I);
			}
			stateParams.clear();
		}
		else
		{
			AnimatedSprite* temp = new AnimatedSprite;
			temp->loadFrames(surf,tiles.x,tiles.y,0,0);
			temp->setFrameRate(framerate);
			temp->setTransparentColour(transCol);
			temp->setLooping(loops);
			states["default"] = temp;
			startingState = "default";
		}
		setSpriteState(startingState,true);
	}

#ifdef _DEBUG
    collisionColours.insert(Colour(GREEN).getIntColour()); // collision indicator pixels else messing things up
#endif

    return true; // Currently returns always true as no critical error checking is done here
}

bool BaseUnit::processParameter(const PARAMETER_TYPE& value)
{
    bool parsed = true;

    switch (stringToProp[value.first])
    {
    case upClass:
    {
        tag = value.second;
        break;
    }
    case upStartingState:
    {
        startingState = value.second;
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
        if (parent->timeCounter == 0) // during Level::load
		{
			position.x = StringUtility::stringToFloat(token[0]);
			position.y = StringUtility::stringToFloat(token[1]);
			startingPosition = position;
		}
		else
		{
			teleportPosition.x = StringUtility::stringToFloat(token[0]);
			teleportPosition.y = StringUtility::stringToFloat(token[1]);
			isTeleporting = true;
		}
        break;
    }
    case upVelocity:
    {
    	velocity = StringUtility::stringToVec<Vector2df>(value.second);
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
            if (stringToFlag[*flag] == ufNoUnitCollision)
            {
                unitCollisionMode = 0;
                printf("WARNING: Using deprecated flag noUnitCollision on unit with ID \"%s\", use CollisionMode=0 instead!\n",id.c_str());
            }
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
        	Colour temp;
        	if ( !pLoadColour( *col, temp ) )
			{
				parsed = false;
				break;
			}
        	collisionColours.insert( temp.getIntColour() );
        }
        break;
    }
    case upImageOverwrite:
    {
        imageOverwrite = value.second;
        break;
    }
    case upTilesheet:
	{
		tiles = StringUtility::stringToVec<Vector2di>(value.second);
		break;
	}
	case upFramerate:
	{
		framerate = StringUtility::stringToInt(value.second);
		break;
	}
	case upLoops:
	{
		loops = StringUtility::stringToInt(value.second);
		break;
	}
    case upTransCol:
    {
        parsed = pLoadColour( value.second, transCol );
        break;
    }
    case upColour:
    {
        parsed = pLoadColour( value.second, col );
        break;
    }
    case upHealth:
    {
        collisionInfo.squashThreshold = StringUtility::stringToInt(value.second);
        break;
    }
    case upID:
    {
        id = value.second;
        break;
    }
    case upOrder:
    {
        vector<string> params;
        StringUtility::tokenize(value.second, params, DELIMIT_STRING);
        if (params.size() < 2)
        {
            Order temp;
            temp.key = stringToOrder[params.front()];
            temp.ticks = 1;
            temp.randomTicks = -1;
            orderList.push_back(temp);
        }
        else
        {
            Order temp;
			pLoadTime(params[1], temp.ticks);
			if (temp.ticks <= 0)
				temp.ticks = 1;
            pIsRandomTime(params[1], temp.randomTicks);
            temp.key = stringToOrder[params.front()];
            temp.params.insert(temp.params.begin(), params.begin()+1, params.end());
            orderList.push_back(temp);
        }
        break;
    }
    case upCollisionMode:
    {
        if (value.second == "false" || value.second == "0" || value.second == "never")
            unitCollisionMode = 0;
        else if (value.second == "true" || value.second == "1" || value.second == "always")
            unitCollisionMode = 1;
        else if (value.second == "colour" || value.second == "color" || value.second == "2")
			unitCollisionMode = 2;
        else
            parsed = false;
        break;
    }
    case upState:
	{
		vector<string> params;
		StringUtility::tokenize(value.second, params, DELIMIT_STRING);
		if (params.size() < 3)
		{
			parsed = false;
		}
		State temp;
		temp.name = params[0];
		temp.start = StringUtility::stringToInt(params[1]);
		temp.length = StringUtility::stringToInt(params[2]);
        if (params.size() > 5)
		{
			temp.fps = StringUtility::stringToInt(params[3]);
			temp.loops = StringUtility::stringToInt(params[4]);
			if (params[5] == "reverse" || StringUtility::stringToInt(params[5]) == pmReverse)
				temp.mode = pmReverse;
			else if (params[5] == "pulse" || StringUtility::stringToInt(params[5]) == pmPulse)
				temp.mode = pmPulse;
			else
				temp.mode = pmNormal;
		}
		else
		{
			temp.fps = framerate;
			temp.loops = loops;
			temp.mode = pmNormal;
		}
		stateParams.push_back(temp);
		break;
	}
    default:
        parsed = false;
    }

    return parsed;
}

void BaseUnit::reset()
{
    velocity = Vector2df(0,0);
    acceleration[0] = Vector2df(0,0);
    acceleration[1] = Vector2df(0,0);
    collisionInfo.clear();
    for (map<string,AnimatedSprite*>::iterator iter = states.begin(); iter != states.end(); ++iter)
    {
        delete iter->second;
    }
    states.clear();
    orderList.clear();
    toBeRemoved = false;
    load(parameters);
}

void BaseUnit::resetOrder(const bool &clear)
{
    if (clear)
    {
        orderList.clear();
        orderRunning = false;
        currentOrder = 0;
    }
    else if (!orderList.empty())
    {
        currentOrder = 0;
        processOrder(orderList.front());
    }
}

void BaseUnit::resetTemporary()
{
    collisionInfo.clear();
}

SDL_Surface* BaseUnit::getSurface(CRstring filename, CRbool optimize) const
{
    return SURFACE_CACHE->loadSurface(filename,parent->chapterPath,optimize);
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
        return Vector2df(position.x + getWidth() / 2.0f, position.y + getHeight() - 1.0f);
    case diTOPLEFT:
        return position;
    case diTOPRIGHT:
        return Vector2df(position.x + getWidth() - 1.0f, position.y);
    case diBOTTOMLEFT:
        return Vector2df(position.x, position.y + getHeight() - 1.0f);
    case diBOTTOMRIGHT:
        return position + getSize() - Vector2df(1,1);
    default:
        return position + Vector2df(getWidth() / 2.0f,getHeight() / 2.0f);
    }
}

SDL_Rect BaseUnit::getRect() const
{
    SDL_Rect result = {position.x,position.y,getWidth(),getHeight()};
    return result;
}

void BaseUnit::update()
{
	if (initOrders)
	{
		resetOrder( false );
		initOrders = false;
	}

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
        if (velocity.x > 0.0f)
            direction = 1;
        else if (velocity.x < 0.0f)
            direction = -1;

        if (currentSprite)
            currentSprite->update();
        if (orderRunning && orderTimer > 0 && !orderList.empty() && currentOrder < orderList.size())
        {
            updateOrder(orderList[currentOrder]);
            if (--orderTimer <= 0)
            {
                finishOrder(orderList[currentOrder]);
                if (++currentOrder < orderList.size())
                    processOrder(orderList[currentOrder]);
                else
                {
                    orderRunning = false;
                }
            }
        }
    }
}

void BaseUnit::updateScreenPosition(const Vector2di& offset)
{
    if (currentSprite)
        currentSprite->setPosition(floor(position.x) - offset.x, floor(position.y) - offset.y);
}

void BaseUnit::render(SDL_Surface* surf)
{
    if (currentSprite)
        currentSprite->render(surf);

#ifdef _DEBUG
    for (vector<MapCollisionEntry>::const_iterator temp = collisionInfo.pixels.begin(); temp != collisionInfo.pixels.end(); ++temp)
    {
        GFX::setPixel(temp->pos - parent->drawOffset,GREEN);
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
            printf("Unrecognized sprite state \"%s\" and fallback \"%s\" on unit with id \"%s\"\n",
                   newState.c_str(),fallbackState.c_str(),id.c_str());
        }
    }
    else
        printf("Unrecognized sprite state \"%s\" on unit with id \"%s\"\n",
               newState.c_str(),id.c_str());
    if (reset && currentSprite)
        currentSprite->rewind();
    return currentSprite;
}

void BaseUnit::hitMap(const Vector2df& correctionOverride)
{
    if ( correctionOverride.y != 0.0f ) // hitting the ground
    {
        position.y += velocity.y + correctionOverride.y;
        velocity.y = 0;
    }
    if ( correctionOverride.x != 0.0f )
    {
        position.x += velocity.x + correctionOverride.x;
        velocity.x = 0;
    }
    position += collisionInfo.positionCorrection;
}

bool BaseUnit::checkCollisionColour(const Colour& col) const
{
    // NOTE: This effectively always adds the unit's colour to the collision list (done to speed check up)
    if (col == this->col)
        return true;
    set<int>::const_iterator iter = collisionColours.find(col.getIntColour());
    if (iter != collisionColours.end())
        return true;
    return false;
}

bool BaseUnit::hitUnitCheck(const BaseUnit* const caller) const
{
    switch (unitCollisionMode)
    {
    case 0:
        return false;
        break;
    case 1:
        return true;
        break;
    default:
        if (not caller->checkCollisionColour(col) && not checkCollisionColour(caller->col))
            return false;
        break;
    }
    return true;
}

void BaseUnit::hitUnit(const UnitCollisionEntry& entry)
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
        int time = 0;
        // limit number of particles so bigger images will create less
		int inc;
        switch (ENGINE->settings->getParticleDensity())
        {
		case Settings::pdOff:
			MUSIC_CACHE->playSound("sounds/die.wav",parent->chapterPath);
			toBeRemoved = true;
			return;
		case Settings::pdFew:
			inc = round(max((float)(currentSprite->getWidth() + currentSprite->getHeight()) / 32.0f,4.0f));
			break;
		case Settings::pdMany:
			inc = round(max((float)(currentSprite->getWidth() + currentSprite->getHeight()) / 64.0f,2.0f));
			break;
		case Settings::pdTooMany:
			inc = 1;
			break;
		default:
			inc = round(max((float)(currentSprite->getWidth() + currentSprite->getHeight()) / 64.0f,2.0f));
			break;
        }
        for (int X = currentSprite->getWidth()-1; X >= 0; X-=inc)
        {
            for (int Y = currentSprite->getHeight()-1; Y >= 0; Y-=inc)
            {
                pix = currentSprite->getPixel(X,Y);
                if (pix != none)
                {
                    vel.x = Random::nextFloat(-5,5);
                    vel.y = Random::nextFloat(-8,-3);
                    time = Random::nextInt(45,75);
                    parent->addParticle(this,pix,position + Vector2df(X,Y),vel,time);
                }
            }
        }
        MUSIC_CACHE->playSound("sounds/die.wav",parent->chapterPath);
    }
    toBeRemoved = true;
}

#ifdef _DEBUG
string BaseUnit::debugInfo()
{
    string result = "";
    result += tag + ";" + id + "\n";
    result += "S: " + StringUtility::intToString(getWidth()) + "," + StringUtility::intToString(getHeight()) + "\n" +
			  "P: " + StringUtility::vecToString(position) + "\n" +
              "V: " + StringUtility::vecToString(velocity) + "\n" +
              "A: " + StringUtility::vecToString(acceleration[0]) + " to " + StringUtility::vecToString(acceleration[1]) + "\n" +
              "C: " + StringUtility::vecToString(collisionInfo.correction) + " " + StringUtility::vecToString(collisionInfo.positionCorrection) + " | ";
    for (set<int>::const_iterator I = collisionColours.begin(); I != collisionColours.end(); ++I)
		result += StringUtility::intToString(*I) + ",";
    result += " | ";
    for (vector<UnitCollisionEntry>::const_iterator I = collisionInfo.units.begin(); I != collisionInfo.units.end(); ++I)
        result += (*I).unit->id + ",";
	result += "\n";
    result += "F: " + StringUtility::intToString(flags.flags) + "\n" +
              "S: " + currentState;
    if (currentSprite)
        result += " (" + StringUtility::intToString(currentSprite->getCurrentFrame()) + ")\n";
    else
        result += "\n";
	result += "C: " + StringUtility::intToString(col.red) + "r" + StringUtility::intToString(col.green) + "g" + StringUtility::intToString(col.blue) + "b\n";
    if (!orderList.empty())
    {
        if (currentOrder < orderList.size())
		{
			string temp = StringUtility::combine(orderList[currentOrder].params, DELIMIT_STRING);
			result += "O: " + StringUtility::intToString(orderList[currentOrder].key) +
					"," + StringUtility::intToString(orderList[currentOrder].ticks) +
					"," + StringUtility::intToString(orderList[currentOrder].randomTicks) +
					"," + temp + "(" + StringUtility::intToString(currentOrder) + ")\n";
		}
        else
            result += "O: finished\n";
    }
    return result;
}
#endif

/// ---protected----------------------------------------------------------------

bool BaseUnit::pLoadColour(CRstring input, Colour& output)
{
	int val = StringUtility::stringToInt(input);
	if ( input[input.length()-1] == 'x' ) // hex colour: 00FFEEx
		output.setColour(StringUtility::hexToInt(input.substr(0,input.length()-1)));
	else if ( input.find('r') != string::npos && input[0] >= 48 && input[0] <= 57 ) // rgb colour: 0r255g238b
	{
		vector<string> tokens;
		StringUtility::tokenize(input,tokens,"rgb");
		if (tokens.size() < 3)
			return false;
		output.setColour(StringUtility::stringToInt(tokens[0]),
						 StringUtility::stringToInt(tokens[1]),
						 StringUtility::stringToInt(tokens[2]));
	}
	else if ( val > 0 || input == "0" ) // numeric colour code: 196590
	{
		output.setColour(val);
	}
	else // string colour code
		output.setColour(input);
	if ( GFX::getBPP() < 24 ) // convert colour to lower bitdepth (slight accuracy loss)
		output.adjustToDisplayColour();
	return true;
}

bool BaseUnit::pLoadUintIDs(CRstring input, vector<string>& output)
{
	output.clear();
	vector<string> tokens;
	StringUtility::tokenize( input, tokens, DELIMIT_STRING );
	output.insert( output.begin(), tokens.begin(), tokens.end() );
	return true;
}

bool BaseUnit::pLoadTime(CRstring input, int& output)
{
	if (input[input.length()-1] == 'f')
		output = StringUtility::stringToInt(input.substr(0,input.length()-1));
	else if (input[input.length()-1] == 'r')
	{
		int temp = StringUtility::stringToInt(input.substr(0,input.length()-1));
		output = Random::nextInt(1, temp);
	}
	else // This is kinda fucked up, because of the frame based movement
		output = round(StringUtility::stringToFloat(input) / 1000.0f * (float)FRAME_RATE);
	return true;
}

bool BaseUnit::pIsRandomTime(CRstring input, int &output)
{
	if (input[input.length()-1] == 'r')
		output = StringUtility::stringToInt(input.substr(0,input.length()-1));
	else
		output = 0;
	return true;
}


void BaseUnit::move()
{
	if ( isTeleporting )
	{
		position = teleportPosition;
		isTeleporting = false;
	}
	else
		position += velocity;
}

void BaseUnit::loadState(SDL_Surface* surf, State state)
{
    AnimatedSprite* temp = new AnimatedSprite;
    temp->loadFrames(surf, tiles.x, tiles.y, state.start, state.length);
    temp->setTransparentColour(transCol);
    temp->setFrameRate(state.fps);
    temp->setLooping(state.loops);
    temp->setPlayMode(state.mode);
    if (states.find(state.name) != states.end())
		printf("Warning: State \"%s\" already present in unit with id %s, will be overridden.", state.name.c_str(), id.c_str());
    states[state.name] = temp;
}


bool BaseUnit::processOrder(Order& next)
{
    bool parsed = true;
    bool badOrder = false;

    if (next.randomTicks > 0)
		next.ticks = Random::nextInt(1, next.randomTicks);

    switch (next.key)
    {
    case okIdle:
    {
        velocity = Vector2df(0,0);
        acceleration[0] = Vector2df(0,0);
        acceleration[1] = Vector2df(0,0);
        break;
    }
    case okPosition:
    {
        if (next.params.size() < 3)
        {
            badOrder = true;
            break;
        }
        Vector2df dest = Vector2df(StringUtility::stringToFloat(next.params[1]),StringUtility::stringToFloat(next.params[2]));
        velocity = (dest - position) / (float)next.ticks; // set speed to pixels per framerate
        break;
    }
    case okRepeat:
    {
        if (currentOrder != 0) // prevent possible endless loop
        {
            currentOrder = 0;
            processOrder(orderList.front());
        }
        return true;
    }
    case okColour:
    {
        if (next.params.size() < 2)
        {
            badOrder = true;
            break;
        }
        tempColour.x = col.red;
        tempColour.y = col.green;
        tempColour.z = col.blue;

		Colour temp;
		pLoadColour( next.params[1], temp );

        tempColourChange.x = (float)(temp.red - col.red) / (float)next.ticks;
        tempColourChange.y = (float)(temp.green - col.green) / (float)next.ticks;
        tempColourChange.z = (float)(temp.blue - col.blue) / (float)next.ticks;
        break;
    }
    case okExplode:
    {
        explode();
        break;
    }
    case okRemove:
    {
        toBeRemoved = true;
        break;
    }
    case okIncrement:
    {
        if (next.params.size() < 3)
        {
            badOrder = true;
            break;
        }
        Vector2df dest = Vector2df(StringUtility::stringToFloat(next.params[1]),StringUtility::stringToFloat(next.params[2]));
        velocity = dest / (float)next.ticks; // set speed to pixels per framerate
        break;
    }
    case okParameter:
	{
		PARAMETER_TYPE param;
		int delimPos = next.params.front().find('=');
		param.first = next.params.front().substr(0, delimPos);
		param.second = next.params.front().substr(delimPos + 1);
		if (not processParameter(param))
		{
			printf("WARNING: Unprocessed parameter \"%s\" on unit with id \"%s\" in Order #%i\n",
					param.first.c_str(), id.c_str(), currentOrder + 1);
		}
		break;
	}
	case okSound:
	{
		MUSIC_CACHE->playSound(next.params.front(), parent->chapterPath, 0);
		break;
	}
	case okState:
	{
		switch (next.params.size())
		{
		case 0:
			printf("WARNING: Bad order parameter \"%s\" on unit with id \"%s\" in Order #%i\n",
					StringUtility::combine(next.params).c_str(), id.c_str(), currentOrder + 1);
			break;
		case 1:
			setSpriteState(next.params.front());
			break;
		case 2:
			setSpriteState(next.params[0], StringUtility::stringToBool(next.params[1]));
			break;
		default:
			setSpriteState(next.params[0], StringUtility::stringToBool(next.params[1]), next.params[2]);
		}
		break;
	}
    default:
        printf("ERROR: Unknown order key (Order #%i) on unit id \"%s\"\n", currentOrder + 1, id.c_str());
        if (currentOrder >= 0)
            orderList.erase(orderList.begin() + currentOrder);
        orderTimer = 1; // process next order in next cycle
		orderRunning = true;
        return false;
    }

    if (badOrder)
    {
    	string temp = StringUtility::combine(next.params, DELIMIT_STRING);
        printf("ERROR: Bad order parameter \"%s\" in Order #%i on unit id \"%s\"\n",
				temp.c_str(), currentOrder + 1, id.c_str());
        if (currentOrder >= 0)
            orderList.erase(orderList.begin() + currentOrder);
        orderTimer = 1; // process next order in next cycle
		orderRunning = true;
        return false;
    }

    orderTimer = next.ticks;
    orderRunning = true;
    return parsed;
}

bool BaseUnit::updateOrder(const Order& curr)
{
    bool parsed = true;

    switch (curr.key)
    {
    case okColour:
        tempColour += tempColourChange;
        col.red = tempColour.x;
        col.green = tempColour.y;
        col.blue = tempColour.z;
        break;
    default:
        parsed = false;
    }

    return parsed;
}

bool BaseUnit::finishOrder(const Order& curr)
{
    bool parsed = true;

    switch (curr.key)
    {
    case okPosition:
    {
        Vector2df dest = Vector2df(StringUtility::stringToFloat(curr.params[1]),
				StringUtility::stringToFloat(curr.params[2]));
        position = dest;
        velocity = Vector2df(0,0);
        break;
    }
    case okIncrement:
    {
        velocity = Vector2df(0,0);
        break;
    }
    default:
        parsed = false;
    }

    return parsed;
}
