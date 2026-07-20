#pragma once

#include <memory>

class Graph;
class Road;
class Vehicle;

// CollisionManager implements a simplified car-following model (similar in
// spirit to an Adaptive Cruise Control / a basic IDM) plus traffic-light
// awareness.
//
// Design note: CollisionManager does NOT move vehicles itself. Each
// simulation tick it only recomputes every vehicle's targetSpeed based on
// what is ahead of it (another vehicle on the same road, or a red/yellow
// light at the end of the road). Vehicle::update() is the one that actually
// integrates that targetSpeed into motion. Keeping "decide how fast" and
// "actually move" in separate classes keeps each one small and testable
// (Single Responsibility Principle).
//
// Known simplification / possible extension: this only prevents rear-end
// collisions on the same road and enforces stopping for red lights. It does
// not yet arbitrate conflicting movements between vehicles from *different*
// roads merging at the same intersection - a good next step would be a
// TrafficController that reserves intersection "slots" the same way
// TrafficLightManager reserves signal phases.
class CollisionManager {
public:
    // Minimum bumper-to-bumper gap that must always be kept between two
    // vehicles on the same road, regardless of speed.
    static constexpr double MIN_GAP = 4.0; // map units

    // Extra following distance added per unit of the follower's speed
    // ("time headway" - the classic car-following safety parameter).
    static constexpr double TIME_HEADWAY = 1.6; // seconds

    // Distance from the end of a road at which the lead vehicle starts
    // reacting to a red/yellow light.
    static constexpr double STOP_REACTION_DISTANCE = 120.0; // map units

    // Comfortable braking deceleration used to compute a safe approach speed
    // toward a stop line (v = sqrt(2 * a * d)).
    static constexpr double COMFORT_DECELERATION = 30.0; // map units / s^2

    // Recomputes targetSpeed for every vehicle currently on the graph's roads.
    void update(const Graph& graph, double dt) const;

private:
    void updateRoad(const std::shared_ptr<Road>& road) const;

    double computeCarFollowingSpeed(const Vehicle* vehicle, const Vehicle* leader, double bumperGap) const;
    double computeStopLineSpeed(double distanceToStopLine) const;
};
