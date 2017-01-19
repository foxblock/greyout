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

#ifndef CHAPTER_H
#define CHAPTER_H

#include <vector>
#include <map>
#include <string>

#include "PenjinTypes.h"

#include "fileTypeDefines.h"

/**
Organizing and ordering levels loaded from a chapter information file
Also displaying a name and image if needed
Additionally providing other classes with variables for file loading (such as the chapter path)

Chapters are a bunch of levels put together in a folder, they can use their own
graphic set and sounds as well as override or use the default ones
You can order the levels in the info.txt file
User progress will be saved in global savegame file
**/

using namespace std;

class Chapter
{
public:
	Chapter();
	virtual ~Chapter();

	virtual void clear();

	// Load a chapter from a file (name and at least one level are mandatory)
	// returns true on success, false otherwise (and sets errorString variable)
	virtual bool loadFromFile(CRstring filename);
	// Get the level at index pos, returns "" if out of bounds
	virtual string getLevelFilename(CRint pos);
	// Get the index of the passed level, returns -1 if not found (or empty string)
	virtual int getLevelIndex(CRstring filename);

	// Load the user's progress in this chapter from the savegame file
	virtual int getProgress() const;
	// Get the next level after the passed one
	//returns "" if not found, at the end of the chapter or on error
	virtual string getNextLevel(CRstring current);
	// returns the next level after the passed one and saves the progress to the savegame file
	// returns "" on error or ending of chapter
	virtual string getNextLevelAndSave(CRstring current);

	string filename; // info.txt filename and path
	string path; // path to chapter folder (where info.txt is in) including trailing backslash
	string name; // name of the chapter (mandatory)
	string imageFile; // image displayed in the chapter menu
	string dialogueFile; // file containing the strings loaded as dialogue
	bool autoDetect; // make this chapter auto-detect level files in the folder

	// used for error output
	string errorString;

	vector<string> levels;

	enum ChapterProp
	{
		cpUnknown,
		cpName,
		cpImage,
		cpLevel,
		cpDialogue,
		cpAutoDetect
	};
	map<string,int> stringToProp;
protected:
	bool processParameter(const PARAMETER_TYPE& value);

private:

};


#endif // CHAPTER_H

