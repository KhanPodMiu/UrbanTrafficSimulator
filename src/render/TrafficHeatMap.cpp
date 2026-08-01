#include "render/TrafficHeatMap.hpp"

TrafficFlowLevel TrafficHeatMapScale::classify(
    int congestionPercentage) noexcept
{
    if (congestionPercentage >= HEAVY_THRESHOLD)
        return TrafficFlowLevel::Heavy;

    if (congestionPercentage >= MODERATE_THRESHOLD)
        return TrafficFlowLevel::Moderate;

    return TrafficFlowLevel::Low;
}

SDL_Color TrafficHeatMapScale::colorFor(
    int congestionPercentage) noexcept
{
    switch (classify(congestionPercentage))
    {
        case TrafficFlowLevel::Heavy:
            return SDL_Color{239, 68, 68, 255};

        case TrafficFlowLevel::Moderate:
            return SDL_Color{245, 158, 11, 255};

        case TrafficFlowLevel::Low:
        default:
            return SDL_Color{34, 197, 94, 255};
    }
}
