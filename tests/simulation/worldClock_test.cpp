#include <gtest/gtest.h>
#include <unistd.h>
#include "simulation/WorldClock.hpp"

TEST(WorldClockTest, Initialization)
{
    // Initialize SDL if it hasn't been initialized by gtest main
    if (SDL_WasInit(SDL_INIT_TIMER) == 0) {
        SDL_Init(SDL_INIT_TIMER);
    }

    WorldClock clock;
    EXPECT_DOUBLE_EQ(clock.getDeltaTime(), 0.0);
    EXPECT_DOUBLE_EQ(clock.getSimulationTime(), 0.0);
}

TEST(WorldClockTest, UpdateTime)
{
    if (SDL_WasInit(SDL_INIT_TIMER) == 0) {
        SDL_Init(SDL_INIT_TIMER);
    }

    WorldClock clock;
    
    // Sleep for a short duration to allow time to pass
    usleep(100000); // 100 milliseconds
    
    clock.update();
    
    double dt = clock.getDeltaTime();
    double simTime = clock.getSimulationTime();
    
    // dt and simTime should be around 0.1s
    EXPECT_GT(dt, 0.05);
    EXPECT_LT(dt, 0.2);
    EXPECT_DOUBLE_EQ(dt, simTime);
    
    usleep(50000); // 50 milliseconds
    
    clock.update();
    
    double dt2 = clock.getDeltaTime();
    double simTime2 = clock.getSimulationTime();
    
    EXPECT_GT(dt2, 0.02);
    EXPECT_LT(dt2, 0.1);
    EXPECT_NEAR(simTime2, dt + dt2, 0.001);
}
