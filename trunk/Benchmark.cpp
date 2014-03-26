#include "Benchmark.h"

#define GRAPH_UPDATE 1000

#define LEFT_REGION SDL_Rect left = {50,50,250,200}
#define RIGHT_REGION SDL_Rect right = {500,50,250,200}

#include <limits.h>

/*
	Greyout - a colourful platformer about love

	Greyout is Copyright (c)2011-2014 Janek Schäfer

	This file is part of Greyout.

    Greyout is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

	Greyout is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Please direct any feedback, questions or comments to
	Janek Schäfer (foxblock), foxblock_at_gmail_dot_com
*/

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
    boxCount = 0;
    particleCount = 0;
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

    printf("----------\n");
    printf("Benchmark results:\n");
    printf("----------\n");
    printf("Duration (ms): %i\n",phases[sizeof(phases)/sizeof(phases[0])-1]);
    printf("Graph update (ms): %i\n",second.getLimit());
    printf("FPS (min): %.2f\n",minFPS);
    printf("FPS (max): %.2f\n",maxFPS);
    printf("FPS (avg): %.2f\n",averageFPS);
    printf("FPS Graph:");
    for (int I = 0; I < secondIndex.size()-1; ++I)
    {
        averageFPS = 0;
        for (int K = secondIndex[I]; K < secondIndex[I+1]; ++K)
            averageFPS += fpsData[K];
        averageFPS /= secondIndex[I+1] - secondIndex[I];
        printf(" %.2f",averageFPS);
    }
    printf("\n");
    fpsCount = 0;
    for (int I = 0; I < fpsData.size(); ++I)
        fpsCount += fpsData[I];
    printf("Update cycles (#): %i\n",fpsCount);
    printf("Boxes total (#): %i\n",boxCount);
    printf("Particles total (#): %i\n",particleCount);
    printf("----------\n");
    printf("Benchmark finished successfully!\n");
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
