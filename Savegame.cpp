#include "Savegame.h"

#include <fstream>
#include "StringUtility.h"
#include "fileTypeDefines.h"

#define SAVE_VERSION 2

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
        printf("Failed to open file for read: \"%s\"\n",filename.c_str());
        // file does not exists -> try to open for write
        ofstream file2(filename.c_str());
        if (file2.fail())
        {
            printf("Failed to reserve save file!\n");
            return false;
        }
        this->filename = filename;
        if (file2.is_open())
            file2.close();

        return true;
    }

    // check save file version and check for encryption
    getline(file,line);
	line = StringUtility::stripLineEndings(line);
    if (line[0] == 0 || line.size() > 2 || StringUtility::stringToInt(line) != SAVE_VERSION)
	{
		printf("Old save file detected! Contents will be overwritten on next save operation!\n");
		file.close();
		this->filename = filename;
		return true;
	}
	getline(file,line);
	line = StringUtility::stripLineEndings(line);
	bool encrypted = StringUtility::stringToBool(line);

    // parse file line by line
    while (getline(file,line))
    {
        line = StringUtility::stripLineEndings(line);
        if (encrypted)
			line = crypt.decryptBuffer(line);

        if (line.substr(0,COMMENT_STRING.length()) == COMMENT_STRING) // comment line - disregard
            continue;

        vector<string> tokens;
        StringUtility::tokenize(line,tokens,VALUE_STRING);
        if (tokens.size() == 2)
        {
            data[tokens[0]] = tokens[1];
        }
    }

    if (file.is_open())
        file.close();

    printf("Save file successfully loaded!\n");

    this->filename = filename;
    return true;
}

bool Savegame::save()
{
    string line;
    ofstream file(filename.c_str());

    if (file.fail())
    {
        printf("Failed to open file for write: \"%s\"\n",filename.c_str());
        return false;
    }
    // write file version and encryption status
    file << SAVE_VERSION << "\n" <<
    #ifdef _DEBUG
    "false"
    #else
    "true"
    #endif // _DEBUG
    << "\n";

    for (map<string,string>::const_iterator iter = data.begin(); iter != data.end(); ++iter)
    {
        line = iter->first + VALUE_STRING + iter->second;
        #ifndef _DEBUG
		line = crypt.encryptBuffer(line);
        #endif // _DEBUG

        file << line << "\n";
    }

    if (file.is_open())
    {
        file.close();
        printf("Game (and the world) saved!\n");
        return true;
    }

    return false;
}

bool Savegame::clear()
{
    data.clear();
    tempData.clear();
    levelDataCache.clear();
    chapterDataCache.clear();

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

bool Savegame::hasData(CRstring key) const
{
    map<string,string>::const_iterator iter = data.find(key);

    return (iter != data.end());
}

string Savegame::getTempData(CRstring key) const
{
    map<string,string>::const_iterator iter = tempData.find(key);

    if (iter != tempData.end())
        return iter->second;
    return "";
}

bool Savegame::writeTempData(CRstring key, CRstring value, CRbool overwrite)
{
    if (not overwrite)
    {
        map<string,string>::const_iterator iter = tempData.find(key);
        if (iter != tempData.end())
            return false;
    }
    tempData[key] = value;

    return true;
}

bool Savegame::hasTempData(CRstring key) const
{
    map<string,string>::const_iterator iter = tempData.find(key);

    return (iter != tempData.end());
}

int Savegame::getChapterProgress(CRstring chapterFile)
{
	return getChapterStats(chapterFile).progress;
}

bool Savegame::setChapterProgress(CRstring chapterFile, CRint progress, CRbool overwrite)
{
	ChapterStats temp = {progress,-1};
    return setChapterStats(chapterFile,temp,overwrite);
}

Savegame::ChapterStats Savegame::getChapterStats(CRstring chapterFile)
{
	map<string,ChapterStats>::iterator iter = chapterDataCache.find(chapterFile);
	if (iter != chapterDataCache.end())
		return iter->second;

    string value = getData(chapterFile);
    vector<string> tokens;
    StringUtility::tokenize(value,tokens,DELIMIT_STRING);
    ChapterStats result = {0,-1};

    if (tokens.size() >= 2)
    {
        result.progress = StringUtility::stringToInt(tokens.front());
        result.time = StringUtility::stringToInt(tokens[1]);
    }
    chapterDataCache[chapterFile] = result;

    return result;
}

bool Savegame::setChapterStats(CRstring chapterFile, ChapterStats newStats, CRbool overwrite)
{
    ChapterStats stats = getChapterStats(chapterFile);

    if (not overwrite)
    {
        if (newStats.time > stats.time && stats.time > 0)
            newStats.time = stats.time;
		if (newStats.progress < stats.progress)
			newStats.progress = stats.progress;
    }
    chapterDataCache[chapterFile] = newStats;
	string temp = StringUtility::intToString(newStats.progress) + DELIMIT_STRING +
		StringUtility::intToString(newStats.time);
    return writeData(chapterFile,temp,true);
}

bool Savegame::setLevelStats(CRstring levelFile, const LevelStats& newStats, CRbool overwrite)
{
    LevelStats stats = getLevelStats(levelFile);

    if (not overwrite && stats.time >= 0)
    {
        if (newStats.time > stats.time)
            return false;
    }
    levelDataCache[levelFile] = newStats;
    return writeData(levelFile,StringUtility::intToString(newStats.time),true);
}

Savegame::LevelStats Savegame::getLevelStats(CRstring levelFile)
{
	map<string,LevelStats>::iterator iter = levelDataCache.find(levelFile);
	if (iter != levelDataCache.end())
		return iter->second;

    string value = getData(levelFile);
    vector<string> tokens;
    StringUtility::tokenize(value,tokens,DELIMIT_STRING);
    LevelStats result = {-1};

    if (tokens.size() >= 1)
    {
        result.time = StringUtility::stringToInt(tokens.front());
    }
    levelDataCache[levelFile] = result;

    return result;
}

///---protected---

///---private---
