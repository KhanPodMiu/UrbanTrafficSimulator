#pragma once

#include <memory>
#include <string>

#include "vehicles/Vehicle.hpp"

class Graph;
class RoutingManager;
class RouteOptimizer;

class VehicleFactory {
public:
    static std::shared_ptr<Vehicle> createVehicle(VehicleType type, const std::string& id);

    static std::shared_ptr<Vehicle> spawnRandomVehicle(
        const Graph& graph,
        const RoutingManager& routingManager,
        const std::shared_ptr<RouteOptimizer>& optimizer,
        int idNumber);

    //Added: Spawn from click position
    static std::shared_ptr<Vehicle> spawnVehicleFrom (
        const Graph& graph,
        const RoutingManager& routingManager,
        const std::shared_ptr<RouteOptimizer>& optimizer,
        int idNumber,
        const std::string& startIntersectionID);
};

