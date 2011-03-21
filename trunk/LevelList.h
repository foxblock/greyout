#ifndef LEVELLIST_H
#define LEVELLIST_H

#include <string>
#include <vector>
using namespace std;

typedef struct
{
    string LevelClass;
    string FileName;
    string Params;
} LevelData;

class LevelList
{
    public:
        LevelList(string filename);
        ~LevelList();

        LevelData GetFirstLevel();
        LevelData GetNextLevel(string filename);
        LevelData ParseLevelLine(string line);
    protected:
    private:
    	vector<LevelData> data;
};

#endif // LEVELLIST_H
