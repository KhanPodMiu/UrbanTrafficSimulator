#pragma once

#include <vector>
#include <memory>
#include <string>
#include <random>
#include "vehicles/Vehicle.hpp"

class Graph;
class Road;
class RoutingManager;

// Owns every Vehicle in the simulation, spawns/despawns them to reach a
// target on-map count, and drives their per-frame movement + car-following
// collision handling.
class VehicleManager {
private:
    std::vector<std::shared_ptr<Vehicle>> m_vehicles;
    int m_targetCount;
    int m_nextVehicleId;
    mutable std::mt19937 m_rng;

    bool spawnRandomVehicle(Graph& graph, RoutingManager& routingManager);
    double computeAllowedSpeed(Vehicle& vehicle, const std::shared_ptr<Road>& road) const;
    void updatePosition(Vehicle& vehicle, const std::shared_ptr<Road>& road) const;
    // Pops the vehicle off its current road and pushes it onto the next road
    // in its route. Returns false if there is no next road (route broken).
    bool transitionToNextRoad(const std::shared_ptr<Vehicle>& vehicle, Graph& graph);

public:
    VehicleManager();

    void setTargetVehicleCount(int count);
    int getTargetVehicleCount() const;
    int getCurrentVehicleCount() const;

    void update(double dt, Graph& graph, RoutingManager& routingManager);

    const std::vector<std::shared_ptr<Vehicle>>& getVehicles() const;
};