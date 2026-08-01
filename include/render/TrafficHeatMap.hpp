#pragma once

#include "SDL2/SDL.h"

/// Semantic traffic bands used by the map overlay and its legend.
enum class TrafficFlowLevel
{
    Low,
    Moderate,
    Heavy
};

/// Centralizes the heat-map thresholds and palette so rendering, tests, and
/// future routing UI all interpret congestion in exactly the same way.
class TrafficHeatMapScale final
{
public:
    static constexpr int MODERATE_THRESHOLD = 40;
    static constexpr int HEAVY_THRESHOLD = 70;

    static TrafficFlowLevel classify(int congestionPercentage) noexcept;
    static SDL_Color colorFor(int congestionPercentage) noexcept;
};
