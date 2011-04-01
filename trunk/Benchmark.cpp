#include "Benchmark.h"

#define BECHMARK_DURATION 10000
#define GRAPH_UPDATE 1000

#define LEFT_REGION SDL_Rect left = {50,50,250,200}
#define RIGHT_REGION SDL_Rect right = {500,50,250,200}

#include "BaseUnit.h"
#include "ControlUnit.h"
#include "LevelLoader.h"
#include "MyGame.h"

#include "StringUtility.h"

using namespace StringUtility;

Benchmark::Benchmark() : Level()
{
    ENGINE->setFrameRate(250);
}

Benchmark::~Benchmark()
{
    fpsData.clear();
    secondData.clear();
    secondAverage.clear();
    ENGINE->setFrameRate(30);
}

void Benchmark::init()
{
    distance = 0;

    second.init(GRAPH_UPDATE,MILLI_SECONDS,this,Benchmark::secondCallback);
    second.setRewind(REWIND);
    second.start();
    counter.init(BECHMARK_DURATION,MILLI_SECONDS,this,Benchmark::timerCallback);
    counter.setRewind(REWIND);
    counter.start();

    winCounter = 1;
}

void Benchmark::userInput()
{
#ifdef PLATFORM_PC
    if (input->isQuit())
    {
        nullifyState();
        return;
    }
#endif
    if (input->isStart())
    {
        nullifyState();
        return;
    }
}

void Benchmark::update()
{
    Level::update();
    distance += players.back()->velocity.y;
    #ifdef _DEBUG
    int value = MyGame::getMyGame()->getFPS();
    #else
    int value = 0;
    #endif
    fpsData.push_back(value);
    secondData.push_back(value);
    second.update();
    counter.update();
}

void Benchmark::pauseUpdate()
{
    update();
}

void Benchmark::generateFPSData()
{
    int minFPS = 2147483647;
    int maxFPS = 0;
    double averageFPS = 0;

    for (vector<int>::iterator I = fpsData.begin(); I < fpsData.end(); ++I)
    {
        minFPS = min((*I),minFPS);
        maxFPS = max((*I),maxFPS);
        averageFPS += (*I);
    }
    averageFPS /= fpsData.size();

    cout << "----------" << endl;
    cout << "Benchmark results:" << endl;
    cout << "----------" << endl;
    cout << "Duration (ms): " << intToString(counter.getLimit()) << endl;
    cout << "Graph update (ms): " << intToString(second.getLimit()) << endl;
    cout << "Distance travelled (pixels): " << intToString(distance) << endl;
    cout << "FPS (min): " << intToString(minFPS) << endl;
    cout << "FPS (max): " << intToString(maxFPS) << endl;
    cout << "FPS (avg): " << doubleToString(averageFPS) << endl;
    cout << "FPS Graph:";
    for (vector<float>::iterator I = secondAverage.begin(); I < secondAverage.end(); ++I)
        cout << " " << floatToString(*I);
    cout << endl;
    cout << "Update cycles (#): " << intToString(fpsData.size()) << endl;
    cout << "Boxes created (#): " << intToString(units.size()) << endl;
    cout << "----------" << endl;
    cout << "Benchmark finished successfully!" << endl;
}

void Benchmark::secondUpdate()
{
    // calculate the average fps for this second
    float average = 0;
    for (vector<int>::iterator I = secondData.begin(); I < secondData.end(); ++I)
    {
        average += (*I);
    }
    average /= secondData.size();
    secondAverage.push_back(average);
    secondData.clear();

    // spawn some boxes
    LEFT_REGION;
    list<pair<string,string> > params;
    params.push_back(make_pair("class","pushablebox"));
    params.push_back(make_pair("collision","0"));
    params.push_back(make_pair("size","24,24"));
    params.push_back(make_pair("health","2"));

    BaseUnit* box = LEVEL_LOADER->createUnit(params,this);
    box->position.x = rand() % (left.w) + left.x;
    box->position.y = rand() % (left.h / 2) + left.y;
    units.push_back(box);
    box = LEVEL_LOADER->createUnit(params,this);
    box->position.x = rand() % (left.w) + left.x;
    box->position.y = rand() % (left.h / 2) + left.y + (left.h / 2);
    units.push_back(box);
    RIGHT_REGION;
    box = LEVEL_LOADER->createUnit(params,this);
    box->position.x = rand() % (right.w) + right.x;
    box->position.y = rand() % (right.h / 2) + right.y;
    units.push_back(box);
    box = LEVEL_LOADER->createUnit(params,this);
    box->position.x = rand() % (right.w) + right.x;
    box->position.y = rand() % (right.h / 2) + right.y + (right.h / 2);
    units.push_back(box);
}

void Benchmark::timerCallback(void* object)
{
    Benchmark* self = (Benchmark*) object;
    self->generateFPSData();
    self->nullifyState();
}

void Benchmark::secondCallback(void* object)
{
    Benchmark* self = (Benchmark*) object;
    self->secondUpdate();
}
