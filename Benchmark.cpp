#include "Benchmark.h"

#define GRAPH_UPDATE 1000

#define LEFT_REGION SDL_Rect left = {50,50,250,200}
#define RIGHT_REGION SDL_Rect right = {500,50,250,200}

#include <limits.h>

#include "BaseUnit.h"
#include "ControlUnit.h"
#include "LevelLoader.h"
#include "MyGame.h"


Benchmark::Benchmark() : Level()
{
    phases[0] = 0;
    phases[1] = 10000;
    phases[2] = 20000;
    currentPhase = 0;
    secondIndex.push_back(0);
    fpsCount = 0;
}

Benchmark::~Benchmark()
{
    fpsData.clear();
    secondIndex.clear();
    ENGINE->setFrameRate(FRAME_RATE);
}

void Benchmark::init()
{
    ENGINE->setFrameRate(1000);
    second.init(GRAPH_UPDATE,MILLI_SECONDS,this,Benchmark::secondCallback);
    second.setRewind(REWIND);
    second.start();
    counter.init(phases[currentPhase+1]-phases[currentPhase],MILLI_SECONDS,this,Benchmark::timerCallback);
    counter.setRewind(STOP);
    counter.start();

    winCounter = 1;
    SDL_BlitSurface(levelImage,NULL,collisionLayer,NULL);
    boxCount = 0;
    particleCount = 0;
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
        setNextState(STATE_MAIN);
        return;
    }
}

void Benchmark::update()
{
    Level::update();
    ++fpsCount;
    second.update();
    counter.update();
}

void Benchmark::pauseUpdate()
{
    update();
}

void Benchmark::generateFPSData()
{
    float minFPS = INT_MAX;
    float maxFPS = 0;
    double averageFPS = 0;

    for (vector<float>::iterator I = fpsData.begin(); I < fpsData.end(); ++I)
    {
        minFPS = min((*I),minFPS);
        maxFPS = max((*I),maxFPS);
        averageFPS += (*I);
    }
    averageFPS /= fpsData.size();

    cout << "----------" << endl;
    cout << "Benchmark results:" << endl;
    cout << "----------" << endl;
    cout << "Duration (ms): " << phases[sizeof(phases)/sizeof(phases[0])-1] << endl;
    cout << "Graph update (ms): " << second.getLimit() << endl;
    cout << "FPS (min): " << minFPS << endl;
    cout << "FPS (max): " << maxFPS << endl;
    cout << "FPS (avg): " << averageFPS << endl;
    cout << "FPS Graph:";
    for (int I = 0; I < secondIndex.size()-1; ++I)
    {
        averageFPS = 0;
        for (int K = secondIndex[I]; K < secondIndex[I+1]; ++K)
            averageFPS += fpsData[K];
        averageFPS /= secondIndex[I+1] - secondIndex[I];
        cout << " " << averageFPS;
    }
    cout << endl;
    fpsCount = 0;
    for (int I = 0; I < fpsData.size(); ++I)
        fpsCount += fpsData[I];
    cout << "Update cycles (#): " << fpsCount << endl;
    cout << "Boxes total (#): " << boxCount << endl;
    cout << "Particles total (#): " << particleCount << endl;
    cout << "----------" << endl;
    cout << "Benchmark finished successfully!" << endl;
}

void Benchmark::secondUpdate()
{
    fpsData.push_back(fpsCount);
    fpsCount = 0;
    secondIndex.push_back(fpsData.size());

    switch (currentPhase)
    {
    case 0:
        spawnBox();
        break;
    case 1:
        explodeBox();
        break;
    default:
        break;
    }
}

void Benchmark::spawnBox()
{
    // spawn some boxes
    LEFT_REGION;
    list<PARAMETER_TYPE > params;
    params.push_back(make_pair("class","pushablebox"));
    params.push_back(make_pair("collision","0"));
    params.push_back(make_pair("size","24,24"));
    params.push_back(make_pair("health","2"));

    BaseUnit* box = LEVEL_LOADER->createUnit(params,this);
    box->position.x = rand() % (left.w) + left.x;
    box->position.y = rand() % (left.h / 2) + left.y;
    units.push_back(box);
    boxCount++;
    box = LEVEL_LOADER->createUnit(params,this);
    box->position.x = rand() % (left.w) + left.x;
    box->position.y = rand() % (left.h / 2) + left.y + (left.h / 2);
    units.push_back(box);
    boxCount++;

    RIGHT_REGION;
    box = LEVEL_LOADER->createUnit(params,this);
    box->position.x = rand() % (right.w) + right.x;
    box->position.y = rand() % (right.h / 2) + right.y;
    units.push_back(box);
    boxCount++;
    box = LEVEL_LOADER->createUnit(params,this);
    box->position.x = rand() % (right.w) + right.x;
    box->position.y = rand() % (right.h / 2) + right.y + (right.h / 2);
    units.push_back(box);
    boxCount++;
}

void Benchmark::explodeBox()
{
    int I = 0;
    for (int K = 0; K < min((int)units.size(),4); ++K)
    {
        int temp = effects.size();
        do
        {
            I = rand() % units.size();
        }
        while (units[I]->toBeRemoved);
        units[I]->explode();
        particleCount += effects.size() - temp;
    }
}

void Benchmark::timerCallback(void* object)
{
    Benchmark* self = (Benchmark*) object;
    if (self->secondIndex.size() <= self->phases[self->currentPhase] / GRAPH_UPDATE)
    {
        self->secondUpdate();
        self->second.start(GRAPH_UPDATE);
    }
    self->currentPhase++;
    if (self->currentPhase >= sizeof(self->phases) / sizeof(self->phases[0]))
    {
        self->generateFPSData();
        self->setNextState(STATE_MAIN);
    }
    else
    {
        self->counter.start(self->phases[self->currentPhase+1] - self->phases[self->currentPhase]);
    }
}

void Benchmark::secondCallback(void* object)
{
    Benchmark* self = (Benchmark*) object;
    self->secondUpdate();
}
