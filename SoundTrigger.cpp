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
