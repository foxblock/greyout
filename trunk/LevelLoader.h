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
