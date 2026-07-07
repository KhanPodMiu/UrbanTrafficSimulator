#pragma once

#include "SDL2/SDL.h"

class WorldClock {
public:
    WorldClock();
    void update();
    double getDeltaTime() const;
    double getSimulationTime() const;

private:
    Uint64 lastCounter;
    double deltaTime;
    double simulationTime;
};
