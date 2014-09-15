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

#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <list>

#include "PenjinTypes.h"

/**
A "factory" class which produces units and levels from a set of parameters
usually read from a level file
**/

#define LEVEL_LOADER LevelLoader::getLevelLoader()

#include "fileTypeDefines.h"

class Level;
class ControlUnit;
class BaseUnit;

using namespace std;

class LevelLoader
{
private:
	LevelLoader();
	static LevelLoader *self;
public:
	virtual ~LevelLoader();
	static LevelLoader* getLevelLoader();

// parses a level file and sets a level object accordingly (creates units, etc.)
// returns a pointer to the created level object or NULL on failure
	Level* loadLevelFromFile(CRstring filename, CRstring chapterPath="");

// Creates a level object amd load parameters such as flags, image, etc.
// parameter loading depending on Level::load implementation
	Level* createLevel(list<PARAMETER_TYPE >& params, CRstring chapterPath, CRint lineNumber=-1);

// Creates a player object from a list of parameters
// parameter parsing depending on BaseUnit::load (or inherited function) implementation
// LineNumber solely for error output
	ControlUnit* createPlayer(list<PARAMETER_TYPE >& params, Level* const parent, CRint lineNumber=-1);

// Creates a unit object from a list of parameters
// parameter parsing depending on BaseUnit::load (or inherited function) implementation
	BaseUnit* createUnit(list<PARAMETER_TYPE >& params, Level* const parent, CRint lineNumber=-1);

// critical errors will get stored here ready to be read and shouted into the
// angry face of the user
	string errorString;

	enum ErrorCode
	{
		ecNone,
		ecWarning,
		ecCritical,
		ecFile
	};

};

#endif // LEVELLOADER_H
