#pragma once

#include <memory>

class Graph;
class Road;
class Vehicle;

class CollisionManager {
public:
    static constexpr double MIN_GAP = 100.0; // map units

    static constexpr double TIME_HEADWAY = 1.6; // seconds

    static constexpr double STOP_REACTION_DISTANCE = 120.0; // map units

    static constexpr double COMFORT_DECELERATION = 30.0; // map units / s^2

    void update(const Graph& graph, double dt) const;

private:
    void updateRoad(const std::shared_ptr<Road>& road) const;

    double computeCarFollowingSpeed(const Vehicle* vehicle, const Vehicle* leader, double bumperGap) const;
    double computeStopLineSpeed(double distanceToStopLine) const;
};
