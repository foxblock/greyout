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

#include "SoundTrigger.h"

#include "MusicCache.h"
#include "Level.h"

SoundTrigger::SoundTrigger(Level* newParent) : BaseTrigger(newParent)
{
	stringToProp["file"] = spFile;
	stringToProp["playcount"] = spPlayCount;
	stringToProp["loops"] = spLoops;
	filename = "";
	playcount = 1;
	count = 0;
	loops = 0;
	triggerCol = LIGHT_BLUE;
}

SoundTrigger::~SoundTrigger()
{
	//
}

///---public---

bool SoundTrigger::processParameter(const PARAMETER_TYPE& value)
{
	if (BaseTrigger::processParameter(value))
		return true;

	bool parsed = true;

	switch (stringToProp[value.first])
	{
	case spFile:
	{
		filename = value.second;
		break;
	}
	case spPlayCount:
	{
		playcount = StringUtility::stringToInt(value.second);
		break;
	}
	case spLoops:
	{
		loops = StringUtility::stringToInt(value.second);
		break;
	}
	default:
		parsed = false;
	}

	return parsed;
}

void SoundTrigger::reset()
{
	count = 0;
	BaseTrigger::reset();
}

///---protected---

void SoundTrigger::doTrigger(const UnitCollisionEntry& entry)
{
	if (playcount > 0 && ++count >= playcount)
		BaseTrigger::doTrigger(entry);

	MUSIC_CACHE->playSound(filename,parent->chapterPath,loops);
}

///---private---
