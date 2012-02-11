#include "LevelLoader.h"

#include <fstream>
#include <vector>
#include <map>

#include "StringUtility.h"
#include "Image.h"

#include "userStates.h"
#include "GreySurfaceCache.h"
#include "SimpleFlags.h"

// Level classes
#include "Level.h"
#include "Benchmark.h"
#include "Playground.h"

// Unit classes
#include "BaseUnit.h"
#include "ControlUnit.h"
#include "BasePlayer.h"
#include "PushableBox.h"
#include "SolidBox.h"
#include "Exit.h"
#include "DialogueTrigger.h"
#include "Gear.h"
#include "Switch.h"
#include "Key.h"
#include "BaseTrigger.h"
#include "ExitTrigger.h"
#include "SoundTrigger.h"
#include "CameraTrigger.h"
#include "TextObject.h"
#include "FadingBox.h"

using namespace std;

enum DataIdent
{
    diUnknown,
    diLevel,
    diPlayer,
    diUnit
};

enum LevelIdent
{
    lcUnknown,
    lcGeneric,
    lcBenchmark,
    lcPlayground
};

enum PlayerIdent
{
    pcUnknown,
    pcGeneric,
    pcBlack,
    pcWhite
};

enum UnitIdent
{
    ucUnknown,
    ucGeneric,
    ucPushableBox,
    ucSolidBox,
    ucExit,
    ucDialogueTrigger,
    ucGear,
    ucSwitch,
    ucKey,
    ucBaseTrigger,
    ucExitTrigger,
    ucSoundTrigger,
    ucCameraTrigger,
    ucTextObject,
    ucFadingBox
};

// mapping the ident string used in the map file to a ident integer for use in
// a switch statement for cleaner and faster map loading
static map<string, DataIdent> dataIdents;
static map<string, LevelIdent> levelClasses;
static map<string, PlayerIdent> playerClasses;
static map<string, UnitIdent> unitClasses;

void createIdentMaps()
{
    dataIdents["level"] = diLevel;
    dataIdents["player"] = diPlayer;
    dataIdents["unit"] = diUnit;

    levelClasses["generic"] = lcGeneric;
    levelClasses["benchmark"] = lcBenchmark;
    levelClasses["playground"] = lcPlayground;

    playerClasses["generic"] = pcGeneric;
    playerClasses["black"] = pcBlack;
    playerClasses["white"] = pcWhite;

    unitClasses["generic"] = ucGeneric;
    unitClasses["pushablebox"] = ucPushableBox;
    unitClasses["solidbox"] = ucSolidBox;
    unitClasses["exit"] = ucExit;
    unitClasses["dialoguetrigger"] = ucDialogueTrigger;
    unitClasses["gear"] = ucGear;
    unitClasses["switch"] = ucSwitch;
    unitClasses["key"] = ucKey;
    unitClasses["basetrigger"] = ucBaseTrigger;
    unitClasses["exittrigger"] = ucExitTrigger;
    unitClasses["soundtrigger"] = ucSoundTrigger;
    unitClasses["cameratrigger"] = ucCameraTrigger;
    unitClasses["text"] = ucTextObject;
    unitClasses["fadingbox"] = ucFadingBox;
}

LevelLoader* LevelLoader::self = NULL;

LevelLoader::LevelLoader()
{
    errorString = "";
    createIdentMaps();
}

LevelLoader::~LevelLoader()
{
    //
}

LevelLoader* LevelLoader::getLevelLoader()
{
    if (not self)
        self = new LevelLoader();
    return self;
}

Level* LevelLoader::loadLevelFromFile(CRstring filename, CRstring chapterPath)
{
    cout << "---------------------------------------------------------" << endl;
    cout << "Trying to load level file \"" << filename << "\"" << endl;

    string line;
    ifstream file(filename.c_str());
    int lineNumber = 0; // for error output
    errorString = "";
    bool lastField = true;

    if (file.fail())
    {
        errorString = "Failed to open file for read!";
        return NULL;
    }

    ErrorCode error = ecNone;

    string field = ""; // the current field
    list<PARAMETER_TYPE > params; // list of field parameters (key=value)
    Level* level = NULL;

    // parse file line by line
    while (file.good())
    {
        getline(file,line);
        ++lineNumber;

        // solve win-lin compatibility issues
        line = StringUtility::stripLineEndings(line);
        // make case in-sensitive
        line = StringUtility::lower(line);

        // A value in [] brackets indicates a new field
        // all key=value pairs following will be loaded in a map as parameters
        string nextField = "";
        if (line.substr(0,COMMENT_STRING.length()) == COMMENT_STRING)
        {
            // comment line - disregard
        }
        else if (line.substr(0,FIELD_STRING.length()) == FIELD_STRING) // new field
        {
            // strip brackets
            nextField = line.substr(1,line.length()-2);
        }
        else if (line[0] == 0) // empty line
        {
            // disregard
        }
        else // key=value pair
        {
            if (field[0] == 0)
            {
                errorString = "Parameter outside of field specification on line "
                              + StringUtility::intToString(lineNumber);
                error = ecCritical;
            }
            else
            {
                // add pair to the map
                vector<string> tokens;
                StringUtility::tokenize(line,tokens,VALUE_STRING,2);
                if (tokens.size() != 2)
                {
                    cout << "Error: Incorrect key-value pair on line " << lineNumber << endl;
                    error = ecWarning;
                }
                else
                {
                    // make sure class parameter is always the first element
                    if (tokens[0] == CLASS_STRING)
                        params.push_front(make_pair(tokens[0],tokens[1]));
                    else
                        params.push_back(make_pair(tokens[0],tokens[1]));
                }

            }
        }

        // add current field with parameters to the level when reaching a new field or the end of the file
        if ((nextField[0] != 0) && (field[0] != 0) || not (file.good()))
        {
            switch (dataIdents[field])
            {
            case diLevel:
            {
                params.push_back(make_pair("filename",filename));
                level = createLevel(params,chapterPath,lineNumber);
                if (not level)
                {
                    if (errorString[0] == 0)
                        errorString = "Level creation failed!";
                    error = ecCritical;
                }
                break;
            }
            case diPlayer:
            {
                if (not level)
                {
                    errorString = "The level field has to be specified first! After that you can add units and stuff...";
                    error = ecCritical;
                    break;
                }

                ControlUnit* player = createPlayer(params,level,lineNumber);
                if (player)
                    level->players.push_back(player);
                else
                    error = ecWarning;
                break;
            }
            case diUnit:
            {
                if (not level)
                {
                    errorString = "The level field has to be specified first! After that you can add units and stuff...";
                    error = ecCritical;
                    break;
                }

                BaseUnit* unit = createUnit(params,level,lineNumber);
                if (unit)
                    level->units.push_back(unit);
                else
                    error = ecWarning;
                break;
            }
            default:
            {
                cout << "Error: Unknown field \"" << field << "\" ending on line " << lineNumber << endl;
                error = ecWarning;
            }
            }
        }

        if (nextField[0] != 0) // set the next field loaded above
        {
            field = nextField;
            params.clear();
        }

        if (error == ecCritical) // encountered a critical error, abort
            break;
    } // while


    if (file.is_open())
        file.close();

    // Additional check for missing initialisation
    if (level)
    {
        if (level->players.size() == 0)
        {
            cout << "Error: No player unit has been specified in the map, what did you plan to play with?" << endl;
            error = ecWarning;
        }
        vector<ControlUnit*>::const_iterator unit;
        for (unit = level->players.begin(); unit != level->players.end(); ++unit)
        {
            if ((*unit)->takesControl)
                break;
        }
        if (unit == level->players.end())
        {
            cout << "Warning: No controlable unit found! Push the buttons...(nothing happens)" << endl;
            error = ecWarning;
        }
        cout << "Level has " << level->players.size() << " players and " << level->units.size() << " units." << endl;
    }

    switch (error)
    {
    case ecNone:
        cout << "Successfully loaded level file!" << endl;
        break;
    case ecWarning:
        cout << "Finished loading level file with minor errors!" << endl;
        break;
    case ecCritical:
        // clean up and return NULL on critical error
        cout << "Encountered critical error while loading map!" << endl;
        delete level;
        level = NULL;
        break;
    default:
        break;
    }

    cout << "---------------------------------------------------------" << endl;
    return level;
}

Level* LevelLoader::createLevel(list<PARAMETER_TYPE >& params, CRstring chapterPath, CRint lineNumber)
{
    Level* result = NULL;

    if (params.front().first != CLASS_STRING)
    {
        cout << "Error: Level definition ending on line " << lineNumber << " missing mandatory parameter \"" << CLASS_STRING << "\"" << endl;
        return NULL;
    }

    switch (levelClasses[params.front().second])
    {
    case lcGeneric:
    {
        result = new Level;
        break;
    }
    case lcBenchmark:
    {
        result = new Benchmark;
        break;
    }
    case lcPlayground:
    {
        result = new Playground;
        break;
    }
    default:
    {
        cout << "Error: Unknown level class \"" << params.front().second << "\"" << endl;
        return NULL;
    }
    }

    result->chapterPath = chapterPath;

    if (not result->load(params))
    {
        errorString = result->errorString;
        delete result;
        result = NULL;
    }

    return result;
}

ControlUnit* LevelLoader::createPlayer(list<PARAMETER_TYPE >& params, Level* const parent, CRint lineNumber)
{
    ControlUnit* result = NULL;

    if (params.front().first != CLASS_STRING)
    {
        cout << "Error: Player definition ending on line " << lineNumber << " missing mandatory parameter \"" << CLASS_STRING << "\"" << endl;
        return NULL;
    }

    switch (playerClasses[params.front().second])
    {
    case pcGeneric:
    {
        result = new BasePlayer(parent);
        break;
    }
    case pcBlack:
    {
        // unit specific parameters as this is merely an alias for a customized BasePlayer
        params.push_back(make_pair("collision","black,red"));
        params.push_back(make_pair("imageoverwrite","images/player/black_big.png"));
        params.push_back(make_pair("colour","black"));
        result = new BasePlayer(parent);
        break;
    }
    case pcWhite:
    {
        params.push_back(make_pair("collision","red,white"));
        params.push_back(make_pair("imageoverwrite","images/player/white_big.png"));
        params.push_back(make_pair("colour","white"));
        result = new BasePlayer(parent);
        break;
    }
    default:
        cout << "Error: Unknown player class \"" << params.front().second << "\" on line " << lineNumber << endl;
        return NULL;
    }

    result->parameters.insert(result->parameters.begin(),params.begin(),params.end());
    if (not result->load(result->parameters))
    {
        cout << "Error loading unit id \"" << result->id << "\"" << endl;
        delete result;
        result = NULL;
    }

    return result;
}


BaseUnit* LevelLoader::createUnit(list<PARAMETER_TYPE >& params, Level* const parent, CRint lineNumber)
{
    BaseUnit* result = NULL;

    if (params.front().first != CLASS_STRING)
    {
        cout << "Error: Unit definition ending on line " << lineNumber << " missing mandatory parameter \"" << CLASS_STRING << "\"" << endl;
        return NULL;
    }

    switch (unitClasses[params.front().second])
    {
    case ucGeneric:
    {
        result = new BaseUnit(parent);
        break;
    }
    case ucPushableBox:
    {
        result = new PushableBox(parent);
        break;
    }
    case ucSolidBox:
    {
        result = new SolidBox(parent);
        break;
    }
    case ucExit:
    {
        result = new Exit(parent);
        break;
    }
    case ucDialogueTrigger:
    {
        result = new DialogueTrigger(parent);
        break;
    }
    case ucGear:
    {
        result = new Gear(parent);
        break;
    }
    case ucSwitch:
    {
        result = new Switch(parent);
        break;
    }
    case ucKey:
    {
        result = new Key(parent);
        break;
    }
    case ucBaseTrigger:
    {
        result = new BaseTrigger(parent);
        break;
    }
    case ucExitTrigger:
    {
        result = new ExitTrigger(parent);
        break;
    }
    case ucSoundTrigger:
    {
        result = new SoundTrigger(parent);
        break;
    }
    case ucCameraTrigger:
    {
        result = new CameraTrigger(parent);
        break;
    }
    case ucTextObject:
    {
        result = new TextObject(parent);
        break;
    }
    case ucFadingBox:
    {
        result = new FadingBox(parent);
        break;
    }
    default:
        cout << "Error: Unknown unit class \"" << params.front().second << "\" on line " << lineNumber << endl;
        return NULL;
    }

    result->parameters.insert(result->parameters.begin(),params.begin(),params.end());
    if (not result->load(result->parameters))
    {
        cout << "Error loading unit id \"" << result->id << "\"" << endl;
        delete result;
        result = NULL;
    }

    return result;
}
