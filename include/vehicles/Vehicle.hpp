#pragma once

#include <memory>
#include <string>
#include <vector>

#include "utils/vector2i.hpp"

class Road;
class Intersection;
class RouteOptimizer;

// VehicleType distinguishes the concrete subclasses produced by VehicleFactory
// (Car / Bus / EmergencyVehicle) without forcing every caller to know about
// those headers - useful for rendering (choosing a color) and for collision
// logic (e.g. an EmergencyVehicle ignores red lights).
enum class VehicleType {
    CAR,
    BUS,
    EMERGENCY
};

class Vehicle {
protected:
    // Protected "full" constructor used by subclasses (Car, Bus, EmergencyVehicle)
    // to set their own default speed/length/type while still sharing all of
    // Vehicle's state and behavior (classic inheritance + encapsulation).
    Vehicle(const std::string& id, double maxSpeed, double length, VehicleType type);

private:
    std::string m_id;
    VehicleType m_type;

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
    Vehicle(const std::string& id, double maxSpeed = 60.0);

    virtual ~Vehicle() = default;

    void update(double dt);

    void assignRoute(const std::shared_ptr<RouteOptimizer>& optimizer);

    void updateWorldPosition();

    bool tryAdvanceToNextRoad();

    virtual bool ignoresTrafficLights() const { return false; }

    const std::string& getId() const;

    VehicleType getType() const;

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
