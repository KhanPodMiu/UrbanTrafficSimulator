#include "Vehicle.hpp"
#include "Intersection.hpp" 
#include "Road.hpp"        

#include <string>
#include <memory>
#include <vector>
#include "../utils/vector2i.hpp" 

Vehicle::Vehicle(const std::string& id, double initialSpeed)
    : m_id(id), 
      m_speed(initialSpeed), 
      m_progress(0.0), 
      m_currentRoad(nullptr), 
      m_destination(nullptr), 
      m_routeOptimizer(nullptr),
      m_position{0.0, 0.0} 
{}

void Vehicle::update(double dt) {
    m_progress += m_speed * dt;
}

void Vehicle::assignRoute(const std::shared_ptr<RouteOptimizer>& routeOptimizer) {
    m_routeOptimizer = routeOptimizer;
}

std::string Vehicle::getId() const {
    return m_id;
}

std::shared_ptr<Road> Vehicle::getCurrentRoad() const {
    return m_currentRoad;
}

std::shared_ptr<Intersection> Vehicle::getDestination() const {
    return m_destination;
}


void Vehicle::setDestination(const std::shared_ptr<Intersection>& destination) {
    m_destination = destination;
}

double Vehicle::getMovementProgress() const {
    return m_progress;
}

double Vehicle::getCurrentSpeed() const {
    return m_speed;
}

Vector2 Vehicle::getPosition() const {
    return m_position;
}

void Vehicle::setSpeed(double speed) {
    m_speed = speed;
}

void Vehicle::setPosition(const Vector2& position) {
    m_position = position;
}