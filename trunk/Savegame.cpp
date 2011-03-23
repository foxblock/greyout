#include "Savegame.h"

#include <fstream>

#include "StringUtility.h"

#include "fileTypeDefines.h"

Savegame* Savegame::self = NULL;

Savegame::Savegame()
{
    autoSave = true;
    filename = "";
}

Savegame::~Savegame()
{
    if (filename[0] != 0)
        save();
    clear();
}

Savegame* Savegame::getSavegame()
{
    if (not self)
        self = new Savegame();
    return self;
}

///---public---

bool Savegame::setFile(CRstring filename)
{
    string line;
    ifstream file(filename.c_str());

    if (file.fail())
    {
        cout << "Failed to open file for read: \"" << filename << "\"" << endl;
        // file does not exists -> try to open for write
        ofstream file2(filename.c_str());
        if (file.fail())
        {
            cout << "Failed to reserve savegame file!" << endl;
            return false;
        }
        this->filename = filename;
        if (file2.is_open())
            file2.close();

        return true;
    }

    // parse file line by line
    while (getline(file,line))
    {
        line = StringUtility::stripLineEndings(line);
        line = crypt.decryptBuffer(line);

        if (line.substr(0,COMMENT_STRING.length()) == COMMENT_STRING) // comment line - disregard
            continue;

        vector<string> tokens;
        StringUtility::tokenize(line,tokens,VALUE_STRING);
        if (tokens.size() == 2)
        {
            data[tokens.at(0)] = tokens.at(1);
        }
    }

    if (file.is_open())
        file.close();

    cout << "Savegame successfully loaded!" << endl;

    this->filename = filename;
    return true;
}

bool Savegame::save()
{
    string line;
    ofstream file(filename.c_str());

    if (file.fail())
    {
        cout << "Failed to open file for write: \"" << filename << "\"" << endl;
        return false;
    }

    for (map<string,string>::const_iterator iter = data.begin(); iter != data.end(); ++iter)
    {
        line = iter->first + VALUE_STRING + iter->second;
        line = crypt.encryptBuffer(line);

        file << line << "\n";
    }

    if (file.is_open())
    {
        file.close();
        cout << "Progress saved!" << endl;
        return true;
    }

    return false;
}

bool Savegame::clear()
{
    data.clear();

    if (autoSave)
        return save();

    return true;
}

string Savegame::getData(CRstring key) const
{
    map<string,string>::const_iterator iter = data.find(key);

    if (iter != data.end())
        return iter->second;
    return "";
}

bool Savegame::writeData(CRstring key, CRstring value, CRbool overwrite)
{
    if (not overwrite)
    {
        map<string,string>::const_iterator iter = data.find(key);
        if (iter != data.end())
            return false;
    }
    data[key] = value;

    if (autoSave)
        return save();

    return true;
}

int Savegame::getChapterProgress(CRstring chapterFile) const
{
    string value = getData(chapterFile);
    // will return 0 on an empty string (e.g. when chapterFile not found in savegame)
    return StringUtility::stringToInt(value);
}

bool Savegame::setChapterProgress(CRstring chapterFile, CRint progress, CRbool overwrite)
{
    if (not overwrite)
    {
        int value = getChapterProgress(chapterFile);
        if (progress <= value)
            return false;
    }
    return writeData(chapterFile,StringUtility::intToString(progress),true);
}

///---protected---

///---private---
