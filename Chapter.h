#ifndef CHAPTER_H
#define CHAPTER_H

#include <vector>
#include <map>
#include <string>

#include "PenjinTypes.h"

using namespace std;

class Chapter
{
public:
    Chapter();
    virtual ~Chapter();

    virtual void clear();

    virtual bool loadFromFile(CRstring filename);
    virtual string getNextLevel(CRstring current);
    virtual string getLevelFilename(CRint pos);

    string filename;
    string path;
    string name;
    string imageFile;

    string errorString;

    vector<string> levels;

    enum ChapterProp
    {
        cpUnknown,
        cpName,
        cpImage,
        cpLevel
    };
    map<string,int> stringToProp;
protected:
    bool processParameter(const pair<string,string>& value);

private:

};


#endif // CHAPTER_H

