#pragma once

enum class TrafficFlowLevel
{
    Low,
    Moderate,
    Heavy
};

class TrafficFlow
{
public:
    static constexpr int MODERATE_THRESHOLD = 40;
    static constexpr int HEAVY_THRESHOLD = 70;

    static TrafficFlowLevel classify(int congestionPercentage) noexcept;
};
