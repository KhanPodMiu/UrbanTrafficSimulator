#pragma once

#include <cstddef>
#include <memory>
#include <vector>

class Graph;
class Road;
class VehicleManager;

enum class PresidentialRouteState
{
    Inactive,
    Preparing,
    Active
};

/// Coordinates a safe two-phase presidential route closure.
///
/// During Preparing, target roads are unavailable to all new routes while
/// vehicles already inside the corridor are allowed to leave. The route only
/// becomes Active after every target road is clear and no remaining vehicle
/// route points into the corridor.
class PresidentialRouteManager
{
public:
    // Legacy fixed-delay option (disabled):
    // static constexpr double DEFAULT_CLEARANCE_SECONDS = 5.0;

    PresidentialRouteManager(Graph& graph, VehicleManager& vehicleManager);

    /// Converts roads loaded as VIP-exclusive into pending closures, reroutes
    /// affected vehicles, and returns the number of routes rebuilt.
    int beginPreparation();

    /// Returns true exactly once, on the update that promotes the fully clear
    /// and fully rerouted corridor to Active.
    bool update();

    /// Cancels either a pending or active presidential route.
    void cancel();

    PresidentialRouteState getState() const noexcept;
    std::size_t getTargetRoadCount() const noexcept;
    int getVehiclesRemainingOnRoute() const noexcept;
    int getVehiclesStillRoutedToCorridor() const noexcept;

private:
    bool isCorridorReady() const noexcept;

    Graph& graph_;
    VehicleManager& vehicleManager_;
    PresidentialRouteState state_ = PresidentialRouteState::Inactive;
    std::vector<std::shared_ptr<Road>> targetRoads_;
};
