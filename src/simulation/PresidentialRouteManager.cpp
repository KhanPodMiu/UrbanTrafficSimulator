#include "simulation/PresidentialRouteManager.hpp"

#include <algorithm>
#include <unordered_set>

#include "graph/Graph.hpp"
#include "graph/Road.hpp"
#include "simulation/VehicleManager.hpp"
#include "vehicles/Vehicle.hpp"

PresidentialRouteManager::PresidentialRouteManager(
    Graph& graph,
    VehicleManager& vehicleManager)
    : graph_(graph),
      vehicleManager_(vehicleManager)
{
}

int PresidentialRouteManager::beginPreparation()
{
    targetRoads_.clear();
    // Legacy fixed-delay state (disabled):
    // elapsedPreparationSeconds_ = 0.0;
    // minimumClearanceSeconds_ = std::max(0.0, minimumClearanceSeconds);

    for (const auto& [roadID, road] : graph_.getRoads())
    {
        if (!road ||
            (!road->isVIPExclusive() && !road->isVIPClosurePending()))
        {
            continue;
        }

        road->setVIPClosurePending(true);
        targetRoads_.push_back(road);
    }

    if (targetRoads_.empty())
    {
        state_ = PresidentialRouteState::Inactive;
        return 0;
    }

    state_ = PresidentialRouteState::Preparing;
    return vehicleManager_.rerouteVehiclesAroundBannedRoads();
}

bool PresidentialRouteManager::update()
{
    if (state_ != PresidentialRouteState::Preparing)
        return false;

    // Legacy time gate (disabled):
    // elapsedPreparationSeconds_ += std::max(0.0, dt);
    // if (elapsedPreparationSeconds_ < minimumClearanceSeconds_)
    if (!isCorridorReady())
        return false;

    for (const auto& road : targetRoads_)
    {
        if (road)
            road->setVIPExclusive(true);
    }

    state_ = PresidentialRouteState::Active;
    return true;
}

void PresidentialRouteManager::cancel()
{
    for (const auto& road : targetRoads_)
    {
        if (!road)
            continue;

        road->setVIPClosurePending(false);
        road->setVIPExclusive(false);
    }

    targetRoads_.clear();
    // elapsedPreparationSeconds_ = 0.0; // Legacy fixed-delay state.
    state_ = PresidentialRouteState::Inactive;
}

PresidentialRouteState PresidentialRouteManager::getState() const noexcept
{
    return state_;
}

std::size_t PresidentialRouteManager::getTargetRoadCount() const noexcept
{
    return targetRoads_.size();
}

int PresidentialRouteManager::getVehiclesRemainingOnRoute() const noexcept
{
    int vehicleCount = 0;
    for (const auto& road : targetRoads_)
    {
        if (road)
            vehicleCount += road->getTotalVehicles();
    }
    return vehicleCount;
}

int PresidentialRouteManager::getVehiclesStillRoutedToCorridor() const noexcept
{
    std::unordered_set<const Road*> targetRoadPointers;
    targetRoadPointers.reserve(targetRoads_.size());
    for (const auto& road : targetRoads_)
    {
        if (road)
            targetRoadPointers.insert(road.get());
    }

    int vehicleCount = 0;
    std::unordered_set<const Vehicle*> inspectedVehicles;
    for (const auto& [roadID, road] : graph_.getRoads())
    {
        if (!road)
            continue;

        for (const Vehicle* vehicle : road->getVehicles())
        {
            if (!vehicle || vehicle->isFinished() ||
                !inspectedVehicles.insert(vehicle).second)
            {
                continue;
            }

            const auto& route = vehicle->getRoute();
            const std::size_t startIndex = std::min(
                vehicle->getRouteIndex(),
                route.size());
            for (std::size_t i = startIndex; i < route.size(); ++i)
            {
                if (route[i] &&
                    targetRoadPointers.count(route[i].get()) > 0)
                {
                    ++vehicleCount;
                    break;
                }
            }
        }
    }
    return vehicleCount;
}

bool PresidentialRouteManager::isCorridorReady() const noexcept
{
    return getVehiclesRemainingOnRoute() == 0 &&
           getVehiclesStillRoutedToCorridor() == 0;
}
