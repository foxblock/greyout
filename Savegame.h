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

#ifndef SAVEGAME_H
#define SAVEGAME_H

#include <map>

#include "PenjinTypes.h"
#include "Encryption.h"

#define SAVEGAME Savegame::getSavegame()

/**
Save progress to and load from a file (encrypted)
Also provides functions for generic data saving and loading
Chapter progress is saved by the highest level reached
**/

class Savegame
{
private:
	Savegame();
	static Savegame *self;
public:
	virtual ~Savegame();
	static Savegame* getSavegame();

	/// all writers return true on success and false otherwise

	// set the savegame file, also tries to load progress if file is found
	virtual bool setFile(CRstring filename);
	// enable/disable encryption for saving
	virtual void setEncryption(CRbool useEncryption);

	// save progress to file
	virtual bool save();
	// clear progress
	virtual bool clear();

	// generic data getter and setter, if overwrite is true the new value will be
	// written without prior check of existance of the key
	virtual string getData(CRstring key) const;
	virtual bool writeData(CRstring key, CRstring value, CRbool overwrite=true);
	virtual bool hasData(CRstring key) const;

	// temporary data will not be saved to disk and can be used without calling setFile
	virtual string getTempData(CRstring key) const;
	virtual bool writeTempData(CRstring key, CRstring value, CRbool overwrite=true);
	virtual bool hasTempData(CRstring key) const;

	struct ChapterStats
	{
		int progress;
		int bestSpeedrunTime;
	};

	// wrappers for getData and writeData
	// will check whether passed progress is greater than loaded if overwrite is false
	virtual int getChapterProgress(CRstring chapterFile);
	virtual bool setChapterProgress(CRstring chapterFile, CRint progress, CRbool overwrite=false);
	virtual ChapterStats getChapterStats(CRstring chapterFile);
	virtual bool setChapterStats(CRstring chapterFile, ChapterStats newStats, CRbool overwrite=false);

	struct LevelStats
	{
		int bestSpeedrunTime;
		int totalDeaths;
		int totalResets;
		int timesAttempted;
		int timesCompleted;
		int totalTimeOnLevel;
	};

	virtual bool setLevelStats(CRstring levelFile, LevelStats newStats, CRbool overwrite=false);
	virtual LevelStats getLevelStats(CRstring levelFile);

	// set this to false to disable saving after data operation
	bool autoSave;
protected:
	map<string,string> data;
	map<string,string> tempData;
	map<string,LevelStats> levelDataCache;
	map<string,ChapterStats> chapterDataCache;
	string filename;
	bool encrypt;

	Encryption crypt;
private:

};

#endif // SAVEGAME_H

