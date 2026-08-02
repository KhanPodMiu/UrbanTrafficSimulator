#include "simulation/TrafficFlow.hpp"

TrafficFlowLevel TrafficFlow::classify(int congestionPercentage) noexcept
{
    if (congestionPercentage >= HEAVY_THRESHOLD)
        return TrafficFlowLevel::Heavy;

    if (congestionPercentage >= MODERATE_THRESHOLD)
        return TrafficFlowLevel::Moderate;

    return TrafficFlowLevel::Low;
}
