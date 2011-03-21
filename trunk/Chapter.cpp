#include "Chapter.h"

#include <fstream>

#include "StringUtility.h"

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

    return true;
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
            else
                return "";
        }
    }

    errorString = "Current level not found in chapter!";
    return "";
}

string Chapter::getLevelFilename(CRint pos)
{
    if (pos < 0 || pos >= levels.size())
    {
        errorString = "Level number out of range " + StringUtility::intToString(pos);
        return "";
    }
    return path + levels.at(pos);
}

/// ---protected---

bool Chapter::processParameter(const pair<string,string>& value)
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
    }

    return parsed;
}
