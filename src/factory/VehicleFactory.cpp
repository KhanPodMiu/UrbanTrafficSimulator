#include "factory/VehicleFactory.hpp"

#include <random>
#include <vector>
//#include <iostream> //For cout debug

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

    //Added: Prevent spawning vehicle exceed road capacity 
    //Prevent spawning vehicle when the previous one not yet exit the intersection

    auto firstRoad = route.front();
    const auto& vehiclesOnFirstRoad = firstRoad->getVehicles();

    double roadLength = firstRoad->getDistance();
    int roadCapacity = static_cast<int>(roadLength / Road::VEHICLE_HITBOX_SIZE);

    if (static_cast<int>(vehiclesOnFirstRoad.size()) >= roadCapacity) {
        return nullptr; 
    }

    if (!vehiclesOnFirstRoad.empty()) {
        auto lastVehicle = vehiclesOnFirstRoad.back();
        if (lastVehicle->getDistanceOnRoad() < Road::VEHICLE_HITBOX_SIZE) {
            return nullptr; 
        }
    }
    
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

//Added
std::shared_ptr<Vehicle> VehicleFactory::spawnVehicleFrom(
    const Graph& graph,
    const RoutingManager& routingManager,
    const std::shared_ptr<RouteOptimizer>& optimizer,
    int idNumber,
    const std::string& startIntersectionID)
{
    auto destNode = graph.getRandomIntersectionExcept(startIntersectionID);
    if (!destNode) return nullptr;

    std::string destID = destNode->getIntersectionID();

    RouteRequest request(startIntersectionID, destID);
    RouteResult route = routingManager.calculateRoute(graph, request);

    if (!route.isSuccess || route.intersectionIDs.size() < 2) {
        return nullptr;
    }

    std::string vehicleIdStr = "V_" + std::to_string(idNumber);
    auto vehicle = createVehicle(pickRandomType(), vehicleIdStr); 

    if (vehicle) {
        std::vector<std::shared_ptr<Road>> roadPath;

        for (size_t i = 0; i < route.intersectionIDs.size() - 1; ++i) {
            const std::string& currNode = route.intersectionIDs[i];
            const std::string& nextNode = route.intersectionIDs[i+1];
            
            const auto& connectedRoads = graph.getConnectedRoads(currNode);
            
            for (const auto& road : connectedRoads) {
                if (road && road->getDestinationIntersection() && 
                    road->getDestinationIntersection()->getIntersectionID() == nextNode) {
                    roadPath.push_back(road);
                    break;
                }
            }
        }
        

        if (!roadPath.empty()) {
            auto firstRoad = roadPath.front(); 

            const auto& vehiclesOnFirstRoad = firstRoad->getVehicles(); 

            double roadLength = firstRoad->getDistance(); 
            int roadCapacity = static_cast<int>(roadLength / Road::VEHICLE_HITBOX_SIZE);

            /* std::cout << "[DEBUG] Duong " << firstRoad->getRoadId() 
            << " | Road lenght " << roadLength 
            << " | Capacity: " << roadCapacity 
            << " | Current vehilce: " << vehiclesOnFirstRoad.size() << std::endl; */

            if (static_cast<int>(vehiclesOnFirstRoad.size()) >= roadCapacity) {

                /* std::cout << "[DEBUG] Spawn denied: Reach maximum " << firstRoad->getRoadId() 
                          << " vehicle (" << roadCapacity << " cars)!" << std::endl; */

                return nullptr; 
            }

            bool isSafeToSpawn = true;
            if (!vehiclesOnFirstRoad.empty()) {
                auto lastVehicle = vehiclesOnFirstRoad.back(); 
                if (lastVehicle->getDistanceOnRoad() < Road::VEHICLE_HITBOX_SIZE) {
                    isSafeToSpawn = false; 
                }
            }

            if (isSafeToSpawn) {
                vehicle->setRoute(roadPath); 
                vehicle->setCurrentRoad(firstRoad);
                vehicle->setDestination(graph.getIntersection(destID));
                vehicle->setDistanceOnRoad(0.0);
                vehicle->setCurrentSpeed(0.0);
                vehicle->setTargetSpeed(vehicle->getMaxSpeed());

                if (optimizer) {
                    vehicle->assignRoute(optimizer);
                }

                firstRoad->vehicleEnters(vehicle.get());
                vehicle->updateWorldPosition(); 
            } else {

                /* std::cout << "[DEBUG] Wait for more spawn" << std::endl; */
                
                return nullptr; 
            }
        } else {
            return nullptr;
        }
    }

    return vehicle;
}