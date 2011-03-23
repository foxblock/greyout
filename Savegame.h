#ifndef SAVEGAME_H
#define SAVEGAME_H

#include <map>

#include "PenjinTypes.h"
#include "Encryption.h"

#define SAVEGAME Savegame::getSavegame()

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

    // save progress to file
    virtual bool save();
    // clear progress
    virtual bool clear();

    // generic data getter and setter, if overwrite is true the new value will be
    // written without prior check of existance of the key
    virtual string getData(CRstring key) const;
    virtual bool writeData(CRstring key, CRstring value, CRbool overwrite=true);

    // wrappers for getData and writeData
    // will check whether passed progress is greater than loaded if overwrite is false
    virtual int getChapterProgress(CRstring chapterFile) const;
    virtual bool setChapterProgress(CRstring chapterFile, CRint progress, CRbool overwrite=false);

    // set this to false to disable saving after data operation
    bool autoSave;
protected:
    map<string,string> data;
    string filename;

    Encryption crypt;
private:

};

#endif // SAVEGAME_H

