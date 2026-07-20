#pragma once

#include <memory>
#include <vector>

class Graph;
class RoutingManager;
class RouteOptimizer;
class Vehicle;
class CollisionManager;

class VehicleManager {
public:
    VehicleManager(
        Graph& graph,
        const RoutingManager& routingManager,
        std::shared_ptr<RouteOptimizer> optimizer,
        int maxVehicles = 40,
        double spawnIntervalSeconds = 1.2);

    ~VehicleManager();

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
