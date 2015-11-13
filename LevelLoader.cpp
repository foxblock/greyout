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

#include "LevelLoader.h"

#include <fstream>
#include <vector>

#include "StringUtility.h"
#include "Image.h"

#include "userStates.h"
#include "GreySurfaceCache.h"
#include "SimpleFlags.h"

// Level classes
#include "Level.h"
#include "Benchmark.h"
#include "Playground.h"

// Unit classes
#include "BaseUnit.h"
#include "ControlUnit.h"
#include "BasePlayer.h"
#include "PushableBox.h"
#include "SolidBox.h"
#include "Exit.h"
#include "DialogueTrigger.h"
#include "Gear.h"
#include "Switch.h"
#include "Key.h"
#include "BaseTrigger.h"
#include "ExitTrigger.h"
#include "SoundTrigger.h"
#include "CameraTrigger.h"
#include "TextObject.h"
#include "FadingBox.h"
#include "LevelTrigger.h"
#include "ParticleEmitter.h"
#include "ControlSprite.h"

using namespace std;

// mapping the ident string used in the map file to a ident integer for use in
// a switch statement for cleaner and faster map loading
map<string, int> LevelLoader::dataIdents;
map<string, int> LevelLoader::levelClasses;
map<string, int> LevelLoader::playerClasses;
map<string, int> LevelLoader::unitClasses;

void LevelLoader::createIdentMaps()
{
	dataIdents["level"] = diLevel;
	dataIdents["player"] = diPlayer;
	dataIdents["unit"] = diUnit;

	levelClasses["generic"] = lcGeneric;
	levelClasses["benchmark"] = lcBenchmark;
	levelClasses["playground"] = lcPlayground;

	playerClasses["generic"] = pcGeneric;
	playerClasses["black"] = pcBlack;
	playerClasses["white"] = pcWhite;

	unitClasses["generic"] = ucGeneric;
	unitClasses["pushablebox"] = ucPushableBox;
	unitClasses["solidbox"] = ucSolidBox;
	unitClasses["exit"] = ucExit;
	unitClasses["dialoguetrigger"] = ucDialogueTrigger;
	unitClasses["gear"] = ucGear;
	unitClasses["switch"] = ucSwitch;
	unitClasses["key"] = ucKey;
	unitClasses["basetrigger"] = ucBaseTrigger;
	unitClasses["exittrigger"] = ucExitTrigger;
	unitClasses["soundtrigger"] = ucSoundTrigger;
	unitClasses["cameratrigger"] = ucCameraTrigger;
	unitClasses["text"] = ucTextObject;
	unitClasses["fadingbox"] = ucFadingBox;
	unitClasses["leveltrigger"] = ucLevelTrigger;
	unitClasses["emitter"] = ucEmitter;
	unitClasses["controlsprite"] = ucControlSprite;
}

LevelLoader* LevelLoader::self = NULL;

LevelLoader::LevelLoader()
{
	errorString = "";
	createIdentMaps();
}

LevelLoader::~LevelLoader()
{
	//
}

LevelLoader* LevelLoader::getLevelLoader()
{
	if (not self)
		self = new LevelLoader();
	return self;
}

Level* LevelLoader::loadLevelFromFile(CRstring filename, CRstring chapterPath)
{
	printf("---------------------------------------------------------\n");
	printf("Trying to load level file \"%s\"\n",filename.c_str());

	string line;
	ifstream file(filename.c_str());
	int lineNumber = 0; // for error output
	errorString = "";

	if (file.fail())
	{
		errorString = "Failed to open file for read!";
		return NULL;
	}

	ErrorCode error = ecNone;

	string field = ""; // the current field
	list<PARAMETER_TYPE > params; // list of field parameters (key=value)
	Level* level = NULL;

	// parse file line by line
	while (file.good())
	{
		getline(file,line);
		++lineNumber;

		// solve win-lin compatibility issues
		line = StringUtility::stripLineEndings(line);
		// make case in-sensitive
		line = StringUtility::lower(line);

		// A value in [] brackets indicates a new field
		// all key=value pairs following will be loaded in a map as parameters
		string nextField = "";
		if (line.substr(0,COMMENT_STRING.length()) == COMMENT_STRING)
		{
			// comment line - disregard
		}
		else if (line.substr(0,FIELD_STRING.length()) == FIELD_STRING) // new field
		{
			// strip brackets
			nextField = line.substr(1,line.length()-2);
		}
		else if (line[0] == 0) // empty line
		{
			// disregard
		}
		else // key=value pair
		{
			if (field[0] == 0)
			{
				errorString = "Parameter outside of field specification on line "
							  + StringUtility::intToString(lineNumber);
				error = ecCritical;
			}
			else
			{
				// add pair to the map
				vector<string> tokens;
				StringUtility::tokenize(line,tokens,VALUE_STRING,2);
				if (tokens.size() != 2)
				{
					printf("ERROR: Incorrect key-value pair on line %i\n",lineNumber);
					error = ecWarning;
				}
				else
				{
					// make sure class parameter is always the first element
					if (tokens[0] == CLASS_STRING)
						params.push_front(make_pair(tokens[0],tokens[1]));
					else
						params.push_back(make_pair(tokens[0],tokens[1]));
				}
			}
		}

		// add current field with parameters to the level when reaching a new field or the end of the file
		if ((nextField[0] != 0) && (field[0] != 0) || not (file.good()))
		{
			switch (dataIdents[field])
			{
			case diLevel:
			{
				params.push_back(make_pair("filename",filename));
				level = createLevel(params,chapterPath,lineNumber);
				if (not level)
				{
					if (errorString[0] == 0)
						errorString = "Level creation failed!";
					error = ecCritical;
				}
				break;
			}
			case diPlayer:
			{
				if (not level)
				{
					errorString = "The level field has to be specified first! After that you can add units and stuff...";
					error = ecCritical;
					break;
				}

				ControlUnit* player = createPlayer(params,level,lineNumber);
				if (player)
					level->players.push_back(player);
				else
					error = ecWarning;
				break;
			}
			case diUnit:
			{
				if (not level)
				{
					errorString = "The level field has to be specified first! After that you can add units and stuff...";
					error = ecCritical;
					break;
				}

				BaseUnit* unit = createUnit(params,level,lineNumber);
				if (unit)
					level->units.push_back(unit);
				else
					error = ecWarning;
				break;
			}
			default:
			{
				printf("ERROR: Unknown field \"%s\" ending on line %i\n",field.c_str(),lineNumber);
				error = ecWarning;
			}
			}
		}

		if (nextField[0] != 0) // set the next field loaded above
		{
			field = nextField;
			params.clear();
		}

		if (error == ecCritical) // encountered a critical error, abort
			break;
	} // while


	if (file.is_open())
		file.close();

	// Additional check for missing initialisation
	if (level)
	{
		if (level->players.empty())
		{
			printf("ERROR: No player unit has been specified in the map, what did you plan to play with?\n");
			error = ecWarning;
		}
		vector<ControlUnit*>::const_iterator unit;
		for (unit = level->players.begin(); unit != level->players.end(); ++unit)
		{
			if ((*unit)->takesControl)
				break;
		}
		if (unit == level->players.end())
		{
			printf("WARNING: No controlable unit found! Push the buttons...(nothing happens)\n");
			error = ecWarning;
		}
		printf("Level has %i players and %i units.\n",level->players.size(),level->units.size());
	}

	switch (error)
	{
	case ecNone:
		printf("Successfully loaded level file!\n");
		break;
	case ecWarning:
		printf("WARNING: Finished loading level file with minor errors!\n");
		break;
	case ecCritical:
		// clean up and return NULL on critical error
		printf("ERROR: Encountered critical error while loading map!\n");
		delete level;
		level = NULL;
		break;
	default:
		break;
	}

	return level;
}

Level* LevelLoader::createLevel(list<PARAMETER_TYPE >& params, CRstring chapterPath, CRint lineNumber)
{
	Level* result = NULL;

	if (params.front().first != CLASS_STRING)
	{
		printf("ERROR: Level definition ending on line %i missing mandatory parameter \"%s\"\n",lineNumber,CLASS_STRING.c_str());
		return NULL;
	}

	switch (levelClasses[params.front().second])
	{
	case lcGeneric:
	{
		result = new Level;
		break;
	}
	case lcBenchmark:
	{
		result = new Benchmark;
		break;
	}
	case lcPlayground:
	{
		result = new Playground;
		break;
	}
	default:
	{
		printf("ERROR: Unknown level class \"%s\"\n",params.front().second.c_str());
		return NULL;
	}
	}

	result->chapterPath = chapterPath;
	result->parameters.insert(result->parameters.begin(),params.begin(),params.end());
	if (not result->load(params))
	{
		errorString = result->errorString;
		delete result;
		result = NULL;
	}

	return result;
}

ControlUnit* LevelLoader::createPlayer(list<PARAMETER_TYPE >& params, Level* const parent, CRint lineNumber)
{
	ControlUnit* result = NULL;

	if (params.front().first != CLASS_STRING)
	{
		printf("ERROR: Player definition ending on line %i missing mandatory parameter \"%s\"\n",lineNumber,CLASS_STRING.c_str());
		return NULL;
	}

	switch (playerClasses[params.front().second])
	{
	case pcGeneric:
	{
		result = new BasePlayer(parent);
		break;
	}
	case pcBlack:
	{
		// unit specific parameters as this is merely an alias for a customized BasePlayer
		params.push_front(make_pair("collision","black,red"));
		params.push_front(make_pair("imageoverwrite","images/player/black_big.png"));
		params.push_front(make_pair("colour","black"));
		params.push_front(make_pair("tiles","8,9"));
		params.push_front(make_pair("transparentcolour","255r0g255b"));
		params.push_front(make_pair("state","default,0,1,10,0,normal"));
		params.push_front(make_pair("state","stand,0,1,10,0,normal"));
		params.push_front(make_pair("state","wave,1,3,15,1,pulse"));
		params.push_front(make_pair("state","fallright,32,7,10,-1,reverse"));
		params.push_front(make_pair("state","turnright,8,3,10,0,normal"));
		params.push_front(make_pair("state","pushright,48,7,7,-1,normal"));
		params.push_front(make_pair("state","runright,16,6,10,-1,pulse"));
		params.push_front(make_pair("state","jumpright,64,2,10,0,normal"));
		params.push_front(make_pair("state","flyright,65,1,10,0,normal"));
		params.push_front(make_pair("state","fallleft,40,7,10,-1,reverse"));
		params.push_front(make_pair("state","turnleft,11,3,10,0,normal"));
		params.push_front(make_pair("state","pushleft,56,7,7,-1,normal"));
		params.push_front(make_pair("state","runleft,24,6,10,-1,pulse"));
		params.push_front(make_pair("state","jumpleft,66,2,10,0,normal"));
		params.push_front(make_pair("state","flyleft,67,1,10,0,normal"));
		result = new BasePlayer(parent);
		break;
	}
	case pcWhite:
	{
		params.push_front(make_pair("collision","red,white"));
		params.push_front(make_pair("imageoverwrite","images/player/white_big.png"));
		params.push_front(make_pair("colour","white"));
		params.push_front(make_pair("tiles","8,9"));
		params.push_front(make_pair("transparentcolour","255r0g255b"));
		params.push_front(make_pair("state","default,0,1,10,0,normal"));
		params.push_front(make_pair("state","stand,0,1,10,0,normal"));
		params.push_front(make_pair("state","wave,1,3,15,1,pulse"));
		params.push_front(make_pair("state","fallright,32,7,10,-1,reverse"));
		params.push_front(make_pair("state","turnright,8,3,10,0,normal"));
		params.push_front(make_pair("state","pushright,48,7,7,-1,normal"));
		params.push_front(make_pair("state","runright,16,6,10,-1,pulse"));
		params.push_front(make_pair("state","jumpright,64,2,10,0,normal"));
		params.push_front(make_pair("state","flyright,65,1,10,0,normal"));
		params.push_front(make_pair("state","fallleft,40,7,10,-1,reverse"));
		params.push_front(make_pair("state","turnleft,11,3,10,0,normal"));
		params.push_front(make_pair("state","pushleft,56,7,7,-1,normal"));
		params.push_front(make_pair("state","runleft,24,6,10,-1,pulse"));
		params.push_front(make_pair("state","jumpleft,66,2,10,0,normal"));
		params.push_front(make_pair("state","flyleft,67,1,10,0,normal"));
		result = new BasePlayer(parent);
		break;
	}
	default:
		printf("ERROR: Unknown player class \"%s\" on line %i\n", params.front().second.c_str(),lineNumber);
		return NULL;
	}

	result->parameters.insert(result->parameters.begin(),params.begin(),params.end());
	if (not result->load(result->parameters))
	{
		printf("ERROR loading unit id \"%s\"\n",result->id.c_str());
		delete result;
		result = NULL;
	}

	return result;
}


BaseUnit* LevelLoader::createUnit(list<PARAMETER_TYPE >& params, Level* const parent, CRint lineNumber)
{
	BaseUnit* result = NULL;

	if (params.front().first != CLASS_STRING)
	{
		printf("ERROR: Unit definition ending on line %i missing mandatory parameter \"%s\"\n",lineNumber,CLASS_STRING.c_str());
		return NULL;
	}

	switch (unitClasses[params.front().second])
	{
	case ucGeneric:
	{
		result = new BaseUnit(parent);
		break;
	}
	case ucPushableBox:
	{
		result = new PushableBox(parent);
		break;
	}
	case ucSolidBox:
	{
		result = new SolidBox(parent);
		break;
	}
	case ucExit:
	{
		result = new Exit(parent);
		break;
	}
	case ucDialogueTrigger:
	{
		result = new DialogueTrigger(parent);
		break;
	}
	case ucGear:
	{
		result = new Gear(parent);
		break;
	}
	case ucSwitch:
	{
		result = new Switch(parent);
		break;
	}
	case ucKey:
	{
		result = new Key(parent);
		break;
	}
	case ucBaseTrigger:
	{
		result = new BaseTrigger(parent);
		break;
	}
	case ucExitTrigger:
	{
		result = new ExitTrigger(parent);
		break;
	}
	case ucSoundTrigger:
	{
		result = new SoundTrigger(parent);
		break;
	}
	case ucCameraTrigger:
	{
		result = new CameraTrigger(parent);
		break;
	}
	case ucTextObject:
	{
		result = new TextObject(parent);
		break;
	}
	case ucFadingBox:
	{
		result = new FadingBox(parent);
		break;
	}
	case ucLevelTrigger:
	{
		result = new LevelTrigger(parent);
		break;
	}
	case ucEmitter:
	{
		result = new ParticleEmitter(parent);
		break;
	}
	case ucControlSprite:
	{
		result = new ControlSprite(parent);
		break;
	}
	default:
		printf("ERROR: Unknown unit class \"%s\" on line %i\n",params.front().second.c_str(),lineNumber);
		return NULL;
	}

	// I forgot why I copy the parameters here (and have the variable public)
	// instead of in the load function... but I am sure there is a good reason
	// for that (chances are it's just me being stupid, though)
	result->parameters.insert(result->parameters.begin(),params.begin(),params.end());

	if (not result->load(result->parameters))
	{
		printf("ERROR loading unit id \"%s\"\n",result->id.c_str());
		delete result;
		result = NULL;
	}

	return result;
}
