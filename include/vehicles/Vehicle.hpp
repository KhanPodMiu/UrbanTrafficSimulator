#pragma once

#include <memory>
#include <string>
#include <vector>

#include "utils/vector2i.hpp"

class Road;
class Intersection;
class RouteOptimizer;
class Graph;
class RoutingManager;

enum class VehicleType {
    CAR,
    BUS,
    EMERGENCY
};

class Vehicle {
protected:
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

    bool m_committedToIntersection = false;

    // Once true, this vehicle is rendered/treated as having pulled out into the
    // right-hand shoulder lane to get around a slower vehicle ahead. It is
    // sticky by design: the vehicle does not merge back once it has passed.
    bool m_isPassing = false;

    Vector2 m_position;

public:
    Vehicle(const std::string& id, double maxSpeed = 60.0);

    virtual ~Vehicle() = default;

    void update(double dt);

    void assignRoute(const std::shared_ptr<RouteOptimizer>& optimizer);

    void updateWorldPosition();

    bool tryAdvanceToNextRoad();

    /// Rebuilds the remaining route without restricted roads. If this vehicle
    /// is already on a newly restricted road, it keeps that road as the first
    /// route segment and evacuates through its forward endpoint.
    bool rerouteAroundBannedRoads(
        const Graph& graph,
        const RoutingManager& routingManager);

    virtual bool ignoresTrafficLights() const { return false; }

    // Whether this vehicle is allowed to pull out around a slower vehicle
    // blocking its lane instead of slowing down to follow it (e.g. ambulances).
    virtual bool canOvertakeBlockedTraffic() const { return false; }

    bool isInPassingLane() const;
    void setInPassingLane(bool passing);

    bool isCommittedToIntersection() const;
    void setCommittedToIntersection(bool committed);

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

    std::string getSourceIntersectionId() const;

    std::string getDestinationIntersectionId() const;

    std::string getCurrentRoadId() const;

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

    double getHeadingAngle() const;

};
