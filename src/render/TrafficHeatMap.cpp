#include "render/TrafficHeatMap.hpp"

SDL_Color TrafficHeatMapScale::colorFor(int congestionPercentage) noexcept
{
    return colorFor(classify(congestionPercentage));
}

SDL_Color TrafficHeatMapScale::colorFor(TrafficFlowLevel level) noexcept
{
    switch (level)
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
