#ifndef CHAPTER_H
#define CHAPTER_H

#include <vector>
#include <map>
#include <string>

#include "PenjinTypes.h"

/**
Organizing and ordering levels loaded from a chapter information file
Also displaying a name and image if needed
Additionally providing other classes with variables for file loading (such as the chapter path)

Chapters are a bunch of levels put together in a folder, they can use their own
graphic set and sounds as well as override or use the default ones
You can order the levels in the info.txt file
User progress will be saved in global savegame file
**/

using namespace std;

class Chapter
{
public:
    Chapter();
    virtual ~Chapter();

    virtual void clear();

    // Load a chapter from a file (name and at least one level are mandatory)
    // returns true on success, false otherwise (and sets errorString variable)
    virtual bool loadFromFile(CRstring filename);
    // Get the next level after the passed one
    //returns "" if not found, at the end of the chapter or on error
    virtual string getNextLevel(CRstring current);
    // Get the level at index pos, returns "" if out of bounds
    virtual string getLevelFilename(CRint pos);
    // Get the index of the passed level, returns -1 if not found (or empty string)
    virtual int getLevelIndex(CRstring filename);

    // Load the user's progress in this chapter from the savegame file
    virtual int getProgress() const;
    // returns the next level after the passed one and saves the progress to the savegame file
    // returns "" on error or ending of chapter
    virtual string getNextLevelAndSave(CRstring current);

    string filename; // info.txt filename and path
    string path; // path to chapter folder (where info.txt is in)
    string name; // name of the chapter (mandatory)
    string imageFile; // image displayed in the chapter menu
    string dialogueFile; // file containing the strings loaded as dialogue

    // used for error output
    string errorString;

    vector<string> levels;

    enum ChapterProp
    {
        cpUnknown,
        cpName,
        cpImage,
        cpLevel,
        cpDialogue
    };
    map<string,int> stringToProp;
protected:
    bool processParameter(const pair<string,string>& value);

private:

};


#endif // CHAPTER_H

