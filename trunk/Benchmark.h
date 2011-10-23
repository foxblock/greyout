#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <vector>

#include "Level.h"
#include "CountDown.h"

class Benchmark : public Level
{
    public:
        Benchmark();
        virtual ~Benchmark();

        virtual void init();
        virtual void userInput();
        virtual void update();
        virtual void pauseUpdate();
    protected:
        virtual void generateFPSData();
        virtual void secondUpdate();
        virtual void spawnBox();
        virtual void explodeBox();

        vector<float> fpsData;
        vector<int> secondIndex;
        CountDown counter;
        CountDown second;
        int phases[3];
        int currentPhase;
        int boxCount;
        int particleCount;
        #ifndef _DEBUG
        int fpsCount;
        #endif
    private:
        static void timerCallback(void* object);
        static void secondCallback(void* object);
};


#endif // BENCHMARK_H

