#pragma once
#ifndef ROAD_HPP
#define ROAD_HPP

#include <string>
#include <queue>
#include <vector>
#include <algorithm>

class Intersection;
class Vehicle;
// ─────────────────────────────────────────────────────────────────────────────
//  TrafficLightState – embedded in Road because each Road owns its own light.
// ─────────────────────────────────────────────────────────────────────────────
enum class TrafficLightState {
    RED,
    YELLOW,
    GREEN
};

class Road {
    friend class TrafficLightManager;
public:
    // ── validation bounds (public so tests can reference them) ────────────────
    static constexpr int MIN_SPEED_LIMIT = 5;     // km/h
    static constexpr int MAX_SPEED_LIMIT = 130;   // km/h
    static constexpr int MIN_CONGESTION  = 0;
    static constexpr int MAX_CONGESTION  = 100;
    static constexpr int MIN_DISTANCE    = 1;     // map units
    static constexpr int MAX_DISTANCE    = 66400;  // ≈ 1.4× map diagonal

    // ── traffic light duration bounds ────────────────────────────────────────
    static constexpr int DEFAULT_GREEN_DURATION  = 10;  // seconds
    static constexpr int DEFAULT_YELLOW_DURATION = 3;   // seconds
    static constexpr int DEFAULT_RED_DURATION    = 3;  // seconds
    static constexpr int MIN_DURATION            = 1;   // seconds
    static constexpr int MAX_DURATION            = 120; // seconds

    //ADDED ── For BPR Funtion and CongestionLevel ──────────────────────────
    static constexpr double VEHICLE_HITBOX_SIZE = 100.0; // Length of vehicle hitbox in map units can change after
    static constexpr double BPR_ALPHA = 0.15;         
    static constexpr double BPR_BETA = 4.0;          

    // ── constructor / destructor ──────────────────────────────────────────────
    Road(
        const std::string& id,
        const Intersection*      source,
        const Intersection*      destination,
        int                speedLimit
    );

    ~Road();

    // ── getters ───────────────────────────────────────────────────────────────
    // Returns a const-ref to avoid copying the string.
    const std::string& getRoadId()                  const;

    // Returns raw pointers – callers must NOT take ownership.
    const Intersection*      getSourceIntersection()      const;
    const Intersection*      getDestinationIntersection() const;

    double  getDistance()       const;
    int  getSpeedLimit()     const;
    int  getCongestionLevel()const;
    double  getTravelCost()     const;

    // ── traffic light getters ─────────────────────────────────────────────────
    TrafficLightState getTrafficLightState()  const;
    bool              isTrafficLightEnabled() const;
    bool              isGreen()              const;
    int               getTimeRemaining()     const;
    int               getGreenDuration()     const;
    int               getYellowDuration()    const;
    int               getRedDuration()       const;

    //ADDED: Function to get total current Vehicle on Road:
    int               getTotalVehicles() const;

    // ===== Vehicle Management =====
    const std::vector<Vehicle*>& getVehicles() const;

    Vehicle* getVehicleAhead(const Vehicle* vehicle) const;

    void sortVehicles();

    void vehicleEnters(Vehicle* vehicle);

    void vehicleExits(Vehicle* vehicle);

    // ── setters ───────────────────────────────────────────────────────────────
    bool setSourceIntersection(const Intersection* source);
    bool setDestinationIntersection(const Intersection* destination);
    bool setDistance(int distance);     // accepted range: [MIN_DISTANCE,  MAX_DISTANCE ]
    bool setSpeedLimit(int speedLimit); // accepted range: [MIN_SPEED_LIMIT,MAX_SPEED_LIMIT]

    // ── traffic light setters ─────────────────────────────────────────────────
    bool setGreenDuration(int duration);
    bool setYellowDuration(int duration);
    bool setRedDuration(int duration);
    void setTrafficLightState(TrafficLightState state);
    void setTrafficLightEnabled(bool enabled);

    // ── road operations ───────────────────────────────────────────────────────

    // updateCongestion: sets congestionLevel and recomputes travelCost.
    // Returns false when newCongestionLevel is outside [MIN_CONGESTION, MAX_CONGESTION].
    bool updateCongestion();

    // calculateTravelCost: recomputes travelCost from current attributes.
    // Called automatically by every setter that touches a relevant attribute.
    // Returns false only if speedLimit is 0 (division-by-zero guard).
    bool calculateTravelCost();

    // ── traffic light operations ──────────────────────────────────────────────

    // Reset traffic light to GREEN with full timer.
    void resetTrafficLight();

    // NOTE: Road no longer drives its own traffic light timing. TrafficLightManager
    // is the single source of control for all signalized intersections: it writes
    // trafficLightState directly (it is a friend of Road) and sets trafficLightEnabled
    // for roads it manages. The self-driving updateTrafficLight()/switchToNextState()/
    // enableTrafficLight()/disableTrafficLight() methods that used to exist here were
    // removed to avoid two competing systems mutating the same state.

    // Check whether the destination intersection's type requires a traffic
    // light (T_INTERSECTION or CROSS → true). ROUNDABOUT intersections use
    // yield-style right-of-way instead of signals, so they are excluded.
    bool needsTrafficLightAtDestination() const;


private:
    std::string   roadId;

    const Intersection* sourceIntersection;
    const Intersection* destinationIntersection;

    double distance;        // map units      [MIN_DISTANCE,   MAX_DISTANCE  ]
    int speedLimit;      // km/h           [MIN_SPEED_LIMIT, MAX_SPEED_LIMIT]
    int congestionLevel; // dimensionless  [MIN_CONGESTION,  MAX_CONGESTION ]
    double travelCost;      // dimensionless weight, kept in sync by setters
    
    std::vector<Vehicle*> m_vehicles;
    
    // ── traffic light state ──────────────────────────────────────────────────
    TrafficLightState trafficLightState;
    bool              trafficLightEnabled;
    int               greenDuration;
    int               yellowDuration;
    int               redDuration;
    int               timeRemaining;   // countdown timer in seconds
};

#endif // ROAD_HPP