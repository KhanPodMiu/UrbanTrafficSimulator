#pragma once

#include <memory>
#include <string>

#include "vehicles/Vehicle.hpp"

class Graph;
class RoutingManager;
class RouteOptimizer;

// Factory Pattern: centralizes creation of Vehicle subclasses (Car / Bus /
// EmergencyVehicle) so callers never need to know which concrete class to
// instantiate. spawnRandomVehicle additionally encapsulates "where should a
// new vehicle appear and where is it going", including picking a valid
// origin/destination pair and computing a route via the injected
// RoutingManager - all business logic the rest of the codebase shouldn't
// need to duplicate.
class VehicleFactory {
public:
    // Creates a bare vehicle of the requested type with default type-specific
    // stats. The caller is still responsible for placing it on the map
    // (setRoute / setCurrentRoad / setDestination).
    static std::shared_ptr<Vehicle> createVehicle(VehicleType type, const std::string& id);

    // Picks a random vehicle type, a random start intersection (preferring
    // DEAD_END intersections so vehicles enter "from outside the map" like
    // real traffic; falls back to any intersection if no dead end exists),
    // a random destination, and computes a route between them.
    //
    // Returns nullptr if no valid spawn could be produced this call (e.g. the
    // graph is too small, or no path exists between the chosen pair) -
    // callers should simply try again on a later tick.
    static std::shared_ptr<Vehicle> spawnRandomVehicle(
        const Graph& graph,
        const RoutingManager& routingManager,
        const std::shared_ptr<RouteOptimizer>& optimizer,
        int idNumber);
};
