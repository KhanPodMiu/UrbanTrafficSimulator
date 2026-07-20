#pragma once

#include <memory>
#include <vector>

class Graph;
class RoutingManager;
class RouteOptimizer;
class Vehicle;
class CollisionManager;

// Owns and drives the lifecycle of every Vehicle in the simulation:
//   - periodically spawns new vehicles via VehicleFactory,
//   - each tick asks CollisionManager to decide safe speeds,
//   - advances every vehicle's kinematics and road-to-road transitions,
//   - removes vehicles that have reached their destination.
//
// This keeps main.cpp thin (it just calls update() once per frame) and keeps
// the Graph / RoutingManager / RouteOptimizer / CollisionManager collaborators
// wired together in one place instead of scattered across main.cpp.
class VehicleManager {
public:
    VehicleManager(
        Graph& graph,
        const RoutingManager& routingManager,
        std::shared_ptr<RouteOptimizer> optimizer,
        int maxVehicles = 40,
        double spawnIntervalSeconds = 1.2);

    ~VehicleManager();

    // Advances the whole simulation by dt seconds: spawning, collision
    // avoidance, movement, road transitions and cleanup of finished trips.
    void update(double dt);

    const std::vector<std::shared_ptr<Vehicle>>& getVehicles() const;

private:
    void trySpawnVehicle();
    void removeFinishedVehicles();

    Graph& m_graph;
    const RoutingManager& m_routingManager;
    std::shared_ptr<RouteOptimizer> m_routeOptimizer;
    std::unique_ptr<CollisionManager> m_collisionManager;

    std::vector<std::shared_ptr<Vehicle>> m_vehicles;

    int m_maxVehicles;
    double m_spawnInterval;
    double m_spawnTimer;
    int m_nextVehicleId;
};
