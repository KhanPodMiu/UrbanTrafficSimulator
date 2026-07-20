#include "simulation/CollisionManager.hpp"

#include <algorithm>
#include <cmath>

#include "graph/Graph.hpp"
#include "graph/Road.hpp"
#include "vehicles/Vehicle.hpp"

void CollisionManager::update(const Graph& graph, double /*dt*/) const
{
    for (const auto& [roadID, road] : graph.getRoads())
    {
        if (!road)
            continue;

        updateRoad(road);
    }
}

void CollisionManager::updateRoad(const std::shared_ptr<Road>& road) const
{
    road->sortVehicles();

    const auto& vehicles = road->getVehicles();
    const double roadLength = road->getDistance();
    const double speedLimit = static_cast<double>(road->getSpeedLimit());
    const bool green = road->isGreen();

    for (size_t i = 0; i < vehicles.size(); ++i)
    {
        Vehicle* vehicle = vehicles[i];
        if (!vehicle)
            continue;

        double desiredSpeed = std::min(vehicle->getMaxSpeed(), speedLimit);

        if (i > 0)
        {
            Vehicle* leader = vehicles[i - 1];
            double bumperGap = (leader->getDistanceOnRoad() - leader->getVehicleLength())
                              - vehicle->getDistanceOnRoad();

            desiredSpeed = std::min(desiredSpeed, computeCarFollowingSpeed(vehicle, leader, bumperGap));
        }

        if (i == 0 && !green && !vehicle->ignoresTrafficLights())
        {
            double distanceToStopLine = roadLength - vehicle->getDistanceOnRoad() - 200.0f;
            if (distanceToStopLine < STOP_REACTION_DISTANCE)
            {
                desiredSpeed = std::min(desiredSpeed, computeStopLineSpeed(distanceToStopLine));
            }
        }

        vehicle->setTargetSpeed(std::max(0.0, desiredSpeed));
    }
}

double CollisionManager::computeCarFollowingSpeed(const Vehicle* vehicle, const Vehicle* leader, double bumperGap) const
{
    if (bumperGap <= MIN_GAP)
        return 0.0;

    double desiredGap = MIN_GAP + vehicle->getCurrentSpeed() * TIME_HEADWAY;

    if (bumperGap >= desiredGap)
        return vehicle->getMaxSpeed();

    double ratio = (bumperGap - MIN_GAP) / (desiredGap - MIN_GAP);
    ratio = std::clamp(ratio, 0.0, 1.0);

    return leader->getCurrentSpeed() * ratio;
}

double CollisionManager::computeStopLineSpeed(double distanceToStopLine) const
{
    if (distanceToStopLine <= 0.0)
        return 0.0;

    return std::sqrt(2.0 * COMFORT_DECELERATION * distanceToStopLine);
}
