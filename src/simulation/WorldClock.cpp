#include "simulation/WorldClock.hpp"

WorldClock::WorldClock()
    : lastCounter(SDL_GetPerformanceCounter()), deltaTime(0.0), simulationTime(0.0) {}

void WorldClock::update() {
    Uint64 currentCounter = SDL_GetPerformanceCounter();
    Uint64 counterElapsed = currentCounter - lastCounter;
    
    deltaTime = static_cast<double>(counterElapsed) / static_cast<double>(SDL_GetPerformanceFrequency());
    simulationTime += deltaTime;
    
    lastCounter = currentCounter;
}

double WorldClock::getDeltaTime() const {
    return deltaTime;
}

double WorldClock::getSimulationTime() const {
    return simulationTime;
}
