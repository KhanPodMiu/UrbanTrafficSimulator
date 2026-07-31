#include "simulation/VehicleManager.hpp"

#include <algorithm>
#include <iostream>
#include "factory/VehicleFactory.hpp"
#include "graph/Road.hpp"
#include "simulation/CollisionManager.hpp"
#include "vehicles/Vehicle.hpp"

VehicleManager::VehicleManager(
    Graph& graph,
    const RoutingManager& routingManager,
    std::shared_ptr<RouteOptimizer> optimizer,
    int maxVehicles,
    double spawnIntervalSeconds)
    : m_graph(graph),
      m_routingManager(routingManager),
      m_routeOptimizer(std::move(optimizer)),
      m_collisionManager(std::make_unique<CollisionManager>()),
      m_maxVehicles(maxVehicles),
      m_spawnInterval(spawnIntervalSeconds),
      m_spawnTimer(0.0),
      m_nextVehicleId(1)
{
}

VehicleManager::~VehicleManager() = default;

void VehicleManager::reset()
{
    for (auto& vehicle : m_vehicles)
    {
        if (vehicle && vehicle->getCurrentRoad())
        {
            vehicle->getCurrentRoad()->vehicleExits(vehicle.get());
        }
    }

    m_vehicles.clear();
    m_spawnTimer = 0.0;
    m_nextVehicleId = 1;
}

void VehicleManager::update(double dt)
{
    m_spawnTimer += dt;
    if (m_spawnTimer >= m_spawnInterval)
    {
        m_spawnTimer = 0.0;
        trySpawnVehicle();
    }

    m_collisionManager->update(m_graph, dt);

    for (auto& vehicle : m_vehicles)
    {
        vehicle->update(dt);
    }

    for (auto& vehicle : m_vehicles)
    {
        int guard = 0;
        while (!vehicle->isFinished() &&
               vehicle->getCurrentRoad() &&
               vehicle->getDistanceOnRoad() >= vehicle->getCurrentRoad()->getDistance() &&
               guard++ < 8)
        {
            if (!vehicle->tryAdvanceToNextRoad())
                break;
        }
    }

    removeFinishedVehicles();
}

void VehicleManager::trySpawnVehicle()
{
    if (static_cast<int>(m_vehicles.size()) >= m_maxVehicles)
        return;

    auto vehicle = VehicleFactory::spawnRandomVehicle(m_graph, m_routingManager, m_routeOptimizer, m_nextVehicleId);
    if (vehicle)
    {
        m_nextVehicleId++;
        m_vehicles.push_back(vehicle);
    }
}

void VehicleManager::removeFinishedVehicles()
{
    m_vehicles.erase(
        std::remove_if(
            m_vehicles.begin(),
            m_vehicles.end(),
            [](const std::shared_ptr<Vehicle>& v) { return v->isFinished(); }),
        m_vehicles.end());
}

const std::vector<std::shared_ptr<Vehicle>>& VehicleManager::getVehicles() const
{
    return m_vehicles;
}

void VehicleManager::spawnVehicleAtIntersection(const std::string& intersectionID) 
{    
    auto newVehicle = VehicleFactory::spawnVehicleFrom(
        m_graph, 
        m_routingManager, 
        m_routeOptimizer, 
        m_nextVehicleId, 
        intersectionID
    );


    if (newVehicle) {

        /* std::cout << "[DEBUG] Create vehicle success: " << newVehicle->getId() << std::endl;  */

        m_vehicles.push_back(newVehicle); 
        m_nextVehicleId++;
    } else {
        
        /* std::cout << "[DEBUG] Error: Cannot created vehicle !" << std::endl;  */
    }
}