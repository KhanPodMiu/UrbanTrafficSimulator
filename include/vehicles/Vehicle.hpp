#pragma once

#include <memory>
#include <string>
#include <vector>

#include "utils/vector2i.hpp"

class Road;
class Intersection;
class RouteOptimizer;

class Vehicle {
private:
    std::string m_id;

    double m_currentSpeed;
    double m_targetSpeed;
    double m_maxSpeed;

    double m_distanceOnRoad;

    double m_length;

    bool m_finished;

    std::shared_ptr<Road> m_currentRoad;
    std::shared_ptr<Intersection> m_destination;
    std::shared_ptr<RouteOptimizer> m_routeOptimizer;

    std::vector<std::shared_ptr<Road>> m_route;
    size_t m_routeIndex;

    Vector2 m_position;

public:
    Vehicle(const std::string& id, double maxSpeed = 15.0);

    ~Vehicle() = default;

    void update(double dt);

    void assignRoute(const std::shared_ptr<RouteOptimizer>& optimizer);

    const std::string& getId() const;

    std::shared_ptr<Road> getCurrentRoad() const;

    std::shared_ptr<Intersection> getDestination() const;

    Vector2 getPosition() const;

    double getCurrentSpeed() const;

    double getTargetSpeed() const;

    double getMaxSpeed() const;

    double getDistanceOnRoad() const;

    double getVehicleLength() const;

    bool isFinished() const;

    double getMovementProgress() const;

    const std::vector<std::shared_ptr<Road>>& getRoute() const;

    size_t getRouteIndex() const;

    void setCurrentRoad(const std::shared_ptr<Road>& road);

    void setDestination(const std::shared_ptr<Intersection>& destination);

    void setPosition(const Vector2& position);

    void setCurrentSpeed(double speed);

    void setTargetSpeed(double speed);

    void setDistanceOnRoad(double distance);

    void setRoute(const std::vector<std::shared_ptr<Road>>& route);

    void setRouteIndex(size_t index);

    void setFinished(bool finished);

    void advanceDistance(double distance);

    std::shared_ptr<Road> getNextRoad() const;
};