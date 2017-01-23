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

#include "Switch.h"

#include "Level.h"
#include "ControlUnit.h"
#include "MusicCache.h"
#include "MyGame.h"

#define SWITCH_TIMEOUT 30

map<string,int> Switch::stringToFunc;

Switch::Switch(Level* newParent) : BaseUnit(newParent)
{
	flags.addFlag(ufNoMapCollision);
	flags.addFlag(ufNoGravity);
	unitCollisionMode = 0;

	col = Colour(50,217,54);
	collisionColours.insert(Colour(BLACK).getIntColour());
	collisionColours.insert(Colour(WHITE).getIntColour());
	switchTimer = 0;
	switchOn = NULL;
	switchOff = NULL;

	linkTimer = 0;

	stringToProp["function"] = spFunction;

	stringToFunc["movement"] = sfMovement;
	stringToFunc["parameter"] = sfParameter;
	stringToFunc["parameteron"] = sfParameterOn;
	stringToFunc["parameteroff"] = sfParameterOff;
}

Switch::~Switch()
{
	targets.clear();
	targetIDs.clear();
}

///---public---

bool Switch::load(list<PARAMETER_TYPE >& params)
{
	bool result = BaseUnit::load(params);

	if (imageOverwrite[0] == 0)
	{
		imageOverwrite = "images/units/switch.png";
	}
	else // clear sprites loaded by BaseUnit
	{
		for (map<string,AnimatedSprite*>::iterator I = states.begin(); I != states.end(); ++I)
		{
			delete I->second;
		}
		states.clear();
	}
	AnimatedSprite* temp = new AnimatedSprite;
	temp->loadFrames(getSurface(imageOverwrite),2,1,0,1);
	temp->setTransparentColour(MAGENTA);
	states["off"] = temp;
	temp = new AnimatedSprite;
	temp->loadFrames(getSurface(imageOverwrite),2,1,1,1);
	temp->setTransparentColour(MAGENTA);
	states["on"] = temp;

	if (startingState[0] == 0 || startingState == "default")
		startingState = "off";
	setSpriteState(startingState,true);

	if ((!switchOn || !switchOff) && ENGINE->currentState != STATE_EDITOR)
	{
		printf("WARNING: No Function specified for switch with ID \"%s\"\n",id.c_str());
	}
	if (targetIDs.empty() && ENGINE->currentState != STATE_EDITOR)
	{
		printf("ERROR: No targets specified for switch with ID \"%s\"\n",id.c_str());
		result = false;
	}

	return result;
}

bool Switch::processParameter(const PARAMETER_TYPE& value)
{
	switch (stringToProp[value.first])
	{
	case spFunction:
	{
		vector<string> tokens;
		StringUtility::tokenize(value.second,tokens,DELIMIT_STRING,2);
		switch (stringToFunc[tokens.front()])
		{
		case sfMovement:
			switchOn = &Switch::movementOn;
			switchOff = &Switch::movementOff;
			break;
		case sfParameter:
		{
			switchOn = &Switch::parameterOn;
			switchOff = &Switch::parameterOff;
			vector<string> temp;
			StringUtility::tokenize(tokens.back(),temp,VALUE_STRING,2);
			if (tokens.size() != 2 || temp.size() != 2)
			{
				printf("ERROR: Invalid switch parameters (needs a key=value pair)!\n");
				return false;
			}
			paramOn.first = temp.front();
			paramOn.second = temp.back();
			paramOff.first = temp.front();
			paramOff.second = temp.back();
			break;
		}
		case sfParameterOn:
		{
			switchOn = &Switch::parameterOn;
			vector<string> temp;
			StringUtility::tokenize(tokens.back(),temp,VALUE_STRING,2);
			if (tokens.size() != 2 || temp.size() != 2)
			{
				printf("ERROR: Invalid switch parameters (needs a key=value pair)!\n");
				return false;
			}
			paramOn.first = temp.front();
			paramOn.second = temp.back();
			break;
		}
		case sfParameterOff:
		{
			switchOff = &Switch::parameterOff;
			vector<string> temp;
			StringUtility::tokenize(tokens.back(),temp,VALUE_STRING,2);
			if (tokens.size() < 2 || temp.size() < 2)
			{
				printf("ERROR: Invalid switch parameters (needs a key=value pair)!\n");
				return false;
			}
			paramOff.first = temp.front();
			paramOff.second = temp.back();
			break;
		}
		default:
			printf("Unknown function parameter for switch \"%s\"\n",id.c_str());
		}
		return true;
	}
	case BaseUnit::upTarget:
	{
		if (!pLoadUintIDs(value.second, targetIDs))
		{
			printf("ERROR loading unit IDs!\n");
			return false;
		}
		return true;
	}
	default:
		return BaseUnit::processParameter(value);
	}
}

void Switch::generateParameters()
{
	BaseUnit::generateParameters();
	if (switchOn != NULL)
	{
		if (switchOn == &Switch::movementOn)
			parameters.push_back(make_pair("function", "movement"));
		else if (switchOn == &Switch::parameterOn)
		{
			parameters.push_back(make_pair("function", "parameteron" + DELIMIT_STRING + paramOn.first + VALUE_STRING + paramOn.second));
			if (switchOff != NULL)
				parameters.push_back(make_pair("function", "parameteroff" + DELIMIT_STRING + paramOff.first + VALUE_STRING + paramOff.second));
		}
	}
	if (!targetIDs.empty())
	{
		string temp = "";
		for (vector<string>::iterator I = targetIDs.begin(); I != targetIDs.end(); ++I)
			temp += (*I) + DELIMIT_STRING;
		temp.erase(temp.length()-1);
		parameters.push_back(make_pair("target", temp));
	}
}

void Switch::reset()
{
	if (startingState == "off" && switchOff)
		for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
			(this->*switchOff)(*I);
	else if (switchOn)
		for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
			(this->*switchOn)(*I);
	switchTimer = 0;
	linkTimer = 0;
	BaseUnit::reset();
}

void Switch::update()
{
	if (!targetIDs.empty())
	{
		targets.clear();
		parent->getUnitsByID(targetIDs,targets);

		if (startingState == "off" && switchOff)
			for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
				(this->*switchOff)(*I);
		else if (switchOn)
			for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
				(this->*switchOn)(*I);

		targetIDs.clear();
	}

	if (switchTimer > 0)
		--switchTimer;
	if (linkTimer > 0)
	{
		--linkTimer;
		if (linkTimer == 0)
			parent->removeLink(this);
	}
	BaseUnit::update();
}

void Switch::hitUnit(const UnitCollisionEntry& entry)
{
	// standing still on the ground
	if (entry.unit->isPlayer && (int)entry.unit->velocity.x == 0 && abs(entry.unit->velocity.y) < 4 &&
		entry.overlap.x > 10 && ((ControlUnit*)entry.unit)->takesControl)
	{
		if (parent->getInput()->isUp() && switchTimer == 0)
		{
			if (currentState == "off")
			{
				setSpriteState("on",true);
				if (switchOn)
					for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
						(this->*switchOn)(*I);
				MUSIC_CACHE->playSound("sounds/switch.wav",parent->chapterPath);
			}
			else
			{
				setSpriteState("off",true);
				if (switchOff)
					for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
						(this->*switchOff)(*I);
				MUSIC_CACHE->playSound("sounds/switch_off.wav",parent->chapterPath);
			}
			switchTimer = SWITCH_TIMEOUT;
		}
		if (linkTimer == 0) // no links shown, player needs to stand still once to activate them
		{
			for (vector<BaseUnit*>::iterator I = targets.begin(); I != targets.end(); ++I)
				parent->addLink(this,*I);
			linkTimer = 3;
		}
	}
	if (linkTimer > 0 && entry.unit->isPlayer && ((ControlUnit*)entry.unit)->takesControl) // once activated, show until player leaves switch area
		linkTimer = 3;
}

///---protected---

void Switch::movementOn(BaseUnit* unit)
{
	unit->flags.removeFlag(BaseUnit::ufNoUpdate);
}

void Switch::movementOff(BaseUnit* unit)
{
	unit->flags.addFlag(BaseUnit::ufNoUpdate);
}

void Switch::parameterOn(BaseUnit* unit)
{
	if (paramOn.first == "state")
	{
		unit->setSpriteState(paramOn.second,true);
		return;
	}
	if (paramOn.first == "order")
		unit->resetOrder(true);
	unit->processParameter(paramOn);
	if (paramOn.first == "order")
		unit->resetOrder(false);
}

void Switch::parameterOff(BaseUnit* unit)
{
	if (paramOff.first == "state")
	{
		unit->setSpriteState(paramOff.second,true);
		return;
	}
	if (paramOff.first == "order")
		unit->resetOrder(true);
	unit->processParameter(paramOff);
	if (paramOff.first == "order")
		unit->resetOrder(false);
}

///---private---
