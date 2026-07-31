#include <gtest/gtest.h>
// Replaced POSIX unistd.h and usleep with std::thread and std::chrono for Windows compatibility
#include <chrono>
#include <thread>
#include "simulation/WorldClock.hpp"

TEST(WorldClockTest, Initialization)
{
    // Initialize SDL if it hasn't been initialized by gtest main
    if (SDL_WasInit(SDL_INIT_TIMER) == 0)
    {
        SDL_Init(SDL_INIT_TIMER);
    }

    WorldClock clock;
    EXPECT_DOUBLE_EQ(clock.getDeltaTime(), 0.0);
    EXPECT_DOUBLE_EQ(clock.getSimulationTime(), 0.0);
}

TEST(WorldClockTest, UpdateTime)
{
    if (SDL_WasInit(SDL_INIT_TIMER) == 0)
    {
        SDL_Init(SDL_INIT_TIMER);
    }

    WorldClock clock;

    // Sleep for a short duration to allow time to pass
    std::this_thread::sleep_for(std::chrono::microseconds(100000)); // 100 milliseconds

    clock.update();

    double dt = clock.getDeltaTime();
    double simTime = clock.getSimulationTime();

    // dt and simTime should be around 0.1s
    EXPECT_GT(dt, 0.05);
    EXPECT_LT(dt, 0.2);
    EXPECT_DOUBLE_EQ(dt, simTime);

    std::this_thread::sleep_for(std::chrono::microseconds(50000)); // 50 milliseconds
    
    clock.update();
    
    double dt2 = clock.getDeltaTime();
    double simTime2 = clock.getSimulationTime();
    
    EXPECT_GT(dt2, 0.02);
    EXPECT_LT(dt2, 0.1);
    EXPECT_NEAR(simTime2, dt + dt2, 0.001);
}

TEST(WorldClockTest, PauseStopsSimulationTime)
{
    if (SDL_WasInit(SDL_INIT_TIMER) == 0)
    {
        SDL_Init(SDL_INIT_TIMER);
    }

    WorldClock clock;

    std::this_thread::sleep_for(std::chrono::microseconds(50000));
    clock.update();
    const double timeBeforePause = clock.getSimulationTime();

    clock.pause();
    EXPECT_FALSE(clock.isRunning());
    EXPECT_DOUBLE_EQ(clock.getDeltaTime(), 0.0);

    std::this_thread::sleep_for(std::chrono::microseconds(100000));
    clock.update();

    EXPECT_DOUBLE_EQ(clock.getDeltaTime(), 0.0);
    EXPECT_DOUBLE_EQ(clock.getSimulationTime(), timeBeforePause);
}

TEST(WorldClockTest, StartResumesWithoutPausedTimeJump)
{
    if (SDL_WasInit(SDL_INIT_TIMER) == 0)
    {
        SDL_Init(SDL_INIT_TIMER);
    }

    WorldClock clock;

    std::this_thread::sleep_for(std::chrono::microseconds(50000));
    clock.update();
    const double timeBeforePause = clock.getSimulationTime();

    clock.pause();
    std::this_thread::sleep_for(std::chrono::microseconds(100000));
    clock.start();

    std::this_thread::sleep_for(std::chrono::microseconds(50000));
    clock.update();

    EXPECT_TRUE(clock.isRunning());
    EXPECT_GT(clock.getDeltaTime(), 0.02);
    EXPECT_LT(clock.getDeltaTime(), 0.15);
    EXPECT_NEAR(
        clock.getSimulationTime(),
        timeBeforePause + clock.getDeltaTime(),
        0.01);
}

TEST(WorldClockTest, ResetClearsSimulationTime)
{
    if (SDL_WasInit(SDL_INIT_TIMER) == 0)
    {
        SDL_Init(SDL_INIT_TIMER);
    }

    WorldClock clock;

    std::this_thread::sleep_for(std::chrono::microseconds(50000));
    clock.update();
    EXPECT_GT(clock.getSimulationTime(), 0.0);

    clock.reset();

    EXPECT_DOUBLE_EQ(clock.getSimulationTime(), 0.0);
    EXPECT_DOUBLE_EQ(clock.getDeltaTime(), 0.0);
}

TEST(WorldClockTest, SpeedMultiplierScalesDeltaTime)
{
    if (SDL_WasInit(SDL_INIT_TIMER) == 0)
    {
        SDL_Init(SDL_INIT_TIMER);
    }

    WorldClock clock;
    clock.setSpeedMultiplier(2.0);

    std::this_thread::sleep_for(std::chrono::microseconds(50000));
    clock.update();

    EXPECT_TRUE(clock.getSpeedMultiplier() == 2.0);
    EXPECT_GT(clock.getDeltaTime(), 0.06);
    EXPECT_LT(clock.getDeltaTime(), 0.3);
}
