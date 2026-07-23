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

    // Sorted with index 0 = vehicle furthest along the road (closest to the
    // intersection / stop line).
    const auto& vehicles = road->getVehicles();
    const double roadLength = road->getDistance();
    const double speedLimit = static_cast<double>(road->getSpeedLimit());
    const bool green = road->isGreen();

    Vehicle* mainLaneLeader = nullptr;
    Vehicle* passingLaneLeader = nullptr;
    bool seenMainLaneFront = false; // has the main lane's front vehicle been assigned yet?

    for (size_t i = 0; i < vehicles.size(); ++i)
    {
        Vehicle* vehicle = vehicles[i];
        if (!vehicle)
            continue;

        double desiredSpeed = std::min(vehicle->getMaxSpeed(), speedLimit);

        Vehicle* leader = vehicle->isInPassingLane() ? passingLaneLeader : mainLaneLeader;

        if (leader)
        {
            double bumperGap = (leader->getDistanceOnRoad() - leader->getVehicleLength())
                              - vehicle->getDistanceOnRoad();

            double followSpeed = computeCarFollowingSpeed(vehicle, leader, bumperGap);

            if (!vehicle->isInPassingLane() &&
                vehicle->canOvertakeBlockedTraffic() &&
                followSpeed < desiredSpeed - 0.5)
            {
                // The vehicle ahead is slowing us down: pull out to the
                // shoulder instead of following it. From this point on this
                // vehicle is no longer part of the main lane's queue/gap
                // chain, so it doesn't inherit that leader's speed cap.
                vehicle->setInPassingLane(true);
            }
            else
            {
                desiredSpeed = std::min(desiredSpeed, followSpeed);
            }
        }

        // Traffic light / stop line only applies to whichever vehicle is at
        // the front of the MAIN lane (a vehicle out on the shoulder has
        // already committed to passing and isn't queueing at the light here).
        if (!vehicle->isInPassingLane() && !seenMainLaneFront &&
            !green && !vehicle->ignoresTrafficLights() && !vehicle->isCommittedToIntersection())
        {
            double distanceToIntersection =
                roadLength - vehicle->getDistanceOnRoad();

            double currentSpeed = vehicle->getCurrentSpeed();
            double stoppingDistance = (currentSpeed * currentSpeed) / (2.0 * COMFORT_DECELERATION);

            if (distanceToIntersection <= 0.0 || distanceToIntersection < stoppingDistance)
            {
                vehicle->setCommittedToIntersection(true);
            }
            else if (distanceToIntersection <= STOP_REACTION_DISTANCE)
            {
                desiredSpeed = std::min(
                    desiredSpeed,
                    computeStopLineSpeed(distanceToIntersection));
            }
        }

        if (vehicle->isInPassingLane())
            passingLaneLeader = vehicle;
        else
        {
            mainLaneLeader = vehicle;
            seenMainLaneFront = true;
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