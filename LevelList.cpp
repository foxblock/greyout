#include "LevelList.h"
#include "Factory.h"
#include "StringUtility.h"

LevelList::LevelList(string filename)
{
    ifstream file(filename.c_str());
    string line;

    while (std::getline(file, line))
    {
        line = StringUtility::stripLineEndings(line);
        data.push_back(ParseLevelLine(line));
    }
};

LevelList::~LevelList()
{
};

LevelData LevelList::GetFirstLevel()
{
    if (data.size() == 0) {
    {
        cout << "Critical error: Trying to get level, but level-list is empty" << endl;
        exit(1);
    }
    } else return data[0];
}

LevelData LevelList::GetNextLevel(string filename)
{
    unsigned int i;
    for (i=0; i < data.size()-1; i++) {
        if (data[i].FileName == filename) {
            return data[i+1];
        }
    }
#ifdef _DEBUG
    cout << "Error: Unable to find next level in list" << endl;
#endif
    LevelData l;
    l.LevelClass = "";
    l.FileName = "";
    l.Params = "";
    return l;
};

LevelData LevelList::ParseLevelLine(string line)
{
//	Factory *f = Factory::GetFactory();
    LevelData l;
    vector<string> split;

    if (line == "") {
    #ifdef _DEBUG
        cout << "Error: Malformed (empty) level-line" << endl;
    #endif
        l.LevelClass = "";
        l.FileName = "";
        l.Params = "";
        return l;
    }

    tokenize(line,split,",",3);

    l.LevelClass = split[0];
    l.FileName = split[1];
    if (split.size()>2) {
        l.Params = split[2];
    } else {
        l.Params = "";
    }
    return l;
};
