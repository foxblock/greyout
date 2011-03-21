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
    protected:
        virtual void generateFPSData();
        virtual void secondUpdate();

        vector<int> fpsData;
        vector<int> secondData;
        vector<float> secondAverage;
        int distance;
        CountDown counter;
        CountDown second;
    private:
        static void timerCallback(void* object);
        static void secondCallback(void* object);
};


#endif // BENCHMARK_H

