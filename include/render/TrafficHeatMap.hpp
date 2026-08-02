#pragma once

#include "SDL2/SDL.h"
#include "simulation/TrafficFlow.hpp"

class TrafficHeatMapScale final
{
public:
    static constexpr int MODERATE_THRESHOLD = TrafficFlow::MODERATE_THRESHOLD;
    static constexpr int HEAVY_THRESHOLD = TrafficFlow::HEAVY_THRESHOLD;

    static TrafficFlowLevel classify(int congestionPercentage) noexcept
    {
        return TrafficFlow::classify(congestionPercentage);
    }

    static SDL_Color colorFor(int congestionPercentage) noexcept;
    static SDL_Color colorFor(TrafficFlowLevel level) noexcept;
};
