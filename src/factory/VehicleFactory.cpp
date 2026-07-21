#include "factory/VehicleFactory.hpp"

#include <random>
#include <vector>

#include "algorithms/RoutingManager.hpp"
#include "graph/Graph.hpp"
#include "graph/Intersection.hpp"
#include "graph/Road.hpp"
#include "simulation/RouteOptimizer.hpp"
#include "simulation/RouteRequest.hpp"
#include "simulation/RouteResult.hpp"
#include "vehicles/Bus.hpp"
#include "vehicles/Car.hpp"
#include "vehicles/EmergencyVehicle.hpp"

namespace {
    std::mt19937& rng()
    {
        static std::mt19937 generator(std::random_device{}());
        return generator;
    }

    // Uniformly picks an index in [0, size).
    size_t randomIndex(size_t size)
    {
        std::uniform_int_distribution<size_t> dist(0, size - 1);
        return dist(rng());
    }

    VehicleType pickRandomType()
    {
        std::uniform_int_distribution<int> dist(1, 100);
        int roll = dist(rng());

        if (roll <= 70) return VehicleType::CAR;
        if (roll <= 90) return VehicleType::BUS;
        return VehicleType::EMERGENCY;
    }
}

std::shared_ptr<Vehicle> VehicleFactory::createVehicle(VehicleType type, const std::string& id)
{
    switch (type)
    {
        case VehicleType::BUS:
            return std::make_shared<Bus>(id);
        case VehicleType::EMERGENCY:
            return std::make_shared<EmergencyVehicle>(id);
        case VehicleType::CAR:
        default:
            return std::make_shared<Car>(id);
    }
}

std::shared_ptr<Vehicle> VehicleFactory::spawnRandomVehicle(
    const Graph& graph,
    const RoutingManager& routingManager,
    const std::shared_ptr<RouteOptimizer>& optimizer,
    int idNumber)
{
    const auto& intersections = graph.getIntersections();

    std::vector<std::string> allIDs;
    std::vector<std::string> deadEndIDs;
    allIDs.reserve(intersections.size());

    for (const auto& [id, intersection] : intersections)
    {
        if (!intersection || intersection->getDegree() == 0)
            continue; 

        allIDs.push_back(id);

        if (intersection->getType() == IntersectionType::DEAD_END)
            deadEndIDs.push_back(id);
    }

    if (allIDs.size() < 2)
        return nullptr;

    const std::vector<std::string>& startPool = !deadEndIDs.empty() ? deadEndIDs : allIDs;
    std::string startID = startPool[randomIndex(startPool.size())];

    std::string destID = startID;
    for (int attempt = 0; attempt < 8 && destID == startID; ++attempt)
    {
        destID = allIDs[randomIndex(allIDs.size())];
    }
    if (destID == startID)
        return nullptr;

    RouteRequest request(startID, destID);
    RouteResult result = routingManager.calculateRoute(graph, request);

    if (!result.isSuccess || result.intersectionIDs.size() < 2)
        return nullptr; // no path between the chosen pair, try again next tick

    std::vector<std::shared_ptr<Road>> route;
    route.reserve(result.intersectionIDs.size() - 1);

    for (size_t i = 0; i + 1 < result.intersectionIDs.size(); ++i)
    {
        Road* road = graph.getRoadBetween(result.intersectionIDs[i], result.intersectionIDs[i + 1]);
        if (!road)
            return nullptr;

        std::shared_ptr<Road> roadPtr = graph.getRoad(road->getRoadId());
        if (!roadPtr)
            return nullptr;

        route.push_back(roadPtr);
    }

    if (route.empty())
        return nullptr;

    VehicleType type = pickRandomType();
    std::string id = "V" + std::to_string(idNumber);

    std::shared_ptr<Vehicle> vehicle = createVehicle(type, id);

    vehicle->setRoute(route);
    vehicle->setCurrentRoad(route.front());
    vehicle->setDestination(graph.getIntersection(destID));
    vehicle->setDistanceOnRoad(0.0);
    vehicle->setCurrentSpeed(0.0);
    vehicle->setTargetSpeed(vehicle->getMaxSpeed());

    if (optimizer)
        vehicle->assignRoute(optimizer);

    route.front()->vehicleEnters(vehicle.get());
    vehicle->updateWorldPosition();

    return vehicle;
}
