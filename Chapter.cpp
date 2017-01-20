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

#include "Chapter.h"

#include <fstream>
#include <limits.h>

#include "StringUtility.h"
#include "FileLister.h"

#include "Savegame.h"
#include "fileTypeDefines.h"
#include "gameDefines.h"

Chapter::Chapter()
{
	filename = "";
	path = "";
	errorString = "";
	name = "";
	imageFile = "";
	autoDetect = false;

	stringToProp["name"] = cpName;
	stringToProp["image"] = cpImage;
	stringToProp["level"] = cpLevel;
	stringToProp["dialogue"] = cpDialogue;
	stringToProp["autodetect"] = cpAutoDetect;
}

Chapter::~Chapter()
{
	clear();
}

void Chapter::clear()
{
	levels.clear();
	filename = "";
	path = "";
	errorString = "";
	name = "";
	imageFile = "";
	dialogueFile = "";
}

bool Chapter::loadFromFile(CRstring filename)
{
	clear();

	printf("Trying to open chapter file \"%s\"\n",filename.c_str());

	string line;
	ifstream file(filename.c_str());
	int lineNumber = 0; // for error output

	if (file.fail())
	{
		errorString = "Failed to open file for read!";
		return false;
	}

	this->filename = filename;
	path = filename.substr(0,filename.find_last_of('/')+1);

	// parse file line by line
	while (getline(file,line))
	{
		++lineNumber;

		if (line.substr(0,COMMENT_STRING.length()) == COMMENT_STRING) // comment line - disregard
			continue;

		line = StringUtility::stripLineEndings(line);

		if (line[0] == 0) // empty line
			continue;

		vector<string> tokens;
		StringUtility::tokenize(line,tokens,VALUE_STRING);
		if (tokens.size() != 2)
		{
			printf("ERROR: Incorrect key-value pair on line %i\n",lineNumber);
		}
		else
		{
			tokens[0] = StringUtility::lower(tokens[0]);
			processParameter(make_pair(tokens[0],tokens[1]));
		}
	}

	if (file.is_open())
		file.close();

	// Check whether levels have to be "auto detected"
	if (autoDetect)
	{
		FileLister levelLister;
		levelLister.addFilter("txt");
		levelLister.setPath(path);

		vector<string> files;
		files = levelLister.getListing();
		// delete first element which is the current folder
		files.erase(files.begin());
		for (vector<string>::iterator I = files.begin(); I != files.end(); ++I)
		{
			// skip info.txt file and files already present
			if (*I != DEFAULT_CHAPTER_INFO_FILE &&
				getLevelIndex(path + *I) < 0)
			{
				levels.push_back(*I);
			}
		}
		files.clear();
	}

	// Check for missing initialisation
	if (name[0] == 0)
	{
		errorString = "Mandatory parameter \"name\" not set!";
		return false;
	}
	if (levels.empty())
	{
		errorString = "No levels added to chapter!";
		return false;
	}

	return true;
}

bool Chapter::saveToFile(string filename)
{
	if (filename[0] == 0)
		filename = this->filename;
	printf("Trying to save chapter file \"%s\"\n",filename.c_str());
	if (levels.empty() && !autoDetect)
	{
		errorString = "No levels added to this chapter and auto detect is set to false.";
		return false;
	}
	if (name[0] == 0)
	{
		errorString = "Chapter name not set.";
		return false;
	}
	if (filename.substr(filename.length()-8) != "info.txt")
		printf("WARNING: Chapter file has to be named ""info.txt"" to be detected by the game and show up in the chapter listing.\n");

	path = filename.substr(0,filename.find_last_of('/')+1);
	ofstream file;
	file.open(filename.c_str(), ios::out | ios::trunc);
	if (!file.is_open() || !file.good())
	{
		errorString = "I/O error.";
		return false;
	}
	file << "Name" << VALUE_STRING << name << "\n";
	if (imageFile[0] != 0)
		file << "Image" << VALUE_STRING << imageFile << "\n";
	if (dialogueFile[0] != 0)
		file << "Dialogue" << VALUE_STRING << dialogueFile << "\n";
	file << "AutoDetect" << VALUE_STRING << StringUtility::boolToString(autoDetect, true) << "\n";
	if (autoDetect && !levels.empty())
		printf("WARNING: This chapter has a list of levels and autoDetect set at the same time. AutoDetected levels will be added after the listed levels.\n");
	for (vector<string>::iterator I = levels.begin(); I != levels.end(); ++I)
	{
		file << "Level" << VALUE_STRING << (*I) << "\n";
	}
	file.close();
	this->filename = filename;
	return true;
}

string Chapter::getLevelFilename(CRint pos)
{
	errorString = "";

	if (pos < 0 || pos >= (int)levels.size())
	{
		errorString = "Level number out of range " + StringUtility::intToString(pos);
		return "";
	}
	return path + levels[pos];
}

int Chapter::getLevelIndex(CRstring filename)
{
	errorString = "";

	if (filename[0] == 0) // empty string
		return -1;

	for (int I = levels.size()-1; I >= 0; --I)
	{
		if (path + levels[I] == filename)
			return I;
	}
	errorString = "Level not found in chapter!";
	return -1;
}

int Chapter::getProgress() const
{
	if (filename[0] != 0)
	{
	#ifdef _DEBUG
		return INT_MAX;
	#else
		return SAVEGAME->getChapterProgress(filename);
	#endif
	}
	return -1;
}

string Chapter::getNextLevel(CRstring current)
{
	errorString = "";

	if (levels.empty())
	{
		errorString = "No chapter file was loaded!";
		return "";
	}
	if (current[0] == 0)
		return path + levels.front();

	for (int I = levels.size()-1; I >= 0; --I)
	{
		if (path + levels[I] == current)
		{
			if ((I+1) < levels.size())
				return path + levels[I+1];
			else // end of the chapter
				return "";
		}
	}

	errorString = "Current level not found in chapter!";
	return "";
}

string Chapter::getNextLevelAndSave(CRstring current)
{
	int index = getLevelIndex(current);

	if (errorString[0] != 0) // we have an error
		return "";

	if (index < (int)levels.size()-1) // there is a next level
	{
		SAVEGAME->setChapterProgress(filename,++index);
		return path + levels[index];
	}

	return "";
}

/// ---protected---

bool Chapter::processParameter(const PARAMETER_TYPE& value)
{
	bool parsed = true;

	switch (stringToProp[value.first])
	{
	case cpName:
	{
		name = value.second;
		break;
	}
	case cpImage:
	{
		imageFile = value.second;
		break;
	}
	case cpLevel:
	{
		if (getLevelIndex(path + value.second) < 0)
			levels.push_back(value.second);
		else
			printf("WARNING: Duplicate level \"%s\" on chapter \"%s\"\n",value.second.c_str(),name.c_str());
		break;
	}
	case cpDialogue:
	{
		dialogueFile = value.second;
		break;
	}
	case cpAutoDetect:
	{
		autoDetect = StringUtility::stringToBool(value.second);
		break;
	}
	}

	return parsed;
}
