#include "Chapter.h"

#include <fstream>

#include "StringUtility.h"

#include "Savegame.h"
#include "fileTypeDefines.h"

Chapter::Chapter()
{
    filename = "";
    path = "";
    errorString = "";
    name = "";
    imageFile = "";

    stringToProp["name"] = cpName;
    stringToProp["image"] = cpImage;
    stringToProp["level"] = cpLevel;
    stringToProp["dialogue"] = cpDialogue;
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

    cout << "Trying to open chapter file \"" << filename << "\"" << endl;

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

        vector<string> tokens;
        StringUtility::tokenize(line,tokens,VALUE_STRING);
        if (tokens.size() != 2)
        {
            cout << "Error: Incorrect key-value pair on line " << lineNumber << endl;
        }
        else
        {
            tokens.at(0) = StringUtility::lower(tokens.at(0));
            processParameter(make_pair(tokens.at(0),tokens.at(1)));
        }
    }

    if (file.is_open())
        file.close();

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

string Chapter::getLevelFilename(CRint pos)
{
    errorString = "";

    if (pos < 0 || pos >= (int)levels.size())
    {
        errorString = "Level number out of range " + StringUtility::intToString(pos);
        return "";
    }
    return path + levels.at(pos);
}

int Chapter::getLevelIndex(CRstring filename)
{
    errorString = "";

    if (filename[0] == 0) // empty string
        return -1;

    for (int I = levels.size()-1; I >= 0; --I)
    {
        if (path + levels.at(I) == filename)
            return I;
    }
    errorString = "Level not found in chapter!";
    return -1;
}

int Chapter::getProgress() const
{
    if (filename[0] != 0)
        return SAVEGAME->getChapterProgress(filename);
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
        if (path + levels.at(I) == current)
        {
            if ((I+1) < levels.size())
                return path + levels.at(I+1);
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
        return path + levels.at(index);
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
        levels.push_back(value.second);
        break;
    }
    case cpDialogue:
    {
        dialogueFile = value.second;
        break;
    }
    }

    return parsed;
}
