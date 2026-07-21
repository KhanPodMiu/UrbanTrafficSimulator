#include "vehicles/Vehicle.hpp"

#include <algorithm>

#include "graph/Intersection.hpp"
#include "graph/Road.hpp"

namespace {
    constexpr double COMFORT_ACCEL = 35.0; 
    constexpr double LANE_OFFSET = 40.0; 
}

Vehicle::Vehicle(const std::string& id, double maxSpeed, double length, VehicleType type)
    : m_id(id),
      m_type(type),
      m_currentSpeed(0.0),
      m_targetSpeed(maxSpeed),
      m_maxSpeed(maxSpeed),
      m_distanceOnRoad(0.0),
      m_length(length),
      m_finished(false),
      m_currentRoad(nullptr),
      m_destination(nullptr),
      m_routeOptimizer(nullptr),
      m_routeIndex(0),
      m_position{0.0, 0.0}
{
}

Vehicle::Vehicle(const std::string& id, double maxSpeed)
    : Vehicle(id, maxSpeed, 5.0, VehicleType::CAR)
{
}

void Vehicle::update(double dt)
{
    if (m_finished)
        return;

    if (m_currentSpeed < m_targetSpeed)
    {
        m_currentSpeed = std::min(m_targetSpeed, m_currentSpeed + COMFORT_ACCEL * dt);
    }
    else if (m_currentSpeed > m_targetSpeed)
    {
        m_currentSpeed = std::max(m_targetSpeed, m_currentSpeed - COMFORT_ACCEL * dt);
    }

    if (m_currentSpeed < 0.0)
        m_currentSpeed = 0.0;

    m_distanceOnRoad += m_currentSpeed * dt;

    updateWorldPosition();
}

void Vehicle::assignRoute(const std::shared_ptr<RouteOptimizer>& optimizer)
{
    m_routeOptimizer = optimizer;
}

void Vehicle::updateWorldPosition()
{
    if (!m_currentRoad)
        return;

    const Intersection* src = m_currentRoad->getSourceIntersection();
    const Intersection* dst = m_currentRoad->getDestinationIntersection();

    if (!src || !dst)
        return;

    double roadLength = m_currentRoad->getDistance();
    double t = (roadLength > 0.0) ? (m_distanceOnRoad / roadLength) : 0.0;
    t = std::clamp(t, 0.0, 1.0);

    double x0 = src->getX();
    double y0 = src->getY();
    double x1 = dst->getX();
    double y1 = dst->getY();

    double dx = x1 - x0;
    double dy = y1 - y0;

    double baseX = x0 + dx * t;
    double baseY = y0 + dy * t;

    if (roadLength > 0.0)
    {
        // Perpendicular unit vector (right-hand side of travel direction).
        double nx = -dy / roadLength;
        double ny = dx / roadLength;

        baseX += nx * LANE_OFFSET;
        baseY += ny * LANE_OFFSET;
    }

    m_position = Vector2(static_cast<float>(baseX), static_cast<float>(baseY));
}

bool Vehicle::tryAdvanceToNextRoad()
{
    if (!m_currentRoad)
        return false;

    if (m_distanceOnRoad < m_currentRoad->getDistance())
        return false; 

    if (!m_currentRoad->isGreen() && !ignoresTrafficLights())
    {
        m_distanceOnRoad = m_currentRoad->getDistance();
        m_currentSpeed = 0.0;
        updateWorldPosition();
        return false;
    }

    double overflow = m_distanceOnRoad - m_currentRoad->getDistance();

    std::shared_ptr<Road> next = getNextRoad();

    m_currentRoad->vehicleExits(this);

    if (!next)
    {
        // No more roads in the route: trip complete.
        m_finished = true;
        m_distanceOnRoad = m_currentRoad->getDistance();
        updateWorldPosition();
        return true;
    }

    m_currentRoad = next;
    m_routeIndex++;
    m_distanceOnRoad = overflow;

    m_currentRoad->vehicleEnters(this);

    updateWorldPosition();
    return true;
}

const std::string& Vehicle::getId() const
{
    return m_id;
}

VehicleType Vehicle::getType() const
{
    return m_type;
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

double Vehicle::getHeadingAngle() const
{
    if (!m_currentRoad)
        return 0.0;

    const Intersection* src = m_currentRoad->getSourceIntersection();
    const Intersection* dst = m_currentRoad->getDestinationIntersection();

    double dx = dst->getX() - src->getX();
    double dy = dst->getY() - src->getY();

    return atan2(dy, dx) * 180.0 / M_PI;
}