#pragma once

#include "SDL2/SDL.h"

class WorldClock {
public:
    WorldClock();
    void update();
    void start();
    void pause();
    void reset();
    void setSpeedMultiplier(double multiplier);
    bool isRunning() const;
    double getDeltaTime() const;
    double getSimulationTime() const;
    double getSpeedMultiplier() const;

private:
    Uint64 lastCounter;
    double deltaTime;
    double simulationTime;
    double speedMultiplier;
    bool running;
};
