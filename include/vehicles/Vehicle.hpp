#pragma once

#include <string>
#include <memory>
#include <vector>
#include "../utils/vector2i.hpp"

class Road;
class Intersection;    
class RouteOptimizer; 

class Vehicle {
private:
    std::string m_id;
    double m_speed;
    double m_progress; 
    std::shared_ptr<Road> m_currentRoad;
    std::shared_ptr<Intersection> m_destination; 
    std::shared_ptr<RouteOptimizer> m_routeOptimizer;
    Vector2 m_position; 
    
public:
    Vehicle(const std::string& id, double initialSpeed = 0.0);
    ~Vehicle() = default;
    void update(double dt);
    void assignRoute(const std::shared_ptr<RouteOptimizer>& routeOptimizer);
    std::string getId() const;
    std::shared_ptr<Road> getCurrentRoad() const;
    std::shared_ptr<Intersection> getDestination() const; 
    void setDestination(const std::shared_ptr<Intersection>& destination);
    double getMovementProgress() const;
    double getCurrentSpeed() const;
    void setSpeed(double speed);
    Vector2 getPosition() const;
    void setPosition(const Vector2& position);
};