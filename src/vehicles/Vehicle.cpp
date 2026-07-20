#include "vehicles/Vehicle.hpp"

#include "graph/Intersection.hpp"
#include "graph/Road.hpp"

Vehicle::Vehicle(const std::string& id, double maxSpeed)
    : m_id(id),
      m_currentSpeed(0.0),
      m_targetSpeed(maxSpeed),
      m_maxSpeed(maxSpeed),
      m_distanceOnRoad(0.0),
      m_length(5.0),
      m_finished(false),
      m_currentRoad(nullptr),
      m_destination(nullptr),
      m_routeOptimizer(nullptr),
      m_routeIndex(0),
      m_position{0.0, 0.0}
{
}

void Vehicle::update(double dt)
{
    m_distanceOnRoad += m_currentSpeed * dt;
}

void Vehicle::assignRoute(const std::shared_ptr<RouteOptimizer>& optimizer)
{
    m_routeOptimizer = optimizer;
}

const std::string& Vehicle::getId() const
{
    return m_id;
}

std::shared_ptr<Road> Vehicle::getCurrentRoad() const
{
    return m_currentRoad;
}

std::shared_ptr<Intersection> Vehicle::getDestination() const
{
    return m_destination;
}

Vector2 Vehicle::getPosition() const
{
    return m_position;
}

double Vehicle::getCurrentSpeed() const
{
    return m_currentSpeed;
}

double Vehicle::getTargetSpeed() const
{
    return m_targetSpeed;
}

double Vehicle::getMaxSpeed() const
{
    return m_maxSpeed;
}

double Vehicle::getDistanceOnRoad() const
{
    return m_distanceOnRoad;
}

double Vehicle::getVehicleLength() const
{
    return m_length;
}

bool Vehicle::isFinished() const
{
    return m_finished;
}

double Vehicle::getMovementProgress() const
{
    if (!m_currentRoad)
        return 0.0;

    return m_distanceOnRoad / m_currentRoad->getDistance();
}

const std::vector<std::shared_ptr<Road>>& Vehicle::getRoute() const
{
    return m_route;
}

size_t Vehicle::getRouteIndex() const
{
    return m_routeIndex;
}

void Vehicle::setCurrentRoad(const std::shared_ptr<Road>& road)
{
    m_currentRoad = road;
}

void Vehicle::setDestination(const std::shared_ptr<Intersection>& destination)
{
    m_destination = destination;
}

void Vehicle::setPosition(const Vector2& position)
{
    m_position = position;
}

void Vehicle::setCurrentSpeed(double speed)
{
    m_currentSpeed = speed;
}

void Vehicle::setTargetSpeed(double speed)
{
    m_targetSpeed = speed;
}

void Vehicle::setDistanceOnRoad(double distance)
{
    m_distanceOnRoad = distance;
}

void Vehicle::setRoute(const std::vector<std::shared_ptr<Road>>& route)
{
    m_route = route;
    m_routeIndex = 0;
}

void Vehicle::setRouteIndex(size_t index)
{
    m_routeIndex = index;
}

void Vehicle::setFinished(bool finished)
{
    m_finished = finished;
}

void Vehicle::advanceDistance(double distance)
{
    m_distanceOnRoad += distance;
}

std::shared_ptr<Road> Vehicle::getNextRoad() const
{
    if (m_routeIndex + 1 >= m_route.size())
        return nullptr;

    return m_route[m_routeIndex + 1];
}