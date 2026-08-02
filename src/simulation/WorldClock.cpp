#include "simulation/WorldClock.hpp"

WorldClock::WorldClock()
    : lastCounter(SDL_GetPerformanceCounter()),
      deltaTime(0.0),
      simulationTime(0.0),
      speedMultiplier(1.0),
      running(true) {}

void WorldClock::update()
{
    if (!running)
    {
        deltaTime = 0.0;
        return;
    }

    const Uint64 currentCounter = SDL_GetPerformanceCounter();
    const Uint64 counterElapsed = currentCounter - lastCounter;
    const double realDeltaTime =
        static_cast<double>(counterElapsed) /
        static_cast<double>(SDL_GetPerformanceFrequency());

    deltaTime = realDeltaTime * speedMultiplier;
    simulationTime += deltaTime;
    lastCounter = currentCounter;
}

void WorldClock::start()
{
    if (running)
    {
        return;
    }

    // Discard the time spent while paused so the next frame has a normal dt.
    lastCounter = SDL_GetPerformanceCounter();
    deltaTime = 0.0;
    running = true;
}

void WorldClock::pause()
{
    running = false;
    deltaTime = 0.0;
    lastCounter = SDL_GetPerformanceCounter();
}

void WorldClock::reset()
{
    simulationTime = 0.0;
    deltaTime = 0.0;
    lastCounter = SDL_GetPerformanceCounter();
}

void WorldClock::setSpeedMultiplier(double multiplier)
{
    if (multiplier > 0.0)
    {
        speedMultiplier = multiplier;
    }
}

bool WorldClock::isRunning() const
{
    return running;
}

double WorldClock::getDeltaTime() const
{
    return deltaTime;
}

double WorldClock::getSimulationTime() const
{
    return simulationTime;
}

double WorldClock::getSpeedMultiplier() const
{
    return speedMultiplier;
}
