#include "simulation/WorldClock.hpp"

WorldClock::WorldClock()
    : lastCounter(SDL_GetPerformanceCounter()),
      deltaTime(0.0),
      simulationTime(0.0),
      running(true) {}

void WorldClock::update() {
    if (!running) {
        deltaTime = 0.0;
        return;
    }

    Uint64 currentCounter = SDL_GetPerformanceCounter();
    Uint64 counterElapsed = currentCounter - lastCounter;
    
    deltaTime = static_cast<double>(counterElapsed) / static_cast<double>(SDL_GetPerformanceFrequency());
    simulationTime += deltaTime;
    
    lastCounter = currentCounter;
}

void WorldClock::start() {
    if (running) {
        return;
    }

    // Discard the time spent while paused so the next frame has a normal dt.
    lastCounter = SDL_GetPerformanceCounter();
    deltaTime = 0.0;
    running = true;
}

void WorldClock::pause() {
    running = false;
    deltaTime = 0.0;
    lastCounter = SDL_GetPerformanceCounter();
}

void WorldClock::reset() {
    simulationTime = 0.0;
    deltaTime = 0.0;
    lastCounter = SDL_GetPerformanceCounter();
}

bool WorldClock::isRunning() const {
    return running;
}

double WorldClock::getDeltaTime() const {
    return deltaTime;
}

double WorldClock::getSimulationTime() const {
    return simulationTime;
}
