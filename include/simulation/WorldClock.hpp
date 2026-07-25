#pragma once

#include "SDL2/SDL.h"

class WorldClock {
public:
    WorldClock();
    void update();
    void start();
    void pause();
    void reset();
    bool isRunning() const;
    double getDeltaTime() const;
    double getSimulationTime() const;

private:
    Uint64 lastCounter;
    double deltaTime;
    double simulationTime;
    bool running;
};
